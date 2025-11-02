#include "Panel.h"

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
    addChild(_batch);
    setStyle(style);
    setChop(Chop::NONE);
    setBorderScale(1.0F);
    setContentSize(Size::ONE * 100.0F);  // Default size
    setCascadeOpacityEnabled(true);
    return true;
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
    auto& cornerSize     = _borderSprites[static_cast<uint8_t>(Border::TOP_RIGHT)]->getContentSize();
    auto& backgroundSize = _backgroundSprite->getContentSize();
    auto borderScale     = _borderScale * cornerSize.width * _baseBorderScale;

    // Update background
    _backgroundSprite->setScaleX((_contentSize.width - borderScale * 2.0F) / backgroundSize.width);
    _backgroundSprite->setScaleY((_contentSize.height - borderScale * 2.0F) / backgroundSize.height);
    _backgroundSprite->setPosition(borderScale, borderScale);

    // Update borders
    for (uint8_t i = 0; i < 8; i++)
    {
        updateBorder(static_cast<Border>(i), borderScale);
    }

    _layoutDirty = false;
}

void Panel::updateBorder(Border border, float scale)
{
    auto index       = static_cast<uint8_t>(border) & 7;
    auto sprite      = _borderSprites[index];
    auto& spriteSize = sprite->getContentSize();

    // Find out whether to hide this border based on chop value
    if (_chop != Chop::NONE)
    {
        auto chopIndex = static_cast<uint8_t>(_chop) - 1;
        auto cornerA   = _chop == Chop::TOP ? 4 : 3;
        auto cornerB   = _chop == Chop::TOP ? 7 : 4;

        if (index == chopIndex || index == chopIndex + cornerA || index == chopIndex + cornerB)
        {
            sprite->setVisible(false);
            return;
        }
    }

    // Update scale, position & rotation
    if (border >= Border::TOP_RIGHT)
    {
        sprite->setScale(scale / spriteSize.width);
    }
    else
    {
        auto vertical = (index | 2) == 3;
        auto scaleX   = (scale * -2.0F + (vertical ? _contentSize.height : _contentSize.width)) / spriteSize.width;
        sprite->setScaleX(scaleX);
        sprite->setScaleY(scale / spriteSize.height);
    }

    auto& offset  = kBorderOffsets[index];
    auto center   = _contentSize * 0.5F;
    auto position = center + offset * (center - Vec2::ONE * (scale * 0.5F));
    sprite->setPosition(position);
    sprite->setRotation((index % 4) * 90.0F);
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

    for (uint8_t i = 0; i < 8; i++)
    {
        auto border       = static_cast<Border>(i);
        auto name         = border >= Border::TOP_RIGHT ? "corner" : "top";
        _borderSprites[i] = createBorderSprite(name);
    }

    _backgroundSprite = createBorderSprite("background");
    _backgroundSprite->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
    _baseBorderScale = style.ends_with("brass") ? 0.65F : 1.0F;  // 0x1000E7FB2
    _layoutDirty     = true;
}

void Panel::setChop(Chop chop)
{
    if (_chop != chop)
    {
        _chop        = chop;
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

}  // namespace opendw
