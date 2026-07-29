#include "ColorUtil.h"

#include "util/MathUtil.h"

USING_NS_AX;

namespace opendw::color_util
{

Color3B lerpColor(const Color3B& from, const Color3B& to, float alpha, bool forceStep)
{
    auto r = math_util::lerpi(from.r, to.r, alpha, forceStep);
    auto g = math_util::lerpi(from.g, to.g, alpha, forceStep);
    auto b = math_util::lerpi(from.b, to.b, alpha, forceStep);
    return Color3B(r, g, b);
}

Color3B saturate(const Color3B& color, float alpha)
{
    auto average = static_cast<float>(color.r + color.g + color.b) / 3.0F;
    auto r       = math_util::lerp(color.r, average, alpha);
    auto g       = math_util::lerp(color.g, average, alpha);
    auto b       = math_util::lerp(color.b, average, alpha);
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

Color3B rgbToColor(int rgb)
{
    auto r = static_cast<uint8_t>((rgb >> 16) & 0xFF);
    auto g = static_cast<uint8_t>((rgb >> 8) & 0xFF);
    auto b = static_cast<uint8_t>(rgb & 0xFF);
    return Color3B(r, g, b);
}

Color4B rgbaToColor(int rgba)
{
    auto r = static_cast<uint8_t>((rgba >> 24) & 0xFF);
    auto g = static_cast<uint8_t>((rgba >> 16) & 0xFF);
    auto b = static_cast<uint8_t>((rgba >> 8) & 0xFF);
    auto a = static_cast<uint8_t>(rgba & 0xFF);
    return Color4B(r, g, b, a);
}

Color4B randomColorRanged(const Color4B& base, const Color4B& range)
{
    auto r = clampf(base.r + range.r * rand_minus1_1() * 0.5F, 0.0F, 255.0F);
    auto g = clampf(base.g + range.g * rand_minus1_1() * 0.5F, 0.0F, 255.0F);
    auto b = clampf(base.b + range.b * rand_minus1_1() * 0.5F, 0.0F, 255.0F);
    auto a = clampf(base.a + range.a * rand_minus1_1() * 0.5F, 0.0F, 255.0F);
    return Color4B(r, g, b, a);
}

}  // namespace opendw::color_util
