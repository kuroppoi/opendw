#include "SkyMountain.h"

#include "graphics/SkyRenderer.h"
#include "zone/WorldZone.h"
#include "CommonDefs.h"

USING_NS_AX;

namespace opendw
{

SkyMountain* SkyMountain::createWithSky(SkyRenderer* sky)
{
    CREATE_INIT(SkyMountain, initWithSky, sky);
}

bool SkyMountain::initWithSky(SkyRenderer* sky)
{
    if (!initWithTexture(sky->getTexture(), Rect::ZERO))
    {
        return false;
    }

    _sky = sky;
    return true;
}

void SkyMountain::onEnter()
{
    ParallaxSprite::onEnter();
    auto& frameMap = _sky->getMountainFrameMap();

    if (frameMap.empty())
    {
        return;
    }

    auto& rng  = _sky->getRNG();
    auto biome = _sky->getZone()->getBiomeType();
    auto type  = rng.max(frameMap.size() - 1);

    // Place base mountain sprites
    if (biome != Biome::BRAIN)
    {
        auto& baseFrames = frameMap[0];
        auto currentX    = 0.0F;

        for (auto frame : baseFrames)
        {
            auto sprite = Sprite::createWithSpriteFrame(frame);
            sprite->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
            sprite->setPositionX(currentX);
            currentX += sprite->getContentSize().width;
            addChild(sprite);
        }

        // Do nothing else if type == base
        if (!type)
        {
            return;
        }
    }

    // Place variant mountain sprites
    auto& frames  = frameMap[type];
    auto currentX = 0.0F;
    float height  = 0.0F;

    for (auto frame : frames)
    {
        auto sprite = Sprite::createWithSpriteFrame(frame);
        sprite->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
        sprite->setPositionX(currentX);
        auto& size = sprite->getContentSize();
        currentX += size.width;
        height = MAX(height, size.height);
        addChild(sprite);
    }

    // Place mountain top sprites
    if (biome == Biome::BRAIN)
    {
        char suffix    = 'a' + type;
        auto frameName = std::format("background/brain-mountain-{}-top", suffix);
        auto topFrame  = SpriteFrameCache::getInstance()->getSpriteFrameByName(frameName);

        if (topFrame)
        {
            for (auto i = 0; i < 4; i++)
            {
                auto sprite = Sprite::createWithSpriteFrame(topFrame);
                sprite->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
                sprite->setPositionY(height + sprite->getContentSize().height * i);
                addChild(sprite);
            }
        }
    }
}

}  // namespace opendw
