#ifndef __SKY_CLOUD_H__
#define __SKY_CLOUD_H__

#include "graphics/ParallaxSprite.h"

namespace opendw
{

class SkyRenderer;

/*
 * CLASS: SkyCloud : ParallaxSprite @ 0x1003187A0
 */
class SkyCloud : public ParallaxSprite
{
public:
    static SkyCloud* createWithSky(SkyRenderer* sky);

    bool initWithSky(SkyRenderer* sky);

    /* FUNC: SkyCloud::onEnter @ 0x1000C5573 */
    void onEnter() override;

    /* FUNC: SkyCloud::parallaxify: @ 0x1000C593C */
    void parallaxify(float deltaTime) override;

private:
    SkyRenderer* _sky;
};

}  // namespace opendw

#endif  // __SKY_CLOUD_H__
