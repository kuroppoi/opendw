#include "SpineManager.h"

#include "spine/SkeletonBatch.h"

#include "CommonDefs.h"

USING_NS_AX;

namespace opendw
{

SpineManager::~SpineManager()
{
    AXLOGD("[SpineManager] In destructor!");
    spine::SkeletonBatch::destroyInstance();  // Axmol doesn't do this itself for some reason

    // Delete cached atlases
    for (auto& atlas : _atlasCache)
    {
        AX_SAFE_DELETE(atlas.second);
    }

    // Delete loaded skeleton data
    for (auto& skeleton : _skeletonCache)
    {
        AX_SAFE_DELETE(skeleton.second);
    }
}

static SpineManager* sInstance;

SpineManager* SpineManager::getInstance()
{
    if (!sInstance)
    {
        sInstance = new SpineManager();
        sInstance->autorelease();
        AX_SAFE_RETAIN(sInstance);
    }

    return sInstance;
}

void SpineManager::destroyInstance()
{
     AX_SAFE_RELEASE_NULL(sInstance);
}

spine::SkeletonData* SpineManager::getSkeletonData(const std::string& name)
{
    if (_skeletonCache.contains(name))
    {
        return _skeletonCache[name];
    }

    auto file      = std::format("{}.json", name);
    auto character = name == "player" || name == "android";
    auto atlasFile = character ? "characters-animated+hd2.atlas" : "entities-animated+hd2.atlas";
    int length     = 0;
    auto data      = spine::SpineExtension::readFile(file.c_str(), &length);
    auto skeleton  = loadSkeletonData(data, length, atlasFile);

    if (skeleton)
    {
        _skeletonCache[name] = skeleton;
    }

    return skeleton;
}

spine::SkeletonData* SpineManager::loadSkeletonData(const char* data, size_t length, const std::string& atlasFile)
{
    AX_ASSERT(data && length >= 0);
    spine::Atlas* atlas = nullptr;

    if (_atlasCache.contains(atlasFile))
    {
        atlas = _atlasCache[atlasFile];
    }
    else
    {
        atlas = new spine::Atlas(atlasFile.c_str(), &_loader);

        if (!atlas)
        {
            AXLOGE("[SpineManager] Failed to load atlas '{}'", atlasFile);
            return nullptr;
        }

        _atlasCache[atlasFile] = atlas;
    }

    auto loader = new spine::AxmolAtlasAttachmentLoader(atlas);
    auto json   = new spine::SkeletonJson(loader);
    json->setScale(BLOCK_SIZE / 500.0F);  // TODO: must be consistent for main menu
    auto skeleton = json->readSkeletonData(data);

    if (!skeleton)
    {
        AXLOGE("[SpineManager] Failed to load skeleton data: {}", json->getError().buffer());
    }

    AX_SAFE_DELETE(loader);
    AX_SAFE_DELETE(json);
    return skeleton;
}

}  // namespace opendw
