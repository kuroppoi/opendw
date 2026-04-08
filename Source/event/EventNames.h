#ifndef __EVENT_NAMES_H__
#define __EVENT_NAMES_H__

namespace opendw::events
{

inline static const auto kDeathMessageChanged     = "deathMessageDidChange";
inline static const auto kPlayerAppearanceChanged = "playerDidChangeAppearance";
inline static const auto kPlayerDeathEvent        = "playerDidDie";
inline static const auto kPlayerEntered           = "playerDidEnter";
inline static const auto kPlayerExited            = "playerDidExit";
inline static const auto kPlayerHealthChanged     = "healthDidChange";
inline static const auto kNotifyAlert             = "alert";
inline static const auto kNotifyBigAlert          = "bigAlert";
inline static const auto kSteamChanged            = "steamDidChange";
inline static const auto kSteamCooldownBegan      = "steamCooldownDidBegin";
inline static const auto kSteamCooldownEnded      = "steamCooldownDidEnd";
inline static const auto kZoneTeleportActivated   = "playerDidActivateZoneTeleport";
inline static const auto kZoneTeleportDeactivated = "playerDidDeactivateZoneTeleport";

}  // namespace opendw::events

#endif  // __EVENT_NAMES_H__
