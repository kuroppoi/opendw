#include "ParallaxSprite.h"

#include "graphics/Parallaxer.h"
#include "CommonDefs.h"

USING_NS_AX;

namespace opendw
{

ParallaxSprite* ParallaxSprite::createWithTexture(Texture2D* texture)
{
    CREATE_INIT(ParallaxSprite, initWithTexture, texture);
}

ParallaxSprite* ParallaxSprite::createWithSpriteFrameName(std::string_view frameName)
{
    CREATE_INIT(ParallaxSprite, initWithSpriteFrameName, frameName);
}

void ParallaxSprite::parallaxify(float deltaTime)
{
    if (!_parallaxer)
    {
        return;
    }

    auto& winSize      = _director->getWinSize();
    auto& viewPosition = _parallaxer->getViewPosition();
    auto viewScale     = _parallaxer->getViewScale();
    setScale(getScaleForZoom(viewScale));
    auto appliedScale = getScale() / _realScale;
    setPosition(getPositionForOffset(-viewPosition) * appliedScale + winSize * 0.5F);
}

Point ParallaxSprite::getPositionForOffset(const Point& offset) const
{
    return _realPosition + offset * getRatio();
}

float ParallaxSprite::getScaleForZoom(float zoom) const
{
    // TODO: multiplying by 4 is a bit of a hack
    auto ratio = getRatio() * 4.0F;
    auto scale = _realScale - ((_realScale - zoom) * ratio);
    return _realScale * scale;
}

float ParallaxSprite::getRatio() const
{
    return MathUtil::lerp(0.00005F, 0.125F, _localZOrder / 100.0F);
}

}  // namespace opendw
