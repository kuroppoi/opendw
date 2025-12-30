#ifndef __SPINE_MANAGER_H__
#define __SPINE_MANAGER_H__

#include "spine/Atlas.h"
#include "spine/SkeletonData.h"
#include "spine/TextureLoader.h"
#include "spine/spine-axmol.h"
#include "axmol.h"

namespace opendw
{

class SpineManager : public ax::Object
{
public:
    ~SpineManager() override;

    static SpineManager* getInstance();
    static void destroyInstance();

    spine::SkeletonData* getSkeletonData(const std::string& name);
    spine::SkeletonData* loadSkeletonData(const char* data, size_t length, const std::string& atlasFile);

private:
    spine::AxmolTextureLoader _loader;
    std::map<std::string, spine::Atlas*> _atlasCache;
    std::map<std::string, spine::SkeletonData*> _skeletonCache;
};

}  // namespace opendw

#endif  // __SPINE_MANAGER_H__
