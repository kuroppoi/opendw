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

/*
 * CLASS: Item : NSObject @ 0x100316E78
 */
class Item : public ax::Object
{
public:
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

    /* FUNC: Item::processSprites @ 0x100004CBEA */
    void processSprites();

    /* FUNC: Item::continuousFor: @ 0x10004D912 */
    bool isContinuousFor(Item* item) const;

    /* FUNC: Item::config @ 0x10004DD4A */
    const ax::ValueMap& getData() const { return _data; }

    /* FUNC: Item::name @ 0x10004DD8C */
    const std::string& getName() const { return _name; }

    /* FUNC: Item::code @ 0x10004DD5B */
    uint16_t getCode() const { return _code; }

    /* FUNC: Item::layer @ 0x10004DD7C */
    BlockLayer getLayer() const { return _layer; }

    /* FUNC: Item::mod @ 0x10004DDF1 */
    ModType getModType() const { return _modType; }

    /* FUNC: Item::specialPlacement @ 0x10004E4E8 */
    SpecialPlacement getSpecialPlacement() const { return _specialPlacement; }

    /* FUNC: Item::material @ 0x10004DDE0 */
    const std::string& getMaterial() const { return _material; }

    /* FUNC: Item::width @ 0x10004E133 */
    int16_t getWidth() const { return _width; }

    /* FUNC: Item::height @ 0x10004E144 */
    int16_t getHeight() const { return _height; }

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

    /* FUNC: Item::jiggle @ 0x10004E301 */
    float getJiggle() const { return _jiggle; }

    /* FUNC: Item::isMirrorable @ 0x10004D726 */
    bool isMirrorable() const { return _mirrorable; }

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

    /* FUNC: Item::borderColor @ 0x10004E37A */
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

    GameConfig* _config;                             // Item::manager @ 0x1003111C8
    ax::ValueMap _data;                              // Item::config @ 0x1003111C0
    std::string _name;                               // Item::name @ 0x1003111E8
    uint16_t _code;                                  // Item::code @ 0x1003111E0
    BlockLayer _layer;                               // Item::layer @ 0x1003111D8
    ModType _modType;                                // Item::mod @ 0x100311388
    SpecialPlacement _specialPlacement;              // Item::specialPlacement @ 0x100311468
    std::string _material;                           // Item::material @ 0x1003111F8
    int16_t _width;                                  // Item::width @ 0x100311370
    int16_t _height;                                 // Item::height @ 0x100311378
    bool _visible;                                   // Item::visible @ 0x100311228
    bool _tileable;                                  // Item::tileable @ 0x100311240
    bool _opaque;                                    // Item::opaque @ 0x100311248
    bool _whole;                                     // Item::whole @ 0x100311250
    bool _center;                                    // Item::center @ 0x100311270
    bool _shadow;                                    // Item::shadow @ 0x100311258
    bool _borderShadow;                              // Item::borderShadow @ 0x1003112B0
    float _jiggle;                                   // Item::jiggle @ 0x100311400
    Item* _parentItem;                               // Item::parentItem @ 0x100311528
    Item* _useChangeItem;                            // Item::useChangeItem @ 0x100311530
    std::vector<Item*> _changeItems;                 // Item::changeItems @ 0x100311528
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
    bool _mirrorable;
};

}  // namespace opendw

#endif  // __ITEM_H__
