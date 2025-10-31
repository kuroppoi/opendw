#ifndef __MASKED_SPRITE_H__
#define __MASKED_SPRITE_H__

#include "axmol.h"

#include "graphics/backend/MaskedQuadCommand.h"

namespace opendw
{

class MaskedSpriteBatchNode;

enum class MaskOrientation : uint8_t
{
    DOWN,
    LEFT,
    UP,
    RIGHT
};

/*
 * CLASS: CCMaskedSprite : CCSprite @ 0x100318138
 *
 * Beware: hacky mess ahead.
 * If you have a better way of doing this, please let me know.
 */
class MaskedSprite : public ax::Sprite
{
public:

    //CREATE_FUNC(MaskedSprite);
    static MaskedSprite* createWithTexture(ax::Texture2D* texture, ax::Texture2D* maskTexture);

    //bool init() override;
    bool initWithTexture(ax::Texture2D* texture, ax::Texture2D* maskTexture);

    void draw(ax::Renderer* renderer, const ax::Mat4& transform, uint32_t flags) override;

    /* FUNC: CCMaskedSprite::updateMask @ 0x1000A31D */
    void updateMaskCoords();
    void updateTransform() override;
    void updateColor() override;

    void copyQuadToMaskedQuad();

    using Sprite::setTextureRect;
    void setTextureRect(const ax::Rect& rect, bool rotated, const ax::Vec2& untrimmedSize) override;

    /* FUNC: CCMaskedSprite::setMaskFrame @ 0x1000A30B1 */
    void setMaskFrame(std::string_view frameName);

    /* FUNC: CCMaskedSprite::setMaskRect @ 0x1000A3075 */
    void setMaskRect(const ax::Rect& rect);

    /* FUNC: CCMaskedSprite::maskRect @ 0x1000A34A4 */
    const ax::Rect& getMaskRect() const { return _maskRect; }

    /* FUNC: CCMaskedSprite::setMaskOrientation @ 0x1000A3144 */
    void setMaskOrientation(MaskOrientation orientation);

    /* FUNC: CCMaskedSprite::maskOrientation @ 0x1000A34D6 */
    MaskOrientation getMaskOrientation() const { return _maskOrientation; }

    ax::Texture2D* getMaskTexture() const { return _maskTexture; }

    const MaskedQuadCommand::Quad& getQuad() const { return _quad; }

    void setBatchNode(MaskedSpriteBatchNode* batchNode);
    // MaskedSpriteBatchNode* getBatchNode() const { return _batchNode; }

    void setBatchIndex(ssize_t index) { _batchIndex = index; }
    ssize_t getBatchIndex() const { return _batchIndex; }

private:
    ax::Texture2D* _maskTexture;
    ax::Rect _maskRect;                // CCMaskedSprite::maskRect @ 0x100312690
    MaskOrientation _maskOrientation;  // CCMaskedSprite::maskOrientation @ 0x100312680
    MaskedQuadCommand _quadCommand;
    MaskedQuadCommand::Quad _quad;
    MaskedSpriteBatchNode* _batchNode;
    ssize_t _batchIndex;
    bool _maskDirty;
};

}  // namespace opendw

#endif  // __MASKED_SPRITE_H__
