#ifndef __COLOR_UTIL_H__
#define __COLOR_UTIL_H__

#include "math/Color.h"

namespace opendw::color_util
{

ax::Color3B lerpColor(const ax::Color3B& from, const ax::Color3B& to, float alpha);
ax::Color3B saturate(const ax::Color3B& color, float alpha);
ax::Color3B hexToColor(const std::string& hex);

}  // namespace opendw::color_util

#endif  // __COLOR_UTIL_H__
