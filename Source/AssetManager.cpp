#include "AssetManager.h"

USING_NS_AX;

namespace opendw
{

bool AssetManager::loadBaseSpriteSheets()
{
    return loadSpriteSheets(assets::kBaseAssets);
}

bool AssetManager::loadSpriteSheets(const std::vector<std::string_view>& files)
{
    auto textureCache = Director::getInstance()->getTextureCache();
    auto frameCache   = SpriteFrameCache::getInstance();

    for (auto& file : files)
    {
        frameCache->addSpriteFramesWithFile(file);

        if (frameCache->isSpriteFramesWithFileLoaded(file))
        {
            AXLOGI("[AssetManager] Loaded asset {}", file);
        }
        else
        {
            AXLOGW("[AssetManager] Failed to load asset {}", file);
            return false;
        }
    }

    return true;
}

}  // namespace opendw
