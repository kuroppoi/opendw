#include "SkyCloud.h"

#include "graphics/Parallaxer.h"
#include "graphics/SkyRenderer.h"
#include "CommonDefs.h"

USING_NS_AX;

namespace opendw
{

SkyCloud* SkyCloud::createWithSky(SkyRenderer* sky)
{
    CREATE_INIT(SkyCloud, initWithSky, sky);
}

bool SkyCloud::initWithSky(SkyRenderer* sky)
{
    if (!initWithSpriteFrameName("particles/steam-2"))
    {
        return false;
    }

    _sky = sky;
    setTextureRect(Rect::ZERO);
    return true;
}

void SkyCloud::onEnter()
{
    ParallaxSprite::onEnter();
    auto& rng     = _sky->getRNG();
    auto amount   = rng.range(8, 33);
    auto size     = rng.rangef(0.25, 0.5F) * _spriteFrame->getOriginalSize().width;
    auto angle    = rng.rangef(-0.5F, 0.5F) + M_PI;
    auto coverage = rng.rangef(0.4F, 1.3F);
    auto offsetX  = 0.0F;
    auto offsetY  = 0.0F;

    for (auto i = 0; i < amount; i++)
    {
        auto frame  = std::format("particles/steam-{}", rng.nextInt(2, 4));
        auto sprite = Sprite::createWithSpriteFrameName(frame);
        auto x      = rng.rangef(-size, size);
        auto y      = rng.rangef(-size, size);
        offsetX += cosf(angle) * size * coverage;
        offsetY += sinf(angle) * size * 0.25F;
        angle += 0.2F;
        sprite->setPosition(x + offsetX, y + offsetY);
        sprite->setRotation(rng.maxf(360.0F));
        sprite->setScale(rng.rangef(1.5F, 2.0F));
        addChild(sprite);
    }

    setContentSize({amount * size, amount * size * 0.5F});
}

void SkyCloud::parallaxify(float deltaTime)
{
    auto& rng        = _sky->getRNG();
    auto& winSize    = _director->getWinSize();
    auto cloudEdge   = _contentSize.width * -1.0F + _position.x;
    auto screenStart = -winSize.width + _parallaxer->getViewPosition().x * getRatio();
    auto screenEnd   = winSize.width / _parent->getScale();

    if (cloudEdge > screenEnd)
    {
        // Recycle cloud if it has drifted off-screen
        auto offsetX = rng.rangef(0.5F, 3.0F) * _contentSize.width * getRatio();
        setRealPosition({screenStart - offsetX, _realPosition.y});
    }
    else
    {
        // Slightly move the cloud towards the east
        auto x = _realPosition.x + _localZOrder * deltaTime * 0.333F;
        setRealPosition({x, _realPosition.y});
    }

    ParallaxSprite::parallaxify(deltaTime);

    // Slightly move the individual components of the cloud
    for (auto& child : _children)
    {
        auto time      = utils::getTimeInMilliseconds() / 1000.0F;
        auto intensity = sinf(time * 0.4F) * deltaTime * 3.0F;
        auto rotation  = child->getRotation() + deltaTime;
        auto x         = child->getPositionX() + cosf(rotation) * intensity;
        auto y         = child->getPositionY() + sinf(rotation) * intensity;
        child->setRotation(rotation);
        child->setPosition(x, y);
    }
}

}  // namespace opendw
