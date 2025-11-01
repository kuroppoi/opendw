#ifndef __WORLD_LAYER_RENDERER_H__
#define __WORLD_LAYER_RENDERER_H__

#include "axmol.h"

#include "Item.h"  // ContinuitySpriteList

namespace opendw
{

class BaseBlock;
class MaskedSprite;
class MaskedSpriteBatchNode;
class WorldRenderer;
class WorldZone;
enum class BlockLayer : uint8_t;
enum class MaskOrientation : uint8_t;

/*
 * CLASS: WorldLayerRenderer : CCNode @ 0x100318188
 *
 * The meat of the world renderer. Responsible for rendering all of the blocks in a zone.
 * Note: parts of the implementation have been customized due to how complex and inflexible the original is.
 */
class WorldLayerRenderer : public ax::Node
{
public:
    /* FUNC: WorldLayerRenderer::dealloc @ 0x1000AB9EB */
    ~WorldLayerRenderer() override;

    static WorldLayerRenderer* createWithLayer(BlockLayer layer, MaskedSpriteBatchNode* batchNode);

    /* FUNC: WorldLayerRenderer::initWithLayer:batchNode: @ 0x1000A3508 */
    bool initWithLayer(BlockLayer layer, MaskedSpriteBatchNode* batchNode);

    /* FUNC: WorldLayerRenderer::clear @ 0x1000AB7FD */
    void clear();

    /* FUNC: WorldLayerRenderer::specialPlaceBlock:item: @ 0x1000A9014 */
    void placeSpecialItem(BaseBlock* block, Item* item);

    /* FUNC: WorldLayerRenderer::specialPlaceUniqueBlock:item: @ 0x1000AA2D4 */
    void placeUniqueItem(BaseBlock* block, Item* item);

    /* FUNC: WorldLayerRenderer::placeBlock: @ 0x1000A3A05 */
    void placeBlock(BaseBlock* block);

    /* FUNC: WorldLayerRenderer::placeBlock:sprite:item:mod:z: @ 0x1000A6F2D */
    void placeItem(BaseBlock* block, Item* item, uint8_t mod);

    /* FUNC: WorldLayerRenderer::placeBlock:border:color:opacity: @ 0x1000A8C82 */
    void placeBorder(BaseBlock* block,
                     const Item::ContinuitySpriteList& border,
                     const ax::Color3B& color = ax::Color3B::WHITE,
                     uint8_t opacity          = 0xFF);

    /* FUNC: WorldLayerRenderer::placeBlock:border:corners:color:opacity: @ 0x1000A8E34 */
    void placeCorners(BaseBlock* block,
                      const Item::ContinuitySpriteList& border,
                      const std::vector<uint16_t>& corners,
                      const ax::Color3B& color = ax::Color3B::WHITE,
                      uint8_t opacity          = 0xFF);

    /* FUNC: WorldLayerRenderer::placeBlock:sprite:spriteCode:tileable:alignment:modType:mod:z:push: @ 0x1000A748B */
    MaskedSprite* placeSprite(BaseBlock* block,
                              MaskedSprite* sprite,
                              ax::SpriteFrame* frame,
                              bool tileable,
                              bool aligned,
                              ModType modType = ModType::NONE,
                              uint16_t mod    = 0,
                              int z           = 0,
                              bool push       = true);

    /* FUNC: WorldLayerRenderer::freshSprite: @ 0x1000AB4FC */
    MaskedSprite* getNextSprite();

    /* FUNC: WorldLayerRenderer::freshSpriteMasked:oriented:z: @ 0x1000AB72C */
    MaskedSprite* getNextSprite(const ax::Rect& maskRect, MaskOrientation maskOrientation);

    void recycleSprite(MaskedSprite* sprite);

    /* FUNC: WorldLayerRenderer::addAltRenderer: @ 0x1000A3971 */
    void addAltRenderer(WorldLayerRenderer* renderer) { _altRenderers.push_back(renderer); }

    /* FUNC: WorldLayerRenderer::setPlaceBackgroundsInAlt: @ 0x1000ABB45 */
    void setPlaceBackgroundsInAlt(bool value) { _placeBackgroundsInAlt = value; }

    /* FUNC: WorldLayerRenderer::setIsBiomeRenderer: @ 0x1000ABB25 */
    void setBiomeRenderer(bool value) { _biomeRenderer = value; }

    /* FUNC: WorldLayerRenderer::isBiomeRenderer @ 0x1000ABB15 */
    bool isBiomeRenderer() const { return _biomeRenderer; }

    /* FUNC: WorldLayerRenderer::layer @ 0x1000ABAB5 */
    BlockLayer getLayer() const { return _layer; }

    /* FUNC: WorldLayerRenderer::batchNode @ 0x1000ABAC5 */
    MaskedSpriteBatchNode* getBatchNode() const { return _batchNode; }

    size_t getRecycledSpriteCount() { return _recycledSprites.size(); }

    // Sprite tags
    static constexpr auto ANIMATED_SPRITE_TAG = 0x2F;
    static constexpr auto ACTION_SPRITE_TAG   = 0x30;

private:
    inline static size_t sTotalSpriteCount;  // 0x10032EB28

    BlockLayer _layer;                               // WorldLayerRenderer::layer @ 0x100312698
    MaskedSpriteBatchNode* _batchNode;               // WorldLayerRenderer::batchNode @ 0x1003126A0
    WorldZone* _zone;                                // WorldLayerRenderer::zone @ 0x1003126B8
    ax::Vector<MaskedSprite*> _recycledSprites;      // WorldLayerRenderer::recycledSprites @ 0x1003126A8
    std::vector<WorldLayerRenderer*> _altRenderers;  // WorldLayerRenderer::altRenderers @ 0x100312708
    bool _placeBackgroundsInAlt;                     // WorldLayerRenderer::placeBackgroundsInAlt @ 0x100312710
    bool _biomeRenderer;                             // WorldLayerRenderer::isBiomeRenderer @ 0x100312718
    ax::SpriteFrame* _shadowShallowFrame;            // WorldLayerRenderer::shadowShallowCode @ 0x1003126E8
    ax::SpriteFrame* _shadowDeepFrame;               // WorldLayerRenderer::shadowDeepCode @ 0x1003126F0
    ax::Rect _shadowDeepSideMask;                    // WorldLayerRenderer::shdowDeepSideMask @ 0x1003126F8
    ax::Rect _zeroMask;                              // WorldLayerRenderer::zeroMask @ 0x1003126C8
};

}  // namespace opendw

#endif  // __WORLD_LAYER_RENDERER_H__
