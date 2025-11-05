#ifndef __TELEPORT_PANEL_H__
#define __TELEPORT_PANEL_H__

#include "axmol.h"

#include "gui/Panel.h"

namespace opendw
{

class BaseBlock;
class SpriteButton;
class TeleportIcon;
struct ZoneSearchInfo;

/*
 * CLASS: TeleportPanel : Panel @ 0x10031A6B8
 */
class TeleportPanel : public Panel
{
public:
    CREATE_FUNC(TeleportPanel);

    /* FUNC: TeleportPanel::init @ 0x1000FDA39 */
    bool init() override;
    
    /* FUNC: TeleportPanel::onEnter @ 0x1000FDB50 */
    void onEnter() override;

    /* FUNC: TeleportPanel::onExit @ 0x100100852 */
    void onExit() override;

    /* FUNC: TeleportPanel::step: @ 0x1000FFDDA */
    void update(float deltaTime) override;

    /* FUNC: TeleportPanel::showFromBlock: @ 0x1000FDD39 */
    void showFromBlock(BaseBlock* block);

    /* FUNC: TeleportPanel::clearGroupInterface @ 0x1000FEE35 */
    void clearGroupInterface();

    /* FUNC: TeleportPanel::clearTeleporterInterface: @ 0x100100309 */
    void clearTeleporterInterface(bool fancy = false);

    /* FUNC: TeleportPanel::showGroups: @ 0x1000FDE59 */
    void showGroups(const ax::ValueVector& groups);

    /* FUNC: TeleportPanel::groupButton: @ 0x1000FE6A9 */
    SpriteButton* createGroupButton(const ax::Value& group);

    /* FUNC: TeleportPanel::teleportGroup:wasSelected: @ 0x1000FE958 */
    void onGroupSelected(const ax::Value& group);

    /* FUNC: TeleportPanel::showZones: @ 0x1000FEE7B */
    void showZones(const std::string& type, const std::vector<ZoneSearchInfo>& data);

    /* FUNC: TeleportPanel::closestTeleportToPoint: @ 0x1000FFB51 */
    TeleportIcon* findClosestTeleportToPoint(const ax::Point& point);

    /* FUNC: TeleportPanel::highlighted @ 0x1000FFD9A */
    TeleportIcon* findCursorHoveredTeleport();

    /* FUNC: TeleportPanel::pointerDown:event: @ 0x1000FDCEF */
    bool onPointerDown(ax::Touch* touch) override;

private:
    ax::Node* _teleportersNode;                  // TeleportPanel::teleportersNode @ 0x1003135B0
    ax::Node* _groupNode;                        // TeleportPanel::groupNode @ 0x1003135B8
    BaseBlock* _origin;                          // TeleportPanel::origin @ 0x1003135D0
    ax::StringMap<SpriteButton*> _groupButtons;  // TeleportPanel::groupButtonDictionary @ 0x1003135C0
    ax::Sprite* _groupIndicator;                 // TeleportPanel::groupIndicator @ 0x1003135E8
    std::string _currentGroup;                   // TeleportPanel::currentGroup @ 0x1003135F0
    ax::Label* _noneFoundLabel;                  // TeleportPanel::noneFoundLabel @ 0x1003135F8
    TeleportIcon* _highlightedTeleport;          // TeleportPanel::highlightedTeleport @ 0x100313600
};

}  // namespace opendw

#endif  // __TELEPORT_PANEL_H__
