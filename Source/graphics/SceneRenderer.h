#ifndef __SCENE_RENDERER_H__
#define __SCENE_RENDERER_H__

#include "axmol.h"

namespace opendw
{

class GameGui;
class WorldRenderer;
class WorldZone;

/*
 * CLASS: SceneRenderer : CCNode @ 0x100317670
 *
 * The scene renderer is responsible for displaying the current zone and in-game GUI.
 */
class SceneRenderer : public ax::Node
{
public:
    static SceneRenderer* createWithZone(WorldZone* zone);

    /* FUNC: SceneRenderer::initWithWorldZone: @ 0x10007BEED */
    bool initWithZone(WorldZone* zone);

    /* FUNC: SceneRenderer::step: @ 0x10007C29E */
    void update(float deltaTime) override;

    /* FUNC: SceneRenderer::ready @ 0x10007C255 */
    void ready();

    /* FUNC: SceneRenderer::clear @ 0x10007C7A3 */
    void clear();

    /* FUNC: SceneRenderer::showSpinner @ 0x10007C420 */
    void showSpinner();

    /* FUNC: SceneRenderer::hideSpinner @ 0x10007C61C */
    void hideSpinner();

    /* FUNC: SceneRenderer::worldRenderer @ 0x10007C8D4 */
    WorldRenderer* getWorldRenderer() const { return _worldRenderer; }

private:
    WorldRenderer* _worldRenderer;  // SceneRenderer::worldRenderer @ 0x100311D98
    GameGui* _gameGui;              // SceneRenderer::gameGui @ 0x100311DA0
    ax::Node* _spinner;             // SceneRenderer::spinner @ 0x100311DE0
};

}  // namespace opendw

#endif  // __SCENE_RENDERER_H__
