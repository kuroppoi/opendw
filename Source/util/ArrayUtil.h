#ifndef __ARRAY_UTIL_H__
#define __ARRAY_UTIL_H__

#include "axmol.h"

namespace opendw::array_util
{

template <typename... T>
ax::ValueVector arrayOf(T... t)
{
    ax::ValueVector result;
    ([&] { result.push_back(ax::Value(t)); }(), ...);
    return result;
}

/* Converts a standard vector of elements into a ValueVector. */
template <typename T>
ax::ValueVector convert(const std::vector<T>& vector)
{
    ax::ValueVector result;

    for (auto& element : vector)
    {
        result.push_back(ax::Value(element));
    }

    return result;
}

}  // namespace opendw::array_util

#endif  // __ARRAY_UTIL_H__
