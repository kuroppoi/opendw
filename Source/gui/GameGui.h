#ifndef __GAME_GUI_H__
#define __GAME_GUI_H__

#include "axmol.h"

#include "event/EventListenerContainer.h"

namespace opendw
{

class BaseBlock;
class EntityAnimatedAvatar;
class GameGuiWindow;
class IconBar;
class InventoryItemSprite;
class InventoryItem;
class ItemContainer;
class ItemSprite;
class Panel;
class SpriteButton;
class TeleportPanel;
class WorldZone;
enum class ContainerType;
struct ZoneSearchInfo;

/*
 * CLASS: GameGui : CCNode @ 0x100316FE0
 *
 * Responsible for displaying all in-game GUI elements.
 */
class GameGui : public ax::Node, EventListenerContainer
{
public:
    /* FUNC: GameGui::main @ 0x1000592A8 */
    static GameGui* getMain() { return sMain; }

    static GameGui* createWithZone(WorldZone* zone);

    /* FUNC: GameGui::initWithWorldZone: @ 0x100059496 */
    bool initWithZone(WorldZone* zone);

    /* FUNC: GameGui::onEnter @ 0x1000597D5 */
    void onEnter() override;

    /* FUNC: GameGui::onExit @ 0x10006A31A */
    void onExit() override;

    /* FUNC: GameGui::step: @ 0x10005CED1 */
    void update(float deltaTime) override;

    /* FUNC: GameGui::ready @ 0x10005CB86 */
    void ready();

    /* FUNC: GameGui::clear @ 0x10006A126 */
    void clear();

    /* FUNC: GameGui::updateInventoryItem: @ 0x10005ED7D */
    void updateInventoryItem(InventoryItem* item);

    void setItemContainerForType(ContainerType type, ItemContainer* container);

    /* FUNC: GameGui::inventoryContainerWithName: @ 0x10005EF8D */
    ItemContainer* getItemContainerForType(ContainerType type) const;
    ContainerType getTypeForItemContainer(ItemContainer* container) const;

    /* FUNC: GameGui::inventoryContainerWithScreenPoint: @ 0x10005EFAA */
    ItemContainer* getItemContainerAtScreenPoint(const ax::Point& point) const;

    /* FUNC: GameGui::updateTooltip: @ 0x10005F403 */
    void updateInventoryTooltip();

    /* FUNC: GameGui::showInventoryTooltipForPoint: @ 0x10005F570 */
    void showInventoryTooltipForPoint(const ax::Point& point);

    /* FUNC: GameGui::showInventoryTooltip: @ 0x10005F7F8 */
    void showInventoryTooltip(ItemSprite* sprite);

    /* FUNC: GameGui::toggleGameMenu @ 0x100064D5D */
    void toggleGameMenu();

    /* FUNC: GameGui::toggleProtectorRangefinder @ 0x100065A66 */
    void toggleProtectorRangeVisibility();

    /* FUNC: GameGui::toggleHud: @ 0x1000693DF */
    void setHudVisible(bool visible);

    /* FUNC: GameGui::showTeleportInterface:fromBlock: @ 0x100069557 */
    void showTeleportInterface(BaseBlock* block);

    /* FUNC: GameGui::hideTeleportInterface @ 0x10006964A */
    void hideTeleportInterface();

    /* FUNC: GameGui::showTeleportZones: @ 0x10006962D */
    void showTeleportZones(const std::string& type, const std::vector<ZoneSearchInfo>& data);

    /* FUNC: GameGui::alert: @ 0x10006264C */
    void showAlert(const std::string& text);
    void showAlert(const ax::Value& data);

    /* FUNC: GameGui::showBigAlert:direction:sound: @ 0x1000614EC */
    void showBigAlert(const std::string& title, const std::string& subtitle = "");
    void showBigAlert(const ax::Value& data);

    /* FUNC: GameGui::pointerInGui: @ 0x10006714C */
    bool isPointInGui(const ax::Point& point) const;

    /* FUNC: GameGui::panelMargin @ 0x10006A672 */
    float getPanelMargin() const { return _panelMargin; }

    /* FUNC: GameGui::panelPadding @ 0x10006A684 */
    float getPanelPadding() const { return _panelPadding; }

    /* FUNC: GameGui::itemMargin @ 0x10006A55E */
    float getItemMargin() const { return _itemMargin; }

    /* FUNC: GameGui::itemSize @ 0x10006A570 */
    float getItemSize() const { return _itemSize; }

    /* FUNC: GameGui::inventoryRows @ 0x10006A51A */
    int32_t getInventoryRows() const { return _inventoryRows; }

    /* FUNC: GameGui::inventoryCols @ 0x10006A52B */
    int32_t getInventoryCols() const { return _inventoryCols; }

    /* FUNC: GameGui::craftingRows @ 0x10006A54D */
    int32_t getCraftingRows() const { return _craftingRows; }

    /* FUNC: GameGui::craftingCols @ 0x10006A53C */
    int32_t getCraftingCols() const { return _craftingCols; }

    /* FUNC: GameGui::respawnMessage @ 0x100059475 */
    const char* getRespawnMessage() const;

    /* FUNC: GameGui::positionDescription: @ 0x10005CD87 */
    std::string getPositionDescription() const;

    /* FUNC: GameGui::guiWindowPosition: @ 0x1000675DA */
    ax::Point getGuiWindowPosition(bool alignRight) const;

    /* FUNC: GameGui::setMouseSprite: @ 0x1000603E0 */
    void setActiveItemSprite(InventoryItemSprite* sprite);

    /* FUNC: GameGui::updateHotbar: @ 0x10005F18D */
    void updateHotbar();

    /* FUNC: GameGui::guiWindowChangedPanel @ 0x10005DEC0 */
    void onGuiWindowPanelChanged();

    /* FUNC: GameGui::healthDidChange: @ 0x10005E036 */
    void onHealthChanged(float health, float maxHealth);

    /* FUNC: GameGui::steamDidChange: @ 0x10005E1F2 */
    void onSteamChanged(float steam);

    /* FUNC: GameGui::steamCooldownDidBegin @ 0x100061178 */
    void onSteamCooldownBegan();

    /* FUNC: GameGui::steamCooldownDidEnd @ 0x10006119A */
    void onSteamCooldownEnded();

    /* FUNC: GameGui::deathMessageDidChange @ 0x10005E3C3 */
    void onDeathMessageChanged(const std::string& deathMessage);

    /* FUNC: GameGui::playerCountDidChange @ 0x10005EC12 */
    void onPlayerCountChanged();

    /* FUNC: GameGui::playerAppearanceDidChange: @ 0x100064497 */
    void onPlayerAppearanceChanged(const ax::ValueMap& data);

    /* FUNC: GameGui::pointerDown:event: @ 0x10006617B */
    bool onTouchBegan(ax::Touch* touch, ax::Event* event);

    /* FUNC: GameGui::pointerDragged:event: @ 0x100066528 */
    void onTouchMoved(ax::Touch* touch, ax::Event* event);

    /* FUNC: GameGui::pointerUp:event: @ 0x100066597 */
    void onTouchEnded(ax::Touch* touch, ax::Event* event);

    /* FUNC: GameGui::screenSizeDidChange: @ 0x100067741 */
    void onWindowResized();

    /* FUNC: GameGui::protectorRangeFinderActive @ 0x10006A5FC */
    bool isProtectorRangeVisible() const { return _protectorRangeVisible; }

private:
    inline static GameGui* sMain;  // 0x10032EAD0

    WorldZone* _zone;                                      // GameGui::zone @ 0x100311788
    ax::Map<ContainerType, ItemContainer*> _containers;    // GameGui::inventoryContainers @ 0x100311790
    ax::Map<uint16_t, InventoryItemSprite*> _itemSprites;  // GameGui::inventoryItems @ 0x100311798
    float _panelMargin;                                    // GameGui::panelMargin @ 0x1003117D8
    float _panelPadding;                                   // GameGui::panelPadding @ 0x1003117E0
    float _itemMargin;                                     // GameGui::itemMargin @ 0x1003117E8
    float _itemSize;                                       // GameGui::itemSize @ 0x1003117F0
    int32_t _inventoryRows;                                // GameGui::inventoryRows @ 0x1003117B8
    int32_t _inventoryCols;                                // GameGui::inventoryCols @ 0x1003117C0
    int32_t _craftingRows;                                 // GameGui::craftingRows @ 0x1003117D0
    int32_t _craftingCols;                                 // GameGui::craftingCols @ 0x1003117C8
    ax::Node* _announcementsNode;                          // GameGui::announcementsNode @ 0x100311948
    ax::Node* _gameMenu;                                   // GameGui::gameMenu @ 0x100311A30
    ax::Node* _hudNode;                                    // GameGui::hudNode @ 0x100311840
    ax::Node* _hudButtonsNode;                             // GameGui::hudButtonsNode @ 0x100311848
    IconBar* _healthBar;                                   // GameGui::healthBar @ 0x100311858
    ax::Label* _steamLabel;                                // GameGui::steamLabel @ 0x100311860
    ax::Label* _nameLabel;                                 // GameGui::nameLabel @ 0x100311868
    ax::Label* _zoneLabel;                                 // GameGui::zoneLabel @ 0x100311878
    ax::Label* _positionLabel;                             // GameGui::posLabel @ 0x1003118A0
    ax::Label* _deathLabel;                                // GameGui::deadLabel @ 0x1003118A8
    GameGuiWindow* _guiWindow;                             // GameGui::guiWindow @ 0x1003118B8
    SpriteButton* _profileButton;                          // GameGui::profileButton @ 0x1003118C0
    EntityAnimatedAvatar* _avatarPicture;                  // GameGui::avatarPicture @ 0x1003118C8
    SpriteButton* _inventoryButton;                        // GameGui::inventoryButton @ 0x1003118D8
    SpriteButton* _craftingButton;                         // GameGui::craftingButton @ 0x1003118E0
    SpriteButton* _socialButton;                           // GameGui::socialButton @ 0x1003118B0
    ax::Label* _socialLabel;                               // GameGui::socialLabel @ 0x1003118F8
    SpriteButton* _worldButton;                            // GameGui::worldButton @ 0x100311900
    SpriteButton* _shopButton;                             // GameGui::shopButton @ 0x1003118E8
    ax::Label* _crownsLabel;                               // GameGui::crownsLabel @ 0x1003118F0
    SpriteButton* _mapButton;                              // GameGui::mapButton @ 0x100311908
    SpriteButton* _consoleButton;                          // GameGui::consoleButton @ 0x100311918
    ItemContainer* _primaryHotbar;                         // GameGui::primaryHotbar @ 0x100311920
    ax::Node* _topSpriteLayer;                             // GameGui::mouseSpriteLayer @ 0x100311938
    InventoryItemSprite* _activeItemSprite;                // GameGui::mouseSprite @ 0x1003119E8
    Panel* _inventoryTooltip;                              // GameGui::inventoryTooltip @ 0x1003119C8
    ItemSprite* _inventoryTooltipOwner;                    // GameGui::inventoryTooltipNode @ 0x1003119D0
    TeleportPanel* _teleportPanel;                         // GameGui::teleportPanel @ 0x100311A58
    double _lastAlertShownAt;                              // GameGui::lastAlertShownAt @ 0x100311978
    std::string _lastAlertShown;                           // GameGui::lastAlertShown @ 0x100311A10
    std::vector<std::string> _pendingAlerts;               // GameGui::pendingAlerts @ 0x1003117A8
    bool _protectorRangeVisible;                           // GameGui::protectorRangefinderActive @ 0x100311A40
    bool _draggingItemSprite;
};

}  // namespace opendw

#endif  // __GAME_GUI_H__
