#include "SkyRenderer.h"

#include "graphics/SkyCloud.h"
#include "graphics/SkyMountain.h"
#include "graphics/SkyRubble.h"
#include "util/ColorUtil.h"
#include "util/MapUtil.h"
#include "zone/WorldZone.h"
#include "CommonDefs.h"

#define MOUNTAIN_COUNT    12
#define MOUNTAIN_OFFSET   400.0F
#define FRONT_CLOUD_COUNT 14
#define BACK_CLOUD_COUNT  FRONT_CLOUD_COUNT * 2
#define FRONT_STAR_COUNT  750
#define BACK_STAR_COUNT   500

USING_NS_AX;

namespace opendw
{

SkyRenderer* SkyRenderer::createWithZone(WorldZone* zone)
{
    CREATE_INIT(SkyRenderer, initWithZone, zone);
}

bool SkyRenderer::initWithZone(WorldZone* zone)
{
    AXASSERT(zone, "SkyRenderer can't be initialized with nullptr zone");

    if (!zone || !Parallaxer::init())
    {
        return false;
    }

    _zone = zone;

    // Create front batch node
    _batchNode = SpriteBatchNode::create("effects+hd2.png", 100);
    _batchNode->setCascadeColorEnabled(true);
    _batchNode->setCascadeOpacityEnabled(true);
    addChild(_batchNode, 15);

    // Create back batch node
    _backBatchNode = SpriteBatchNode::create("effects+hd2.png", 100);
    _backBatchNode->setCascadeColorEnabled(true);
    _backBatchNode->setCascadeOpacityEnabled(true);
    addChild(_backBatchNode, 5);
    return true;
}

void SkyRenderer::rebuild()
{
    Parallaxer::rebuild();
    auto biomeType = _zone->getBiomeType();
    auto biome     = biomeType == Biome::TEMPERATE ? "temperate" : _zone->getBiome();
    _rng.seed(_zone->getSeed());
    _texture = _director->getTextureCache()->addImage(std::format("biome-{}-background+hd2.png", biome));

    if (_texture)
    {
        // Create front biome batch node
        _biomeBatchNode = SpriteBatchNode::createWithTexture(_texture, 100);
        _biomeBatchNode->setCascadeColorEnabled(true);
        addChild(_biomeBatchNode, 20);

        // Create back biome batch node
        _biomeBackBatchNode = SpriteBatchNode::createWithTexture(_texture, 100);
        _biomeBackBatchNode->setCascadeColorEnabled(true);
        addChild(_biomeBackBatchNode, 10);

        // Create sprite frame data
        _mountainFrameMap   = createBackgroundFrameMap("mountain", "", 4);
        _hillFrameMap       = createBackgroundFrameMap("hill", "", 5);
        _hillAccentFrameMap = createBackgroundFrameMap("hill", "accent", 5);
        _dirtFrame =
            SpriteFrameCache::getInstance()->getSpriteFrameByName(std::format("background/{}-hill-tile", biome));

        // 0x1000C3D6D: Place mountains
        auto currentX = -MOUNTAIN_OFFSET * 2.0F;

        for (size_t i = 0; i < MOUNTAIN_COUNT; i++)
        {
            auto mountain = SkyMountain::createWithSky(this);
            mountain->setParallaxer(this);
            mountain->setCascadeColorEnabled(true);
            mountain->setRealPosition({currentX, biomeType == Biome::BRAIN ? -632.0F : -432.0F});  // -150 originally
            _biomeBackBatchNode->addChild(mountain, _rng.nextInt(3, 6));
            currentX += MOUNTAIN_OFFSET;
        }

        // 0x1000C3F52: Place sky rubble
        if (!_hillFrameMap.empty())
        {
            // Place close hills
            auto closeHill = SkyRubble::createWithSky(this);
            closeHill->setParallaxer(this);
            closeHill->setCascadeColorEnabled(true);
            closeHill->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
            _biomeBatchNode->addChild(closeHill, 80);
            closeHill->setRealPosition({0.0F, (closeHill->getRatio() * _zone->getSurfaceBottom() * -BLOCK_SIZE)});

            // Place far hills
            auto farHill = SkyRubble::createWithSky(this);
            farHill->setParallaxer(this);
            farHill->setCascadeColorEnabled(true);
            farHill->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
            farHill->setRealPosition({0.0F, closeHill->getRealPosition().y * 0.6F});
            farHill->setRealScale(closeHill->getRealScale() * 0.6F);
            _biomeBatchNode->addChild(farHill, 50);
        }
    }

    // 0x1000C3709: Place stars
    if (biomeType == Biome::SPACE)
    {
        // Place front stars
        for (size_t i = 0; i < FRONT_STAR_COUNT; i++)
        {
            // TODO range based on world size
            auto x     = _rng.maxf(12.0F) * _contentSize.width;
            auto y     = _rng.maxf(10.0F) * -_contentSize.height;
            auto frame = std::format("particles/diamond-{}", _rng.range(1, 4));
            auto star  = ParallaxSprite::createWithSpriteFrameName(frame);
            star->setParallaxer(this);
            star->setRealPosition({x, y});
            star->setRotation(_rng.maxf(360.0F));
            star->setRealScale(_rng.rangef(0.3F, 0.5F));
            star->setOpacity(_rng.range(200, 250));
            _batchNode->addChild(star, _rng.nextInt(17, 50));
        }

        // Place back stars
        for (size_t i = 0; i < BACK_STAR_COUNT; i++)
        {
            auto x     = _rng.maxf(_contentSize.width);
            auto y     = _rng.maxf(_contentSize.width);
            auto frame = std::format("particles/diamond-{}", _rng.range(1, 4));
            auto star  = Sprite::createWithSpriteFrameName(frame);
            star->setPosition(x, y);
            star->setRotation(_rng.maxf(360.0F));
            star->setScale(_rng.rangef(0.05F, 0.16F));
            star->setOpacity(_rng.range(200, 250));
            _backBatchNode->addChild(star);
        }
    }
    else
    {
        // 0x1000C3B71: Place clouds
        Rect cloudRect(-0.2F, 0.2F, 1.2F, 0.8F);  // 0 = bottom/left, 1 = top/right

        // Place front clouds
        for (size_t i = 0; i < FRONT_CLOUD_COUNT; i++)
        {
            addCloudInRect(cloudRect, false);
        }

        // Place back clouds
        for (size_t i = 0; i < BACK_CLOUD_COUNT; i++)
        {
            addCloudInRect(cloudRect, true);
        }
    }

    // 0x1000C4203: Load sky colors from biome config
    auto& config       = _zone->getBiomeConfig();
    auto& dayColors    = map_util::getArray(config, "sky.day color");
    auto& nightColors  = map_util::getArray(config, "sky.night color");
    _daySkyPureColor   = color_util::hexToColor(dayColors[0].asString());
    _daySkyAcidColor   = color_util::hexToColor(dayColors[1].asString());
    _nightSkyPureColor = color_util::hexToColor(nightColors[0].asString());
    _nightSkyAcidColor = color_util::hexToColor(nightColors[1].asString());
}

void SkyRenderer::clear()
{
    _batchNode->removeAllChildren();
    _backBatchNode->removeAllChildren();

    if (_biomeBatchNode)
    {
        _biomeBatchNode->removeFromParent();
        _biomeBatchNode = nullptr;
        _biomeBackBatchNode->removeFromParent();
        _biomeBackBatchNode = nullptr;
    }
}

void SkyRenderer::updateChildren(float deltaTime)
{
    Parallaxer::updateChildren(deltaTime);
    updateChildrenInNode(_batchNode, deltaTime);
    updateChildrenInNode(_backBatchNode, deltaTime);
    updateChildrenInNode(_biomeBatchNode, deltaTime);
    updateChildrenInNode(_biomeBackBatchNode, deltaTime);
}

void SkyRenderer::updateColors(float deltaTime)
{
    auto biome      = _zone->getBiomeType();
    auto acidity    = biome == Biome::TEMPERATE ? _zone->getAcidity() : 0;  // 0x1000C5002
    auto dayPercent = _zone->getDayPercent();
    auto cloudCover = _zone->getCloudCover();
    auto dayColor   = color_util::lerpColor(_daySkyPureColor, _daySkyAcidColor, acidity);
    auto nightColor = color_util::lerpColor(_nightSkyPureColor, _nightSkyAcidColor, acidity);
    auto skyColor   = color_util::lerpColor(nightColor, dayColor, dayPercent);

    // 0x1000C4C02: Update gradient colors
    auto startColor = color_util::saturate(skyColor, cloudCover * 0.33F);
    startColor      = color_util::lerpColor(startColor, Color3B::BLACK, 0.2F);
    auto endColor   = color_util::lerpColor(startColor, Color3B::BLACK, 0.5F);
    _gradient->setStartColor(startColor);
    _gradient->setEndColor(endColor);

    // TODO: supernova

    if (_biomeBatchNode)
    {
        // 0x1000C500E: Update biome colors
        auto biomeColor = color_util::lerpColor(Color3B::WHITE, dayColor, acidity);
        biomeColor      = color_util::lerpColor(nightColor, biomeColor, _zone->getDayPercent());
        _biomeBatchNode->setColor(biomeColor);
        _biomeBackBatchNode->setColor(biomeColor);

        // 0x1000C4F09: Update cloud colors
        auto cloudColor    = color_util::lerpColor(skyColor, Color3B::WHITE, dayPercent * 0.9F + 0.1F);
        auto farCloudColor = cloudColor;

        // 0x1000C4F62: Hard-coded biome rules
        if (biome == Biome::HELL)
        {
            cloudColor    = color_util::hexToColor("100000");
            farCloudColor = color_util::lerpColor(cloudColor, skyColor, 0.3F);
        }
        else if (biome == Biome::BRAIN)
        {
            // 0x1000C4FC1: It does something here but doesn't seem to use the result anywhere
        }

        _batchNode->setColor(cloudColor);
        _backBatchNode->setColor(farCloudColor);
    }
}

void SkyRenderer::addCloudInRect(const Rect& rect, bool back)
{
    auto x     = _rng.rangef(rect.getMinX() - 0.5F, rect.getMaxX() - 0.5F) * _contentSize.width;
    auto y     = _rng.rangef(rect.getMinY() - 0.5F, rect.getMaxY() - 0.5F) * _contentSize.height;
    auto z     = _rng.range(back ? 1 : 10, back ? 3 : 20);  // Front: 10-20, back: 1-3
    auto scale = MathUtil::lerp(0.5F, 1.0F, z / 100.0F) * 1.7F * (back ? 0.75F : 1.0F);
    auto batch = back ? _backBatchNode : _batchNode;
    auto cloud = SkyCloud::createWithSky(this);
    cloud->setParallaxer(this);
    cloud->setCascadeColorEnabled(true);
    cloud->setCascadeOpacityEnabled(true);
    cloud->setRealScale(scale);
    cloud->setRotation(_rng.rangef(0.0F, 360.0F));
    cloud->setOpacity(z + 200);
    batch->addChild(cloud, z);

    // Convert screen space to parallax space
    auto ratio = cloud->getRatio();
    auto size  = _contentSize * ratio * rect.size;
    auto max   = rect.origin + rect.size * _contentSize;
    cloud->setRealPosition({x / scale + _viewPosition.x * ratio, y / scale});
}

SkyRenderer::FrameMap SkyRenderer::createBackgroundFrameMap(const std::string& type,
                                                            const std::string& subType,
                                                            size_t count) const
{
    FrameMap result;
    auto cache = SpriteFrameCache::getInstance();
    auto biome = _zone->getBiomeType() == Biome::TEMPERATE ? "temperate" : _zone->getBiome();

    // Create frame lists 'a', 'b', 'c' and so forth
    for (int i = 0; i < count; i++)
    {
        Vector<SpriteFrame*> frames;
        char suffix      = 'a' + i;
        auto partialName = std::format("background/{}-{}-{}", biome, type, suffix);

        if (!subType.empty())
        {
            partialName += std::format("-{}", subType);
        }

        size_t current = 1;

        // Add frames '01', '02', '03' and so forth until it can't find the next one
        while (true)
        {
            auto number = current < 10 ? std::format("0{}", current) : std::to_string(current);
            auto frame  = cache->findFrame(std::format("{}-{}", partialName, number));

            if (!frame)
            {
                break;
            }

            frames.pushBack(frame);
            current++;
        }

        if (frames.empty())
        {
            AXLOGW("[SkyRenderer] No frames for {}", partialName);
        }

        result.push_back(frames);
    }

    return result;
}

}  // namespace opendw
