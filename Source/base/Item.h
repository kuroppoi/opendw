#ifndef __ITEM_H__
#define __ITEM_H__

#include "axmol.h"

namespace opendw
{

class GameConfig;
enum class BlockLayer : uint8_t;
enum class ModType : uint8_t
{
    NONE,
    TILING,
    ROTATION_DEGREES,  // Internal only
    ROTATION = 4,      // Rotation = (mod % 4) * 90
    DECAY,
    FIELD,
    CHANGE,
    SPRITE,
    STACK,
    HEIGHT,
    POSITION = 11  // Internal only; 1 = down, 2 = right, 3 = up, 4 = left
};

enum class SpecialPlacement : uint8_t
{
    NONE,
    FRAMED,
    CREST,
    CLOCK,
    MACHINE,
    UNIQUE
};

// TODO: Move to standalone file
enum class DamageType : uint8_t
{
    NONE,
    BLUDGEONING,
    SLASHING,
    PIERCING,
    CRUSHING,
    ACID,
    FIRE,
    STEAM,
    COLD,
    ENERGY,
    SONIC,
    INK,
    DESSICATION,
    STINK
};

enum class UseType : uint8_t
{
    NONE,
    CHANGE,
    CLIMB,
    PROTECTED,
    PUBLIC,
    SIGN,
    CONTAINER,
    ZONE_TELEPORT,
    GECK,
    COMPOSTER,
    FLY,
    PROPEL,
    HOVER,
    SKILL_BONUS,
    STEAM_BONUS,
    EXPIATOR,
    MINIGAME,
    WARMTH,
    MOVE,
    SWITCH,
    SUPPRESS,
    BUILDING_EXTENSION,
    FIELD_DISPLAY,
    AFTERBURNER,
    UNKNOWN
};

/*
 * CLASS: Item : NSObject @ 0x100316E78
 */
class Item : public ax::Object
{
public:
    enum class Shape
    {
        NONE,
        BOX,
        POLYGONAL
    };

    enum class Action
    {
        NONE,
        MINE,
        DIG,
        SMASH,
        GUN,
        SHIELD,
        HEAL,
        REFILL,
        TELEPORT,
        STEALTH,
        EXOLEG,
        MELEE,
        SKILL_RESET,
        REVIVE,
        NAME_CHANGE
    };

    enum class Fieldable
    {
        YES,
        PLACED,
        NO
    };

    struct InventoryPosition
    {
        int64_t category;
        int64_t slot;
    };

    typedef std::vector<ax::SpriteFrame*> SpriteList;

    /*
     * Model for connector templates in `config-graphics.yml`.
     */
    struct ContinuitySprite
    {
        ax::SpriteFrame* frame;
        SpriteList options;
        uint16_t rotation;
        bool flipX;
        bool flipY;
    };

    typedef std::vector<ContinuitySprite> ContinuitySpriteList;
    typedef std::vector<ContinuitySpriteList> ContinuitySpriteMap;

    /* FUNC: Item::dealloc @ 0x10004DAB0 */
    ~Item() override;

    /* FUNC: Item::itemWithManager:dictionary:name: @ 0x10004A6CE */
    static Item* createWithManager(GameConfig* config, const ax::ValueMap& data, const std::string& name);

    /* FUNC: Item::initWithManager:dictionary:name: @ 0x100004A727 */
    bool initWithManager(GameConfig* config, const ax::ValueMap& data, const std::string& name);

    /* FUNC: Item::postProcess @ 0x10004CB55 */
    void postProcess();

    /* FUNC: Item::processSprites @ 0x100004CBEA */
    void processSprites();

    /* FUNC: Item::continuousFor: @ 0x10004D912 */
    bool isContinuousFor(Item* item) const;

    /* FUNC: Item::config @ 0x10004DD4A */
    const ax::ValueMap& getData() const { return _data; }

    /* FUNC: Item::category @ 0x10004DD6B */
    const std::string& getCategory() const { return _category; }

    /* FUNC: Item::name @ 0x10004DD8C */
    const std::string& getName() const { return _name; }

    /* FUNC: Item::title @ 0x10004DD9D */
    const std::string& getTitle() const { return _title; }

    /* FUNC: Item::code @ 0x10004DD5B */
    uint16_t getCode() const { return _code; }

    /* FUNC: Item::layer @ 0x10004DD7C */
    BlockLayer getLayer() const { return _layer; }

    /* FUNC: Item::mod @ 0x10004DDF1 */
    ModType getModType() const { return _modType; }

    /* FUNC: Item::action @ 0x10004DE12 */
    Action getAction() const { return _action; }

    /* FUNC: Item::inventoryPosition @ 0x10004E00F */
    const InventoryPosition& getInventoryPosition() const { return _inventoryPosition; }

    /* FUNC: Item::specialPlacement @ 0x10004E4E8 */
    SpecialPlacement getSpecialPlacement() const { return _specialPlacement; }

    /* FUNC: Item::inventoryType @ 0x10004E268 */
    const std::string& getInventoryType() const { return _inventoryType; }

    /* FUNC: Item::tooltip @ 0x10004E279 */
    const std::string& getTooltip() const { return _tooltip; }

    /* FUNC: Item::material @ 0x10004DDE0 */
    const std::string& getMaterial() const { return _material; }

    /* FUNC: Item::width @ 0x10004E133 */
    int16_t getWidth() const { return _width; }

    /* FUNC: Item::height @ 0x10004E144 */
    int16_t getHeight() const { return _height; }

    /* FUNC: Item::tool @ 0x10004D688 */
    bool isTool() const { return _tool; }

    /* FUNC: Item::isMiningTool @ 0x10004D6AC */
    bool isMiningTool() const;

    /* FUNC: Item::isSwingableTool @ 0x10004D6C6 */
    bool isSwingableTool() const;

    /* FUNC: Item::isGun @ 0x10004D6FA */
    bool isGun() const;

    /* FUNC: Item::isConsumable @ 0x10004D75F */
    bool isConsumable() const { return _consumable; }

    /* FUNC: Item::isAccessory @ 0x10004D770 */
    bool isAccessory() const { return _accessory; }

    /* FUNC: Item::isEquippableAccessory @ 0x10004D781 */
    bool isEquippableAccessory() const;

    /* FUNC: Item::visible @ 0x10004DE32 */
    bool isVisible() const { return _visible; }

    /* FUNC: Item::tileable @ 0x10004DE65 */
    bool isTileable() const { return _tileable; }

    /* FUNC: Item::opaque @ 0x10004DE76 */
    bool isOpaque() const { return _opaque; }

    /* FUNC: Item::whole @ 0x10004DE87 */
    bool isWhole() const { return _whole; }

    /* FUNC: Item::center @ 0x10004DF11 */
    bool isCentered() const { return _center; }

    /* FUNC: Item::shadow @ 0x10004DE98 */
    bool hasShadow() const { return _shadow; }

    /* FUNC: Item::borderShadow @ 0x10004E497 */
    bool hasBorderShadow() const { return _borderShadow; }

    /* FUNC: Item::mounted @ 0x10004DEA9 */
    bool isMounted() const { return _mounted; }

    /* FUNC: Item::placeable @ 0x10004DFA9 */
    bool isPlaceable() const { return _placeable; }

    /* FUNC: Item::invulnerable @ 0x10004DE43 */
    bool isInvulnerable() const { return _invulnerable; }

    /* FUNC: Item::placeover @ 0x10004DE54 */
    bool canPlaceover() const { return _placeover; }

    /* FUNC: Item::isDiggable @ 0x10004DF98 */
    bool isDiggable() const { return _diggable; }

    /* FUNC: Item::reach @ 0x10004DF22 */
    bool hasReach() const { return _reach; }

    /* FUNC: Item::power @ 0x10004DECB */
    float getPower() const { return _power; }

    /* FUNC: Item::rate @ 0x10004DEDD */
    float getRate() const { return _rate; }

    /* FUNC: Item::jiggle @ 0x10004E301 */
    float getJiggle() const { return _jiggle; }

    /* FUNC: Item::glow @ 0x10004E313 */
    float getGlow() const { return _glow; }

    /* FUNC: Item::light @ 0x10004E0EE */
    float getLight() const { return _light; }

    /* FUNC: Item::placeMod @ 0x10004E28A */
    uint8_t getPlaceMod() const { return _placeMod; }

    /* FUNC: Item::use @ 0x10004E3F1 */
    const ax::ValueMap& getUse() const { return _use; }

    /* FUNC: Item::isUsable @ 0x10004D7C9 */
    bool isUsable() const { return _useMask != 0; }

    /* FUNC: Item::isUsableType: @ 0x10004D7E0 */
    bool isUsableType(UseType use) const;

    /* FUNC: Item::isClimbable @ 0x10004D7FA */
    bool isClimbable() const;

    /* FUNC: Item::useMask @ 0x10004E3E0 */
    uint64_t getUseMask() const { return _useMask; }

    /* FUNC: Item::shape @ 0x10004DDBF */
    Shape getShape() const { return _shape; }

    /* FUNC: Item::shapeDefinition @ 0x10004DDCF */
    const std::string& getShapeDefinition() const { return _shapeDefinition; }

    /* FUNC: Item::field @ 0x10004DF44 */
    int32_t getField() const { return _field; }

    /* FUNC: Item::fieldPlace @ 0x10004DF55 */
    bool canPlaceInField() const { return _fieldPlace; }

    /* FUNC: Item::fieldable @ 0x10004DF88 */
    Fieldable getFieldable() const { return _fieldable; }

    /* FUNC: Item::fieldDamageType @ 0x10004DF66 */
    DamageType getFieldDamageType() const { return _fieldDamageType; }

    /* FUNC: Item::miningSkill @ 0x10004DFCB */
    const std::string& getMiningSkill() const { return _miningSkill; }

    /* FUNC: Item::miningSkillLevel @ 0x10004DFDC */
    int32_t getMiningSkillLevel() const { return _miningSkillLevel; }

    /* FUNC: Item::placingSkill @ 0x10004DFED */
    const std::string& getPlacingSkill() const { return _placingSkill; }

    /* FUNC: Item::placingSkillLevel @ 0x10004DFFE */
    int32_t getPlacingSkillLevel() const { return _placingSkillLevel; }

    /* FUNC: Item::attackInterval @ 0x10004E086 */
    double getAttackInterval() const { return _attackInterval; }

    /* FUNC: Item::lightColor @ 0x10004E100 */
    const ax::Color3B& getLightColor() const { return _lightColor; }

    /* FUNC: Item::lightPosition @ 0x10004E11B */
    const ax::Point& getLightPosition() const { return _lightPosition; }

    /* FUNC: Item::isMirrorable @ 0x10004D726 */
    bool isMirrorable() const { return _mirrorable; }

    /* FUNC: Item::inventoryItem @ 0x10004E246 */
    Item* getInventoryItem() const { return _inventoryItem; }

    /* FUNC: Item::decayInventoryItem @ 0x10004E257 */
    Item* getDecayInventoryItem() const { return _decayInventoryItem; }

    /* FUNC: Item::setParentItem: @ 0x10004E436 */
    void setParentItem(Item* item) { _parentItem = item; }

    /* FUNC: Item::parentItem @ 0x10004E425 */
    Item* getParentItem() const { return _parentItem; }

    /* FUNC: Item::setUseChangeItem: @ 0x10004E458 */
    void setUseChangeItem(Item* item) { _useChangeItem = item; }

    /* FUNC: Item::useChangeItem @ 0x10004E447 */
    Item* getUseChangeItem() const { return _useChangeItem; }

    /* FUNC: Item::setChangeItems: @x10004E47A */
    void setChangeItems(const std::vector<Item*>& changeItems) { _changeItems = changeItems; }

    /* FUNC: Item::changeItems @ 0x10004E469 */
    const std::vector<Item*>& getChangeItems() const { return _changeItems; }

    /* FUNC: Item::color @ 0x10004E2AC */
    const ax::Color3B& getColor() const { return _color; }

    /* FUNC: Item::inventoryFrame @ 0x10004E235 */
    ax::SpriteFrame* getInventoryFrame() const { return _inventoryFrame; }

    /* FUNC: Item::spriteName @ 0x10004E177 */
    const std::string& getSpriteName() const { return _spriteName; }

    /* FUNC: Item::spriteFrame @ 0x10004D5A4 */
    ax::SpriteFrame* getSpriteFrame() const { return _spriteFrame; }

    /* FUNC: Item::spriteOptions @ 0x10004E188 */
    const SpriteList& getSpriteOptions() const { return _spriteOptions; }

    /* FUNC: Item::spriteColor @ 0x10004E199 */
    const ax::Color3B& getSpriteColor() const { return _spriteColor; }

    /* FUNC: Item::spriteAnimation @ 0x10004E209 */
    const SpriteList& getSpriteAnimation() const { return _spriteAnimation; }

    /* FUNC: Item::spriteAnimationColor @ 0x10004E21A */
    const ax::Color3B& getSpriteAnimationColor() const { return _spriteAnimationColor; }

    /* FUNC: Item::backgroundCode @ 0x10004E1E7 */
    ax::SpriteFrame* getBackground() const { return _background; }

    /* FUNC: Item::backgroundOptions @ 0x10004E1F8 */
    const SpriteList& getBackgroundOptions() const { return _backgroundOptions; }

    /* FUNC: Item::continuityCode @ 0x10004E325 */
    const std::string& getContinuity() const { return _continuity; }

    /* FUNC: Item::borderContinuityExternal @ 0x10004E38B */
    const SpriteList& getBorderContinuityExternal() const { return _borderContinuityExternal; }

    /* FUNC: Item::spriteContinuity @ 0x10004E336 */
    const ContinuitySpriteMap& getSpriteContinuity() const { return _spriteContinuity; }

    /* FUNC: Item::spriteContinuityAnimation @ 0x10004E347 */
    const ContinuitySpriteMap& getSpriteContinuityAnimation() const { return _spriteContinuityAnimation; }

    /* FUNC: Item::spriteContinuityAnimationOpacity @ 0x10004E358 */
    uint8_t getSpriteContinuityAnimationOpacity() const { return _spriteContinuityAnimationOpacity; }

    /* FUNC: Item::borderWholeness @ 0x10004E369 */
    const ContinuitySpriteMap& getBorderWholeness() const { return _borderWholeness; }

    /* FUNC: Item::borderContinuity @ 0x10004E37A */
    const ContinuitySpriteMap& getBorderContinuity() const { return _borderContinuity; }

    /* FUNC: Item::borderContinuityColor @ 0x10004E39C */
    const ax::Color3B& getBorderColor() const { return _borderColor; }

    /* FUNC: Item::maskCode @ 0x10004E1B4 */
    ax::SpriteFrame* getMaskFrame() const { return _maskFrame; }

    /* FUNC: Item::maskOptions @ 0x10004E1C5 */
    const SpriteList& getMaskOptions() const { return _maskOptions; }

    /* FUNC: Item::spriteZ @ 0x10004E166 */
    int getSpriteZ() const { return _spriteZ; }

    /* FUNC: Item::sequentialSpriteCodeArray:count:step: @ 0x10004CA4D */
    SpriteList createSequentialSpriteList(const std::string& name, size_t count, size_t step = 1) const;

    /* FUNC: Item::directionalMapForKeys:configKey:prefix:animationFrames:*/
    ContinuitySpriteMap createContinuitySpriteMap(const std::string& name,
                                                  const std::string& type,
                                                  const std::string& prefix,
                                                  size_t frames = 0) const;

private:
    /* SNIPPET: 0x10004A818 - 0X10004A888 */
    static BlockLayer getLayerForName(const std::string& name);

    /* SNIPPET: 0x10004B360 - 0x10004B469 */
    static ModType getModTypeForName(const std::string& name);

    /* SNIPPET: 0x10004BE0B - 0x10004BED2 */
    static SpecialPlacement getSpecialPlacementForName(const std::string& name);

    /* FUNC: 0x10005322D */
    static DamageType getDamageTypeForName(const std::string& name);

    /* FUNC: 0x100052F09 */
    static UseType getUseTypeForName(const std::string& name);

    /* FUNC: 0x10005312E */
    static Action getActionForName(const std::string& name);

    GameConfig* _config;                             // Item::manager @ 0x1003111C8
    ax::ValueMap _data;                              // Item::config @ 0x1003111C0
    std::string _category;                           // Item::category @ 0x1003111D0
    std::string _name;                               // Item::name @ 0x1003111E8
    std::string _title;                              // Item::title @ 0x1003111F0
    uint16_t _code;                                  // Item::code @ 0x1003111E0
    BlockLayer _layer;                               // Item::layer @ 0x1003111D8
    ModType _modType;                                // Item::mod @ 0x100311388
    Action _action;                                  // Item::action @ 0x100311398
    InventoryPosition _inventoryPosition;            // Item::inventoryPosition @ 0x100311310
    SpecialPlacement _specialPlacement;              // Item::specialPlacement @ 0x100311468
    std::string _inventoryType;                      // Item::inventoryType @ 0x1003113A8
    std::string _tooltip;                            // Item::tooltip @ 0x1003113B0
    std::string _material;                           // Item::material @ 0x1003111F8
    int16_t _width;                                  // Item::width @ 0x100311370
    int16_t _height;                                 // Item::height @ 0x100311378
    bool _consumable;                                // Item::consumable @ 0x100311208
    bool _accessory;                                 // Item::accessory @ 0x100311210
    bool _visible;                                   // Item::visible @ 0x100311228
    bool _tileable;                                  // Item::tileable @ 0x100311240
    bool _opaque;                                    // Item::opaque @ 0x100311248
    bool _whole;                                     // Item::whole @ 0x100311250
    bool _center;                                    // Item::center @ 0x100311270
    bool _shadow;                                    // Item::shadow @ 0x100311258
    bool _borderShadow;                              // Item::borderShadow @ 0x1003112B0
    bool _mounted;                                   // Item::mounted @ 0x100311260
    bool _placeable;                                 // Item::isPlaceable @ 0x100311470
    bool _invulnerable;                              // Item::invulnerable @ 0x100311230
    bool _placeover;                                 // Item::placeover = 0x100311238
    bool _diggable;                                  // Item::isDiggable @ 0x1003112D0
    bool _reach;                                     // Item::reach @ 0x100311278
    float _power;                                    // Item::power @ 0x100311288
    float _rate;                                     // Item::rate @ 0x100311290
    float _jiggle;                                   // Item::jiggle @ 0x100311400
    float _glow;                                     // Item::glow @ 0x100311408
    float _light;                                    // Item::light @ 0x1003113B8
    uint8_t _placeMod;                               // Item::placeMod @ 0x100311410
    ax::ValueMap _use;                               // Item::use @ 0x100311418
    uint64_t _useMask;                               // Item::useMask @ 0x100311428
    Shape _shape;                                    // Item::shape @ 0x100311218
    std::string _shapeDefinition;                    // Item::shapeDefinition @ 0x100311220
    int32_t _field;                                  // Item::field @ 0x1003112B8
    bool _fieldPlace;                                // Item::fieldPlace @ 0x1003112C0
    Fieldable _fieldable;                            // Item::fieldable @ 0x1003112C8
    DamageType _fieldDamageType;                     // Item::fieldDamageType @ 0x1003112D8
    std::string _miningSkill;                        // Item::miningSkill @ 0x1003112E8
    int32_t _miningSkillLevel;                       // Item::miningSkillLevel @ 0x1003112F0
    std::string _placingSkill;                       // Item::placingSkill @ 0x1003112F8
    int32_t _placingSkillLevel;                      // Item::placingSkillLevel @ 0x100311300
    double _attackInterval;                          // Item::attackInterval @ 0x100311340
    ax::Color3B _lightColor;                         // Item::lightColor @ 0x1003113C0
    ax::Point _lightPosition;                        // Item::lightPosition @ 0x1003113C8
    Item* _inventoryItem;                            // Item::inventoryItem @ 0x100311498
    Item* _decayInventoryItem;                       // Item::decayInventoryItem @ 0x1003114A0
    Item* _parentItem;                               // Item::parentItem @ 0x100311528
    Item* _useChangeItem;                            // Item::useChangeItem @ 0x100311530
    std::vector<Item*> _changeItems;                 // Item::changeItems @ 0x100311528
    ax::Color3B _color;                              // Item::color @ 0x100311458
    ax::SpriteFrame* _inventoryFrame;                // Item::inventoryFrame @ 0x100311480
    std::string _spriteName;                         // Item::spriteName @ 0x1003114B0
    ax::SpriteFrame* _spriteFrame;                   // Item::spriteCode @ 0x1003114B8
    SpriteList _spriteOptions;                       // Item::spriteOptions @ 0x1003114A8
    ax::Color3B _spriteColor;                        // Item::spriteColor @ 0x1003113E8
    SpriteList _spriteAnimation;                     // Item::spriteAnimation @ 0x1003114D0
    ax::Color3B _spriteAnimationColor;               // Item::spriteAnimationColor @ 0x1003114D8
    ax::SpriteFrame* _background;                    // Item::backgroundCode @ 0x1003114C8
    SpriteList _backgroundOptions;                   // Item::backgroundOptions @ 0x1003114C0
    std::string _continuity;                         // Item::continuityCode @ 0x1003114E0
    SpriteList _borderContinuityExternal;            // Item::borderContinuityExternal @ 0x100311510
    ContinuitySpriteMap _spriteContinuity;           // Item::spriteContinuity @ 0x1003114E8
    ContinuitySpriteMap _spriteContinuityAnimation;  // Item::spriteContinuityAnimation @ 0x1003114F0
    uint8_t _spriteContinuityAnimationOpacity;       // Item::spriteContinuityAnimationOpacity @ 0x1003114F8
    ContinuitySpriteMap _borderWholeness;            // Item::borderWholeness @ 0x100311500
    ContinuitySpriteMap _borderContinuity;           // Item::borderContinuity @ 0x100311508
    ax::Color3B _borderColor;                        // Item::borderContinuityColor @ 0x1003113F0
    ax::SpriteFrame* _maskFrame;                     // Item::maskCode @ 0x1003113D8
    SpriteList _maskOptions;                         // Item::maskOptions @ 0x1003113D0
    int _spriteZ;                                    // Item::spriteZ @ 0x100311518
    bool _tool;
    bool _mirrorable;
};

}  // namespace opendw

#endif  // __ITEM_H__
