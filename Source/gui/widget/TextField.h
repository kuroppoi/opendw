#ifndef __TEXT_FIELD_H__
#define __TEXT_FIELD_H__

#include "ui/UIEditBox/UIEditBox.h"
#include "axmol.h"

namespace opendw
{

class Panel;

/*
 * CLASS: TextField : CCLayer @ 0x1003179B8
 *
 * This implementation uses EditBox as its input backend but it isn't perfect.
 * It'd be nice if you had more control over things like cursor position and text selection...
 */
class TextField : public ax::Node, ax::ui::EditBoxDelegate
{
public:
    /* FUNC: TextField::dealloc @ 0x100095EAD */
    ~TextField() override;

    static TextField* createWithFont(const std::string& fontFile);

    /* FUNC: TextField::initWithValue:font:color: @ 0x1000948E2 */
    bool initWithFont(const std::string& fontFile);

    void visit(ax::Renderer* renderer, const ax::Mat4& transform, uint32_t flags) override;

    /* FUNC: TextField::updateLayout @ 0x10009537A */
    void updateLayout();

    /* FUNC: TextField::updateValueLabel @ 0x100094F1D */
    void updateValueLabel();

    /* FUNC: TextField::updateCursor @ 0x1000956C8 */
    void updateCursor();

    /* FUNC: TextField::setActive: @ 0x1000952E1 */
    void setActive(bool active);

    /* FUNC: TextField::usePanelBackground:color: @ 0x100095191 */
    void setBackgroundStyle(const std::string& style);

    void setFont(const std::string& fontFile);

    /* FUNC: TextField::setTitle:font:color: @ 0x100094BAA */
    void setTitle(const std::string& title);
    void setTitleColor(const ax::Color3B& color);

    /* FUNC: TextField::setValue: @ 0x100094E7C */
    void setText(const std::string& text);
    void setTextColor(const ax::Color3B& color);

    /* FUNC: TextField::value @ 0x100095F38 */
    std::string getText() const { return _editBox->getText(); }

    /* FUNC: TextField::setHint:font:color: @ 0x100094CAC */
    void setHint(const std::string& hint);
    void setHintColor(const ax::Color3B& color);

    /* FUNC: TextField::setValueMax: @ 0x100095F7C */
    void setMaxLength(int length) { _editBox->setMaxLength(length); }

    /* FUNC: TextField::setWidth: @ 0x1000952B5 */
    void setWidth(float width);

    /* FUNC: TextField::setPadding: @ 0x1000960E9 */
    void setPadding(float padding);

    /* FUNC: TextField::setPassword: @ 0x1000960C7 */
    void setPassword(bool password);

    /* FUNC: TextField::pointerDown:event: @ 0x100095BDF */
    bool onTouchBegan(ax::Touch* touch, ax::Event* event);

    void editBoxEditingDidBegin(ax::ui::EditBox* editBox) override;
    void editBoxReturn(ax::ui::EditBox* editBox) override;
    void editBoxTextChanged(ax::ui::EditBox* editBox, std::string_view text) override;

private:
    ax::Label* _titleLabel;   // TextField::titleLabel @ 0x1003123C0
    ax::Label* _valueLabel;   // TextField::valueLabel @ 0x100312398
    ax::Label* _hintLabel;    // TextField::hintLabel @ 0x1003123C8
    Panel* _backgroundPanel;  // TextField::valueBackground 0x1003123E8
    float _width;             // TextField::width @ 0x1003123A0
    float _padding;           // TextField::padding @ 0x100312380
    bool _password;           // TextField::password @ 0x1003123F0
    bool _cursorVisible;      // TextField::cursorVisible @ 0x1003123B0
    ax::EventListenerTouchOneByOne* _touchListener;
    ax::BMFontConfiguration* _fontConfiguration;
    ax::ui::EditBox* _editBox;
    bool _layoutDirty;
};

}  // namespace opendw

#endif  // __TEXT_FIELD_H__
