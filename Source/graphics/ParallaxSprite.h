#ifndef __PARALLAX_SPRITE_H__
#define __PARALLAX_SPRITE_H__

#include "axmol.h"

namespace opendw
{

class Parallaxer;

/*
 * CLASS: ParallaxSprite : CCSprite @ 0x10031CF08
 */
class ParallaxSprite : public ax::Sprite
{
public:
    static ParallaxSprite* createWithTexture(ax::Texture2D* texture);
    static ParallaxSprite* createWithSpriteFrameName(std::string_view frameName);

    /* FUNC: ParallaxSprite::parallaxify @ 0x1001508F8 */
    virtual void parallaxify(float deltaTime);

    /* FUNC: ParallaxSprite::positionForOffset: @ 0x10015093E */
    ax::Point getPositionForOffset(const ax::Point& offset) const;
    float getScaleForZoom(float zoom) const;

    /* FUNC: ParallaxSprite::ratio @ 0x100150984 */
    float getRatio() const;

    /* FUNC: ParallaxSprite::setParallaxer: @ 0x100150A02 */
    void setParallaxer(Parallaxer* parallaxer) { _parallaxer = parallaxer; }

    /* FUNC: ParallaxSprite::setRealPosition: @ 0x100150A2B */
    void setRealPosition(const ax::Point& position) { _realPosition = position; }

    /* FUNC: ParallaxSprite::realPosition @ 0x100150A13 */
    const ax::Point& getRealPosition() const { return _realPosition; }

    void setRealScale(float scale) { _realScale = scale; }
    float getRealScale() const { return _realScale; }

protected:
    Parallaxer* _parallaxer;  // ParallaxSprite::parallaxer @ 0x1003144A08
    ax::Point _realPosition;  // ParallaxSprite::realPosition @ 0x100314A10
    float _realScale = 1.0F;
};

}  // namespace opendw

#endif  // __PARALLAX_SPRITE_H__
