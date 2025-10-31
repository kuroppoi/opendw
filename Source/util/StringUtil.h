#ifndef __STRING_UTIL_H__
#define __STRING_UTIL_H__

#include "axmol.h"
#include <string>

namespace opendw::string_util
{

template <typename T>
std::string join(const T& t, const std::string& separator = ", ")
{
    std::ostringstream result;

    for (auto it = t.begin(); it != t.end(); it++)
    {
        auto value = *it;
        result << value;

        if (it + 1 != t.end())
        {
            result << separator;
        }
    }

    return result.str();
}

}  // namespace opendw::string_util

#endif  // __STRING_UTIL_H__
