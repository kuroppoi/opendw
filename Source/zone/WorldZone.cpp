#include "WorldZone.h"

#include "graphics/SceneRenderer.h"
#include "graphics/WorldRenderer.h"
#include "input/InputManager.h"
#include "network/tcp/MessageIdent.h"
#include "util/ArrayUtil.h"
#include "util/MapUtil.h"
#include "util/StringUtil.h"
#include "zone/BaseBlock.h"
#include "zone/WorldChunk.h"
#include "AudioManager.h"
#include "CommonDefs.h"
#include "GameConfig.h"
#include "GameManager.h"
#include "Player.h"

#define CHUNK_PREALLOC_COUNT   60
#define WEATHER_STATUS_LENGTH  6
#define CHUNK_REQUEST_INTERVAL 0.5
#define BLOCKS_IGNORE_INTERVAL 0.666
#define MAX_CHUNK_PENDING_TIME 10.0

USING_NS_AX;

namespace opendw
{

WorldZone* WorldZone::createWithGame(GameManager* game)
{
    CREATE_INIT(WorldZone, initWithGame, game);
}

bool WorldZone::initWithGame(GameManager* game)
{
    if (!Node::init())
    {
        return false;
    }

    _game  = game;
    _state = State::INACTIVE;
    _inactiveChunks.reserve(CHUNK_PREALLOC_COUNT);
    return true;
}

void WorldZone::setup()
{
    AXLOGI("[WorldZone] Setting up zone");
    _sceneRenderer = SceneRenderer::createWithZone(this);
    _worldRenderer = _sceneRenderer->getWorldRenderer();
    addChild(_sceneRenderer);
}

void WorldZone::configure(const ValueMap& data)
{
    _receivedInitialStatus = false;

    auto config  = _game->getConfig();
    _documentId  = map_util::getString(data, "id");
    _name        = map_util::getString(data, "name", "Unknown Zone");
    _biome       = map_util::getString(data, "biome", "plain");
    _biomeType   = getBiomeForName(_biome);
    _biomeConfig = config->getBiomeConfig(_biome);
    _seed        = map_util::getUInt64(data, "seed", getDefaultSeed());

    // NOTE: originally done further down at 0x100040482
    config->loadBiome(_biome);
    _worldRenderer->loadBiome(_biome);

    // Configure dimensions
    auto& size      = map_util::getArray(data, "size");
    auto& chunkSize = map_util::getArray(data, "chunk_size");
    _blocksWidth    = size[0].asUint();
    _blocksHeight   = size[1].asUint();
    _blockCount     = _blocksWidth * _blocksHeight;
    _chunkWidth     = chunkSize[0].asUint();
    _chunkHeight    = chunkSize[1].asUint();
    AX_ASSERT(_blocksWidth % _chunkWidth == 0 && _blocksHeight % _chunkHeight == 0);
    _chunkSize   = _chunkWidth * _chunkHeight;
    _chunkCount  = _blockCount / _chunkSize;
    _chunkCountX = _blocksWidth / _chunkWidth;
    _chunkCountY = _blocksHeight / _chunkHeight;
    AXLOGI("[WorldZone] Block instance size: {}", sizeof(BaseBlock));
    AXLOGI("[WorldZone] Chunk instance size: {} ({} with {} blocks)", sizeof(WorldChunk),
           _chunkSize * sizeof(BaseBlock), _chunkSize);

    // Configure surface
    auto& surface = map_util::getArray(data, "surface");
    AX_ASSERT(surface.size() == _blocksWidth);
    _surfaceTop    = _blocksHeight;
    _surfaceBottom = 0;

    for (auto& element : surface)
    {
        auto y         = static_cast<uint16_t>(element.asUint());
        _surfaceTop    = MIN(_surfaceTop, y);
        _surfaceBottom = MAX(_surfaceBottom, y);
    }

    // Preallocate a bunch of chunks if none are allocated at the moment
    if (WorldChunk::getChunksAllocated() == 0)
    {
        for (size_t i = 0; i < CHUNK_PREALLOC_COUNT; i++)
        {
            auto chunk = WorldChunk::createWithZone(this, 0, 0, _chunkSize);
            _inactiveChunks.pushBack(chunk);
        }

        AXLOGI("[WorldZone] Preallocated {} chunks", CHUNK_PREALLOC_COUNT);
    }

    // 0x1000400E9: Configure depth graphics
    _depthGraphics.clear();
    auto& depthConfig = map_util::getMap(data, "depth");

    for (auto& source : depthConfig)
    {
        auto& key   = source.first;
        auto& array = source.second.asValueVector();
        std::vector<std::pair<double, SpriteFrame*>> frames;

        for (auto& target : array)
        {
            auto& array = target.asValueVector();
            auto depth  = array[0].asDouble();
            auto frame  = SpriteFrameCache::getInstance()->findFrame(array[1].asStringRef());
            frames.push_back({depth, frame});
        }

        _depthGraphics[key] = frames;
    }
}

void WorldZone::update(float deltaTime)
{
    Node::update(deltaTime);

    // Don't update if we're teleporting to another zone
    if (_game->getPlayer()->isZoneTeleporting())
    {
        return;
    }

    // 0x1000415CF: Update day percent
    _dayPercent = _dayTime >= 0.5F ? 1.0F - (_dayTime - 0.5F) * 2.0F : _dayTime * 2.0F;

    // 0x1000416F9: Create a list of on-screen chunks that need to be requested
    // TODO: can probably be optimized by just incrementing by chunk size
    std::unordered_set<uint32_t> chunksToRequest;
    Point clampMax(_blocksWidth - 1, _blocksHeight - 1);
    auto upperLeft  = getUpperLeftScreenBlockPoint().getClampPoint(Point::ZERO, clampMax);
    auto lowerRight = getLowerRightScreenBlockPoint().getClampPoint(Point::ZERO, clampMax);

    for (uint16_t y = upperLeft.y; y <= lowerRight.y; y++)
    {
        for (uint16_t x = upperLeft.x; x <= lowerRight.x; x++)
        {
            // 0x10004181F: tests for getBlockAt(x, y);
            auto chunkX     = x / _chunkWidth;
            auto chunkY     = y / _chunkHeight;
            auto chunkIndex = chunkY * _chunkCountX + chunkX;

            if (_chunks.find(chunkIndex) == _chunks.end())
            {
                chunksToRequest.insert(chunkIndex);
            }
        }
    }

    // 0x100041943: If there are no on-screen chunks to request, try requesting surrounding chunks instead
    if (chunksToRequest.empty())
    {
        if (!_justWaitedForBlocks)
        {
            bool teleporting = false;  // TODO

            if (!teleporting || _pendingChunks.empty())
            {
                if (teleporting && !_worldRenderer->hasRenderedAllPlacedBlocks())
                {
                    waitForBlocks();
                }
                else
                {
                    // TODO: doesn't match original implementation
                    for (uint16_t y = upperLeft.y - 20.0F; y <= lowerRight.y + 20.0F; y += _chunkWidth)
                    {
                        for (uint16_t x = upperLeft.x - 20.0F; x <= lowerRight.x + 20.0F; x += _chunkHeight)
                        {
                            auto chunkX     = x / _chunkWidth;
                            auto chunkY     = y / _chunkHeight;
                            auto chunkIndex = chunkY * _chunkCountX + chunkX;

                            if (_chunks.find(chunkIndex) == _chunks.end())
                            {
                                chunksToRequest.insert(chunkIndex);
                            }
                        }
                    }
                }
            }
        }
        else
        {
            _doneWaitingForBlocksAt = utils::gettime();
            _justWaitedForBlocks    = false;
            _paused                 = false;  // TODO
        }
    }
    else
    {
        waitForBlocks();
        _justWaitedForBlocks = true;
    }

    // 0x100041E6D: Request chunks from the server if necessary
    if (!chunksToRequest.empty() && utils::gettime() >= _lastBlocksRequestAt + CHUNK_REQUEST_INTERVAL)
    {
        // Delete pending chunks that are taking too long
        std::vector<uint32_t> expiredChunks;

        for (auto it = _pendingChunks.begin(); it != _pendingChunks.end();)
        {
            if (utils::gettime() >= (*it).second + MAX_CHUNK_PENDING_TIME)
            {
                expiredChunks.push_back((*it).first);
                it = _pendingChunks.erase(it);
            }
            else
            {
                it++;
            }
        }

        if (!expiredChunks.empty())
        {
            AXLOGW("[WorldZone] Giving up on waiting for pending chunks {}", string_util::join(expiredChunks));
        }

        // Create final request list and append to pending chunks
        std::vector<uint32_t> chunksToSend;

        for (auto chunk : chunksToRequest)
        {
            if (!_pendingChunks.contains(chunk))
            {
                chunksToSend.push_back(chunk);
                _pendingChunks[chunk] = utils::gettime();
            }
        }

        // Finally, send the blocks request
        if (!chunksToSend.empty())
        {
            AXLOGI("[WorldZone] Requesting chunks for [{}]", string_util::join(chunksToSend));
            _game->sendMessage(MessageIdent::BLOCKS, array_util::arrayOf(array_util::convert(chunksToSend)));
            _lastBlocksRequestAt = utils::gettime();
        }
    }

    // TODO: implement pause
    if (_paused)
    {
        return;
    }

    // 0x100042281: Send block ignore request
    if (!_cleanedChunks.empty() && utils::gettime() >= _lastBlocksIgnoreAt + BLOCKS_IGNORE_INTERVAL)
    {
        AXLOGI("[WorldZone] Ignoring chunks [{}]", string_util::join(_cleanedChunks));
        _game->sendMessage(MessageIdent::BLOCKS_IGNORE, array_util::arrayOf(array_util::convert(_cleanedChunks)));
        _cleanedChunks.clear();
        _lastBlocksIgnoreAt = utils::gettime();
    }

    _sceneRenderer->hideSpinner();
    _game->hideSnapshotSpinner();
    _game->getInputManager()->checkInput(deltaTime);
    _game->getPlayer()->update(deltaTime);
    _sceneRenderer->update(deltaTime);

    // 0x100042714: Update timed status
    if (_timedStatus.size() >= WEATHER_STATUS_LENGTH)
    {
        updateTimedStatus(deltaTime);
        _receivedInitialStatus = true;
    }
}

void WorldZone::updateTimedStatus(float deltaTime)
{
    auto dayTime    = _timedStatus[0].asFloat() / 10000.0F;
    auto acidity    = _timedStatus[5].asFloat() / 10000.0F;
    auto cloudCover = _timedStatus[3].asFloat() / 10000.0F;
    auto smoothTime = _receivedInitialStatus ? 10.0F : 0.0F;

    if (dayTime < _dayTime)
    {
        _dayTime = dayTime;
    }
    else
    {
        MathUtil::smooth(&_dayTime, dayTime, deltaTime, smoothTime);
    }

    MathUtil::smooth(&_acidity, acidity, deltaTime, smoothTime);
    MathUtil::smooth(&_cloudCover, cloudCover, deltaTime, smoothTime);
}

void WorldZone::updateStatus(const ValueMap& status)
{
    if (status.contains("w"))
    {
        _timedStatus = map_util::getArray(status, "w");
    }
}

void WorldZone::enter()
{
    AXLOGI("[WorldZone] Entering zone!");
    _sceneRenderer->ready();
    begin();
}

void WorldZone::begin()
{
    AXLOGI("[WorldZone] Beginning zone!");
    _state = State::ACTIVE;
    _sceneRenderer->setVisible(true);
    _game->getPlayer()->begin();
    AudioManager::getInstance()->fadeOutMusic();  // Fade out menu music
    schedule(AX_CALLBACK_0(WorldZone::cleanupChunks, this), 0.033F, "cleanupChunks");
}

void WorldZone::waitForBlocks()
{
    _paused = true;
    _sceneRenderer->showSpinner();
}

void WorldZone::cleanupChunks()
{
    // TODO: static worlds are apparently not cleaned up
    if (_state != State::ACTIVE)
    {
        return;
    }

    auto start   = utils::gettime();
    auto player  = _game->getPlayer();
    auto cleaned = 0;

    for (auto it = _activeChunks.begin(); it != _activeChunks.end();)
    {
        auto chunk = *it;

        // Don't try to recycle chunk if it has been active for less than 5 seconds
        if (start <= chunk->getBeganAt() + 5.0F)
        {
            it++;
            continue;
        }

        auto centerX  = (float)chunk->getBlockX() + _chunkWidth * 0.5F;
        auto centerY  = (float)chunk->getBlockY() + _chunkHeight * 0.5F;
        auto position = player->getBlockPosition();
        auto distance = hypotf(position.x - centerX, position.y - centerY);

        // TODO: this distance check might clean up chunks on screen if zoomed out far enough!
        if (distance <= _chunkWidth * 5)
        {
            it++;
            continue;
        }

        auto index = chunk->getIndex();
        chunk->recycle();
        _chunks.erase(index);
        _inactiveChunks.pushBack(chunk);
        it = _activeChunks.erase(it);
        _cleanedChunks.push_back(index);
        cleaned++;

        if (cleaned >= 2)
        {
            break;
        }
    }
}

void WorldZone::removePendingChunk(uint16_t x, uint16_t y)
{
    auto chunkX = x / _chunkWidth;
    auto chunkY = y / _chunkHeight;
    _pendingChunks.erase(chunkY * _chunkCountX + chunkX);
}

void WorldZone::leave()
{
    if (_state != State::ACTIVE)
    {
        return;
    }

    AXLOGI("[WorldZone] Leaving zone!");
    _state = State::LEAVING;
    unscheduleAllCallbacks();
    _sceneRenderer->clear();
    _sceneRenderer->setVisible(false);

    // 0x10004988A: Recycle all active chunks
    for (auto chunk : _activeChunks)
    {
        chunk->recycle();
    }

    // 0x1000498F0: Clear containers
    _inactiveChunks.clear();
    _activeChunks.clear();
    _pendingChunks.clear();
    _chunks.clear();  // free(chunks); chunks = nullptr;
    // TODO: clear other containers as we add them
}

BaseBlock* WorldZone::getBlockAt(uint16_t x, uint16_t y, bool allowChunkAlloc)
{
    // Check world bounds
    if (x < 0 || x >= _blocksWidth || y < 0 || y >= _blocksHeight)
    {
        return nullptr;
    }

    auto chunkX     = x / _chunkWidth;
    auto chunkY     = y / _chunkHeight;
    auto chunkIndex = chunkY * _chunkCountX + chunkX;
    auto chunk      = _chunks.at(chunkIndex);

    if (!chunk)
    {
        if (!allowChunkAlloc)
        {
            return nullptr;
        }

        // Recycle inactive chunk or allocate a new one if none are available
        if (_inactiveChunks.empty())
        {
            chunk = WorldChunk::createWithZone(this, chunkX, chunkY, _chunkSize);
            _chunks.insert(chunkIndex, chunk);
            _activeChunks.pushBack(chunk);
        }
        else
        {
            chunk = _inactiveChunks.back();
            chunk->setPosition(chunkX, chunkY);
            _chunks.insert(chunkIndex, chunk);
            _activeChunks.pushBack(chunk);
            _inactiveChunks.popBack();  // Do this last to prevent refcount from reaching zero
        }
    }

    // If chunk is nullptr at this stage then we've got bigger problems
    return chunk->getBlockAt(x - chunk->getBlockX(), y - chunk->getBlockY());
}

uint64_t WorldZone::getDefaultSeed() const
{
    if (_documentId.size() < 10)
    {
        return 0;
    }

    return static_cast<uint64_t>(std::stoll(_documentId.substr(2, 8), nullptr, 16));
}

Point WorldZone::getPointAtBlock(uint16_t x, uint16_t y) const
{
    return Point(((float)x + 0.5F) * BLOCK_SIZE, ((float)-y - 0.5F) * BLOCK_SIZE);
}

Point WorldZone::getBlockPointAtNodePoint(const Point& point) const
{
    return Point(floorf(point.x / BLOCK_SIZE), floorf(-point.y / BLOCK_SIZE));
}

Point WorldZone::getUpperLeftScreenBlockPoint() const
{
    auto& winSize = _director->getWinSize();
    Point point(1.0F, winSize.height - 1.0F);
    return getBlockPointAtNodePoint(_worldRenderer->getNodePointForScreenPoint(point));
}

Point WorldZone::getLowerRightScreenBlockPoint() const
{
    auto& winSize = _director->getWinSize();
    Point point(winSize.width - 1.0F, 1.0F);
    return getBlockPointAtNodePoint(_worldRenderer->getNodePointForScreenPoint(point));
}

Biome WorldZone::getBiomeForName(const std::string& name)
{
    if (name == "plain" || name == "temperate")
        return Biome::TEMPERATE;
    else if (name == "hell")
        return Biome::HELL;
    else if (name == "arctic")
        return Biome::ARCTIC;
    else if (name == "brain")
        return Biome::BRAIN;
    else if (name == "desert")
        return Biome::DESERT;
    else if (name == "deep")
        return Biome::DEEP;
    else if (name == "space")
        return Biome::SPACE;

    AX_ASSERT(!"Invalid biome name");
    return Biome::TEMPERATE;
}

}  // namespace opendw
