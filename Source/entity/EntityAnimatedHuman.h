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

    /* FUNC: EntityAnimatedHuman::hideTool @ 0x100176ACD */
    void hideTool();

    /* FUNC: EntityAnimatedHuman::hideExo @ 0x100176B27 */
    void hideExo();

    /* FUNC: EntityAnimatedHuman::animateEye:duration: @ 0x100179154 */
    void animateEye(const std::string& suffix, float duration);

protected:
    bool _bareHanded;                   // EntityAnimatedHuman::bareHanded @ 0x1003150B0
    Item* _suitItem;                    // EntityAnimatedHuman::suitItem @ 0x100315070
    Item* _topsOverlayItem;             // EntityAnimatedHuman::topsOverlayItem @ 0x100315088
    Item* _legsOverlayItem;             // EntityAnimatedHuman::legsOverlayItem @ 0x100315090
    ax::Color3B _skinColor;             // EntityAnimatedHuman::skinColor @ 0x1003150A0
    ax::Color3B _hairColor;             // EntityAnimatedHuman::hairColor @ 0x100315098
    ax::Color3B _facialGearGlowColor;   // EntityAnimatedHuman::facialGearGlowColor @ 0x100315080
    ax::Color3B _topsOverlayGlowColor;  // EntityAnimatedHuman::topsOverlayColor @ 0x100315078
    float _facialGearGlow;              // EntityAnimatedHuman::facialGearGlowAmount @ 0x1003150B8
    float _topsOverlayGlow;             // EntityAnimatedHuman::topsOverlayGlowAmount @ 0x1003150C0
    double _nextEyeChangeAt;            // EntityAnimatedHuman::nextEyeChangeAt @ 0x100315130
    bool _lastEyeWasBlink;              // EntityAnimatedHuman::lastEyeWasBlink @ 0x100315138
    Item* _topsItem;
};

}  // namespace opendw

#endif  // __ENTITY_ANIMATED_HUMAN_H__
