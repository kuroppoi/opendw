#ifndef __PANEL_H__
#define __PANEL_H__

#include "axmol.h"

namespace opendw
{

/*
 * CLASS: Panel : CCNode @ 0x100319948
 */
class Panel : public ax::Node
{
public:
    enum class Border : uint8_t
    {
        TOP,
        RIGHT,
        BOTTOM,
        LEFT,
        TOP_RIGHT,
        BOTTOM_RIGHT,
        BOTTOM_LEFT,
        TOP_LEFT
    };

    enum class Chop : uint8_t
    {
        NONE,
        TOP,
        RIGHT,
        BOTTOM,
        LEFT
    };

    static Panel* createWithStyle(const std::string& style);

    /* FUNC: Panel::initWithStyle:rect: @ 0x1000E7A20 */
    bool initWithStyle(const std::string& style);

    /* FUNC: Panel::onEnter @ 0x1000E7BE9 */
    void onEnter() override;

    /* FUNC: Panel::onExit @ 0x1000E9289 */
    void onExit() override;

    void visit(ax::Renderer* renderer, const ax::Mat4& transform, uint32_t flags) override;

    /* FUNC: Panel::update @ 0x1000E8024 */
    void updateLayout();
    void updateBorder(Border border, float scale);

    /* FUNC: Panel::borderSprite: @ 0x1000E7F44 */
    ax::Sprite* createBorderSprite(const std::string& name);

    void setStyle(const std::string& style);

    /* FUNC: Panel::setBackgroundTexture:scale:color:opacity: @ 0x1000E9094 */
    void setBackgroundTexture(const std::string& textureFile, uint8_t opacity = 0xFF);

    /* FUNC: Panel::setChop: @ 0x1000E93BD */
    void setChop(Chop chop);

    /* FUNC: Panel::setBorderScale: @ 0x1000E9401 */
    void setBorderScale(float scale);

    /* FUNC: Panel::setContentSize: @ 0x1000E893C */
    void setContentSize(const ax::Size& contentSize) override;

    /* FUNC: Panel::ccMouseDown: @ 0x1000E7751 */
    bool onTouchBegan(ax::Touch* touch, ax::Event* event);

    /* FUNC: Panel::pointerDown:event: @ 0x1000E921C */
    virtual bool onPointerDown(ax::Touch* touch) { return false; };

private:
    ax::SpriteBatchNode* _batch;           // Panel::batch @ 0x1003130D0
    std::string _style;                    // Panel::style @ 0x100313088
    Chop _chop;                            // Panel::chop @ 0x100313098
    float _borderScale;                    // Panel::borderScale @ 0x1003130C8
    float _baseBorderScale;                // Panel::baseBorderScale @ 0x100313138
    ax::Sprite* _backgroundSprite;         // Panel::backgroundSprite @ 0x1003130D8
    ax::Sprite* _backgroundTextureSprite;  // Panel::backgroundTextureSprite @ 0x100313148
    ax::Sprite* _borderSprites[8];
    bool _layoutDirty;
    ax::EventListenerTouchOneByOne* _touchListener;
};

}  // namespace opendw

#endif  // __PANEL_H__
