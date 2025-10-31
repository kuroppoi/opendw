#include "AssetManager.h"

USING_NS_AX;

namespace opendw
{

void AssetManager::loadBaseSpriteSheets()
{
    loadSpriteSheets(assets::kBaseAssets);
}

void AssetManager::loadSpriteSheets(const std::vector<std::string_view>& files)
{
    auto textureCache = Director::getInstance()->getTextureCache();
    auto frameCache   = SpriteFrameCache::getInstance();

    for (auto& file : files)
    {
        // TODO: there's... no way to know if this actually succeeds or not
        frameCache->addSpriteFramesWithFile(file);
        AXLOGI("[AssetManager] Loaded asset {}", file);
    }
}

}  // namespace opendw
