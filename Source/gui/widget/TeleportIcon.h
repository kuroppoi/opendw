#ifndef __TELEPORT_ICON_H__
#define __TELEPORT_ICON_H__

#include "axmol.h"

namespace opendw
{

/*
 * Model for GameCommandZoneSearch zone data.
 */
struct ZoneSearchInfo
{
    std::string documentId;
    std::string name;
    int32_t playerCount;
    int32_t followeeCount;
    int32_t explored;
    std::string biome;
    std::string status;
    std::string accessibility;
    int32_t protection;
    std::string scenario;
};

/*
 * CLASS: TeleportIcon : CCSprite @ 0x10031A6E0
 */
class TeleportIcon : public ax::Node
{
public:
    static TeleportIcon* createWithInfo(const ZoneSearchInfo& info);

    /* FUNC: TeleportIcon::initWithPanel:target: @ 0x100100933 */
    bool initWithInfo(const ZoneSearchInfo& info);

    /* FUNC: TeleportIcon::activate @ 0x1001016E2 */
    void activate();

    /* FUNC: TeleportIcon::moveTo: @ 0x100101455 */
    void moveTo(const ax::Point& point);

    /* FUNC: TeleportIcon::setHighlighted: @ 0x10010130F */
    void setHighlighted(bool highlighted);

    /* FUNC: TeleportIcon::target @ 0x1001019BC */
    const std::string& getTarget() const { return _target; }

    /* FUNC: TeleportIcon::setActive: @ 0x100101A7D */
    void setEnabled(bool enabled) { _enabled = enabled; }

    /* FUNC: TeleportIcon::active @ 0x100101A6C */
    bool isEnabled() const { return _enabled; }

private:
    std::string _target;  // TeleportIcon::target @ 0x100313660
    bool _premium;        // TeleportIcon::premium @ 0x100313620
    bool _inaccessible;   // TeleportIcon::inaccessible @ 0x100313638
    bool _enabled;        // TeleportIcon::active @ 0x100313618
};

}  // namespace opendw

#endif  // __TELEPORT_ICON_H__
