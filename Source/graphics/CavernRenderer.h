#ifndef __CAVERN_RENDERER_H__
#define __CAVERN_RENDERER_H__

#include "axmol.h"

#include "graphics/Parallaxer.h"

namespace opendw
{

class ParallaxSprite;
class WorldZone;

/*
 * CLASS: CavernRenderer : Parallaxer @ 0x10031CDC8
 */
class CavernRenderer : public Parallaxer
{
public:
    static CavernRenderer* createWithZone(WorldZone* zone);

    bool initWithZone(WorldZone* zone);

    /* FUNC: CavernRenderer::rebuild @ 0x10014F1D5 */
    void rebuild() override;

    /* FUNC: CavernRenderer::stepChildren: @ 0x100150237 */
    void updateChildren(float deltaTime) override;

    /* FUNC: CavernRenderer::updateColors: @ 0x10014F419 */
    void updateColors(float deltaTime) override;

    /* FUNC: CavernRenderer::setTopColor:bottomColor: @ 0x10014F3C9 */
    void setColors(const ax::Color3B& topColor, const ax::Color3B& bottomColor);

    /* FUNC: CavernRenderer::setContentSize: @ 0x10014F9F4 */
    void setContentSize(const ax::Size& contentSize) override;

private:
    ParallaxSprite* _cavernFar;       // CavernRenderer::cavernFar @ 0x100314968
    ParallaxSprite* _cavernClose;     // CavernRenderer::cavernClose @ 0x100314978
    ax::Color3B _cavernCloseColor;    // CavernRenderer::cavernCloseColor @ 0x100314970
    ax::Color3B _shallowTopColor;     // CavernRenderer::shallowTopColor @ 0x100314980
    ax::Color3B _shallowBottomColor;  // CavernRenderer::shallowBottomColor @ 0x100314988
    ax::Color3B _deepTopColor;        // CavernRenderer::deepTopColor @ 0x100314990
    ax::Color3B _deepBottomColor;     // CavernRenderer::deepBottomColor @ 0x100314998
    WorldZone* _zone;
};

}  // namespace opendw

#endif  // __CAVERN_RENDERER_H__
