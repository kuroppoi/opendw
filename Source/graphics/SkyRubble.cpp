#include "SkyRubble.h"

#include "graphics/SkyRenderer.h"
#include "zone/WorldZone.h"
#include "CommonDefs.h"

USING_NS_AX;

namespace opendw
{

SkyRubble* SkyRubble::createWithSky(SkyRenderer* sky)
{
    CREATE_INIT(SkyRubble, initWithSky, sky);
}

bool SkyRubble::initWithSky(SkyRenderer* sky)
{
    if (!Sprite::initWithTexture(sky->getTexture(), Rect::ZERO))
    {
        return false;
    }

    _sky = sky;
    return true;
}

void SkyRubble::onEnter()
{
    ParallaxSprite::onEnter();
    auto& frameMap = _sky->getHillFrameMap();

    if (frameMap.empty())
    {
        return;
    }

    auto& winSize        = _director->getWinSize();
    auto& accentFrameMap = _sky->getHillAccentFrameMap();
    auto dirtFrame       = _sky->getDirtFrame();
    auto texture         = _sky->getTexture();
    auto& rng            = _sky->getRNG();
    auto biome           = _sky->getZone()->getBiomeType();
    auto blocksWidth     = _sky->getZone()->getBlocksWidth();
    auto drawWidth       = blocksWidth * BLOCK_SIZE * getRatio() / _realScale;

    // Create dirt sprites
    if (dirtFrame)
    {
        float offset = biome == Biome::ARCTIC ? 0.5F : biome == Biome::DESERT ? 0.333F : 0.25F;
        auto& size   = dirtFrame->getOriginalSize();

        for (auto x = 0.0F; x < drawWidth; x += size.width)
        {
            for (auto i = 0; i < 2; i++)
            {
                auto sprite = Sprite::createWithSpriteFrame(dirtFrame);
                sprite->setAnchorPoint(Point::ANCHOR_TOP_LEFT);
                sprite->setPosition(x, size.height * offset + i * -size.height);
                addChild(sprite);
            }
        }
    }

    // Create hills
    for (auto x = 0.0F; x < drawWidth;)
    {
        auto type     = rng.max(frameMap.size() - 1);
        auto& frames  = frameMap[type];
        auto currentX = -5.0F;

        // Place individual hill component sprites
        for (auto frame : frames)
        {
            auto sprite = Sprite::createWithSpriteFrame(frame);
            sprite->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
            sprite->setPositionX(x + currentX);
            currentX += sprite->getContentSize().width;
            addChild(sprite, 1);
        }

        // Failsafe: break loop if X position is still zero to prevent an infinite loop
        if (currentX <= FLT_EPSILON)
        {
            AXLOGW("[SkyRubble] Hill has a total width of 0 -- aborting!");
            break;
        }

        // Try to place a random hill accent sprite
        if (type < accentFrameMap.size())
        {
            auto& accentFrames = accentFrameMap[type];
            auto accent        = rng.max(accentFrames.size());  // Don't subtract 1

            // Small chance to not have an accent
            if (accent < accentFrames.size())
            {
                auto sprite = Sprite::createWithSpriteFrame(accentFrames[accent]);
                sprite->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
                sprite->setPositionX(x);
                addChild(sprite, 2);
            }
        }

        x += currentX;
    }
}

}  // namespace opendw
