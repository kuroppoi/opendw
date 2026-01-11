#ifndef __MATH_UTIL_H__
#define __MATH_UTIL_H__

#include "axmol.h"

namespace opendw::math_util
{

ax::Vec2 rotateVector(const ax::Vec2& vector, float rotation);

/* Clamps rect A to rect B and returns the result. */
ax::Rect clampRect(const ax::Rect& rectA, const ax::Rect& rectB, ax::Vec2* offset = nullptr);

/* @return The intersection between both rects. */
ax::Rect getRectIntersection(const ax::Rect& rectA, const ax::Rect& rectB);

/* @return The node's scaled size calculated by multiplying the node's content size by its scale. */
ax::Size getScaledSize(ax::Node* node);

float getScaledWidth(ax::Node* node);
float getScaledHeight(ax::Node* node);

/* @return The distance between 2 points. */
float getDistance(float x, float y, float x2, float y2);

}  // namespace opendw::math_util

#endif // __MATH_UTIL_H__
