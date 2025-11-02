#include "ColorUtil.h"

#include "math/MathUtil.h"

USING_NS_AX;

namespace opendw::color_util
{

Color3B lerpColor(const Color3B& from, const Color3B& to, float alpha)
{
    auto r = MathUtil::lerp(from.r, to.r, alpha);
    auto g = MathUtil::lerp(from.g, to.g, alpha);
    auto b = MathUtil::lerp(from.b, to.b, alpha);
    return Color3B(r, g, b);
}

Color3B saturate(const Color3B& color, float alpha)
{
    auto average = static_cast<float>(color.r + color.g + color.b) / 3.0F;
    auto r       = MathUtil::lerp(color.r, average, alpha);
    auto g       = MathUtil::lerp(color.g, average, alpha);
    auto b       = MathUtil::lerp(color.b, average, alpha);
    return Color3B(r, g, b);
}

Color3B hexToColor(const std::string& hex)
{
    uint8_t offset = hex.size() == 7 && hex[0] == '#';

    if (hex.size() - offset != 6)
    {
        return Color3B::WHITE;
    }

    auto r = static_cast<uint8_t>(std::stoi(hex.substr(offset, 2), nullptr, 16) & 0xFF);
    auto g = static_cast<uint8_t>(std::stoi(hex.substr(offset + 2, 2), nullptr, 16) & 0xFF);
    auto b = static_cast<uint8_t>(std::stoi(hex.substr(offset + 4, 2), nullptr, 16) & 0xFF);
    return Color3B(r, g, b);
}

Color4B hexToColor4(const std::string& hex)
{
    uint8_t offset = hex.size() == 9 && hex[0] == '#';

    if (hex.size() - offset != 8)
    {
        return Color4B::WHITE;
    }

    auto r = static_cast<uint8_t>(std::stoi(hex.substr(offset, 2), nullptr, 16) & 0xFF);
    auto g = static_cast<uint8_t>(std::stoi(hex.substr(offset + 2, 2), nullptr, 16) & 0xFF);
    auto b = static_cast<uint8_t>(std::stoi(hex.substr(offset + 4, 2), nullptr, 16) & 0xFF);
    auto a = static_cast<uint8_t>(std::stoi(hex.substr(offset + 6, 2), nullptr, 16) & 0xFF);
    return Color4B(r, g, b, a);
}

}  // namespace opendw::color_util
