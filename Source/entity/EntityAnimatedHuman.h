#ifndef __ENTITY_ANIMATED_HUMAN_H__
#define __ENTITY_ANIMATED_HUMAN_H__

#include "entity/EntityAnimated.h"

namespace opendw
{

class Item;

/*
 * CLASS: EntityAnimatedHuman : EntityAnimated @ 0x10031E308
 */
class EntityAnimatedHuman : public EntityAnimated
{
public:
    /* FUNC: EntityAnimatedHuman::onEnter @ 0x100174977 */
    virtual void onEnter() override;

    /* FUNC: EntityAnimatedHuman::step: @ 0x100178640 */
    virtual void update(float deltaTime) override;

    /* FUNC: EntityAnimatedHuman::computeContentSize @ 0x100174D40 */
    virtual ax::Size computeContentSize() override;

    /* FUNC: EntityAnimatedHuman::change: @ 0x100174DB8 */
    virtual void change(const ax::ValueMap& data) override;

    /* FUNC: EntityAnimatedHuman::isHuman @ 0x10017496C */
    bool isHuman() const override { return true; }

    /* FUNC: EntityAnimatedHuman::updateAppearance: @ 0x100174EF4 */
    void updateAppearance(const ax::ValueMap& data);

    /* FUNC: EntityAnimatedHuman::hideTorso @ 0x100176829 */
    void hideTorso();

    /* FUNC: EntityAnimatedHuman::hideLeg @ 0x10017692B */
    void hideLegs();

    /* FUNC EntityAnimatedHuman::hideSkirt @ 0x100176A73 */
    void hideSkirt();

    /* FUNC: EntityAnimatedHuman::hideTool @ 0x100176ACD */
    void hideTool();

    /* FUNC: EntityAnimatedHuman::hideExo @ 0x100176B27 */
    void hideExo();

    /* FUNC: EntityAnimatedHuman::showHeadOnly @ 0x100176C9B */
    void showHeadOnly();

    /* FUNC: EntityAnimatedHuman::setAnimatingTool: @ 0x1001770F8 */
    void setAnimatingTool(bool value);

    /* FUNC: EntityAnimatedHuman::setToolItem: @ 0x100177FA1 */
    void setToolItem(Item* item);

    /* FUNC: EntityAnimatedHuman::setInteractionItem: @ 0x100179415 */
    void setTargetItem(Item* item) { _targetItem = item; }

    /* FUNC: EntityAnimatedHuman::updateArms: @ 0x100178150 */
    void updateArms(float deltaTime);

    /* FUNC: EntityAnimatedHuman::animateToolTowardsPoint: @ 0x1001783E4 */
    void animateTool(const ax::Point& point = ax::Point::ZERO);

    /* FUNC: EntityAnimatedHuman::animateEye:duration: @ 0x100179154 */
    void animateEye(const std::string& suffix, float duration);

    /* FUNC: EntityAnimatedHuman::animateToolBeganAt @ 0x100179437 */
    double getLastSwungToolAt() const { return _lastSwungToolAt; }

protected:
    spine::Bone* _toolBone;             // EntityAnimatedHuman::toolBone @ 0x100315028
    spine::Bone* _toolArmBone;          // EntityAnimatedHuman::toolArmBone @ 0x100315040
    spine::Bone* _toolElbowBone;        // EntityAnimatedHuman::toolElbowBone @ 0x100315048
    spine::Bone* _toolHandBone;         // EntityAnimatedHuman::toolHandBone @ 0x100315050
    spine::Slot* _toolGlowSlot;         // EntityAnimatedHuman::toolGlowSlot @ 0x100315030
    bool _bareHanded;                   // EntityAnimatedHuman::bareHanded @ 0x1003150B0
    Item* _suitItem;                    // EntityAnimatedHuman::suitItem @ 0x100315070
    Item* _topsOverlayItem;             // EntityAnimatedHuman::topsOverlayItem @ 0x100315088
    Item* _legsOverlayItem;             // EntityAnimatedHuman::legsOverlayItem @ 0x100315090
    Item* _toolItem;                    // EntityAnimatedHuman::toolItem @ 0x1003150F8
    Item* _targetItem;                  // EntityAnimatedHuman::interactionItem @ 0x100315120
    ax::Point _toolUsePoint;            // EntityAnimatedHuman::toolUsePoint @ 0x100315100
    float _toolRotation;                // EntityAnimatedHuman::toolRotation @ 0x100315108
    ax::Color3B _skinColor;             // EntityAnimatedHuman::skinColor @ 0x1003150A0
    ax::Color3B _hairColor;             // EntityAnimatedHuman::hairColor @ 0x100315098
    ax::Color3B _facialGearGlowColor;   // EntityAnimatedHuman::facialGearGlowColor @ 0x100315080
    ax::Color3B _topsOverlayGlowColor;  // EntityAnimatedHuman::topsOverlayColor @ 0x100315078
    float _facialGearGlow;              // EntityAnimatedHuman::facialGearGlowAmount @ 0x1003150B8
    float _topsOverlayGlow;             // EntityAnimatedHuman::topsOverlayGlowAmount @ 0x1003150C0
    bool _animatingTool;                // EntityAnimatedHuman::animatingTool @ 0x1003150D8
    bool _wasAnimatingTool;             // EntityAnimatedHuman::wasAnimatingTool @ 0x1003150E0
    double _lastAnimatedToolAt;         // EntityAnimatedHuman::lastAnimatedToolAt @ 0x100315110
    double _lastSwungToolAt;            // EntityAnimatedHuman::animateToolBeganAt @ 0x100315118
    double _nextEyeChangeAt;            // EntityAnimatedHuman::nextEyeChangeAt @ 0x100315130
    bool _lastEyeWasBlink;              // EntityAnimatedHuman::lastEyeWasBlink @ 0x100315138
    Item* _topsItem;
};

}  // namespace opendw

#endif  // __ENTITY_ANIMATED_HUMAN_H__
