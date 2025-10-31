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

void MaskedSprite::updateTransform()
{
    // TODO: clean up
    AXASSERT(_batchNode, "Calling updateTransform is only valid in a batching context");

    if (isDirty())
    {
        if (!_visible || (_parent && _parent != _batchNode && static_cast<MaskedSprite*>(_parent)->_shouldBeHidden))
        {
            _quad.br.position.setZero();
            _quad.tl.position.setZero();
            _quad.tr.position.setZero();
            _quad.bl.position.setZero();
            _shouldBeHidden = true;
        }
        else
        {
            _shouldBeHidden = false;

            if (!_parent || _parent == _batchNode)
            {
                _transformToBatch = getNodeToParentTransform();
            }
            else
            {
                AXASSERT(dynamic_cast<MaskedSprite*>(_parent), "Logic error in Sprite. Parent must be a Sprite");
                const Mat4& nodeToParent = getNodeToParentTransform();
                Mat4& parentTransform    = static_cast<MaskedSprite*>(_parent)->_transformToBatch;
                _transformToBatch        = parentTransform * nodeToParent;
            }

            // Update vertex positions
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
            _quad.bl.position.set(SPRITE_RENDER_IN_SUBPIXEL(ax), SPRITE_RENDER_IN_SUBPIXEL(ay), _positionZ);
            _quad.br.position.set(SPRITE_RENDER_IN_SUBPIXEL(bx), SPRITE_RENDER_IN_SUBPIXEL(by), _positionZ);
            _quad.tl.position.set(SPRITE_RENDER_IN_SUBPIXEL(dx), SPRITE_RENDER_IN_SUBPIXEL(dy), _positionZ);
            _quad.tr.position.set(SPRITE_RENDER_IN_SUBPIXEL(cx), SPRITE_RENDER_IN_SUBPIXEL(cy), _positionZ);

            // Update texture coordinates
            setTextureCoords(_rect);
            _quad.bl.texCoord = Sprite::_quad.bl.texCoords;
            _quad.br.texCoord = Sprite::_quad.br.texCoords;
            _quad.tl.texCoord = Sprite::_quad.tl.texCoords;
            _quad.tr.texCoord = Sprite::_quad.tr.texCoords;
        }

        if (_maskDirty)
        {
            updateMaskCoords();
            _maskDirty = false;
        }

        _batchNode->updateSprite(this);
        _recursiveDirty = false;
        setDirty(false);
    }

    Node::updateTransform();
}

void MaskedSprite::updateColor()
{
    Color4B color(_displayedColor.r, _displayedColor.g, _displayedColor.b, _displayedOpacity);

    if (_opacityModifyRGB)
    {
        color.r *= _displayedOpacity / 255.0F;
        color.g *= _displayedOpacity / 255.0F;
        color.b *= _displayedOpacity / 255.0F;
    }

    _quad.tl.color = color;
    _quad.bl.color = color;
    _quad.tr.color = color;
    _quad.br.color = color;

    if (_batchNode)
    {
        if (_batchIndex != INDEX_NOT_INITIALIZED)
        {
            _batchNode->updateSprite(this);
        }
        else
        {
            setDirty(true);
        }
    }
}

void MaskedSprite::draw(Renderer* renderer, const Mat4& transform, uint32_t flags)
{
    // Don't self-draw if batch node is present
    if (_batchNode)
    {
        return;
    }

    setMVPMatrixUniform();

#if AX_USE_CULLING
    // No need to optimize: there likely won't be a lot self-drawing MaskedSprites.
    _insideBounds = renderer->checkVisibility(transform, _contentSize);

    if (!_insideBounds)
    {
        return;
    }
#endif

    copyQuadToMaskedQuad();

    if (_maskDirty)
    {
        updateMaskCoords();
        _maskDirty = false;
    }

    _quadCommand.init(_globalZOrder, _texture, _maskTexture, _blendFunc, &_quad, 1, transform, flags);
    _quadCommand.populateBuffers();
    renderer->addCommand(&_quadCommand);
}

void MaskedSprite::copyQuadToMaskedQuad()
{
    // Yes, this is a hack. Yes, it sucks.
    auto vertexSize = sizeof(V3F_C4B_T2F);
    memcpy(&_quad.tl, &Sprite::_quad.tl, vertexSize);
    memcpy(&_quad.tr, &Sprite::_quad.tr, vertexSize);
    memcpy(&_quad.bl, &Sprite::_quad.bl, vertexSize);
    memcpy(&_quad.br, &Sprite::_quad.br, vertexSize);
}

void MaskedSprite::updateMaskCoords()
{
    auto maskTexture        = _batchNode ? _batchNode->getMaskTexture() : _maskTexture;
    const auto& textureSize = maskTexture->getContentSize();

    auto tl = _maskRect.origin / textureSize;     // Top left
    auto br = _maskRect.size / textureSize + tl;  // Bottom right
    Tex2F tr(br.x, tl.y);                         // Top right
    Tex2F bl(tl.x, br.y);                         // Bottom left

    switch (_maskOrientation)
    {
    case MaskOrientation::LEFT:
        _quad.tl.maskCoord = bl;
        _quad.br.maskCoord = tr;
        _quad.tr.maskCoord = tl;
        _quad.bl.maskCoord = br;
        break;
    case MaskOrientation::UP:
        _quad.tl.maskCoord = br;
        _quad.br.maskCoord = tl;
        _quad.tr.maskCoord = bl;
        _quad.bl.maskCoord = tr;
        break;
    case MaskOrientation::RIGHT:
        _quad.tl.maskCoord = tr;
        _quad.br.maskCoord = bl;
        _quad.tr.maskCoord = br;
        _quad.bl.maskCoord = tl;
        break;
    default:  // Assume down
        _quad.tl.maskCoord = tl;
        _quad.br.maskCoord = br;
        _quad.tr.maskCoord = tr;
        _quad.bl.maskCoord = bl;
        break;
    }
}

void MaskedSprite::setTextureRect(const Rect& rect, bool rotated, const Vec2& untrimmedSize)
{
    Sprite::setTextureRect(rect, rotated, untrimmedSize);
    _quad.bl.texCoord = Sprite::_quad.bl.texCoords;
    _quad.br.texCoord = Sprite::_quad.br.texCoords;
    _quad.tl.texCoord = Sprite::_quad.tl.texCoords;
    _quad.tr.texCoord = Sprite::_quad.tr.texCoords;
    if (_batchNode)
    {
        if (_batchIndex != INDEX_NOT_INITIALIZED)
        {
            _batchNode->updateSprite(this);
        }
        else
        {
            setDirty(true);
        }
    }
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
    _maskRect  = rect;
    _maskDirty = true;
}

void MaskedSprite::setMaskOrientation(MaskOrientation orientation)
{
    _maskOrientation = orientation;
    _maskDirty       = true;
}

void MaskedSprite::setBatchNode(MaskedSpriteBatchNode* batchNode)
{
    _batchNode = batchNode;

    if (!batchNode)
    {
        _batchIndex = INDEX_NOT_INITIALIZED;
        return;
    }

    _transformToBatch = Mat4::IDENTITY;
}

}  // namespace opendw
