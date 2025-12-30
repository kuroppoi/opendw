#ifndef __ENTITY_CONFIG_H__
#define __ENTITY_CONFIG_H__

#include "axmol.h"

namespace opendw
{

/*
 * CLASS: EntityConfig : NSObject @ 0x10031B018
 */
class EntityConfig : public ax::Object
{
public:
    struct Animation
    {
        std::string name;
        std::string sequence;
        std::string after;
        float rotation;
    };

    static EntityConfig* createWithData(const ax::ValueMap& data);

    /* FUNC: EntityConfig::initWithDictionary: @ 0x10011F7B2 */
    bool initWithData(const ax::ValueMap& data);

    /* FUNC: EntityConfig::code @ 0x100120AA3 */
    int32_t getCode() const { return _code; }

    /* FUNC: EntityConfig::group @ 0x100120AC5 */
    const std::string& getGroup() const { return _group; }

    /* FUNC: EntityConfig::classSuffix @ 0x100120AD6 */
    const std::string& getClassSuffix() const { return _classSuffix; }

    /* FUNC: EntityConfig::size @ 0x100120B2C */
    const ax::Size& getSize() const { return _size; }

    /* FUNC: EntityConfig::scaleBase @ 0x100120C4A */
    float getScaleBase() const { return _scaleBase; }

    /* FUNC: EntityConfig::scaleRange @ 0x100120C5C */
    float getScaleRange() const { return _scaleRange; }

    /* FUNC: EntityConfig::flips @ 0x100120E12 */
    bool doesFlipX() const { return _flips; }

    /* FUNC: EntityConfig::ghostly @ 0x100120E23 */
    bool isGhostly() const { return _ghostly; }

    /* FUNC: EntityConfig::block @ 0x100120AE7 */
    bool isBlock() const { return _block; }

    /* FUNC: EntityConfig::spine @ 0x100120E34 */
    const std::string& getSpine() const { return _spine; }

    /* FUNC: EntityConfig::spineSkin @ 0x100120E45 */
    const std::string& getSpineSkin() const { return _spineSkin; }

    /* FUNC: EntityConfig::spineOffset @ 0x100120E56 */
    const ax::Vec2& getSpineOffset() const { return _spineOffset; }

    /* FUNC: EntityConfig::sprites @ 0x100120C06 */
    const ax::ValueVector& getSprites() const { return _sprites; }

    /* FUNC: EntityConfig::animations @ 0x100120C17 */
    const std::vector<Animation>& getAnimations() const { return _animations; }

    /* FUNC: EntityConfig::slots @ 0x100120E6E */
    const std::vector<std::string>& getSlots() const { return _slots; }

    /* FUNC: EntityConfig::attachments @ 0x100120E7F */
    const std::vector<std::string>& getAttachments() const { return _attachments; }

protected:
    int32_t _code;                          // EntityConfig::code @ 0x100313C48
    std::string _group;                     // EntityConfig::group @ 0x100313C58
    std::string _classSuffix;               // EntityConfig::classSuffix @ 0x100313C78
    ax::Size _size;                         // EntityConfig::size @ 0x100313C80
    float _scaleBase;                       // EntityConfig::scaleBase @ 0x100313D10
    float _scaleRange;                      // EntityConfig::scaleRange @ 0x100313d18
    bool _flips;                            // EntityConfig::flips @ 0x100313DD8
    bool _ghostly;                          // EntityConfig::ghostly @ 0x100313DE0
    bool _block;                            // EntityConfig::block @ 0x100313C60
    std::string _spine;                     // EntityConfig::spine @ 0x100313DE8
    std::string _spineSkin;                 // EntityConfig::spineSkin @ 0x100313DF0
    ax::Vec2 _spineOffset;                  // EntityConfig::spineOffset @ 0x100313DF8
    ax::ValueVector _sprites;               // EntityConfig::sprites @ 0x100313CE8
    std::vector<Animation> _animations;     // EntityConfig::animations @ 0x100313CF0
    std::vector<std::string> _slots;        // EntityConfig::slots @ 0x100313E00
    std::vector<std::string> _attachments;  // EntityConfig::attachments @ 0x100313E08
};

}  // namespace opendw

#endif  // __ENTITY_CONFIG_H__
