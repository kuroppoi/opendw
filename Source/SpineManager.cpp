#include "SpineManager.h"

#include "rapidjson/document.h"
#include "rapidjson/error/en.h"
#include "rapidjson/writer.h"
#include "spine/SkeletonBatch.h"

#include "util/MapUtil.h"
#include "CommonDefs.h"
#include "GameConfig.h"
#include "GameManager.h"

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
    for (auto skeleton : _skeletons)
    {
        AX_SAFE_DELETE(skeleton);
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

spine::SkeletonData* SpineManager::getSkeletonData(const std::string& name, const std::string& atlasFile, bool local)
{
    auto& cache = local ? _localSkeletonCache : _remoteSkeletonCache;

    if (cache.contains(name))
    {
        return cache[name];
    }

    spine::SkeletonData* skeleton = nullptr;

    if (local)
    {
        // Load from file
        auto file  = std::format("{}.json", name);
        int length = 0;
        auto data  = spine::SpineExtension::readFile(file.c_str(), &length);
        skeleton   = loadSkeletonData(data, length, atlasFile);
    }
    else
    {
        // Load from config
        auto config = GameManager::getInstance()->getConfig();
        AXASSERT(config, "Can't get remote skeleton data before config has been initialized");
        auto json = map_util::getString(config->getData(), std::format("spine.{}", name));
        skeleton  = loadSkeletonData(json.c_str(), json.length(), atlasFile);
    }

    if (skeleton)
    {
        cache[name] = skeleton;
        _skeletons.push_back(skeleton);
    }

    return skeleton;
}

spine::SkeletonData* SpineManager::loadSkeletonData(const char* data, size_t length, const std::string& atlasFile)
{
    AX_ASSERT(data && length >= 0);
    rapidjson::StringBuffer buffer;
    auto error = updateJsonData(data, length, buffer);

    if (!error.empty())
    {
        AXLOGE("[SpineManager] Failed to update JSON data: {}", error);
        return nullptr;
    }

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
    auto skeleton = json->readSkeletonData(buffer.GetString());

    if (!skeleton)
    {
        AXLOGE("[SpineManager] Failed to load skeleton data: {}", json->getError().buffer());
        return nullptr;
    }

    AX_SAFE_DELETE(loader);
    AX_SAFE_DELETE(json);
    return skeleton;
}

std::string SpineManager::updateJsonData(const char* data, size_t length, rapidjson::StringBuffer& buffer)
{
    AX_ASSERT(data && length >= 0);
    rapidjson::Document document;
    document.Parse(data, length);

    if (document.HasParseError())
    {
        auto error = rapidjson::GetParseError_En(document.GetParseError());
        return std::format("Parse error at offset {}: {}", document.GetErrorOffset(), error);
    }

    auto& allocator = document.GetAllocator();
    auto it         = document.FindMember("skins");

    // Skin data must be changed from object -> array
    if (it != document.MemberEnd())
    {
        auto& skins = (*it).value;

        if (skins.IsObject())
        {
            rapidjson::Value result(rapidjson::kArrayType);

            for (auto& entry : skins.GetObj())
            {
                rapidjson::Value skin(rapidjson::kObjectType);
                rapidjson::Value attachments(rapidjson::kObjectType);
                attachments.CopyFrom(entry.value, allocator);
                skin.AddMember("name", entry.name, allocator);
                skin.AddMember("attachments", attachments, allocator);
                result.PushBack(skin, allocator);
            }

            document.RemoveMember(it);
            document.AddMember("skins", result, allocator);
        }
    }

    // Update animation bone rotations
    it = document.FindMember("animations");

    if (it != document.MemberEnd())
    {
        const auto& animations = (*it).value.GetObj();

        for (auto& entry : animations)
        {
            const auto& animation = entry.value.GetObj();
            auto it               = animation.FindMember("bones");

            // Skip if animation has no bone data
            if (it == animation.MemberEnd())
            {
                continue;
            }

            const auto& bones = (*it).value.GetObj();

            for (auto& entry : bones)
            {
                const auto& bone = entry.value.GetObj();
                auto it          = bone.FindMember("rotate");

                // Skip if bone has no rotation data
                if (it == bone.MemberEnd())
                {
                    continue;
                }

                const auto& rotate = (*it).value.GetArray();

                // Map 'angle' to 'value' & remove 'curve'
                // TODO: find out if curve is actually necessary for something
                for (auto& rotation : rotate)
                {
                    auto it = rotation.FindMember("angle");

                    if (it != rotation.MemberEnd())
                    {
                        rotation.AddMember("value", (*it).value, allocator);
                        rotation.RemoveMember("angle");
                    }

                    rotation.RemoveMember("curve");
                }

                // Interpolate rotation values
                for (rapidjson::SizeType i = 0; i + 1 < rotate.Size(); i++)
                {
                    auto& current     = rotate[i]["value"];
                    auto& next        = rotate[i + 1]["value"];
                    auto currentAngle = MATH_DEG_TO_RAD(current.GetDouble());
                    auto nextAngle    = MATH_DEG_TO_RAD(next.GetDouble());
                    auto distance     = atan2(sin(nextAngle - currentAngle), cos(nextAngle - currentAngle));
                    next.SetDouble(MATH_RAD_TO_DEG(currentAngle + distance));
                }
            }
        }
    }

    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);
    return "";  // No error
}

}  // namespace opendw
