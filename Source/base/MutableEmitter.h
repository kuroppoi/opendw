#ifndef __MUTABLE_EMITTER_H__
#define __MUTABLE_EMITTER_H__

#include "base/Emitter.h"

namespace opendw
{

class MutableEmitter : public Emitter
{
public:
    virtual ~MutableEmitter() override;

    CREATE_FUNC(MutableEmitter);

    static MutableEmitter* createWithEmitter(Emitter* emitter);

    bool init() { return true; }
    bool initWithEmitter(Emitter* emitter);

    void setCollides(bool collides) { _collides = collides; }
    void setGravity(bool gravity) { _gravity = gravity; }

    void setFrequency(float frequency) { _frequency = frequency; }
    void setLife(float life) { _life = life; }

    void setVelocityBase(float velocityBase) { _velocityBase = velocityBase; }
    void setVelocityRange(float velocityRange) { _velocityRange = velocityRange; }

    void setAngularVelocityBase(float angularVelocityBase) { _angularVelocityBase = angularVelocityBase; }
    void setAngularVelocityRange(float angularVelocityRange) { _angularVelocityRange = angularVelocityRange; }

    void setScaleBase(float scaleBase) { _scaleBase = scaleBase; }
    void setScaleRange(float scaleRange) { _scaleRange = scaleRange; }

    void setAngleBase(float angleBase) { _angleBase = angleBase; }
    void setAngleRange(float angleRange) { _angleRange = angleRange; }

    void setPositionRange(const ax::Vec2& positionRange) { _positionRange = positionRange; }

    void setColorBase(const ax::Color4B& colorBase) { _colorBase = colorBase; }
    void setColorRange(const ax::Color4B& colorRange) { _colorRange = colorRange; }

    void setSound(const std::string& sound) { _sound = sound; }
    void setLocalizeSound(bool localizeSound) { _localizeSound = localizeSound; }

    void setSpriteFrames(const std::vector<ax::SpriteFrame*>& spriteFrames) { _spriteFrames = spriteFrames; }

    void setCollisionEmitter(Emitter* emitter);
    MutableEmitter* getCollisionEmitter() const;
};

}  // namespace opendw

#endif  // __MUTABLE_EMITTER_H__
