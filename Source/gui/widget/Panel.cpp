#include "Panel.h"

#include "util/AxUtil.h"
#include "util/MathUtil.h"
#include "CommonDefs.h"

USING_NS_AX;

static constexpr Vec2 kBorderOffsets[8] = {{0.0F, 1.0F}, {1.0F, 0.0F},  {0.0F, -1.0F},  {-1.0F, 0.0F},
                                           {1.0F, 1.0F}, {1.0F, -1.0F}, {-1.0F, -1.0F}, {-1.0F, 1.0F}};

namespace opendw
{
Panel* Panel::createWithStyle(const std::string& style)
{
    CREATE_INIT(Panel, initWithStyle, style);
}

bool Panel::initWithStyle(const std::string& style)
{
    if (!Node::init())
    {
        return false;
    }

    _batch = SpriteBatchNode::create("guiv2.png");
    _batch->setCascadeOpacityEnabled(true);
    addChild(_batch, 1);
    setStyle(style);
    setTip(Edge::NONE);
    setChop(Edge::NONE);
    setBorderScale(1.0F);
    setSize(0.0F, 0.0F);  // Auto set to minimum size
    setCascadeOpacityEnabled(true);
    return true;
}

void Panel::onEnter()
{
    Node::onEnter();

    // Create touch listener
    _touchListener = EventListenerTouchOneByOne::create();
    _touchListener->setSwallowTouches(true);
    _touchListener->onTouchBegan = AX_CALLBACK_2(Panel::onTouchBegan, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(_touchListener, this);
}

void Panel::onExit()
{
    _eventDispatcher->removeEventListener(_touchListener);
    Node::onExit();
}

void Panel::visit(Renderer* renderer, const Mat4& transform, uint32_t flags)
{
    if (isVisible() && _layoutDirty)
    {
        updateLayout();
    }

    Node::visit(renderer, transform, flags);
}

void Panel::updateLayout()
{
    auto& cornerSize     = _borderSprites[static_cast<int>(Border::TOP_RIGHT)]->getContentSize();
    auto& backgroundSize = _backgroundSprite->getContentSize();
    auto borderScale     = _borderScale * cornerSize.width * _baseBorderScale;

    // Update background
    _backgroundSprite->setScaleX((_contentSize.width - borderScale * 2.0F) / backgroundSize.width);
    _backgroundSprite->setScaleY((_contentSize.height - borderScale * 2.0F) / backgroundSize.height);
    _backgroundSprite->setPosition(borderScale, borderScale);

    // Update background texture
    if (_backgroundTextureSprite)
    {
        auto padding = Vec2::ONE * ((_borderScale * 13.0F) * 2.0F);
        auto rect    = Rect(Point::ZERO, _contentSize - padding);
        _backgroundTextureSprite->setPosition(padding * 0.5F);
        _backgroundTextureSprite->setTextureRect(rect);
    }

    // Update borders
    for (auto i = 0; i < 8; i++)
    {
        updateBorder(static_cast<Border>(i), borderScale);
    }

    // Update tip visibility
    if (_tipSprite && _tip.edge == Edge::NONE)
    {
        _tipSprite->setVisible(false);
        _tipBorderSprite->setVisible(false);
    }

    _layoutDirty = false;
}

void Panel::updateBorder(Border border, float scale)
{
    auto index       = static_cast<int>(border) & 7;
    auto sprite      = _borderSprites[index];
    auto& spriteSize = sprite->getContentSize();
    auto tipIndex    = static_cast<int>(_tip.edge) - 1;

    // Find out whether to hide this border based on chop value
    if (_chop != Edge::NONE)
    {
        auto chopIndex = static_cast<int>(_chop) - 1;
        auto cornerA   = _chop == Edge::TOP ? 4 : 3;
        auto cornerB   = _chop == Edge::TOP ? 7 : 4;

        if (index == chopIndex || index == chopIndex + cornerA || index == chopIndex + cornerB)
        {
            sprite->setVisible(false);

            if (index == tipIndex)
            {
                _tipSprite->setVisible(false);
                _tipBorderSprite->setVisible(false);
            }

            return;
        }
    }

    auto vertical = (index | 2) == 3;

    // Update scale, position & rotation
    if (border >= Border::TOP_RIGHT)
    {
        sprite->setScale(scale / spriteSize.width);
    }
    else
    {
        if (index == tipIndex)
        {
            auto tipWidth = _tipSprite->getContentSize().width * _borderScale * _baseBorderScale;
            auto scaleX =
                (scale * -2.0F - tipWidth + (vertical ? _contentSize.height : _contentSize.width)) / spriteSize.width;
            sprite->setScaleX(scaleX * _tip.position);
            _tipBorderSprite->setScaleX(scaleX * (1.0F - _tip.position));
            _tipBorderSprite->setScaleY(scale / spriteSize.height);
        }
        else
        {
            auto scaleX = (scale * -2.0F + (vertical ? _contentSize.height : _contentSize.width)) / spriteSize.width;
            sprite->setScaleX(scaleX);
        }

        sprite->setScaleY(scale / spriteSize.height);
    }

    auto& offset  = kBorderOffsets[index];
    auto center   = _contentSize * 0.5F;
    auto position = center + offset * (center - Vec2::ONE * scale * 0.5F);
    auto rotation = (index % 4) * 90.0F;
    sprite->setPosition(position);
    sprite->setRotation(rotation);

    // Update tip
    if (index == tipIndex)
    {
        auto tipPosition = center + offset * center;

        if (vertical)
        {
            auto tipOffset = math_util::lerp(scale * 2.0F, _contentSize.height - scale * 2.0F, _tip.position);
            sprite->setPositionY(tipOffset * 0.5F);
            _tipSprite->setPosition(tipPosition.x, tipOffset);
            _tipBorderSprite->setPosition(position.x, (tipOffset + _contentSize.height) * 0.5F);
        }
        else
        {
            auto tipOffset = math_util::lerp(scale * 2.0F, _contentSize.width - scale * 2.0F, _tip.position);
            sprite->setPositionX(tipOffset * 0.5F);
            _tipSprite->setPosition(tipOffset, tipPosition.y);
            _tipBorderSprite->setPosition((tipOffset + _contentSize.width) * 0.5F, position.y);
        }

        _tipSprite->setRotation(rotation + 180.0F);
        _tipSprite->setScale(sprite->getScaleY());
        _tipSprite->setVisible(true);
        _tipBorderSprite->setRotation(rotation);
        _tipBorderSprite->setVisible(true);
    }
}

Sprite* Panel::createBorderSprite(const std::string& name)
{
    auto frame  = std::format("panels/{}-{}", _style, name);
    auto sprite = Sprite::createWithSpriteFrameName(frame);
    _batch->addChild(sprite);
    return sprite;
}

void Panel::setStyle(const std::string& style)
{
    if (_style == style)
    {
        return;
    }

    _style = style;
    _batch->removeAllChildren();

    for (auto i = 0; i < 8; i++)
    {
        auto border       = static_cast<Border>(i);
        auto name         = border >= Border::TOP_RIGHT ? "corner" : "top";
        _borderSprites[i] = createBorderSprite(name);
    }

    _backgroundSprite = createBorderSprite("background");
    _backgroundSprite->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);

    if (auto frame = SpriteFrameCache::getInstance()->findFrame(std::format("panels/{}-tip", _style)))
    {
        _tipSprite = Sprite::createWithSpriteFrame(frame);
        _batch->addChild(_tipSprite);
        _tipBorderSprite = createBorderSprite("top");
    }

    _baseBorderScale = style.ends_with("brass") ? 0.65F : 1.0F;  // 0x1000E7FB2
    _layoutDirty     = true;
}

void Panel::setBackgroundTexture(const std::string& textureFile, uint8_t opacity)
{
    removeChild(_backgroundTextureSprite);

    if (textureFile.empty())
    {
        return;
    }

    auto texture = _director->getTextureCache()->addImage(textureFile);
    AX_ASSERT(texture);
    texture->setTexParameters({backend::SamplerFilter::LINEAR, backend::SamplerFilter::LINEAR,
                               backend::SamplerAddressMode::REPEAT, backend::SamplerAddressMode::REPEAT});
    _backgroundTextureSprite = Sprite::createWithTexture(texture);
    _backgroundTextureSprite->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
    _backgroundTextureSprite->setOpacity(opacity);
    addChild(_backgroundTextureSprite, 0);
    _layoutDirty = true;  // Let updateLayout set position & size
}

void Panel::setTip(Edge edge, float position)
{
    if (!_tipSprite)
    {
        edge = Edge::NONE;  // Don't allow setting if style has no tip sprite
    }

    auto clamped = clampf(position, 0.0F, 1.0F);

    if (_tip.edge != edge || _tip.position != clamped)
    {
        _tip.edge     = edge;
        _tip.position = clamped;
        _layoutDirty  = true;
    }
}

void Panel::anchorToTip()
{
    if (_tip.edge != Edge::NONE)
    {
        if (_layoutDirty)
        {
            updateLayout();  // Force layout update
        }

        switch (_tip.edge)
        {
        case Edge::TOP:
            setAnchorPoint({_tipSprite->getPositionX() / _contentSize.width, 1.0F});
            break;
        case Edge::RIGHT:
            setAnchorPoint({1.0F, _tipSprite->getPositionY() / _contentSize.height});
            break;
        case Edge::BOTTOM:
            setAnchorPoint({_tipSprite->getPositionX() / _contentSize.width, 0.0F});
            break;
        case Edge::LEFT:
            setAnchorPoint({0.0F, _tipSprite->getPositionY() / _contentSize.height});
            break;
        }
    }
}

void Panel::setChop(Edge edge)
{
    if (_chop != edge)
    {
        _chop        = edge;
        _layoutDirty = true;
    }
}

void Panel::setBorderScale(float scale)
{
    if (_borderScale != scale)
    {
        _borderScale = scale;
        _layoutDirty = true;
    }
}

void Panel::setContentSize(const Size& contentSize)
{
    Node::setContentSize(contentSize);
    _layoutDirty = true;
}

void Panel::setSize(float width, float height, bool force)
{
    if (force)
    {
        setContentSize({width, height});
        return;
    }

    // Calculate minimum size
    auto& cornerSize = _borderSprites[static_cast<int>(Border::TOP_RIGHT)]->getContentSize();
    auto borderScale = _borderScale * _baseBorderScale;
    auto minWidth    = borderScale * cornerSize.width * 2.0F;
    auto minHeight   = borderScale * cornerSize.height * 2.0F;

    if (_tip.edge != Edge::NONE)
    {
        auto& tipSize = _tipSprite->getContentSize();

        switch (_tip.edge)
        {
        case Edge::BOTTOM:
        case Edge::TOP:
            minWidth += tipSize.width;
            break;
        default:
            minHeight += tipSize.width;
        }
    }

    width  = MAX(minWidth, width);
    height = MAX(minHeight, height);
    setContentSize({width, height});
}

bool Panel::onTouchBegan(Touch* touch, Event* event)
{
    if (!ax_util::isNodeVisible(this))
    {
        return false;
    }

    auto location = touch->getLocation();
    auto rect     = Rect(Point::ZERO, _contentSize);

    if (isScreenPointInRect(location, Camera::getVisitingCamera(), getWorldToNodeTransform(), rect, nullptr))
    {
        if (onPointerDown(touch))
        {
            event->stopPropagation();
            return true;
        }
    }

    return false;
}

}  // namespace opendw
