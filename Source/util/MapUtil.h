#ifndef __MAP_UTIL_H__
#define __MAP_UTIL_H__

#include "axmol.h"

namespace opendw::map_util
{

template <typename... T>
ax::ValueMap mapOf(T... t)
{
    ax::ValueMap result;
    std::string key;
    size_t i = 0;

    ([&] {
        if (i % 2 == 0)
        {
            key = t;
        }
        else
        {
            result[key] = t;
        }

        i++;
    }(), ...);

    return result;
}

void merge(const ax::ValueMap& src, ax::ValueMap& dst);

ax::ValueMap::const_iterator find(const ax::ValueMap& map, const std::string& key);

const ax::Value& getValue(const ax::ValueMap& map, const std::string& path, const ax::Value& def = ax::Value::Null);
const ax::ValueMap& getMap(const ax::ValueMap& map, const std::string& path, const ax::ValueMap& def = ax::ValueMapNull);
const ax::ValueVector& getArray(const ax::ValueMap& map, const std::string& path, const ax::ValueVector& def = ax::ValueVectorNull);

std::string getString(const ax::ValueMap& map, const std::string& path, const std::string& def = "");

uint32_t getUInt32(const ax::ValueMap& map, const std::string& path, uint32_t def = 0);
uint64_t getUInt64(const ax::ValueMap& map, const std::string& path, uint64_t def = 0);

int32_t getInt32(const ax::ValueMap& map, const std::string& path, int32_t def = 0);
int64_t getInt64(const ax::ValueMap& map, const std::string& path, int64_t def = 0);

float getFloat(const ax::ValueMap& map, const std::string& path, float def = 0.0F);
double getDouble(const ax::ValueMap& map, const std::string& path, double def = 0.0);

bool getBool(const ax::ValueMap& map, const std::string& path, bool def = false);

}  // namespace opendw::map_util

#endif  // __MAP_UTIL_H__
