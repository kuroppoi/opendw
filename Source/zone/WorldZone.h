#ifndef __WORLD_ZONE_H__
#define __WORLD_ZONE_H__

#include "axmol.h"

namespace opendw
{

class BaseBlock;
class GameManager;
class SceneRenderer;
class WorldRenderer;
class WorldChunk;

enum class Biome : uint8_t
{
    TEMPERATE,
    HELL,
    ARCTIC,
    BRAIN,
    DESERT,
    // OCEAN,
    DEEP = 6,
    SPACE
};

/*
 * CLASS: WorldZone : CCNode @ 0x100316E50
 */
class WorldZone : public ax::Node
{
public:
    enum class State
    {
        INACTIVE,
        ACTIVE,
        LEAVING
    };

    typedef std::unordered_map<std::string, std::vector<std::pair<double, ax::SpriteFrame*>>> DepthGraphics;

    static WorldZone* createWithGame(GameManager* game);

    /* FUNC: WorldZone::initWithGame: @ 0x10003F717 */
    bool initWithGame(GameManager* game);

    /* FUNC: WorldZone::configure: @ 0x10003FA19 */
    void configure(const ax::ValueMap& data);

    /* FUNC: WorldZone::step: @ 0x100041506 */
    void update(float deltaTime) override;

    /* FUNC: WorldZone::updateTimedStatus: @ 0x1000459C7 */
    void updateTimedStatus(float deltaTime);

    /* FUNC: WorldZone::updateStatus: @ 0x10004553F */
    void updateStatus(const ax::ValueMap& status);

    /* FUNC: WorldZone::setup @ 0x100041201 */
    void setup();

    /* FUNC: WorldZone::enter @ 0x100041284 */
    void enter();

    /* FUNC: WorldZone::begin @ 0x100041358 */
    void begin();

    /* FUNC: WorldZone::waitForBlocks @ 0x100046B02 */
    void waitForBlocks();

    /* FUNC: WorldZone::cleanup: @ 0x100045E8C */
    void cleanupChunks();

    /* FUNC: WorldZone::didLoadChunkX:y: @ 0x100046C6B */
    void removePendingChunk(uint16_t x, uint16_t y);

    /* FUNC: WorldZone::leave @ 0x1000497AB */
    void leave();

    /* FUNC: 0x100045192 */
    BaseBlock* getBlockAt(uint16_t x, uint16_t y, bool allowChunkAlloc = false);

    /* FUNC: WorldZone::defaultSeed @ 0x100041497 */
    uint64_t getDefaultSeed() const;

    /* FUNC: WorldZone::worldRenderer @ 0x100004A4A8 */
    WorldRenderer* getWorldRenderer() const { return _worldRenderer; }

    /* FUNC: WorldZone::pointAtBlockX:y: @ 0x10004643C */
    ax::Point getPointAtBlock(uint16_t x, uint16_t y) const;

    /* FUNC: WorldZone::blockIndexAtNodePoint: 0x100046549 */
    ax::Point getBlockPointAtNodePoint(const ax::Point& point) const;

    /* FUNC: WorldZone::upperLeftScreenBlockIndex @ 0x10004685F */
    ax::Point getUpperLeftScreenBlockPoint() const;

    /* FUNC: WorldZone::lowerRightScreenBlockIndex @ 0x1000468D2 */
    ax::Point getLowerRightScreenBlockPoint() const;

    /* FUNC: WorldZone::documentId @ 0x100049FC3 */
    const std::string& getDocumentId() const { return _documentId; }

    /* FUNC: WorldZone::name @ 0x100049F95 */
    const std::string& getZoneName() const { return _name; }

    /* FUNC: WorldZone::biome @ 0x10004A034 */
    const std::string& getBiome() const { return _biome; }

    /* FUNC: WorldZone::biomeType @ 0x10004A062 */
    Biome getBiomeType() const { return _biomeType; }

    /* FUNC: WorldZone::biomeConfig @ 0x10004A072 */
    const ax::ValueMap& getBiomeConfig() const { return _biomeConfig; }

    /* FUNC: WorldZone::depthGraphics @ 0x10004A2F3 */
    const DepthGraphics& getDepthGraphics() const { return _depthGraphics; }

    /* FUNC: WorldZone::seed @ 0x10004A012 */
    uint64_t getSeed() const { return _seed; }

    /* FUNC: WorldZone::blocksWidth @ 0x100049D33 */
    uint16_t getBlocksWidth() const { return _blocksWidth; }

    /* FUNC: WorldZone::blocksHeight @ 0x100049D44 */
    uint16_t getBlocksHeight() const { return _blocksHeight; }

    /* FUNC: WorldZone::chunkWidth @ 0x100049DEA */
    uint16_t getChunkWidth() const { return _chunkWidth; }

    /* FUNC: WorldZone::chunkHeight @ 0x100049DFB */
    uint16_t getChunkHeight() const { return _chunkHeight; }

    /* FUNC: WorldZone::chunksCount @ 0x100049E3F */
    uint32_t getChunkCount() const { return _chunkCount; }

    /* FUNC: WorldZone::xChunksCount @ 0x100049E1D */
    uint16_t getChunkCountX() const { return _chunkCountX; }

    /* FUNC: WorldZone::yChunksCount @ 0x100049E2E */
    uint16_t getChunkCountY() const { return _chunkCountY; }

    /* FUNC: WorldZone::surfaceTop @ 0x10004A28D */
    uint16_t getSurfaceTop() const { return _surfaceTop; }

    /* FUNC: WorldZone::surfaceBottom @ 0x10004A29E */
    uint16_t getSurfaceBottom() const { return _surfaceBottom; }

    /* FUNC: WorldZone::dayPercent @ 0x10004A177 */
    float getDayPercent() const { return _dayPercent; }

    /* FUNC: WorldZone::daytime @ 0x10004A153 */
    float getDayTime() const { return _dayTime; }

    /* FUNC: WorldZone::acidty @ 0x10004A304 */
    float getAcidity() const { return _acidity; }

    /* FUNC: WorldZone::cloudCover @ 0x10004A1D1 */
    float getCloudCover() const { return _cloudCover; }

    /* FUNC: WorldZone::doneWaitingForBlocksAt @ 0x100049F62 */
    double getDoneWaitingForBlocksAt() const { return _doneWaitingForBlocksAt; }

    /* FUNC: WorldZone::state @ 0x10004A507 */
    State getState() const { return _state; }

private:
    /* SNIPPET: 0x10003FD44 - 0x10003FEAE */
    static Biome getBiomeForName(const std::string& name);

    GameManager* _game;                         // WorldZone::game @ 0x100310EA8
    SceneRenderer* _sceneRenderer;              // WorldZone::sceneRenderer @ 0x1003110B8
    WorldRenderer* _worldRenderer;              // WorldZone::worldRenderer @ 0x100310FC0
    ax::Vector<WorldChunk*> _inactiveChunks;    // WorldZone::inactiveChunks @ 0x100310EE8
    ax::Vector<WorldChunk*> _activeChunks;      // WorldZone::activeChunks @ 0x100310EE0
    std::map<uint32_t, double> _pendingChunks;  // WorldZone::pendingChunks @ 0x100310EF0
    std::vector<uint32_t> _cleanedChunks;       // WorldZone::cleanedChunks @ 0x100310F08
    ax::Map<uint32_t, WorldChunk*> _chunks;     // WorldZone::chunks @ 0x100311088
    std::string _documentId;                    // WorldZone::documentId @ 0x100311170
    std::string _name;                          // WorldZone::name @ 0x100311190
    std::string _biome;                         // WorldZone::biome @ 0x100310F98
    Biome _biomeType;                           // WorldZone::biomeType @ 0x100310FA0
    ax::ValueMap _biomeConfig;                  // WorldZone::biomeConfig @ 0x100310FC8
    DepthGraphics _depthGraphics;               // WorldZone::depthGraphics @ 0x100310FB8
    uint64_t _seed;                             // WorldZone::seed @ 0x1003111A0
    uint16_t _blocksWidth;                      // WorldZone::blocksWidth @ 0x100310FF8
    uint16_t _blocksHeight;                     // WorldZone::blocksHeight @ 0x100311000
    uint32_t _blockCount;                       // WorldZone::blocksCount @ 0x100311050
    uint16_t _chunkWidth;                       // WorldZone::chunkWidth @ 0x100311008
    uint16_t _chunkHeight;                      // WorldZone::chunkHeight @ 0x100311010
    uint16_t _chunkSize;                        // WorldZone::chunkSize @ 0x100311018
    uint32_t _chunkCount;                       // WorldZone::chunksCount @ 0x100311010
    uint16_t _chunkCountX;                      // WorldZone::xChunksCount @ 0x100311020
    uint16_t _chunkCountY;                      // WorldZone::yChunksCount @ 0x100311028
    uint16_t _surfaceTop;                       // WorldZone::surfaceTop @ 0x100311060
    uint16_t _surfaceBottom;                    // WorldZone::surfaceBottom @ 0x100311068
    ax::ValueVector _timedStatus;               // WorldZone::status @ 0x100311110
    bool _receivedInitialStatus;                // WorldZone::receivedInitialStatus @ 0x100310F58
    float _dayPercent;                          // WorldZone::dayPercent @ 0x1003110E8
    float _dayTime;                             // WorldZone::daytime @ 0x1003110E0
    float _acidity;                             // WorldZone::acidity @ 0x100311138
    float _cloudCover;                          // WorldZone::cloudCover @ 0x100311160
    double _lastBlocksRequestAt;                // WorldZone::lastBlocksRequestAt @ 0x100311100
    double _lastBlocksIgnoreAt;                 // WorldZone::lastBlocksIgnoreAt @ 0x100311108
    double _doneWaitingForBlocksAt;             // WorldZone::doneWaitingForBlocksAt @ 0x1003110F8
    bool _justWaitedForBlocks;                  // WorldZone::justWaitedForBlocks @ 0x1003110F0
    State _state;                               // WorldZone::state @ 0x100310F28
    bool _paused;                               // WorldZone::paused @ 0x100310F30
};

}  // namespace opendw

#endif  // __WORLD_ZONE_H__
