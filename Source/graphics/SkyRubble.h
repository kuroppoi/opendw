#ifndef __SKY_RUBBLE_H__
#define __SKY_RUBBLE_H__

#include "graphics/ParallaxSprite.h"

namespace opendw
{

class SkyRenderer;

/*
 * CLASS: SkyRubble : ParallaxSprite @ 0x100318778
 */
class SkyRubble : public ParallaxSprite
{
public:
    static SkyRubble* createWithSky(SkyRenderer* sky);

    /* FUNC: SkyRubble::initWithSky: @ 0x1000C63BE */
    bool initWithSky(SkyRenderer* sky);

    /* FUNC: SkyRubble::onEnter @ 0x1000C64EB */
    void onEnter() override;

private:
    SkyRenderer* _sky;  // SkyRubble::sky @ 0x100312AD0
};

}  // namespace opendw

#endif  // __SKY_RUBBLE_H__
