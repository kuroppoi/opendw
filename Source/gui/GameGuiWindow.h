#ifndef __GAME_GUI_WINDOW_H__
#define __GAME_GUI_WINDOW_H__

#include "gui/widget/Panel.h"

namespace opendw
{

class CraftingPanel;
class GameGui;
class InventoryPanel;

/*
 * CLASS: GameGuiWindow : Panel @ 0x10031E858
 */
class GameGuiWindow : public Panel
{
public:
    enum class PanelType
    {
        NONE,
        PROFILE,
        INVENTORY,
        CRAFTING,
        SHOP,
        SOCIAL,
        SETTINGS,
        FEED
    };

    static GameGuiWindow* createWithGui(GameGui* gui, const ax::Size& size);

    /* FUNC: GameGuiWindow::initWithGameGui: @ 0x10017E9AD */
    bool initWithGui(GameGui* gui, const ax::Size& size);

    /* FUNC: GameGuiWindow::addPanel: @ 0x10017EF47 */
    void addPanel(ax::Node* node);

    /* FUNC: GameGuiWindow::toggle: @ 0x10017EFE7 */
    void toggle(PanelType type);

    /* FUNC: GameGuiWindow::show: @ 0x10017F055 */
    void show(PanelType type);

    /* FUNC: GameGuiWindow::hide @ 0x10017F4B7 */
    void hide();

    /* FUNC: GameGuiWindow::updatePosition @ 0x10017F3FD */
    void updatePosition();

    /* FUNC: GameGuiWindow::panelForType: @ 0x10017F491 */
    ax::Node* getPanelForType(PanelType type) const;

    /* FUNC: GameGuiWindow::activePanelType @ 0x10017F7EF */
    PanelType getActivePanelType() const { return _activePanelType; }

    /* @return -1 if panel should align left or 1 if it should align right. */
    int getAlignmentForType(PanelType type) const;

private:
    GameGui* _gui;                    // GameGuiWindow::gui @ 0x100315348
    InventoryPanel* _inventoryPanel;  // GameGuiWindow::inventoryPanel @ 0x100315358
    CraftingPanel* _craftingPanel;    // GameGuiWindow::craftingPanel @ 0x100315360
    PanelType _activePanelType;       // GameGuiWindow::activePanelType @ 0x100315390
    ax::Node* _activePanel;           // GameGuiWindow::activePanel @ 0x100315398
};

}  // namespace opendw

#endif  // __GAME_GUI_WINDOW_H__
