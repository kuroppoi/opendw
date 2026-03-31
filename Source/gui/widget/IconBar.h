#ifndef __ICON_BAR_H__
#define __ICON_BAR_H__

#include "axmol.h"

namespace opendw
{

/*
 * CLASS: IconBar : CCNode @ 0x100319AD8
 */
class IconBar : public ax::Node
{
public:
    /* FUNC: IconBar::dealloc @ 0x1000EA855 */
    virtual ~IconBar() override;

    static IconBar* createWithIcon(const std::string& icon, float maxValue);

    bool initWithIcon(const std::string& icon, float maxValue);

    void updateLayout();

    /* FUNC: IconBar::setColor: @ 0x1000EA659 */
    void setIconColor(const ax::Color3B& color);

    /* FUNC: IconBar::color @ 0x1000EA945 */
    const ax::Color3B& getIconColor() const { return _iconColor; }

    /* FUNC: IconBar::setPadding: @ 0x1000EA933 */
    void setPadding(float padding);

    /* FUNC: IconBar::padding @ 0x1000EA921 */
    float getPadding() const { return _padding; }

    /* FUNC: IconBar::setMax: @ 0x1000E9C53 */
    void setMaxValue(float maxValue);

    /* FUNC: IconBar::max @ 0x1000EA8CF */
    float getMaxValue() const { return _maxValue; }

    /* FUNC: IconBar::setValue: @ 0x1000EA2EF */
    void setValue(float value);

    /* FUNC: IconBar::value @ 0x1000EA8E1 */
    float getValue() const { return _value; }

private:
    ax::SpriteBatchNode* _batchNode;            // IconBar::batchNode @ 0x1003131B0
    ax::SpriteFrame* _iconFrame;                // IconBar::icon @ 0x1003131D8
    ax::Color3B _iconColor;                     // IconBar::color @ 0x1003131E0
    std::vector<ax::Sprite*> _backgroundIcons;  // IconBar::bgIcons @ 0x1003131C0
    std::vector<ax::Sprite*> _foregroundIcons;  // IconBar::fgIcons @ 0x1003131C8
    float _padding;                             // IconBar::padding @ 0x1003131B8
    float _maxValue;                            // IconBar::max @ 0x1003131D0
    float _value;                               // IconBar::value @ 0x1003131E8
};

}  // namespace opendw

#endif  // __ICON_BAR_H__
