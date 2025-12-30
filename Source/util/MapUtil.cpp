#include "MapUtil.h"

USING_NS_AX;

namespace opendw::map_util
{

void merge(const ValueMap& src, ValueMap& dst)
{
    for (auto& entry : src)
    {
        dst.insert_or_assign(entry.first, entry.second);
    }
}

// TODO: this functionality will extend to EVERYTHING, not just items, which might not be 100% intended.
ValueMap::const_iterator find(const ValueMap& map, const std::string& key)
{
    auto it = map.find(key);

    if (it != map.end() || !key.find('_'))
    {
        return it;
    }

    std::string nextKey = key;
    std::replace(nextKey.begin(), nextKey.end(), '_', ' ');
    it = map.find(nextKey);

    if (it != map.end())
    {
        return it;
    }

    nextKey = key;
    std::replace(nextKey.begin(), nextKey.end(), '_', '-');
    return map.find(nextKey);
}

const Value& getValue(const ValueMap& map, const std::string& path, const ax::Value& def)
{
    for (size_t i = 0; i < path.size(); i++)
    {
        if (path[i] == '.')
        {
            auto it = find(map, path.substr(0, i));

            if (it == map.end())
            {
                return def;
            }

            auto& next = it.value();

            if (next.getType() != Value::Type::MAP)
            {
                return def;
            }

            // TODO: recursion is not ideal
            return getValue(next.asValueMap(), path.substr(i + 1), def);
        }
        else if (i + 1 == path.size())
        {
            auto it = find(map, path.substr(0, i + 1));

            if (it == map.end())
            {
                return def;
            }

            return it.value();
        }
    }

    return def;
}

const ValueMap& getMap(const ValueMap& map, const std::string& path, const ValueMap& def)
{
    auto& value = getValue(map, path);

    if (value.getType() == Value::Type::MAP)
    {
        return value.asValueMap();
    }

    return def;
}

const ValueVector& getArray(const ValueMap& map, const std::string& path, const ValueVector& def)
{
    auto& value = getValue(map, path);

    if (value.getType() == Value::Type::VECTOR)
    {
        return value.asValueVector();
    }

    return def;
}

std::string getString(const ValueMap& map, const std::string& path, const std::string& def)
{
    auto& value = getValue(map, path);

    switch (value.getTypeFamily())
    {
    case Value::Type::BOOLEAN:
    case Value::Type::STRING:
    case Value::Type::FLOAT:
    case Value::Type::DOUBLE:
    case Value::Type::INTEGER:
        return value.asString();
    default:
        return def;
    }
}

uint32_t getUInt32(const ValueMap& map, const std::string& path, uint32_t def)
{
    return getValue(map, path).asUint(def);
}

uint64_t getUInt64(const ValueMap& map, const std::string& path, uint64_t def)
{
    return getValue(map, path).asUint64(def);
}

int32_t getInt32(const ValueMap& map, const std::string& path, int32_t def)
{
    return getValue(map, path).asInt(def);
}

int64_t getInt64(const ValueMap& map, const std::string& path, int64_t def)
{
    return getValue(map, path).asInt64(def);
}

float getFloat(const ValueMap& map, const std::string& path, float def)
{
    return getValue(map, path).asFloat(def);
}

double getDouble(const ValueMap& map, const std::string& path, double def)
{
    return getValue(map, path).asDouble(def);
}

bool getBool(const ValueMap& map, const std::string& path, bool def)
{
    return getValue(map, path).asBool(def);
}

}  // namespace opendw::map_util
