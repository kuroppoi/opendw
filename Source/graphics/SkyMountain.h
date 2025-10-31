#ifndef __SKY_MOUNTAIN_H__
#define __SKY_MOUNTAIN_H__

#include "graphics/ParallaxSprite.h"

namespace opendw
{

class SkyRenderer;

/*
 * CLASS: SkyMountain : ParallaxSprite @ 0x100318750 
 */
class SkyMountain : public ParallaxSprite
{
public:
    static SkyMountain* createWithSky(SkyRenderer* sky);

    /* FUNC: SkyMountain::initWithSky: @ 0x1000C5CC0 */
    bool initWithSky(SkyRenderer* sky);

    /* FUNC: SkyMountain::onEnter @ 0x1000C5DCA */
    void onEnter() override;

private:
    SkyRenderer* _sky;  // SkyMountain::_sky @ 0x100312AB8
};

}  // namespace opendw

#endif  // __SKY_MOUNTAIN_H__
