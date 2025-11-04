#include "TextField.h"

#include "gui/Panel.h"
#include "util/AxUtil.h"
#include "util/ColorUtil.h"
#include "util/MathUtil.h"
#include "CommonDefs.h"

#define ACTIVE_OPACITY   0xFF
#define INACTIVE_OPACITY 0xC0
#define CURSOR           "|"

USING_NS_AX;
using namespace ui;

namespace opendw
{

TextField::~TextField()
{
    AX_SAFE_RELEASE(_fontConfiguration);
}

TextField* TextField::createWithFont(const std::string& fontFile)
{
    CREATE_INIT(TextField, initWithFont, fontFile);
}

bool TextField::initWithFont(const std::string& fontFile)
{
    if (!Node::init())
    {
        return false;
    }

    _fontConfiguration = BMFontConfiguration::create(fontFile);
    AX_SAFE_RETAIN(_fontConfiguration);

    // Create value clipping node
    auto clippingNode = ClippingNode::create();
    addChild(clippingNode, 1);

    // Create touch listener
    _touchListener               = EventListenerTouchOneByOne::create();
    _touchListener->onTouchBegan = AX_CALLBACK_2(TextField::onTouchBegan, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(_touchListener, this);

    // Create edit box
    _editBox = EditBox::create(Size::ZERO, "");
    _editBox->setInputMode(EditBox::InputMode::SINGLE_LINE);
    _editBox->setDelegate(this);
    _editBox->setVisible(false);
    addChild(_editBox);

    // Create title label
    _titleLabel = Label::createWithBMFont(fontFile, "");
    _titleLabel->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
    _titleLabel->setScale(0.85F);
    _titleLabel->setVisible(false);
    addChild(_titleLabel);

    // Create value label
    _valueLabel = Label::createWithBMFont(fontFile, "");
    _valueLabel->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
    _valueLabel->setOpacity(INACTIVE_OPACITY);
    clippingNode->addChild(_valueLabel);

    // Create hint label
    _hintLabel = Label::createWithBMFont(fontFile, "");
    _hintLabel->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
    _hintLabel->setScale(0.75F);
    _hintLabel->setVisible(false);
    addChild(_hintLabel);

    // Create background panel
    _backgroundPanel = Panel::createWithStyle("input/border");
    _backgroundPanel->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
    clippingNode->setStencil(_backgroundPanel);
    addChild(_backgroundPanel);

    // Misc
    setText("");
    setMaxLength(20);
    setTitleColor(color_util::hexToColor("FFDC0A"));
    setHintColor(color_util::hexToColor("281401"));
    setWidth(400.0F);
    setPadding(6.0F);
    setCascadeOpacityEnabled(true);
    return true;
}

void TextField::visit(Renderer* renderer, const Mat4& transform, uint32_t flags)
{
    if (isVisible() && _layoutDirty)
    {
        updateLayout();
    }

    Node::visit(renderer, transform, flags);
}

void TextField::updateLayout()
{
    auto paddingY = _padding * 0.75F;
    auto currentY = 0.0F;

    // Update hint label
    if (_hintLabel->isVisible())
    {
        _hintLabel->setMaxLineWidth(_width / _hintLabel->getScaleX());
        _hintLabel->setPositionX(_padding);
        currentY += math_util::getScaledHeight(_hintLabel) + paddingY;
    }

    // Update background panel
    auto height = _valueLabel->getLineHeight() + _padding * 2.0F;
    _backgroundPanel->setPositionY(currentY);
    _backgroundPanel->setContentSize({_width, height});
    currentY += paddingY;

    // Update value label
    _valueLabel->setPositionY(currentY);
    currentY += height - _padding;

    // Update title label
    if (_titleLabel->isVisible())
    {
        _titleLabel->setPosition(_padding, currentY + paddingY);
        currentY += math_util::getScaledHeight(_titleLabel) + paddingY;
    }

    setContentSize({_width, currentY});
    _layoutDirty = false;
}

void TextField::updateValueLabel()
{
    std::string text = getText();  // Explicit copy

    // Censor text if necessary
    if (_password)
    {
        text = text.replace(text.begin(), text.end(), text.length(), '*');
    }

    _valueLabel->setString(text + CURSOR);  // HACK: Get width of text + cursor
    auto offsetX = fmaxf(0.0F, (_valueLabel->getContentSize().width + _padding * 2.0F) - _width);

    // Append cursor if visible
    if (_cursorVisible)
    {
        text += CURSOR;
    }

    _valueLabel->setString(text == "" ? " " : text);  // HACK: Ensure it always updates properly
    _valueLabel->setPositionX(_padding - offsetX);
}

void TextField::updateCursor()
{
    _cursorVisible = !_cursorVisible;
    updateValueLabel();
}

void TextField::setActive(bool active)
{
    if (active)
    {
        _editBox->openKeyboard();
    }
    else
    {
        _editBox->closeKeyboard();
    }
}

void TextField::setBackgroundStyle(const std::string& style)
{
    _backgroundPanel->setStyle(style);
}

void TextField::setFont(const std::string& fontFile)
{
    _titleLabel->setBMFontFilePath(fontFile);
    _valueLabel->setBMFontFilePath(fontFile);
    _hintLabel->setBMFontFilePath(fontFile);
    _layoutDirty = true;
}

void TextField::setTitle(const std::string& title)
{
    _titleLabel->setString(title);
    _titleLabel->setVisible(!title.empty());
    _layoutDirty = true;
}

void TextField::setTitleColor(const Color3B& color)
{
    _titleLabel->setColor(color);
}

void TextField::setText(const std::string& text)
{
    _editBox->setText(text.c_str());
    updateValueLabel();
}

void TextField::setTextColor(const Color3B& color)
{
    _valueLabel->setColor(color);
}

void TextField::setHint(const std::string& hint)
{
    _hintLabel->setString(hint);
    _hintLabel->setVisible(!hint.empty());
    _layoutDirty = true;
}

void TextField::setHintColor(const Color3B& color)
{
    _hintLabel->setColor(color);
}

void TextField::setWidth(float width)
{
    if (_width != width)
    {
        _width       = width;
        _layoutDirty = true;
    }
}

void TextField::setPadding(float padding)
{
    if (_padding != padding)
    {
        _padding     = padding;
        _layoutDirty = true;
    }
}

void TextField::setPassword(bool password)
{
    if (_password != password)
    {
        _password = password;
        updateValueLabel();
    }
}

bool TextField::onTouchBegan(Touch* touch, Event* event)
{
    if (!ax_util::isNodeVisible(this))
    {
        return false;
    }

    auto rect = Rect(_backgroundPanel->getPosition(), _backgroundPanel->getContentSize());

    if (isScreenPointInRect(touch->getLocation(), Camera::getVisitingCamera(), getWorldToNodeTransform(), rect,
                            nullptr))
    {
        _editBox->openKeyboard();
        return true;
    }

    return false;
}

void TextField::editBoxEditingDidBegin(EditBox* editBox)
{
    _cursorVisible = true;
    _valueLabel->setOpacity(ACTIVE_OPACITY);
    schedule(AX_CALLBACK_0(TextField::updateCursor, this), 0.5F, "updateCursor");
    setText(getText());  // Hack: reset cursor position
    updateValueLabel();
}

void TextField::editBoxReturn(ax::ui::EditBox* editBox)
{
    _cursorVisible = false;
    _valueLabel->setOpacity(INACTIVE_OPACITY);
    unschedule("updateCursor");
    updateValueLabel();
}

void TextField::editBoxTextChanged(ax::ui::EditBox* editBox, std::string_view text)
{
    std::ostringstream result;
    auto charset  = _fontConfiguration->getCharacterSet();
    auto filtered = false;

    // Filter characters that are not supported by the font
    for (auto c : text)
    {
        if (charset->contains(c))
        {
            result << c;
        }
        else
        {
            filtered = true;
        }
    }

    if (filtered)
    {
        setText(result.str());
    }
    else
    {
        updateValueLabel();
    }
}

}  // namespace opendw
