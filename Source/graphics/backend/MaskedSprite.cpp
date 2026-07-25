#include "MaskedSprite.h"

#include "graphics/backend/MaskedQuadBatch.h"
#include "graphics/backend/MaskedSpriteBatchNode.h"
#include "CommonDefs.h"

#define EMPTY_MASK   "masks/opaque"
#define DEFAULT_MASK EMPTY_MASK

USING_NS_AX;

namespace opendw
{

MaskedSprite* MaskedSprite::createWithTexture(Texture2D* texture, Texture2D* maskTexture)
{
    CREATE_INIT(MaskedSprite, initWithTexture, texture, maskTexture);
}

bool MaskedSprite::initWithTexture(Texture2D* texture, Texture2D* maskTexture)
{
    AX_ASSERT(maskTexture);

    if (!Sprite::initWithTexture(texture))
    {
        return false;
    }

    _maskTexture = maskTexture;
    _renderMode  = RenderMode::QUAD;
    setMaskFrame(DEFAULT_MASK);
    return true;
}

void MaskedSprite::draw(Renderer* renderer, const Mat4& transform, uint32_t flags)
{
    if (_renderMode == RenderMode::QUAD_BATCHNODE)
    {
        return;
    }

    setMVPMatrixUniform();

#if AX_USE_CULLING
    // No need to optimize: there likely won't be a lot of self-drawing MaskedSprites.
    _insideBounds = renderer->checkVisibility(transform, _contentSize);

    if (!_insideBounds)
    {
        return;
    }
#endif

    updateColor();
    copyQuadToMaskedQuad();

    if (_maskDirty)
    {
        updateMaskCoords();
        _maskDirty = false;
    }


    _maskedQuadCommand.init(_globalZOrder, _texture, _maskTexture, _blendFunc, &_maskedQuad, 1, transform, flags);
    _maskedQuadCommand.populateBuffers();
    renderer->addCommand(&_maskedQuadCommand);
}

void MaskedSprite::updateTransform()
{
    if (_renderMode != RenderMode::QUAD_BATCHNODE)
    {
        return;
    }

    if (!_maskedBatchNode)
    {
        Sprite::updateTransform();  // Delegate to superclass
        return;
    }

    auto updateInBatch = false;

    // Copied from Sprite with a few changed/additions to support masking
    if (isDirty())
    {
        if (!_visible ||
            (_parent && _parent != _maskedBatchNode && static_cast<MaskedSprite*>(_parent)->_shouldBeHidden))
        {
            _quad.br.vertices.setZero();
            _quad.tl.vertices.setZero();
            _quad.tr.vertices.setZero();
            _quad.bl.vertices.setZero();
            _shouldBeHidden = true;
        }
        else
        {
            _shouldBeHidden = false;

            if (!_parent || _parent == _maskedBatchNode)
            {
                _transformToBatch = getNodeToParentTransform();
            }
            else
            {
                AXASSERT(dynamic_cast<MaskedSprite*>(_parent),
                         "Logic error in MaskedSprite. Parent must be a MaskedSprite");
                const Mat4& nodeToParent = getNodeToParentTransform();
                Mat4& parentTransform    = static_cast<MaskedSprite*>(_parent)->_transformToBatch;
                _transformToBatch        = parentTransform * nodeToParent;
            }

            Vec2& size = _rect.size;
            float x1   = _offsetPosition.x;
            float y1   = _offsetPosition.y;
            float x2   = x1 + size.width;
            float y2   = y1 + size.height;
            float x    = _transformToBatch.m[12];
            float y    = _transformToBatch.m[13];
            float cr   = _transformToBatch.m[0];
            float sr   = _transformToBatch.m[1];
            float cr2  = _transformToBatch.m[5];
            float sr2  = -_transformToBatch.m[4];
            float ax   = x1 * cr - y1 * sr2 + x;
            float ay   = x1 * sr + y1 * cr2 + y;
            float bx   = x2 * cr - y1 * sr2 + x;
            float by   = x2 * sr + y1 * cr2 + y;
            float cx   = x2 * cr - y2 * sr2 + x;
            float cy   = x2 * sr + y2 * cr2 + y;
            float dx   = x1 * cr - y2 * sr2 + x;
            float dy   = x1 * sr + y2 * cr2 + y;
            _quad.bl.vertices.set(SPRITE_RENDER_IN_SUBPIXEL(ax), SPRITE_RENDER_IN_SUBPIXEL(ay), _positionZ);
            _quad.br.vertices.set(SPRITE_RENDER_IN_SUBPIXEL(bx), SPRITE_RENDER_IN_SUBPIXEL(by), _positionZ);
            _quad.tl.vertices.set(SPRITE_RENDER_IN_SUBPIXEL(dx), SPRITE_RENDER_IN_SUBPIXEL(dy), _positionZ);
            _quad.tr.vertices.set(SPRITE_RENDER_IN_SUBPIXEL(cx), SPRITE_RENDER_IN_SUBPIXEL(cy), _positionZ);
            setTextureCoords(_rect, &_quad);
        }

        copyQuadToMaskedQuad();
        updateInBatch   = true;
        _recursiveDirty = false;
        setDirty(false);
    }

    if (_maskDirty)
    {
        updateMaskCoords();
        _maskDirty    = false;
        updateInBatch = true;
    }

    if (updateInBatch)
    {
        _maskedBatchNode->updateSprite(this);
    }

    Node::updateTransform();
}

void MaskedSprite::updateMaskCoords()
{
    if (_renderMode == RenderMode::QUAD_BATCHNODE && !_maskedBatchNode)
    {
        return;  // We're in a regular SpriteBatchNode, so do nothing.
    }

    auto maskTexture        = _maskedBatchNode ? _maskedBatchNode->getMaskTexture() : _maskTexture;
    const auto& textureSize = maskTexture->getContentSize();

    auto tl = _maskRect.origin / textureSize;     // Top left
    auto br = _maskRect.size / textureSize + tl;  // Bottom right
    Tex2F tr(br.x, tl.y);                         // Top right
    Tex2F bl(tl.x, br.y);                         // Bottom left

    switch (_maskOrientation)
    {
    case MaskOrientation::LEFT:
        _maskedQuad.tl.maskCoord = bl;
        _maskedQuad.br.maskCoord = tr;
        _maskedQuad.tr.maskCoord = tl;
        _maskedQuad.bl.maskCoord = br;
        break;
    case MaskOrientation::UP:
        _maskedQuad.tl.maskCoord = br;
        _maskedQuad.br.maskCoord = tl;
        _maskedQuad.tr.maskCoord = bl;
        _maskedQuad.bl.maskCoord = tr;
        break;
    case MaskOrientation::RIGHT:
        _maskedQuad.tl.maskCoord = tr;
        _maskedQuad.br.maskCoord = bl;
        _maskedQuad.tr.maskCoord = br;
        _maskedQuad.bl.maskCoord = tl;
        break;
    case MaskOrientation::DOWN:
    default:
        _maskedQuad.tl.maskCoord = tl;
        _maskedQuad.br.maskCoord = br;
        _maskedQuad.tr.maskCoord = tr;
        _maskedQuad.bl.maskCoord = bl;
        break;
    }
}

void MaskedSprite::copyQuadToMaskedQuad()
{
    auto vertexSize = sizeof(V3F_C4B_T2F);
    memcpy(&_maskedQuad.tl, &_quad.tl, vertexSize);
    memcpy(&_maskedQuad.tr, &_quad.tr, vertexSize);
    memcpy(&_maskedQuad.bl, &_quad.bl, vertexSize);
    memcpy(&_maskedQuad.br, &_quad.br, vertexSize);
}

void MaskedSprite::setMaskFrame(std::string_view frameName)
{
    auto frame = SpriteFrameCache::getInstance()->findFrame(frameName);

    if (!frame)
    {
        AXLOGW("[MaskedSprite] No frame for {}", frameName);
        setMaskRect(Rect::ZERO);
        return;
    }

    setMaskRect(frame->getRect());
}

void MaskedSprite::setMaskRect(const Rect& rect)
{
    if (!_maskRect.equals(rect))
    {
        _maskRect  = rect;
        _maskDirty = true;
    }
}

void MaskedSprite::setMaskOrientation(MaskOrientation orientation)
{
    if (_maskOrientation != orientation)
    {
        _maskOrientation = orientation;
        _maskDirty       = true;
    }
}

void MaskedSprite::setMaskedBatchNode(MaskedSpriteBatchNode* batchNode)
{
    _maskedBatchNode = batchNode;  // Weak ref

    if (batchNode)
    {
        _renderMode       = RenderMode::QUAD_BATCHNODE;
        _transformToBatch = Mat4::IDENTITY;
    }
    else
    {
        _renderMode       = RenderMode::QUAD;
        _maskedBatchIndex = INDEX_NOT_INITIALIZED;
        _recursiveDirty   = false;
        setDirty(false);
        float x1 = _offsetPosition.x;
        float y1 = _offsetPosition.y;
        float x2 = x1 + _rect.size.width;
        float y2 = y1 + _rect.size.height;
        _quad.bl.vertices.set(x1, y1, 0);
        _quad.br.vertices.set(x2, y1, 0);
        _quad.tl.vertices.set(x1, y2, 0);
        _quad.tr.vertices.set(x2, y2, 0);
    }
}

void MaskedSprite::updateColor()
{
    if (!_maskedBatchNode)
    {
        Sprite::updateColor();  // Delegate to superclass
        return;
    }

    Color4B color(_displayedColor.r, _displayedColor.g, _displayedColor.b, _displayedOpacity);

    if (_opacityModifyRGB)
    {
        color.r *= _displayedOpacity / 255.0F;
        color.g *= _displayedOpacity / 255.0F;
        color.b *= _displayedOpacity / 255.0F;
    }

    _quad.tl.colors = color;
    _quad.bl.colors = color;
    _quad.tr.colors = color;
    _quad.br.colors = color;

    if (_maskedBatchIndex != INDEX_NOT_INITIALIZED)
    {
        copyQuadToMaskedQuad();
        _maskedBatchNode->updateSprite(this);
    }
    else
    {
        setDirty(true);
    }
}

void MaskedSprite::setTextureCoords(const ax::Rect& rect, ax::V3F_C4B_T2F_Quad* outQuad)
{
    if (!_maskedBatchNode)
    {
        Sprite::setTextureCoords(rect, outQuad);  // Delegate to superclass
        return;
    }

    // This is a very stupid hack, but it works!
    auto batched = _renderMode == RenderMode::QUAD_BATCHNODE;
    auto texture = _texture;

    if (batched)
    {
        _renderMode = RenderMode::QUAD;
        _texture    = _maskedBatchNode->getTexture();
    }

    Sprite::setTextureCoords(rect, outQuad);

    if (batched)
    {
        _renderMode = RenderMode::QUAD_BATCHNODE;
        _texture    = texture;
    }
}

}  // namespace opendw
