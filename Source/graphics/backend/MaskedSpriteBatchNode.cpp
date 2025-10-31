#include "MaskedSpriteBatchNode.h"

#include "graphics/backend/MaskedQuadBatch.h"
#include "graphics/backend/MaskedSprite.h"
#include "CommonDefs.h"

#define DEFAULT_CAPACITY 600

USING_NS_AX;

namespace opendw
{

MaskedSpriteBatchNode::~MaskedSpriteBatchNode()
{
    AX_SAFE_RELEASE(_quadBatch);
}

MaskedSpriteBatchNode* MaskedSpriteBatchNode::createWithTexture(Texture2D* texture,
                                                                Texture2D* maskTexture,
                                                                ssize_t capacity)
{
    CREATE_INIT(MaskedSpriteBatchNode, initWithTexture, texture, maskTexture, capacity);
}

bool MaskedSpriteBatchNode::initWithTexture(Texture2D* texture, Texture2D* maskTexture, ssize_t capacity)
{
    AXASSERT(texture, "Texture can't be nullptr");
    AXASSERT(maskTexture, "Mask texture can't be nullptr");

    if (!Node::init())
    {
        return false;
    }

    if (texture->hasPremultipliedAlpha() ^ maskTexture->hasPremultipliedAlpha())
    {
        AXLOGW("[MaskedSpriteBatchNode] Texture and mask texture alpha premultiplication do not match");
    }

    if (capacity <= 0)
    {
        capacity = DEFAULT_CAPACITY;
    }

    _texture     = texture;
    _maskTexture = maskTexture;
    _blendFunc = texture->hasPremultipliedAlpha() ? BlendFunc::ALPHA_PREMULTIPLIED : BlendFunc::ALPHA_NON_PREMULTIPLIED;
    _quadBatch = MaskedQuadBatch::createWithCapacity(capacity);
    AX_SAFE_RETAIN(_quadBatch);
    _children.reserve(capacity);
    _descendants.reserve(capacity);
    return true;
}

void MaskedSpriteBatchNode::visit(Renderer* renderer, const Mat4& parentTransform, uint32_t parentFlags)
{
    if (!_visible)
    {
        return;
    }

    sortAllChildren();
    uint32_t flags = processParentFlags(parentTransform, parentFlags);

    if (isVisitableByVisitingCamera())
    {
        _director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
        _director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, _modelViewTransform);
        draw(renderer, _modelViewTransform, flags);
        _director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
    }
}

void MaskedSpriteBatchNode::draw(Renderer* renderer, const Mat4& transform, uint32_t flags)
{
    auto totalQuads = _quadBatch->getTotalQuads();
    auto quads      = _quadBatch->getQuads();

    if (totalQuads == 0)
    {
        return;
    }

    for (const auto& child : _children)
    {
        child->updateTransform();
    }

    _quadCommand.init(_globalZOrder, _texture, _maskTexture, _blendFunc, quads, totalQuads, transform, flags);

    // Only update command buffers if quad batch is dirty
    if (_quadBatch->isDirty())
    {
        _quadCommand.populateBuffers();
        _quadBatch->setDirty(false);
    }

    renderer->addCommand(&_quadCommand);
}

void MaskedSpriteBatchNode::addChild(Node* child, int zOrder, int tag)
{
    auto sprite = dynamic_cast<MaskedSprite*>(child);
    AXASSERT(sprite, "Child must be of type MaskedSprite");
    AXASSERT(sprite->getTexture() == _texture && sprite->getMaskTexture() == _maskTexture,
             "Sprite textures must be the same as the batch node's");
    Node::addChild(child, zOrder, tag);
    insertSprite(sprite);
}

void MaskedSpriteBatchNode::addChild(Node* child, int zOrder, std::string_view name)
{
    auto sprite = dynamic_cast<MaskedSprite*>(child);
    AXASSERT(sprite, "Child must be of type MaskedSprite");
    AXASSERT(sprite->getTexture() == _texture && sprite->getMaskTexture() == _maskTexture,
             "Sprite textures must be the same as the batch node's");
    Node::addChild(child, zOrder, name);
    insertSprite(sprite);
}

void MaskedSpriteBatchNode::removeChild(Node* child, bool cleanup)
{
    AXASSERT(_children.contains(child), "Node must be a child of this batch");
    auto sprite = static_cast<MaskedSprite*>(child);
    removeSprite(sprite);
    Node::removeChild(child, cleanup);
}

void MaskedSpriteBatchNode::removeAllChildrenWithCleanup(bool cleanup)
{
    for (auto& sprite : _descendants)
    {
        sprite->setBatchNode(nullptr);
    }

    Node::removeAllChildrenWithCleanup(cleanup);
    _descendants.clear();
    _quadBatch->clear();
}

void MaskedSpriteBatchNode::updateSprite(MaskedSprite* sprite)
{
    _quadBatch->setQuad(sprite->getBatchIndex(), sprite->getQuad());
}

void MaskedSpriteBatchNode::insertSprite(MaskedSprite* sprite)
{
    _reorderChildDirty = true;
    sprite->setBatchNode(this);
    sprite->setDirty(true);
    auto capacity = _quadBatch->getCapacity();

    if (_quadBatch->getTotalQuads() == capacity)
    {
        setCapacity((capacity + 1) * 4 / 3);
    }

    auto index = static_cast<ssize_t>(_descendants.size());
    _descendants.push_back(sprite);
    sprite->setBatchIndex(index);
    auto&& quad = sprite->getQuad();
    _quadBatch->insertQuad(index, quad);

    // Recursively insert child sprites
    for (const auto& child : sprite->getChildren())
    {
        auto sprite = dynamic_cast<MaskedSprite*>(child);
        AX_ASSERT(sprite);
        insertSprite(sprite);
    }
}

void MaskedSpriteBatchNode::removeSprite(MaskedSprite* sprite)
{
    auto it = std::find(_descendants.begin(), _descendants.end(), sprite);
    AXASSERT(it != _descendants.end(), "Sprite must be a descendant of this batch");

    // Move all subsequent descendants one index to the left
    for (auto next = std::next(it); next != _descendants.end(); next++)
    {
        auto descendant = static_cast<MaskedSprite*>(*next);
        descendant->setBatchIndex(descendant->getBatchIndex() - 1);
    }

    _descendants.erase(it);
    _quadBatch->removeQuad(sprite->getBatchIndex());
    sprite->setBatchNode(nullptr);

    // Recursively remove child sprites
    for (auto& child : sprite->getChildren())
    {
        removeSprite(static_cast<MaskedSprite*>(child));
    }
}

void MaskedSpriteBatchNode::sortAllChildren()
{
    if (!_reorderChildDirty)
    {
        return;
    }

    if (!_children.empty())
    {
        sortNodes(_children);

        for (auto& child : _children)
        {
            child->sortAllChildren();
        }

        ssize_t currentIndex = 0;

        for (auto& child : _children)
        {
            auto sprite = static_cast<MaskedSprite*>(child);
            updateBatchIndices(sprite, &currentIndex);
        }
    }

    _reorderChildDirty = false;
}

void MaskedSpriteBatchNode::updateBatchIndices(MaskedSprite* sprite, ssize_t* currentIndex)
{
    auto& children = sprite->getChildren();

    if (children.empty())
    {
        auto oldIndex = sprite->getBatchIndex();

        if (oldIndex != *currentIndex)
        {
            swapBatchIndices(oldIndex, *currentIndex);
        }

        (*currentIndex)++;
        return;
    }

    bool needsNewIndex = true;

    if (children[0]->getLocalZOrder() >= 0)
    {
        auto oldIndex = sprite->getBatchIndex();

        if (oldIndex != *currentIndex)
        {
            swapBatchIndices(oldIndex, *currentIndex);
        }

        (*currentIndex)++;
        needsNewIndex = false;
    }

    for (auto& child : children)
    {
        if (needsNewIndex && child->getLocalZOrder() >= 0)
        {
            auto oldIndex = sprite->getBatchIndex();

            if (oldIndex != *currentIndex)
            {
                swapBatchIndices(oldIndex, *currentIndex);
            }

            (*currentIndex)++;
            needsNewIndex = false;
        }

        updateBatchIndices(static_cast<MaskedSprite*>(child), currentIndex);
    }

    if (needsNewIndex)
    {
        auto oldIndex = sprite->getBatchIndex();

        if (oldIndex != *currentIndex)
        {
            swapBatchIndices(oldIndex, *currentIndex);
        }

        (*currentIndex)++;
    }
}

void MaskedSpriteBatchNode::swapBatchIndices(ssize_t oldIndex, ssize_t newIndex)
{
    AXASSERT(oldIndex >= 0 && oldIndex < _descendants.size() && newIndex >= 0 && newIndex < _descendants.size(),
             "Index is out of bounds");
    auto quads = _quadBatch->getQuads();
    std::swap(quads[oldIndex], quads[newIndex]);
    _quadBatch->setDirty(true);
    auto oldIt = std::next(_descendants.begin(), oldIndex);
    auto newIt = std::next(_descendants.begin(), newIndex);
    (*newIt)->setBatchIndex(oldIndex);
    (*oldIt)->setBatchIndex(newIndex);
    std::swap(*oldIt, *newIt);
}

bool MaskedSpriteBatchNode::setCapacity(ssize_t capacity)
{
    auto currentCapacity = _quadBatch->getCapacity();

    if (capacity == currentCapacity)
    {
        return true;
    }

    AXLOGI("[MaskedSpriteBatchNode] Resizing MaskedQuadBatch capacity from {} to {}", currentCapacity, capacity);
    return _quadBatch->setCapacity(capacity);
}

}  // namespace opendw
