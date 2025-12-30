#include "EntityConfig.h"

#include "util/MapUtil.h"
#include "CommonDefs.h"

USING_NS_AX;

namespace opendw
{

EntityConfig* EntityConfig::createWithData(const ValueMap& data)
{
    CREATE_INIT(EntityConfig, initWithData, data);
}

bool EntityConfig::initWithData(const ValueMap& data)
{
    _code        = map_util::getInt32(data, "code", -1);
    _group       = map_util::getString(data, "group");
    _classSuffix = map_util::getString(data, "class");
    _flips       = map_util::getBool(data, "flips", true);
    _ghostly     = map_util::getBool(data, "ghostly");
    _block       = map_util::getBool(data, "block");
    _spine       = map_util::getString(data, "spine");
    _spineSkin   = map_util::getString(data, "spine_skin");
    _sprites     = map_util::getArray(data, "sprites");
    _scaleBase   = map_util::getFloat(data, "spine_scale_base", map_util::getFloat(data, "scale_base", 1.0F));  // Orig. scale base
    _scaleRange  = map_util::getFloat(data, "scale_range");  // Orig. scale range

    // 0x10011F944: Configure size
    auto& size = map_util::getArray(data, "size");

    if (size != ValueVectorNull)
    {
        AX_ASSERT(size.size() == 2);
        _size = Size(size[0].asInt(), size[1].asInt());
    }

    // 0x1001206F2: Configure spine offset
    auto& spineOffset = map_util::getArray(data, "spine_offset");

    if (spineOffset != ValueVectorNull)
    {
        AX_ASSERT(spineOffset.size() == 2);
        _spineOffset = Vec2(spineOffset[0].asFloat(), -spineOffset[1].asFloat());  // Negate Y
    }
    // 0x10011FC19: Configure animations
    auto& animations = map_util::getArray(data, "animations", map_util::getArray(data, "spine_animations"));

    if (animations != ValueVectorNull)
    {
        for (auto& animation : animations)
        {
            auto& map     = animation.asValueMap();
            auto name     = map_util::getString(map, "name");
            auto sequence = map_util::getString(map, "sequence_name", map_util::getString(map, "spine_name", name));
            auto after    = map_util::getString(map, "after");
            auto rotation = map_util::getFloat(map, "rotation");
            _animations.push_back(Animation(name, sequence, after, rotation));
        }
    }

    // 0x100120762: Configure slots
    auto& slots = map_util::getArray(data, "slots");

    if (slots != ValueVectorNull)
    {
        for (auto& slot : slots)
        {
            _slots.push_back(slot.asString());
        }
    }

    // 0x100120795: Configure attachments
    auto& attachments = map_util::getArray(data, "attachments");

    if (attachments != ValueVectorNull)
    {
        for (auto& attachment : attachments)
        {
            _attachments.push_back(attachment.asString());
        }
    }

    return true;
}

}  // namespace opendw
