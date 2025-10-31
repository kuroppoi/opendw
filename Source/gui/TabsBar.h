#ifndef __TABS_BAR_H__
#define __TABS_BAR_H__

#include "axmol.h"

namespace opendw
{

/*
 * CLASS: TabsBar : CCSprite @ 0x10031A000
 */
class TabsBar : public ax::Sprite
{
public:
    /* FUNC: TabsBar::dealloc @ 0x1000EF172 */
    ~TabsBar() override;

    CREATE_FUNC(TabsBar);

    /* FUNC: TabsBar::init @ 0x1000ED373 */
    bool init() override;

    void visit(ax::Renderer* renderer, const ax::Mat4& transform, uint32_t flags) override;

    void updateLayout();

    /* FUNC: TabsBar::addTab:image:target: @ 0x1000ED635 */
    void addTab(Node* node);

    void removeTab(size_t index);
    void removeTab(Node* node);

    void removeAllTabs();

    /* FUNC: TabsBar::selectTab: @ 0x1000EE2E1 */
    void selectTab(size_t index);
    void selectTab(Node* node);

    /* FUNC: TabsBar::setBackground: @ 0x1000EF291 */
    void setBackground(const std::string& frameName);

    /* FUNC: TabsBar::setBackgroundScale: @ 0x1000EF2EE */
    void setBackgroundScale(float scale);

    /* FUNC: TabsBar::setSelectedBackground: @ 0x1000EF2BF */
    void setSelectedBackground(const std::string& frameName);

    /* FUNC: TabsBar::pointerDown:event: @ 0x1000EED88 */
    bool onTouchBegan(ax::Touch* touch, ax::Event* event);

    static constexpr auto SELECTED_SPRITE_TAG = 2;

private:
    ax::EventListenerTouchOneByOne* _touchListener;
    ax::Vector<Node*> _targets;            // TabsBar::targets @ 0x100313248
    size_t _selectedTab;                   // TabsBar::selectedTab @ 0x100313288
    ax::SpriteFrame* _background;          // TabsBar::background @ 0x100313260
    float _backgroundScale;                // TabsBar::backgroundScale @ 0x100313200
    ax::SpriteFrame* _selectedBackground;  // TabsBar::selectedBackground @ 0x100313290
    bool _tabsDirty;
};

}  // namespace opendw

#endif  // __TABS_BAR_H__
