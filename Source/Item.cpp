#include "Item.h"

#include "util/ColorUtil.h"
#include "util/MapUtil.h"
#include "zone/BaseBlock.h"  // BlockLayer
#include "CommonDefs.h"
#include "GameConfig.h"
#include "GameManager.h"

#define BASE_EARTH        2
#define BASE_LIMESTONE    4
#define BASE_WATER        9
#define BASE_EARTH_ACCENT 15

USING_NS_AX;

namespace opendw
{

Item::~Item()
{
    AX_SAFE_RELEASE(_spriteFrame);
    AX_SAFE_RELEASE(_background);
    AX_SAFE_RELEASE(_maskFrame);
}

Item* Item::createWithManager(GameConfig* config, const ValueMap& data, const std::string& name)
{
    CREATE_INIT(Item, initWithManager, config, data, name);
}

bool Item::initWithManager(GameConfig* config, const ValueMap& data, const std::string& name)
{
    _config           = config;
    _data             = data;
    _name             = name;
    _code             = map_util::getUInt32(data, "code");
    _layer            = getLayerForName(map_util::getString(data, "layer"));
    _modType          = getModTypeForName(map_util::getString(data, "mod"));
    _specialPlacement = getSpecialPlacementForName(map_util::getString(data, "special_placement"));
    _material         = map_util::getString(data, "material");
    _visible          = map_util::getBool(data, "visible", true);
    _tileable         = map_util::getBool(data, "tileable");
    _opaque           = map_util::getBool(data, "opaque");
    _whole            = map_util::getBool(data, "whole");
    _center           = map_util::getBool(data, "center");
    _shadow           = map_util::getBool(data, "shadow");
    _borderShadow     = map_util::getBool(data, "border_shadow");
    _jiggle           = map_util::getFloat(data, "jiggle");
    _glow             = map_util::getFloat(data, "glow");
    _color            = color_util::hexToColor(map_util::getString(data, "color"));
    _mirrorable       = map_util::getString(data, "rotation") == "mirror";
    _spriteZ          = map_util::getInt32(data, "sprite_z");

    // 0x10004B22C: Configure size
    auto& size = map_util::getArray(data, "size");

    if (size.size() >= 2)
    {
        _width  = size[0].asInt();
        _height = size[1].asInt();
    }

    return true;
}

void Item::processSprites()
{
    AX_SAFE_RELEASE_NULL(_spriteFrame);
    AX_SAFE_RELEASE_NULL(_background);
    AX_SAFE_RELEASE_NULL(_maskFrame);
    _spriteOptions.clear();
    _spriteAnimation.clear();
    _backgroundOptions.clear();
    _spriteContinuity.clear();
    _spriteContinuityAnimation.clear();
    _borderWholeness.clear();
    _borderContinuity.clear();
    _borderContinuityExternal.clear();
    _maskOptions.clear();

    // 0x10004CC16: Process sprite options
    auto cache   = SpriteFrameCache::getInstance();
    auto& sprite = map_util::getValue(_data, "sprite");
    _spriteColor = color_util::hexToColor(map_util::getString(_data, "sprite_color"));

    if (sprite.getType() == Value::Type::VECTOR)
    {
        auto& options = sprite.asValueVector();
        AX_ASSERT(options.size() >= 2);
        auto name      = options[0].asString();
        auto count     = options[1].asUint64();
        auto step      = options.size() >= 3 ? options[2].asUint64() : 1;
        _spriteOptions = createSequentialSpriteList(name, count, step);
    }
    else
    {
        // Prioritize spine sprite for clothing
        _spriteName  = map_util::getString(_data, "spine_sprite",
                                          sprite.getType() == Value::Type::STRING ? sprite.asString() : _name);
        _spriteFrame = _config->getCurrentBiomeFrame(_spriteName);
        AX_SAFE_RETAIN(_spriteFrame);
    }

    // 0x10004D042: Process background sprite options
    auto& background = map_util::getValue(_data, "background");

    if (!background.isNull())
    {
        if (background.getType() == Value::Type::VECTOR)
        {
            auto& options = background.asValueVector();
            AX_ASSERT(options.size() >= 2);
            auto name          = options[0].asString();
            auto count         = options[1].asUint64();
            _backgroundOptions = createSequentialSpriteList(name, count);
        }
        else
        {
            _background = _config->getCurrentBiomeFrame(background.asString());
            AX_SAFE_RETAIN(_background);
        }
    }

    // 0x10004D10C: Process sprite animation
    auto& spriteAnimation = map_util::getArray(_data, "sprite_animation");
    _spriteAnimationColor = color_util::hexToColor(map_util::getString(_data, "sprite_animation_color"));

    if (spriteAnimation != ValueVectorNull)
    {
        auto name        = spriteAnimation[0].asString();
        auto count       = spriteAnimation[1].asUint64();
        _spriteAnimation = createSequentialSpriteList(name, count);
    }

    // 0x10004D1D9: Process continuity
    auto continuity      = map_util::getString(_data, "continuity", _name);
    auto continuityFrame = _config->getCurrentBiomeFrame(continuity);
    _continuity          = continuityFrame ? continuityFrame->getName() : continuity;

    // 0x1004D206: Process sprite continuity
    auto& spriteContinuity = map_util::getValue(_data, "sprite_continuity");

    if (!spriteContinuity.isNull())
    {
        auto name         = spriteContinuity.asString();
        auto connector    = map_util::getString(_data, "sprite_continuity_config", "connectors");
        _spriteContinuity = createContinuitySpriteMap(name, connector, "");
    }

    // 0x10004D287: Process sprite continuity animation
    auto& spriteContinuityAnimation   = map_util::getArray(_data, "sprite_continuity_animation");
    _spriteContinuityAnimationOpacity = map_util::getUInt32(_data, "sprite_continuity_animation_opacity");

    if (spriteContinuityAnimation != ValueVectorNull)
    {
        auto name                  = spriteContinuityAnimation[0].asString();
        auto count                 = spriteContinuityAnimation[1].asUint64();
        _spriteContinuityAnimation = createContinuitySpriteMap(name, "connectors", "", count);
    }

    // 0x10004D347: Process border wholenes
    auto& borderWholeness = map_util::getValue(_data, "border_wholeness");

    if (!borderWholeness.isNull())
    {
        auto name        = borderWholeness.asString();
        _borderWholeness = createContinuitySpriteMap(name, "borders", "borders/");
    }

    // 0x10004D395: Process border continuity
    auto& borderContinuity = map_util::getValue(_data, "border_continuity");
    _borderColor           = color_util::hexToColor(map_util::getString(_data, "border_continuity_color"));

    if (!borderContinuity.isNull())
    {
        auto name         = borderContinuity.asString();
        _borderContinuity = createContinuitySpriteMap(name, "borders", "borders/");
    }

    // 0x10004D3E3: Process external border continuity
    auto& borderExternal = map_util::getValue(_data, "border_continuity_external");

    if (!borderExternal.isNull())
    {
        if (borderExternal.getType() == Value::Type::VECTOR)
        {
            auto& options = borderExternal.asValueVector();
            AX_ASSERT(options.size() >= 2);
            auto name                 = options[0].asString();
            auto count                = options[1].asUint64();
            _borderContinuityExternal = createSequentialSpriteList(name, count);
        }
        else
        {
            _borderContinuityExternal = createSequentialSpriteList("masks/border-external", 12);
        }
    }

    // 0x10004B68E: Process mask options
    auto& mask = map_util::getValue(_data, "mask");

    if (!mask.isNull())
    {
        if (mask.getType() == Value::Type::VECTOR)
        {
            auto& options = mask.asValueVector();
            AX_ASSERT(options.size() >= 2);
            auto name    = options[0].asString();
            auto count   = options[1].asUint64();
            _maskOptions = createSequentialSpriteList(name, count);
        }
        else
        {
            _maskFrame = _config->getCurrentBiomeFrame(mask.asString());
            AX_SAFE_RETAIN(_maskFrame);
        }
    }
}

bool Item::isContinuousFor(Item* item) const
{
    auto layer = item->getLayer();
    auto code  = item->getCode();

    // 0x10004D932: Weird hack for base layer continuity
    if (layer == BlockLayer::BASE && _code >= BASE_EARTH &&
        (code == BASE_EARTH || (code > BASE_LIMESTONE && code < BASE_WATER) || code == BASE_EARTH_ACCENT))
    {
        return true;
    }

    return item->getContinuity() == _continuity;
}

Item::SpriteList Item::createSequentialSpriteList(const std::string& name, size_t count, size_t step) const
{
    auto total = count * step;
    SpriteList result;
    result.reserve(total);

    for (size_t i = 0; i < total; i++)
    {
        auto frame = _config->getCurrentBiomeFrame(std::format("{}-{}", name, i / step + 1));

        // BUGFIX: Try name as-is if first sprite option doesn't exist
        if (!frame && i == 0)
        {
            frame = _config->getCurrentBiomeFrame(name);
        }

        result.push_back(frame);  // Push back even if nullptr (necessary for SPRITE mod)
    }

    return result;
}

Item::ContinuitySpriteMap Item::createContinuitySpriteMap(const std::string& name,
                                                          const std::string& type,
                                                          const std::string& prefix,
                                                          size_t frames) const
{
    ContinuitySpriteMap result;
    result.reserve(17);  // Connector entry count
    auto& templateMap = map_util::getMap(_config->getData(), type);

    for (auto i = 0; i < 17; i++)
    {
        auto& sprites = map_util::getArray(templateMap, std::to_string(i));
        ContinuitySpriteList spritesResult;
        spritesResult.reserve(sprites.size());

        for (auto& sprite : sprites)
        {
            auto& array = sprite.asValueVector();
            AX_ASSERT(array.size() >= 2);
            auto suffix    = array[0].isNull() ? "" : std::format("-{}", array[0].asStringRef());
            auto frameName = std::format("{}{}{}", prefix, name, suffix);
            auto frame     = _config->getCurrentBiomeFrame(frameName);
            auto options   = frames > 0 ? createSequentialSpriteList(frameName, frames) : SpriteList();
            auto rotation  = (uint16_t)array[1].asUint();
            auto flipX     = array.size() >= 3 && array[2].asBool();
            auto flipY     = array.size() >= 4 && array[3].asBool();
            spritesResult.push_back(ContinuitySprite(frame, options, rotation, flipX, flipY));
        }

        result.push_back(spritesResult);
    }

    return result;
}

BlockLayer Item::getLayerForName(const std::string& name)
{
    if (name == "base")
        return BlockLayer::BASE;
    else if (name == "back")
        return BlockLayer::BACK;
    else if (name == "front")
        return BlockLayer::FRONT;
    else if (name == "liquid")
        return BlockLayer::LIQUID;

    return BlockLayer::NONE;
}

ModType Item::getModTypeForName(const std::string& name)
{
    if (name == "tiling")
        return ModType::TILING;
    else if (name == "rotation")
        return ModType::ROTATION;
    else if (name == "decay")
        return ModType::DECAY;
    else if (name == "field")
        return ModType::FIELD;
    else if (name == "change")
        return ModType::CHANGE;
    else if (name == "sprite")
        return ModType::SPRITE;
    else if (name == "stack")
        return ModType::STACK;
    else if (name == "height")
        return ModType::HEIGHT;

    return ModType::NONE;
}

SpecialPlacement Item::getSpecialPlacementForName(const std::string& name)
{
    if (name == "framed")
        return SpecialPlacement::FRAMED;
    else if (name == "crest")
        return SpecialPlacement::CREST;
    else if (name == "clock")
        return SpecialPlacement::CLOCK;
    else if (name == "machine")
        return SpecialPlacement::MACHINE;
    else if (name == "unique")
        return SpecialPlacement::UNIQUE;

    return SpecialPlacement::NONE;
}

}  // namespace opendw
