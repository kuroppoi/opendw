#include "MathUtil.h"

USING_NS_AX;

namespace opendw::math_util
{

Vec2 rotateVector(const Vec2& vector, float rotation)
{
    auto radians = MATH_DEG_TO_RAD(rotation);
    auto cos     = cosf(radians);
    auto sin     = sinf(radians);
    auto x       = vector.x * cos + vector.y * sin;
    auto y       = vector.x * sin - vector.y * cos;
    return Vec2(x, y);
}

Rect clampRect(const Rect& rectA, const Rect& rectB, Vec2* offset)
{
    auto minX   = fmaxf(rectA.getMinX(), rectB.getMinX());
    auto minY   = fmaxf(rectA.getMinY(), rectB.getMinY());
    auto maxX   = fminf(rectA.getMaxX(), rectB.getMaxX());
    auto maxY   = fminf(rectA.getMaxY(), rectB.getMaxY());
    auto result = Rect(minX, minY, maxX - minX, maxY - minY);

    // Calculate and store offset between rectA and clamped rect
    if (offset)
    {
        if (rectA.getMaxX() > rectB.getMaxX())
        {
            offset->x = rectB.getMaxX() - rectA.getMaxX();
        }
        else if (rectA.getMinX() < rectB.getMinX())
        {
            offset->x = rectB.getMinX() - rectA.getMinX();
        }

        if (rectA.getMaxY() > rectB.getMaxY())
        {
            offset->y = rectA.getMaxY() - rectB.getMaxY();
        }
        else if (rectA.getMinY() < rectB.getMinY())
        {
            offset->y = rectA.getMinY() - rectB.getMinY();
        }
    }

    return result;
}

Rect getRectIntersection(const Rect& rectA, const Rect& rectB)
{
    auto minX   = fmaxf(rectA.getMinX(), rectB.getMinX());
    auto minY   = fmaxf(rectA.getMinY(), rectB.getMinY());
    auto maxX   = fminf(rectA.getMaxX(), rectB.getMaxX());
    auto maxY   = fminf(rectA.getMaxY(), rectB.getMaxY());
    auto width  = fmaxf(0.0F, maxX - minX);
    auto height = fmaxf(0.0F, maxY - minY);
    return Rect(minX, minY, width, height);
}

Size getScaledSize(Node* node)
{
    Vec2 scale(node->getScaleX(), node->getScaleY());
    return node->getContentSize() * scale;
}

float getScaledWidth(Node* node)
{
    return node->getContentSize().width * node->getScaleX();
}

float getScaledHeight(Node* node)
{
    return node->getContentSize().height * node->getScaleY();
}

}  // namespace opendw::math_util
