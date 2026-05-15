#ifndef __TABS_BAR_H__
#define __TABS_BAR_H__

#include "axmol.h"

namespace opendw
{

class TabsBarDelegate;

/*
 * CLASS: TabsBar : CCSprite @ 0x10031A000
 */
class TabsBar : public ax::Sprite
{
public:
    struct Tab
    {
        std::string image;
        ax::Node* target;
    };

    /* FUNC: TabsBar::dealloc @ 0x1000EF172 */
    ~TabsBar() override;

    CREATE_FUNC(TabsBar);

    /* FUNC: TabsBar::init @ 0x1000ED373 */
    bool init() override;

    void visit(ax::Renderer* renderer, const ax::Mat4& transform, uint32_t flags) override;

    void updateLayout();

    /* FUNC: TabsBar::addTab:image:target: @ 0x1000ED635 */
    void addTab(const std::string& image, ax::Node* target = nullptr);
    void addTab(ax::Node* target);

    void removeTab(ssize_t index);
    void removeAllTabs();

    ssize_t getTabCount() const { return _tabs.size(); }

    /* FUNC: TabsBar::selectTab: @ 0x1000EE2E1 */
    void selectTab(ssize_t index);

    /* FUNC: TabsBar::selectedTab @ 0x1000EF4FD */
    ssize_t getSelectedTab() const { return _selectedTab; }

    /* FUNC: TabsBar::setMaxColumns: @ 0x1000EF3B9 */
    void setMaxColumns(ssize_t maxColumns);

    /* FUNC: TabsBar::setBackground: @ 0x1000EF291 */
    void setBackground(const std::string& frameName);

    /* FUNC: TabsBar::setBackgroundScale: @ 0x1000EF2EE */
    void setBackgroundScale(float scale);

    /* FUNC: TabsBar::setBackgroundColor: @ 0x1000ED5CC */
    void setBackgroundColor(const ax::Color3B& color);

    /* FUNC: TabsBar::setSelectedBackground: @ 0x1000EF2BF */
    void setSelectedBackground(const std::string& frameName);

    /* FUNC: TabsBar::setSelectedBackgroundColor: @ 0x1000ED5EF */
    void setSelectedBackgroundColor(const ax::Color3B& color);

    /* FUNC: TabsBar::setImageColor: @ 0x1000ED5A9 */
    void setImageColor(const ax::Color3B& color);

    /* FUNC: TabsBar::setSelectedImageColor: @ 0x1000ED612 */
    void setSelectedImageColor(const ax::Color3B& color);

    /* FUNC: TabsBar::setDelegate: @ 0x1000EF4E0 */
    void setDelegate(TabsBarDelegate* delegate) { _delegate = delegate; }

    /* FUNC: TabsBar::pointerDown:event: @ 0x1000EED88 */
    bool onTouchBegan(ax::Touch* touch, ax::Event* event);

    static constexpr auto SELECTED_SPRITE_TAG = 2;
    static constexpr auto IMAGE_SPRITE_TAG    = 3;

private:
    ax::EventListenerTouchOneByOne* _touchListener;
    std::vector<Tab> _tabs;                // TabsBar::targets @ 0x100313248
    ssize_t _maxColumns;                   // TabsBar::maxColumns @ 0x100313270
    ssize_t _selectedTab;                  // TabsBar::selectedTab @ 0x100313288
    ax::SpriteFrame* _background;          // TabsBar::background @ 0x100313260
    float _backgroundScale;                // TabsBar::backgroundScale @ 0x100313200
    ax::Color3B _backgroundColor;          // TabsBar::backgroundColor @ 0x100313228
    ax::SpriteFrame* _selectedBackground;  // TabsBar::selectedBackground @ 0x100313290
    ax::Color3B _selectedBackgroundColor;  // TabsBar::selectedBackgroundColor @ 0x100313230
    ax::Color3B _imageColor;               // TabsBar::imageColor @ 0x100313218
    ax::Color3B _selectedImageColor;       // TabsBar::selectedImageColor @ 0x100313220
    TabsBarDelegate* _delegate;            // TabsBar::delegate @ 0x1003132A0
    bool _tabsDirty;
};

}  // namespace opendw

#endif  // __TABS_BAR_H__
