#ifndef __EMITTER_H__
#define __EMITTER_H__

#include "axmol.h"

namespace opendw
{

/*
 * CLASS: Emitter : NSObject @ 0x10031A028
 *
 * Configuration model for particle effects.
 */
class Emitter : public ax::Object
{
public:
    static Emitter* createWithData(const ax::ValueMap& data, const std::string& name);

    /* FUNC: Emitter::initWithDictionary: @ 0x1000EF55D */
    bool initWithData(const ax::ValueMap& data, const std::string& name);

    /* FUNC: Emitter::name @ 0x1000EFC0C */
    const std::string& getName() const { return _name; }

    /* FUNC: Emitter::code @ 0x1000EFC3A */
    uint16_t getCode() const { return _code; }

    /* FUNC: Emitter::collides @ 0x1000EFE75 */
    bool hasCollision() const { return _collides; }

    /* FUNC: Emitter::gravity @ 0x1000EFE96 */
    bool hasGravity() const { return _gravity; }

    /* FUNC: Emitter::frequency @ 0x1000EFDED */
    float getFrequency() const { return _frequency; }

    /* FUNC: Emitter::life @ 0x1000EFC79 */
    float getLife() const { return _life; }

    /* FUNC: Emitter::velocityBase @ 0x1000EFD15 */
    float getVelocityBase() const { return _velocityBase; }

    /* FUNC: Emitter::velocityRange @ 0x1000EFD39 */
    float getVelocityRange() const { return _velocityRange; }

    /* FUNC: Emitter::angularVelocityBase @ 0x1000EFD5D */
    float getAngularVelocityBase() const { return _angularVelocityBase; }

    /* FUNC: Emitter::angularVelocityRange @ 0x1000EFD81 */
    float getAngularVelocityRange() const { return _angularVelocityRange; }

    /* FUNC: Emitter::scaleBase @ 0x1000EFC9D */
    float getScaleBase() const { return _scaleBase; }

    /* FUNC: Emitter::scaleRange @ 0x1000EFCC1 */
    float getScaleRange() const { return _scaleRange; }

    /* FUNC: Emitter::angleBase @ 0x1000EFDA5 */
    float getAngleBase() const { return _angleBase; }

    /* FUNC: Emitter::angleRange @ 0x1000EFDC9 */
    float getAngleRange() const { return _angleRange; }

    /* FUNC: Emitter::positionRange @ 0x1000EFCE5 */
    const ax::Vec2& getPositionRange() const { return _positionRange; }

    /* FUNC: Emitter::colorBase @ 0x1000EFE11 */
    const ax::Color4B& getColorBase() const { return _colorBase; }

    /* FUNC: Emitter::colorRange @ 0x1000EFE31 */
    const ax::Color4B& getColorRange() const { return _colorRange; }

    /* FUNC: Emitter::sound @ 0x1000EFEE5 */
    const std::string& getSound() const { return _sound; }

    /* FUNC: Emitter::localizeSound @ 0x1000EFF13 */
    bool shouldLocalizeSound() const { return _localizeSound; }

    /* FUNC: Emitter::spriteCodes @ 0x1000EFC4B */
    const std::vector<ax::SpriteFrame*>& getSpriteFrames() const { return _spriteFrames; }

protected:
    std::string _name;                            // Emitter::name @ 0x1003132C8
    uint16_t _code;                               // Emitter::code @ 0x1003132D0
    bool _collides;                               // Emitter::collides @ 0x100313310
    bool _gravity;                                // Emitter::gravity @ 0x100313318
    float _frequency;                             // Emitter::frequency @ 0x100313320
    float _life;                                  // Emitter::life @ 0x100313328
    float _velocityBase;                          // Emitter::velocityBase @ 0x100313330
    float _velocityRange;                         // Emitter::velocityRange @ 0x100313338
    float _angularVelocityBase;                   // Emitter::angularVelocityBase @ 0x100313340
    float _angularVelocityRange;                  // Emitter::angularVelocityRange @ 0x100313348
    float _scaleBase;                             // Emitter::scaleBase @ 0x1003132D8
    float _scaleRange;                            // Emitter::scaleRange @ 0x1003132E0
    float _angleBase;                             // Emitter::angleBase @ 0x1003132F0
    float _angleRange;                            // Emitter::angleRange @ 0x1003132F8
    ax::Vec2 _positionRange;                      // Emitter::positionRange @ 0x1003132E8
    ax::Color4B _colorBase;                       // Emitter::colorBase @ 0x100313300
    ax::Color4B _colorRange;                      // Emitter::colorRange @ 0x100313308
    std::string _sound;                           // Emitter::sound @ 0x100313358
    bool _localizeSound;                          // Emitter::localizeSound @ 0x100313360
    std::vector<ax::SpriteFrame*> _spriteFrames;  // Emitter::spriteCodes @ 0x100313368
};

}  // namespace opendw

#endif  // __EMITTER_H__
