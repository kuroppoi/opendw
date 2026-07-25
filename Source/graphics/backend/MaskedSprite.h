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

class MaskedSprite : public ax::Sprite
{
public:
    static MaskedSprite* createWithTexture(ax::Texture2D* texture, ax::Texture2D* maskTexture);

    bool initWithTexture(ax::Texture2D* texture, ax::Texture2D* maskTexture);

    virtual void draw(ax::Renderer* renderer, const ax::Mat4& transform, uint32_t flags) override;

    virtual void updateTransform() override;

    void updateMaskCoords();
    void copyQuadToMaskedQuad();

    ax::Texture2D* getMaskTexture() const { return _maskTexture; }

    void setMaskFrame(std::string_view frameName);
    void setMaskRect(const ax::Rect& rect);
    const ax::Rect& getMaskRect() const { return _maskRect; }

    void setMaskOrientation(MaskOrientation orientation);
    MaskOrientation getMaskOrientation() const { return _maskOrientation; }

    const MaskedQuadCommand::Quad& getMaskedQuad() const { return _maskedQuad; }

    void setMaskedBatchNode(MaskedSpriteBatchNode* batchNode);
    MaskedSpriteBatchNode* getMaskedBatchNode() const { return _maskedBatchNode; }

    void setMaskedBatchIndex(ssize_t index) { _maskedBatchIndex = index; }
    ssize_t getMaskedBatchIndex() const { return _maskedBatchIndex; }
    
protected:
    virtual void updateColor() override;
    virtual void setTextureCoords(const ax::Rect& rect, ax::V3F_C4B_T2F_Quad* outQuad) override;

    ax::Texture2D* _maskTexture;
    ax::Rect _maskRect;
    MaskOrientation _maskOrientation;
    MaskedQuadCommand _maskedQuadCommand;
    MaskedQuadCommand::Quad _maskedQuad;
    MaskedSpriteBatchNode* _maskedBatchNode;
    ssize_t _maskedBatchIndex;
    bool _maskDirty;
};

}  // namespace opendw

#endif  // __MASKED_SPRITE_H__
