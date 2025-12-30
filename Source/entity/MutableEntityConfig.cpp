#include "MutableEntityConfig.h"

namespace opendw
{

void MutableEntityConfig::inheritFromConfig(EntityConfig* config)
{
    // 0x100121172: Inherit spine skin
    auto& spineSkin = config->getSpineSkin();

    if (!spineSkin.empty())
    {
        _spineSkin = spineSkin;
    }

    // TODO: finish
}

}  // namespace opendw
