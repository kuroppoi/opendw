#ifndef __GAME_GUI_H__
#define __GAME_GUI_H__

#include "axmol.h"

namespace opendw
{

class BaseBlock;
class SpriteButton;
class TeleportPanel;
class WorldZone;
struct ZoneSearchInfo;

/*
 * CLASS: GameGui : CCNode @ 0x100316FE0
 *
 * Responsible for displaying all in-game GUI elements.
 */
class GameGui : public ax::Node
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

    /* FUNC: GameGui::ready @ 0x10005CB86 */
    void ready();

    /* FUNC: GameGui::toggleGameMenu @ 0x100064D5D */
    void toggleGameMenu();

    /* FUNC: GameGui::showTeleportInterface:fromBlock: @ 0x100069557 */
    void showTeleportInterface(BaseBlock* block);

    /* FUNC: GameGui::hideTeleportInterface @ 0x10006964A */
    void hideTeleportInterface();

    /* FUNC: GameGui::showTeleportZones: @ 0x10006962D */
    void showTeleportZones(const std::string& type, const std::vector<ZoneSearchInfo>& data);

    /* FUNC: GameGui::showBigAlert:direction:sound: @ 0x1000614EC */
    void showBigAlert(const std::string& title, const std::string& subtitle = "");
    void showBigAlert(const ax::Value& data);

    /* FUNC: GameGui::screenSizeDidChange: @ 0x100067741 */
    void onWindowResized();

private:
    inline static GameGui* sMain;  // 0x10032EAD0

    WorldZone* _zone;               // GameGui::zone @ 0x100311788
    ax::Node* _announcementsNode;   // GameGui::announcementsNode @ 0x100311948
    ax::Node* _gameMenu;            // GameGui::gameMenu @ 0x100311A30
    ax::Node* _hudButtonsNode;      // GameGui::hudButtonsNode @ 0x100311848
    SpriteButton* _worldButton;     // GameGui::worldButton @ 0x100311900
    TeleportPanel* _teleportPanel;  // GameGui::teleportPanel @ 0x100311A58
    std::vector<ax::EventListener*> _eventListeners;
};

}  // namespace opendw

#endif  // __GAME_GUI_H__
