#include "MutableEmitter.h"

#include "CommonDefs.h"

namespace opendw
{

MutableEmitter::~MutableEmitter()
{
    AX_SAFE_RELEASE(_collisionEmitter);
}

MutableEmitter* MutableEmitter::createWithEmitter(Emitter* emitter)
{
    CREATE_INIT(MutableEmitter, initWithEmitter, emitter);
}

bool MutableEmitter::initWithEmitter(Emitter* emitter)
{
    _collides             = emitter->hasCollision();
    _gravity              = emitter->hasGravity();
    _frequency            = emitter->getFrequency();
    _life                 = emitter->getLife();
    _velocityBase         = emitter->getVelocityBase();
    _velocityRange        = emitter->getVelocityRange();
    _angularVelocityBase  = emitter->getAngularVelocityBase();
    _angularVelocityRange = emitter->getAngularVelocityRange();
    _scaleBase            = emitter->getScaleBase();
    _scaleRange           = emitter->getScaleRange();
    _angleBase            = emitter->getAngleBase();
    _angleRange           = emitter->getAngleRange();
    _positionRange        = emitter->getPositionRange();
    _colorBase            = emitter->getColorBase();
    _colorRange           = emitter->getColorRange();
    _sound                = emitter->getSound();
    _localizeSound        = emitter->shouldLocalizeSound();
    _spriteFrames         = emitter->getSpriteFrames();
    setCollisionEmitter(emitter->getCollisionEmitter());
    return true;
}

void MutableEmitter::setCollisionEmitter(Emitter* emitter)
{
    AX_SAFE_RELEASE_NULL(_collisionEmitter);

    if (auto mutableEmitter = dynamic_cast<MutableEmitter*>(emitter))
    {
        _collisionEmitter = mutableEmitter;
    }
    else
    {
        _collisionEmitter = emitter ? MutableEmitter::createWithEmitter(emitter) : nullptr;
    }

    AX_SAFE_RETAIN(_collisionEmitter);
}

MutableEmitter* MutableEmitter::getCollisionEmitter() const
{
    return static_cast<MutableEmitter*>(_collisionEmitter);
}

}  // namespace opendw
