#include "Panel.h"

#include "util/AxUtil.h"
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
    setChop(Chop::NONE);
    setBorderScale(1.0F);
    setContentSize(Size::ONE * 100.0F);  // Default size
    setCascadeOpacityEnabled(true);
    return true;
}

void Panel::onEnter()
{
    Node::onEnter();

    // Create touch listener
    _touchListener               = EventListenerTouchOneByOne::create();
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
    auto& cornerSize     = _borderSprites[static_cast<uint8_t>(Border::TOP_RIGHT)]->getContentSize();
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
        return onPointerDown(touch);
    }

    return false;
}

}  // namespace opendw
