#ifndef __SPINE_MANAGER_H__
#define __SPINE_MANAGER_H__

#include "rapidjson/stringbuffer.h"
#include "spine/Atlas.h"
#include "spine/SkeletonData.h"
#include "spine/TextureLoader.h"
#include "spine/spine-axmol.h"
#include "axmol.h"

namespace opendw
{

/*
 * Class responsible for updating and managing skeleton data.
 */
class SpineManager : public ax::Object
{
public:
    ~SpineManager() override;

    static SpineManager* getInstance();
    static void destroyInstance();

    spine::SkeletonData* getSkeletonData(const std::string& name, const std::string& atlasFile, bool local = false);
    spine::SkeletonData* loadSkeletonData(const char* data, size_t length, const std::string& atlasFile);

    std::string updateJsonData(const char* data, size_t length, rapidjson::StringBuffer& buffer);

private:
    spine::AxmolTextureLoader _loader;
    std::map<std::string, spine::Atlas*> _atlasCache;
    std::map<std::string, spine::SkeletonData*> _localSkeletonCache;
    std::map<std::string, spine::SkeletonData*> _remoteSkeletonCache;
    std::vector<spine::SkeletonData*> _skeletons;  // Used for deletion
};

}  // namespace opendw

#endif  // __SPINE_MANAGER_H__
