#ifndef __MASKED_SPRITE_BATCH_NODE_H__
#define __MASKED_SPRITE_BATCH_NODE_H__

#include "axmol.h"

#include "graphics/backend/MaskedQuadCommand.h"

namespace opendw
{

class MaskedSprite;
class MaskedQuadBatch;

/*
 * CLASS: CCSpriteBatchNodeMasked : CCSpriteBatchNode @ 0x100318098
 */
class MaskedSpriteBatchNode : public ax::Node, ax::TextureProtocol
{
public:
    ~MaskedSpriteBatchNode() override;

    static MaskedSpriteBatchNode* createWithTexture(ax::Texture2D* texture,
                                                    ax::Texture2D* maskTexture,
                                                    ssize_t capacity = 0);

    /* FUNC: CCSpriteBatchNodeMasked::initWithTexture:capacity: @ 0x1000A2660 */
    bool initWithTexture(ax::Texture2D* texture, ax::Texture2D* maskTexture, ssize_t capacity = 0);

    void visit(ax::Renderer* renderer, const ax::Mat4& transform, uint32_t flags) override;
    void draw(ax::Renderer* renderer, const ax::Mat4& transform, uint32_t flags) override;

    using Node::addChild;
    void addChild(ax::Node* child, int zOrder, int tag) override;
    void addChild(ax::Node* child, int zOrder, std::string_view name) override;
    void removeChild(ax::Node* child, bool cleanup = true) override;
    void removeAllChildrenWithCleanup(bool cleanup = true) override;
    void sortAllChildren() override;

    bool setCapacity(ssize_t capacity);

    void setTexture(ax::Texture2D* texture) override { _texture = texture; }
    ax::Texture2D* getTexture() const override { return _texture; }

    /* FUNC: CCSpriteBatchNodeMasked::setMaskTexture: @ 0x1000A28C0*/
    void setMaskTexture(ax::Texture2D* texture) { _maskTexture = texture; }
    ax::Texture2D* getMaskTexture() const { return _maskTexture; }

    void setBlendFunc(const ax::BlendFunc& blendFunc) override { _blendFunc = blendFunc; }
    const ax::BlendFunc& getBlendFunc() const override { return _blendFunc; }

private:
    friend class MaskedSprite;

    void updateSprite(MaskedSprite* sprite);
    void insertSprite(MaskedSprite* sprite);
    void removeSprite(MaskedSprite* sprite);

    void updateBatchIndices(MaskedSprite* sprite, ssize_t* currentIndex);
    void swapBatchIndices(ssize_t oldIndex, ssize_t newIndex);

    ax::Texture2D* _texture;
    ax::Texture2D* _maskTexture;
    ax::BlendFunc _blendFunc;
    MaskedQuadBatch* _quadBatch;
    MaskedQuadCommand _quadCommand;
    std::vector<MaskedSprite*> _descendants;
};

}  // namespace opendw

#endif  // __MASKED_SPRITE_BATCH_NODE_H__
