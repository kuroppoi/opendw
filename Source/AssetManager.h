#ifndef __ASSET_MANAGER_H__
#define __ASSET_MANAGER_H__

#include "axmol.h"

namespace opendw
{

class AssetManager
{
public:
    /* FUNC: GameManager::loadBaseSpriteSheets @ 0x10003A950 */
    static void loadBaseSpriteSheets();

    /* FUNC: GameManager::loadSpriteSheets:frames:texture:format: @ 0x10003B5D1 */
    static void loadSpriteSheets(const std::vector<std::string_view>& files);
};

namespace assets
{

// Atlas files
inline static const auto kAccentsAtlas          = "accents+hd2.plist"sv;
inline static const auto kBackAtlas             = "back+hd2.plist"sv;
inline static const auto kBaseAtlas             = "base+hd2.plist"sv;
inline static const auto kEffectsAtlas          = "effects+hd2.plist"sv;
inline static const auto kEntitiesAtlas         = "entities+hd2.plist"sv;
inline static const auto kFront0Atlas           = "front-0+hd2.plist"sv;
inline static const auto kFront1Atlas           = "front-1+hd2.plist"sv;
inline static const auto kFrontQualityAtlas     = "front-quality+hd2.plist"sv;
inline static const auto kFrontWholeAtlas       = "front-whole+hd2.plist"sv;
inline static const auto kGuiAtlas              = "guiv2.plist"sv;
inline static const auto kLiquidAtlas           = "liquid+hd2.plist"sv;
inline static const auto kMasksAtlas            = "masks+hd2.plist"sv;
inline static const auto kSignsAtlas            = "signs+hd2.plist"sv;
inline static const auto kTitleAtlas            = "title.plist"sv;
inline static const auto kBiomeArcticAtlas      = "biome-arctic+hd2.plist"sv;
inline static const auto kBiomeArcticBgAtlas    = "biome-arctic-background+hd2.plist"sv;
inline static const auto kBiomeBrainAtlas       = "biome-brain+hd2.plist"sv;
inline static const auto kBiomeBrainBgAtlas     = "biome-brain-background+hd2.plist"sv;
inline static const auto kBiomeDeepAtlas        = "biome-deep+hd2.plist"sv;
inline static const auto kBiomeDesertAtlas      = "biome-desert+hd2.plist"sv;
inline static const auto kBiomeDesertBgAtlas    = "biome-desert-background+hd2.plist"sv;
inline static const auto kBiomeHellAtlas        = "biome-hell+hd2.plist"sv;
inline static const auto kBiomeHellBgAtlas      = "biome-hell-background+hd2.plist"sv;
inline static const auto kBiomeSpaceAtlas       = "biome-space+hd2.plist"sv;
inline static const auto kBiomeTemperateAtlas   = "biome-temperate+hd2.plist"sv;
inline static const auto kBiomeTemperateBgAtlas = "biome-temperate-background+hd2.plist"sv;

// List of assets to be loaded on boot
inline static const auto kBaseAssets = {kGuiAtlas, kTitleAtlas};

// List of assets to be loaded during first-time login
inline static const auto kGameAssets = {
    kAccentsAtlas,      kBackAtlas,       kBaseAtlas,           kEffectsAtlas,         kEntitiesAtlas,
    kFront0Atlas,       kFront1Atlas,     kFrontQualityAtlas,   kFrontWholeAtlas,      kLiquidAtlas,
    kMasksAtlas,        kSignsAtlas,      kBiomeArcticAtlas,    kBiomeArcticBgAtlas,   kBiomeBrainAtlas,
    kBiomeBrainBgAtlas, kBiomeDeepAtlas,  kBiomeDesertAtlas,    kBiomeDesertBgAtlas,   kBiomeHellAtlas,
    kBiomeHellBgAtlas,  kBiomeSpaceAtlas, kBiomeTemperateAtlas, kBiomeTemperateBgAtlas};

}  // namespace assets

}  // namespace opendw

#endif  // __ASSET_MANAGER_H__
