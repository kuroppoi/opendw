#include "Lightmapper.h"

#include "graphics/WorldRenderer.h"
#include "util/MathUtil.h"
#include "zone/BaseBlock.h"
#include "zone/MetaBlock.h"
#include "zone/WorldZone.h"
#include "CommonDefs.h"
#include "Item.h"

#define LIGHT_RING_ITERATIONS      8
#define LIGHTMAP_SCALE             0.5
#define LIGHTMAP_SHADER            "custom/Lightmap_fs"
#define TEXTURE_PADDING            4
#define DEFAULT_BASE_LIGHT         200.0
#define RESTRICT_FIELD_DAMAGE_AURA 0  // Whether or not field damage aura should only display in hell biomes

USING_NS_AX;

namespace opendw
{

Lightmapper::~Lightmapper()
{
    AX_SAFE_DELETE_ARRAY(_lightRings);
    AX_SAFE_DELETE_ARRAY(_textureData);
    AX_SAFE_RELEASE(_texture);
    AX_SAFE_RELEASE(_sprite);
    AX_SAFE_RELEASE(_programState);
}

Lightmapper* Lightmapper::createWithZone(WorldZone* zone)
{
    CREATE_INIT(Lightmapper, initWithZone, zone);
}

bool Lightmapper::initWithZone(WorldZone* zone)
{
    if (!Node::init())
    {
        return false;
    }

    _zone = zone;

    // Create program state
    auto program  = ProgramManager::getInstance()->loadProgram("positionTextureColor_vs", LIGHTMAP_SHADER);
    _programState = new ProgramState(program);
    _programState->autorelease();
    AX_SAFE_RETAIN(_programState);

    // Create lightmap
    auto& winSize = _director->getWinSize();  // Window size is fixed
    _lightmap     = RenderTexture::create(winSize.width * LIGHTMAP_SCALE, winSize.height * LIGHTMAP_SCALE);
    _lightmap->setScale(1.0F / LIGHTMAP_SCALE);
    addChild(_lightmap);

    // 0x10005597A: Compute light ring data
    ssize_t lightRingBytes = 0;

    for (ssize_t i = 0; i < LIGHT_RING_ITERATIONS; i++)
    {
        lightRingBytes += (i + 1) << 4;
    }

    _lightRings   = new int8_t[lightRingBytes];
    ssize_t index = 0;

    for (ssize_t i = 0; i < LIGHT_RING_ITERATIONS; i++)
    {
        ssize_t distance = i + 1;

        for (auto y = -distance; y <= distance; y++)
        {
            for (auto x = -distance; x <= distance; x++)
            {
                if (abs(x) == distance || abs(y) == distance)
                {
                    AX_ASSERT(index >= 0 && index * 2 + 1 < lightRingBytes);
                    _lightRings[index * 2]     = x;
                    _lightRings[index * 2 + 1] = y;
                    index++;
                }
            }
        }
    }

    AX_ASSERT(index * 2 == lightRingBytes);
    return true;
}

void Lightmapper::setupScreen()
{
    AX_SAFE_DELETE_ARRAY(_textureData);
    AX_SAFE_RELEASE(_texture);
    AX_SAFE_RELEASE(_sprite);
    auto worldScale = _zone->getWorldRenderer()->getWorldScale();
    auto& winSize   = _director->getWinSize();

    // Create texture
    _textureWidth     = (int)(ceil(winSize.width / worldScale / BLOCK_SIZE) + TEXTURE_PADDING * 2);
    _textureHeight    = (int)(ceil(winSize.height / worldScale / BLOCK_SIZE) + TEXTURE_PADDING * 2);
    _textureSizeBytes = (ssize_t)_textureWidth * _textureHeight * 4;
    _textureData      = new uint8_t[_textureSizeBytes];
    memset(_textureData, 0x7F, _textureSizeBytes);
    _texture = new Texture2D();
    _texture->autorelease();
    _texture->retain();
    _texture->initWithData(_textureData, _textureSizeBytes, backend::PixelFormat::RGBA8, _textureWidth, _textureHeight);

    // Create sprite
    _sprite = Sprite::createWithTexture(_texture);
    _sprite->retain();
    _sprite->setProgramState(_programState);
    _sprite->setBlendFunc({backend::BlendFactor::ONE, backend::BlendFactor::ONE});
    _sprite->setAnchorPoint(Point::ANCHOR_TOP_LEFT);
}

void Lightmapper::update(float deltaTime)
{
    auto worldRenderer = _zone->getWorldRenderer();
    auto worldScale    = worldRenderer->getWorldScale();
    auto worldSize     = Size(_zone->getBlocksWidth(), _zone->getBlocksHeight());
    auto ul            = _zone->getUpperLeftScreenBlockPoint().getClampPoint(Point::ZERO, worldSize);
    _ul                = ul - Vec2::ONE * TEXTURE_PADDING;
    _lr                = _zone->getLowerRightScreenBlockPoint() + Vec2::ONE * TEXTURE_PADDING;
    auto blocksRect    = Rect(_ul.x, _ul.y, _lr.x - _ul.x, _lr.y - _ul.y);
    _screenBlocks      = _zone->getBlocksInRect(blocksRect);

    // Recreate texture if world scale & screen rect size have changed
    // FIXME: Might not always update when it is supposed to
    if (worldScale != _previousWorldScale && !blocksRect.size.equals(_screenRect.size))
    {
        setupScreen();
    }

    _screenRect         = blocksRect;
    _previousWorldScale = worldScale;

    // Calculate sprite position
    auto scale    = worldScale * LIGHTMAP_SCALE;
    auto& winSize = _director->getWinSize();
    auto winTop   = Point(0.0F, winSize.height);
    auto point    = _zone->getPointAtBlock(ul.x, ul.y) - worldRenderer->getNodePointForScreenPoint(winTop);
    auto offset   = Vec2(-TEXTURE_PADDING - 0.5F, TEXTURE_PADDING + 0.5F) * BLOCK_SIZE;
    _sprite->setPosition(winTop * LIGHTMAP_SCALE + point * scale + offset * scale);
    _sprite->setScale(scale * BLOCK_SIZE);

    // Populate lightmap texture
    _lightmap->beginWithClear(0.0F, 0.0F, 0.0F, 0.0F);
    illuminateBlocks(deltaTime);
    _sprite->visit();
    _lightmap->end();
}

void Lightmapper::illuminateBlocks(float deltaTime)
{
    // Reset state
    memset(_textureData, 0x7F, _textureSizeBytes);
    _skyVisible          = false;
    _skyBlocksVisible    = 0;
    _cavernBlocksVisible = 0;

    for (auto block : _screenBlocks)
    {
        block->setCurrentLightR(0.0F);
        block->setCurrentLightG(0.0F);
        block->setCurrentLightB(0.0F);
        block->setCurrentLightA(0.0F);
        block->setCurrentLightLit(false);
    }

    // NOTE: We perform both passes in a single update
    // 0x100057824: Pass 1 (front lighting & light rings)
    auto surface = (float)(_zone->getBlocksHeight() >> 2);

    for (auto block : _screenBlocks)
    {
        auto x     = block->getX();
        auto y     = block->getY();
        auto front = block->getFrontItem();
        auto light = front->getLight();

        if (light <= 0.0F)
        {
            continue;
        }

        // Set block light color
        auto& color     = front->getLightColor();
        auto lightPoint = Point(x, y) + front->getLightPosition();

        if (_screenRect.containsPoint(lightPoint))
        {
            auto x     = (int16_t)lightPoint.x;
            auto y     = (int16_t)lightPoint.y;
            auto block = _zone->getBlockAt(x, y);

            if (block)
            {
                block->setCurrentLightR(color.r);
                block->setCurrentLightG(color.g);
                block->setCurrentLightB(color.b);
                block->setCurrentLightA(light * (block->getLiquid() > 0 ? 0.9F : 1.0F));
                block->setCurrentLightLit(true);
            }
            else
            {
                AXLOGW("[Lightmapper] Null block at {} {}", x, y);
            }
        }

        // 0x100057CC7: Apply light rings
        light             = clampf(light, 1.0F, (float)LIGHT_RING_ITERATIONS - 1.0F);
        auto ringCount    = MAX(2, MIN(LIGHT_RING_ITERATIONS, (int)light + 1));
        ssize_t ringIndex = 0;

        for (ssize_t i = 0; i < ringCount; i++)
        {
            ssize_t size = (i + 1) << 3;

            for (ssize_t j = 0; j < size; j++)
            {
                auto pointX = _lightRings[ringIndex + j * 2] + x;
                auto pointY = _lightRings[ringIndex + j * 2 + 1] + y;

                if (!_screenRect.containsPoint(Point(pointX, pointY)))
                {
                    continue;
                }

                auto block = _zone->getBlockAt(pointX, pointY);

                if (!block || block->isCurrentLightLit())
                {
                    continue;
                }

                auto distanceX  = (float)abs(pointX - x);
                auto distanceY  = (float)abs(pointY - y);
                auto distance   = distanceX * distanceX + distanceY * distanceY;
                distance        = clampf(distance * distance, 1.0F, 99999.0F);
                auto scale      = block->getLiquid() > 0 ? 0.9F : 1.0F;
                auto colorScale = scale * 0.15F / distance;
                auto alpha      = 200.0F / distance * scale * light * 20.0F;
                block->setCurrentLightR(block->getCurrentLightR() + color.r * colorScale);
                block->setCurrentLightG(block->getCurrentLightG() + color.g * colorScale);
                block->setCurrentLightB(block->getCurrentLightB() + color.b * colorScale);
                block->setCurrentLightA(block->getCurrentLightA() + alpha);
            }

            ringIndex += size << 1;
        }
    }

    // 0x100057824: Pass 2 (sunlight & liquid lighting)
    for (auto block : _screenBlocks)
    {
        auto x     = block->getX();
        auto y     = block->getY();
        auto front = block->getFrontItem();

        // 0x1000578BA: Increment visible base block counter
        auto base = block->getBase();

        if (base < 2 && !block->isOpaque())
        {
            if (base == 0)
            {
                _skyBlocksVisible++;
            }
            else  // 1 = base/empty
            {
                _cavernBlocksVisible++;
            }
        }

        // 0x10005791B: Apply light from sunlight
        auto light    = 0.0F;
        auto sunlight = _zone->getSunlightAt(x);

        if (base == 0)
        {
            light = 250.0F;

            if (sunlight < y && block->getBack() > 0 && !front->isWhole())
            {
                auto above = block->getAbove();

                if (above && above->getFrontItem()->isWhole())
                {
                    light = 0.0F;
                }
            }
        }
        else
        {
            // Get a bit of sunlight from nearby blocks if we can
            auto width = _zone->getBlocksWidth();
            auto depth = clampf(((float)y - surface) / (surface * 3.0F), 0.0F, 1.0F);
            light      = clampf((float)sunlight + 5.0F - y, 0.0F, 5.0F);
            light      = MathUtil::lerp(light / 5.0F * 250.0F, 0.0F, depth);

            if (x > 0)
            {
                auto adjacentLight = clampf((float)_zone->getSunlightAt(x - 1) + 5.0F - y, 0.0F, 5.0F);
                light += MathUtil::lerp(adjacentLight / 5.0F * 150.0F, 0.0F, depth);

                if (x > 1)
                {
                    auto adjacentLight = clampf((float)_zone->getSunlightAt(x - 2) + 5.0F - y, 0.0F, 5.0F);
                    light += MathUtil::lerp(adjacentLight / 5.0F * 75.0F, 0.0F, depth);
                }
            }

            if (x + 1 < width)
            {
                auto adjacentLight = clampf((float)_zone->getSunlightAt(x + 1) + 5.0F - y, 0.0F, 5.0F);
                light += MathUtil::lerp(adjacentLight / 5.0F * 150.0F, 0.0F, depth);

                if (x + 2 < width)
                {
                    auto adjacentLight = clampf((float)_zone->getSunlightAt(x + 2) + 5.0F - y, 0.0F, 5.0F);
                    light += MathUtil::lerp(adjacentLight / 5.0F * 75.0F, 0.0F, depth);
                }
            }
        }

        light = clampf(light, 0.0F, 255.0F);

        // 0x100058425: Apply liquid lighting
        if (block->getLiquid() > 0)
        {
            auto liquid = block->getLiquidItem();
            auto light  = liquid->getLight();
            auto& color = liquid->getLightColor();

            if (light > 0.0F)
            {
                block->setCurrentLightR(block->getCurrentLightR() + color.r * 0.25F);
                block->setCurrentLightG(block->getCurrentLightG() + color.g * 0.25F);
                block->setCurrentLightB(block->getCurrentLightB() + color.b * 0.25F);
                block->setCurrentLightA(block->getCurrentLightA() + light * 50.0F);
            }
        }

        auto red   = block->getCurrentLightR();
        auto green = block->getCurrentLightG();
        auto blue  = block->getCurrentLightB();
        auto alpha = clampf(getBaseLight() - block->getCurrentLightA(), 0.0F, 255.0F);
        alpha -=
            MathUtil::lerp(light * (_zone->getCloudCover() * -0.4F + 1.0F) * _zone->getDayPercent(), 255.0F, _flash);

        // 0x10005866B: Apply pulsating glow effect
        // FIXME: take light position into account
        if (front->getLight() > 0.0F)
        {
            auto offset = MathUtil::lerp(4.0F, 7.0F, clampf((float)x / y, 0.0F, 1.0F));
            auto glow   = sinf(offset * ((float)y + x + utils::gettime())) * 10.0F + 10.0F;
            red -= glow;
            green -= glow;
            blue -= glow;
        }

        // 0x10005873E: Show field damage radius
#if RESTRICT_FIELD_DAMAGE_AURA
        if (_zone->getBiomeType() == Biome::HELL)
#endif
        {
            auto fieldDamageBlock = _zone->getFieldDamageBlock();

            if (fieldDamageBlock)
            {
                auto& color    = fieldDamageBlock->getItem()->getColor();
                auto distance  = math_util::getDistance(x, y, fieldDamageBlock->getX(), fieldDamageBlock->getY());
                auto intensity = clampf(distance / -50.0F + 1.0F, 0.0F, 1.0F);
                red            = MathUtil::lerp(red, color.r, intensity);
                green          = MathUtil::lerp(green, color.g, intensity);
                blue           = MathUtil::lerp(blue, color.b, intensity);
            }
        }

        // TODO: haze
        // TODO: death overlay

        red   = clampf(red, 0.0F, 255.0F);
        green = clampf(green, 0.0F, 255.0F);
        blue  = clampf(blue, 0.0F, 255.0F);
        alpha = clampf(alpha, 0.0F, 255.0F);

        // 0x100058A72: Set pixel in texture
        auto pixelX = (ssize_t)(floor(x - _ul.x));
        auto pixelY = (ssize_t)(floor(y - _ul.y));
        auto pixel  = (pixelY * _textureWidth + pixelX) * 4;

        // FIXME: This CAN happen, and while it *shouldn't* really matter, it'd be better if it didn't.
        if (pixel >= 0 && pixel + 3 < _textureSizeBytes)
        {
            _textureData[pixel]     = (uint8_t)red;
            _textureData[pixel + 1] = (uint8_t)green;
            _textureData[pixel + 2] = (uint8_t)blue;
            _textureData[pixel + 3] = (uint8_t)alpha;
        }
    }

    _flash = clampf(_flash - deltaTime * (255.0F / 0.1F), 0.0F, 255.0F);  // HACK: consistent flash duration
    _texture->updateWithData(_textureData, _textureSizeBytes, backend::PixelFormat::RGBA8, backend::PixelFormat::RGBA8,
                             _textureWidth, _textureHeight, false);
}

float Lightmapper::getBaseLight() const
{
    if (_moodLighting)
    {
        return 255.0F;
    }

    auto baseLight = DEFAULT_BASE_LIGHT;

    if (_zone->getBiomeType() == Biome::HELL)
    {
        baseLight -= 30.0F;
    }

    return baseLight;
}

}  // namespace opendw
