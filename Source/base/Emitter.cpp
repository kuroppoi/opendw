#include "Emitter.h"

#include "base/GameConfig.h"
#include "util/MapUtil.h"
#include "CommonDefs.h"

USING_NS_AX;

namespace opendw
{

Emitter* Emitter::createWithData(const ValueMap& data, const std::string& name)
{
    CREATE_INIT(Emitter, initWithData, data, name);
}

static Vec2 arrayToVec2(const ValueVector& array)
{
    if (array.size() != 2)
    {
        return Vec2::ZERO;
    }

    auto x = array[0].asFloat();
    auto y = array[1].asFloat();
    return Vec2(x, y);
}

static Color4B arrayToColor4(const ValueVector& array)
{
    if (array.size() != 4)
    {
        return Color4B(0, 0, 0, 0);
    }

    auto r = array[0].asUint();
    auto g = array[1].asUint();
    auto b = array[2].asUint();
    auto a = array[3].asUint();
    return Color4B(r, g, b, a);
}

bool Emitter::initWithData(const ValueMap& data, const std::string& name)
{
    // 0x1000EF5B9: Configure basic properties
    _data                 = data;
    _name                 = name;
    _code                 = map_util::getUInt32(data, "code");
    _collides             = map_util::getBool(data, "collides");
    _gravity              = map_util::getBool(data, "gravity");
    _frequency            = map_util::getFloat(data, "frequency");
    _life                 = map_util::getFloat(data, "life");
    _velocityBase         = map_util::getFloat(data, "velocity base");
    _velocityRange        = map_util::getFloat(data, "velocity range");
    _angularVelocityBase  = map_util::getFloat(data, "angular velocity base");
    _angularVelocityRange = map_util::getFloat(data, "angular velocity range");
    _scaleBase            = map_util::getFloat(data, "scale base");
    _scaleRange           = map_util::getFloat(data, "scale range");
    _angleBase            = map_util::getFloat(data, "angle base");
    _angleRange           = map_util::getFloat(data, "angle range");
    _positionRange        = arrayToVec2(map_util::getArray(data, "position range"));
    _colorBase            = arrayToColor4(map_util::getArray(data, "color base"));
    _colorRange           = arrayToColor4(map_util::getArray(data, "color range"));
    _sound                = map_util::getString(data, "sound");
    _localizeSound        = map_util::getBool(data, "localize sound");

    // 0x1000EF967: Configure sprite frames
    auto& sprites = map_util::getValue(data, "sprites");
    auto cache    = SpriteFrameCache::getInstance();

    switch (sprites.getType())
    {
    case Value::Type::STRING:
        _spriteFrames.push_back(cache->getSpriteFrameByName(sprites.asStringRef()));
        break;
    case Value::Type::VECTOR:
        for (auto& sprite : sprites.asValueVector())
        {
            _spriteFrames.push_back(cache->getSpriteFrameByName(sprite.asStringRef()));
        }
        break;
    }

    return true;
}

void Emitter::postInit()
{
    auto name = map_util::getString(_data, "collision emitter");
    _collisionEmitter = GameConfig::getMain()->getEmitterForName(name);
}

}  // namespace opendw
