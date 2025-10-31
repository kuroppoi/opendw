#ifndef __WORLD_RENDERER_H__
#define __WORLD_RENDERER_H__

#include "axmol.h"

namespace opendw
{

class BaseBlock;
class CavernRenderer;
class SkyRenderer;
class WorldLayerRenderer;
class WorldZone;
enum class BlockLayer : uint8_t;

/*
 * CLASS: WorldRenderer : CCNode @ 0x1003176C0
 *
 * The world renderer is responsible for displaying all components of a zone.
 */
class WorldRenderer : public ax::Node
{
public:
    typedef std::vector<std::vector<uint16_t>> CornerMasks;

    static WorldRenderer* createWithZone(WorldZone* zone);

    /* FUNC: WorldRenderer::initWithWorldZone: @ 0x10007CE2D */
    bool initWithZone(WorldZone* zone);

    /* FUNC: WorldRenderer::ready @ 0x10007E3F9 */
    void ready();

    /* FUNC: WorldRenderer::clear @ 0x100086A09 */
    void clear();

    /* FUNC: WorldRenderer::step: @ 0x10007EA32 */
    void update(float deltaTime) override;

    /* FUNC: WorldRenderer::stepBlocks @ 0x10007FF3B */
    void updateBlocks();

    /* FUNC: WorldRenderer::loadBiome: @ 0x100080126 */
    void loadBiome(const std::string& biome);

    /* FUNC: WorldRenderer::arrangeBlockSprites @ 0x100080495 */
    void arrangeBlockSprites();

    /* FUNC: WorldRenderer::renderBlockSprites @ 0x1000808D3 */
    void renderBlockSprites();

    /* FUNC: WorldRenderer::processEffects @ 0x100080E11 */
    void processEffects();

    /* FUNC: WorldRenderer::updateLiquidInBlock: @ 0x100082164 */
    void updateLiquidInBlock(BaseBlock* block);

    /* FUNC: WorldRenderer::updateViewport: @ 0x100082DE3 */
    void updateViewport(float deltaTime);

    /* FUNC: WorldRenderer::viewportPosition @ 0x100082EC5 */
    ax::Point getViewportPosition() const;

    /* FUNC: WorldRenderer::layerRenderer:withLayer:texture:z: @ 0x10007E28D */
    WorldLayerRenderer* createLayerRenderer(const std::string& name,
                                            BlockLayer layer,
                                            const std::string& texture,
                                            bool biomeRenderer = false,
                                            int z              = -1);

    /* FUNC: WorldRenderer::nextZIndex @ 0x10007E39C */
    int getNextZIndex() const;

    /* FUNC: WorldRenderer::isBlockRendered: @ 0x100081B63 */
    bool isBlockInViewport(BaseBlock* block) const;

    /* FUNC: WorldRenderer::queueBlockForRendering: @ 0x100081C10 */
    void queueBlockForRendering(BaseBlock* block);

    /* FUNC: WorldRenderer::hasRenderedAllPlacedBlocks @ 0x100081DAD */
    bool hasRenderedAllPlacedBlocks();

    /* FUNC: WorldRenderer::setWorldScale: @ 0x100082B33 */
    void setWorldScale(float scale);

    /* FUNC: WorldRenderer::worldScale @ 0x100086DD9 */
    float getWorldScale() const { return _worldScale; }

    /* FUNC: WorldRenderer::nodePointForScreenPoint: @ 0x1000830CB */
    ax::Point getNodePointForScreenPoint(const ax::Point& point) const;

    /* FUNC: WorldLayerRenderer::baseAccentsNode @ 0x100086E4D */
    WorldLayerRenderer* getBaseAccentsNode() const { return _baseAccentsNode; }

    /* FUNC: WorldRenderer::backAccentsNode @ 0x100086E5E */
    WorldLayerRenderer* getBackAccentsNode() const { return _backAccentsNode; }

    /* FUNC: WorldRenderer::frontAccentsNode @ 0x100086E6F */
    WorldLayerRenderer* getFronterAccentsNode() const { return _fronterAccentsNode; }

    /* FUNC: WorldRenderer::wholenessCornerMasks @ 0x100086F19 */
    const CornerMasks& getWholenessCornerMasks() const { return _wholenessCornerMasks; }

    /* FUNC: WorldRenderer::continuityCornerMasks @ 0x100086F2A */
    const CornerMasks& getContinuityCornerMasks() const { return _continuityCornerMasks; }

private:
    WorldZone* _zone;                             // WorldRenderer::zone @ 0x100311DE8
    ax::Node* _background;                        // WorldRenderer::background @ 0x100311E30
    ax::Node* _foreground;                        // WorldRenderer::foreground @ 0x100311E48
    CavernRenderer* _cavern;                      // WorldRenderer::cavern @ 0x100311E38
    SkyRenderer* _sky;                            // WorldRenderer::sky @ 0x100311E40
    WorldLayerRenderer* _baseBiomeBlocksNode;     // WorldRenderer::baseBiomeBlocksNode @ 0x100311E50
    WorldLayerRenderer* _baseBlocksNode;          // WorldRenderer::baseBlocksNode @ 0x100311E58
    WorldLayerRenderer* _baseAccentsNode;         // WorldRenderer::baseAccentsNode @ 0x100311E60
    WorldLayerRenderer* _backBiomeBlocksNode;     // WorldRenderer::backBiomeBlocksNode @ 0x100311E68
    WorldLayerRenderer* _backBlocksNode;          // WorldRenderer::backBlocksNode @ 0x100311E70
    WorldLayerRenderer* _backAccentsNode;         // WorldRenderer::backAccentsNode @ 0x100311E78
    WorldLayerRenderer* _frontBiomeBlocksNode;    // WorldRenderer::frontBiomeBlocksNode @ 0x100311E88
    WorldLayerRenderer* _frontBlocksNode;         // WorldRenderer::frontBlocksNode @ 0x100311E80
    WorldLayerRenderer* _signsNode;               // WorldRenderer::signsNode @ 0x100311E90
    WorldLayerRenderer* _frontQualityBlocksNode;  // WorldRenderer::frontQualityBlocksNode @ 0x100311E98
    WorldLayerRenderer* _liquidBlocksNode;        // WorldRenderer::liquidBlocksNode @ 0x100311EC8
    WorldLayerRenderer* _fronterBiomeBlocksNode;  // WorldRenderer::fronterBiomeBlocksNode @ 0x100311ED0
    WorldLayerRenderer* _fronterBlocksNode;       // WorldRenderer::fronterBlocksNode @ 0x100311ED8
    WorldLayerRenderer* _fronterAccentsNode;      // WorldRenderer::frontAccentsNode @ 0x100311EE0
    ax::Vector<BaseBlock*> _renderQueue;          // WorldRenderer::renderQueue @ 0x100311DF8
    ax::Rect _lastArrangeRect;                    // WorldRenderer::lastArrangeRect @ 0x100311FF8
    ax::Rect _blockRect;                          // WorldRenderer::blockRect @ 0x100312008
    float _worldScale;                            // WorldRenderer::worldScale @ 0x100311E20
    CornerMasks _wholenessCornerMasks;            // WorldRenderer::wholenessCornerMasks @ 0x100311F38
    CornerMasks _continuityCornerMasks;           // WorldRenderer::continuityCornerMasks @ 0x100311F40
    double _nextFX;                               // WorldRenderer::nextFX @ 0x100311F90
    size_t _fxFrame;                              // WorldRenderer::fxFrame @ 0x100311F50
    double _nextLiquidCycle;                      // WorldRenderer::nextLiquidCycle @ 0x100311F98
    size_t _liquidFrame;                          // 0x100320BC0
};

}  // namespace opendw

#endif  // __WORLD_RENDERER_H__
