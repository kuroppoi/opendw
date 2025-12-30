#include "EntityAnimatedHuman.h"

#include "util/ColorUtil.h"
#include "util/MapUtil.h"
#include "CommonDefs.h"
#include "GameConfig.h"
#include "Item.h"

#define SLOT_EMPTY ""

USING_NS_AX;

namespace opendw
{

void EntityAnimatedHuman::onEnter()
{
    EntityAnimated::onEnter();
    hideTool();
    hideExo();

    // 0x100174AAE: Set animation mix durations
    _mainSkeleton->setMix("walk", "run", 0.333F);
    _mainSkeleton->setMix("run", "walk", 0.333F);
    _mainSkeleton->setMix("falling-1", "fly", 0.5F);
    _mainSkeleton->setMix("falling-1", "flail", 1.0F);
    _mainSkeleton->setMix("idle-1", "idle-2", 0.25F);
    _mainSkeleton->setMix("idle-1", "idle-3", 0.25F);
    _mainSkeleton->setMix("idle-1", "idle-4", 0.25F);
    _mainSkeleton->setMix("idle-1", "idle-5", 0.25F);
    _mainSkeleton->setMix("idle-2", "idle-1", 0.25F);
    _mainSkeleton->setMix("idle-3", "idle-1", 0.25F);
    _mainSkeleton->setMix("idle-4", "idle-1", 0.25F);
    _mainSkeleton->setMix("idle-5", "idle-1", 0.25F);

    // 0x100174C32: Initialize appearance
    setSlot("character-facial-hair", SLOT_EMPTY);
    setSlot("character-facialgear", SLOT_EMPTY);
    setSlot("character-hair", SLOT_EMPTY);
    setSlot("character-headgear", SLOT_EMPTY);
    setSlot("suit", SLOT_EMPTY);
    setSlot("character-mouth", SLOT_EMPTY);
    setSlot("suit-extra", SLOT_EMPTY);
    updateAppearance(_details);  // NOTE: Originally only called in EntityAnimatedAvatar
}

void EntityAnimatedHuman::update(float deltaTime)
{
    EntityAnimated::update(deltaTime);

    // 0x100178918: Animate exo overlay
    auto time = utils::gettime();
    setSlotColor("exo-eye-glow", _facialGearGlowColor);
    setSlotColor("exo-torso-glow", _topsOverlayGlowColor);
    setSlotOpacity("exo-eye-glow", _facialGearGlow + sinf(time * 1.234F) * 0.08F);
    setSlotOpacity("exo-torso-glow", _topsOverlayGlow + sinf(time * M_PI) * 0.2F);

    // 0x10017885F: Animate eyes
    if (time >= _nextEyeChangeAt)
    {
        if (_lastEyeWasBlink || rand_0_1() >= 0.5F)
        {
            animateEye("", 3.0F);
            _lastEyeWasBlink = false;
        }
        else
        {
            animateEye("blink", 0.25F);
            _lastEyeWasBlink = true;
        }
    }

    // TODO: update face color
    // TODO: update tool animation
}

Size EntityAnimatedHuman::computeContentSize()
{
    return Size(BLOCK_SIZE * 0.8F, BLOCK_SIZE * 1.6F);
}

void EntityAnimatedHuman::change(const ValueMap& data)
{
    EntityAnimated::change(data);
    updateAppearance(data);
}

void EntityAnimatedHuman::updateAppearance(const ValueMap& data)
{
    auto config               = GameConfig::getMain();
    auto& tops                = map_util::getValue(data, "t");
    auto& bottoms             = map_util::getValue(data, "b");
    auto& headgear            = map_util::getValue(data, "hg");
    auto& footwear            = map_util::getValue(data, "fw");
    auto& hair                = map_util::getValue(data, "h");
    auto& facialHair          = map_util::getValue(data, "fh");
    auto& facialGear          = map_util::getValue(data, "fg");
    auto& suit                = map_util::getValue(data, "u");
    auto& skinColor           = map_util::getValue(data, "c*");
    auto& hairColor           = map_util::getValue(data, "h*");
    auto& topsOverlay         = map_util::getValue(data, "to");
    auto& legsOverlay         = map_util::getValue(data, "lo");
    auto& topsOverlayColor    = map_util::getValue(data, "to*");
    auto& facialGearGlowColor = map_util::getValue(data, "fg*");

    // 0x100175759: Set hair color
    if (!hairColor.isNull())
    {
        _hairColor = color_util::hexToColor(hairColor.asString());
        setSlotColor("character-hair", _hairColor);
        setSlotColor("character-facial-hair", _hairColor);
    }

    // 0x1001758D8: Set hair
    if (!hair.isNull())
    {
        auto item = config->getItemForCode(hair.asInt());

        if (item)
        {
            setSlot("character-hair", item->getSpriteName());
        }
    }

    // 0x10017590A: Set facial hair
    if (!facialHair.isNull())
    {
        auto item = config->getItemForCode(facialHair.asInt());

        if (item)
        {
            setSlot("character-facial-hair", item->getSpriteName());
        }
    }

    // 0x100175A3F: Set torso
    if (!tops.isNull())
    {
        _topsItem = config->getItemForCode(tops.asInt());

        if (_topsItem)
        {
            auto& sprite = _topsItem->getSpriteName();
            auto& color  = _topsItem->getColor();
            setSlot("character-torso", sprite);
            setSlot("character-arm-upper", std::format("{}-arm-upper", sprite));
            setSlot("character-arm-upper1", std::format("{}-arm-upper", sprite));
            setSlot("character-arm-lower", std::format("{}-arm-lower", sprite));
            setSlot("character-arm-lower1", std::format("{}-arm-lower", sprite));
            setSlotColor("character-torso", color);
            setSlotColor("character-arm-upper", color);
            setSlotColor("character-arm-upper1", color);
            setSlotColor("character-arm-lower", color);
            setSlotColor("character-arm-lower1", color);

            if (!_topsOverlayItem || _topsOverlayItem->getCode() == 0)
            {
                auto& hand  = map_util::getValue(_topsItem->getData(), "hand");
                _bareHanded = hand.isNull();

                if (_bareHanded)
                {
                    setSlot("character-hand", "avatar/hand");
                    setSlot("character-hand1", "avatar/hand");
                    setSlotColor("character-hand", _skinColor);
                    setSlotColor("character-hand1", _skinColor);
                }
                else
                {
                    setSlot("character-hand", hand.asString());
                    setSlot("character-hand1", hand.asString());
                    setSlotColor("character-hand", Color3B::WHITE);
                    setSlotColor("character-hand1", Color3B::WHITE);
                }
            }
        }
    }

    // 0x100175781: Set skin color
    if (!skinColor.isNull())
    {
        _skinColor = color_util::hexToColor(skinColor.asString());
        setSlotColor("character-head", _skinColor);
        setSlotColor("character-eye", _skinColor);

        if (_bareHanded)
        {
            setSlotColor("character-hand", _skinColor);
            setSlotColor("character-hand1", _skinColor);
        }
    }

    // 0x10017594D: Set facial gear
    if (!facialGear.isNull())
    {
        auto item = config->getItemForCode(facialGear.asInt());

        if (item)
        {
            auto& sprite = item->getSpriteName();
            auto glow    = item->getGlow();
            setSlot("character-facialgear", sprite);
            setSlotColor("character-facialgear", item->getColor());
            setSlot("exo-eye-glow", glow > 0.0F ? std::format("{}-glow", sprite) : SLOT_EMPTY);
            _facialGearGlow = glow;
        }
    }

    // 0x1001754FC: Set facial gear glow color
    if (!facialGearGlowColor.isNull())
    {
        _facialGearGlowColor = color_util::hexToColor(facialGearGlowColor.asString());
    }

    // 0x100175E4E: Set bottoms
    if (!bottoms.isNull())
    {
        auto item          = config->getItemForCode(bottoms.asInt());
        std::string sprite = item->getSpriteName();  // Explicit copy
        auto& color        = item->getColor();
        auto type          = map_util::getString(item->getData(), "type");

        if (type == "tutu")
        {
            setSlot("character-skirt-l", SLOT_EMPTY);
            setSlot("character-skirt-m", sprite);
            setSlot("character-skirt-r", SLOT_EMPTY);
            sprite = "bottoms/pants";
        }
        else if (type == "dress")
        {
            setSlot("character-skirt-l", std::format("{}-l", sprite));
            setSlot("character-skirt-m", std::format("{}-m", sprite));
            setSlot("character-skirt-r", std::format("{}-r", sprite));
            sprite = "bottoms/pants";
        }
        else
        {
            setSlot("character-skirt-l", SLOT_EMPTY);
            setSlot("character-skirt-m", SLOT_EMPTY);
            setSlot("character-skirt-r", SLOT_EMPTY);
        }

        setSlot("character-leg-upper", std::format("{}-upper", sprite));
        setSlot("character-leg-upper1", std::format("{}-upper", sprite));
        setSlot("character-leg-lower", std::format("{}-lower", sprite));
        setSlot("character-leg-lower1", std::format("{}-lower", sprite));
        setSlotColor("character-leg-upper", color);
        setSlotColor("character-leg-upper1", color);
        setSlotColor("character-leg-lower", color);
        setSlotColor("character-leg-lower1", color);
        setSlotColor("character-skirt-l", color);
        setSlotColor("character-skirt-m", color);
        setSlotColor("character-skirt-r", color);
    }

    // 0x100175CD3: Set footwear
    if (!footwear.isNull())
    {
        auto item    = config->getItemForCode(footwear.asInt());
        auto& sprite = item->getSpriteName();
        auto& color  = item->getColor();
        setSlot("character-foot-upper", std::format("{}-upper", sprite));
        setSlot("character-foot-upper1", std::format("{}-upper", sprite));
        setSlot("character-foot-lower", std::format("{}-lower", sprite));
        setSlot("character-foot-lower1", std::format("{}-lower", sprite));
        setSlotColor("character-foot-upper", color);
        setSlotColor("character-foot-upper1", color);
        setSlotColor("character-foot-lower", color);
        setSlotColor("character-foot-lower1", color);
    }

    // 0x1001761C5: Set headgear
    if (!headgear.isNull())
    {
        auto item = config->getItemForCode(headgear.asInt());

        if (item)
        {
            setSlot("character-headgear", item->getSpriteName());
            setSlotColor("character-headgear", item->getColor());
        }
    }

    // 0x100176227: Set suit
    if (!suit.isNull())
    {
        _suitItem = config->getItemForCode(suit.asInt());

        if (_suitItem)
        {
            setSlot("suit", _suitItem->getSpriteName());
        }
    }

    // 0x100176265: Set tops overlay
    if (!topsOverlay.isNull())
    {
        _topsOverlayItem = config->getItemForCode(topsOverlay.asInt());

        if (_topsOverlayItem)
        {
            auto& sprite = _topsOverlayItem->getSpriteName();
            auto glow    = _topsOverlayItem->getGlow();
            setSlot("character-exo-torso", sprite);
            setSlot("character-exo-arm-upper", std::format("{}-arm-upper", sprite));
            setSlot("character-exo-arm-upper2", std::format("{}-arm-upper", sprite));
            setSlot("character-exo-arm-lower", std::format("{}-arm-lower", sprite));
            setSlot("character-exo-arm-lower2", std::format("{}-arm-lower", sprite));
            setSlot("exo-torso-glow", glow > 0.0F ? std::format("{}-glow", sprite) : SLOT_EMPTY);
            _topsOverlayGlow = glow;

            // BUGFIX: Restore avatar hand on unequip
            if (_topsOverlayItem->getCode() == 0)
            {
                auto& hand  = _topsItem ? map_util::getValue(_topsItem->getData(), "hand") : Value::Null;
                _bareHanded = hand.isNull();

                if (_bareHanded)
                {
                    setSlot("character-hand", "avatar/hand");
                    setSlot("character-hand1", "avatar/hand");
                    setSlotColor("character-hand", _skinColor);
                    setSlotColor("character-hand1", _skinColor);
                }
                else
                {
                    setSlot("character-hand", hand.asString());
                    setSlot("character-hand1", hand.asString());
                    setSlotColor("character-hand", Color3B::WHITE);
                    setSlotColor("character-hand1", Color3B::WHITE);
                }
            }
            else
            {
                setSlot("character-hand", std::format("{}-hand", sprite));
                setSlot("character-hand1", std::format("{}-hand", sprite));
                setSlotColor("character-hand", Color3B::WHITE);
                setSlotColor("character-hand1", Color3B::WHITE);
                _bareHanded = false;
            }
        }
    }

    // 0x1001754C9: Set tops overlay glow color
    if (!topsOverlayColor.isNull())
    {
        _topsOverlayGlowColor = color_util::hexToColor(topsOverlayColor.asString());
    }

    // 0x100176540: Set legs overlay
    if (!legsOverlay.isNull())
    {
        _legsOverlayItem = config->getItemForCode(legsOverlay.asInt());

        if (_legsOverlayItem)
        {
            auto& sprite = _legsOverlayItem->getSpriteName();
            setSlot("character-exo-leg-upper", std::format("{}-leg-upper", sprite));
            setSlot("character-exo-leg-upper2", std::format("{}-leg-upper", sprite));
            setSlot("character-exo-leg-lower", std::format("{}-leg-lower", sprite));
            setSlot("character-exo-leg-lower2", std::format("{}-leg-lower", sprite));
            setSlot("character-exo-foot-upper", std::format("{}-foot-upper", sprite));
            setSlot("character-exo-foot-upper2", std::format("{}-foot-upper", sprite));
            setSlot("character-exo-foot-lower", std::format("{}-foot-lower", sprite));
            setSlot("character-exo-foot-lower2", std::format("{}-foot-lower", sprite));
        }
    }
}

void EntityAnimatedHuman::hideTool()
{
    setSlot("tool", SLOT_EMPTY);
    setSlot("tool-end", SLOT_EMPTY);
    setSlot("tool-glow", SLOT_EMPTY);
}

void EntityAnimatedHuman::hideExo()
{
    setSlot("character-exo-torso", SLOT_EMPTY);
    setSlot("exo-torso-glow", SLOT_EMPTY);
    setSlot("exo-eye-glow", SLOT_EMPTY);
    setSlot("character-exo-arm-upper", SLOT_EMPTY);
    setSlot("character-exo-arm-upper2", SLOT_EMPTY);
    setSlot("character-exo-arm-lower", SLOT_EMPTY);
    setSlot("character-exo-arm-lower2", SLOT_EMPTY);
    setSlot("character-exo-hand", SLOT_EMPTY);
    setSlot("character-exo-hand2", SLOT_EMPTY);
    setSlot("character-exo-leg-upper", SLOT_EMPTY);
    setSlot("character-exo-leg-upper2", SLOT_EMPTY);
    setSlot("character-exo-leg-lower", SLOT_EMPTY);
    setSlot("character-exo-leg-lower2", SLOT_EMPTY);
    setSlot("character-exo-foot-upper", SLOT_EMPTY);
    setSlot("character-exo-foot-upper2", SLOT_EMPTY);
    setSlot("character-exo-foot-lower", SLOT_EMPTY);
    setSlot("character-exo-foot-lower2", SLOT_EMPTY);
}

void EntityAnimatedHuman::animateEye(const std::string& suffix, float duration)
{
    if (suffix.empty())
    {
        setSlot("character-eye", "avatar/eye-male");
        setSlot("character-eye-inner", "avatar/inner-eye");
    }
    else
    {
        auto showInner = suffix == "angry" || suffix == "surprised";
        auto inner     = showInner ? std::format("avatar/inner-eye-{}", suffix) : SLOT_EMPTY;
        setSlot("character-eye", std::format("avatar/eye-male-{}", suffix));
        setSlot("character-eye-inner", inner);
    }

    _nextEyeChangeAt = utils::gettime() + duration;
}

}  // namespace opendw
