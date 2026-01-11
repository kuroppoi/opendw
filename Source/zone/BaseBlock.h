#ifndef __BASE_BLOCK_H__
#define __BASE_BLOCK_H__

#include "axmol.h"

namespace opendw
{

class Item;
class MaskedSprite;
class WorldZone;

enum class BlockLayer : uint8_t
{
    NONE,
    BASE,
    BACK,
    FRONT,
    LIQUID,
    // DEBUG = 9
};

/*
 * CLASS: BaseBlock : NSObject @ 0x100316BA8
 */
class BaseBlock : public ax::Object
{
public:
    /* FUNC: BaseBlock::dealloc @ 0x100033050 */
    ~BaseBlock() override;

    static BaseBlock* createWithZone(WorldZone* zone, int16_t x, int16_t y);

    /* FUNC: BaseBlock::blocksAllocated @ 0x10002E308 */
    static size_t getBlocksAllocated() { return sBlocksAllocated; }

    /* FUNC: BaseBlock::initWithWorldZone:x:y: @ 0x10002E322 */
    bool initWithZone(WorldZone* zone, int16_t x, int16_t y);

    /* FUNC: BaseBlock::setData:idx: @ 0x10002E798 */
    void setData(const ax::ValueVector& data, uint32_t index);

    /* FUNC: BaseBlock::postPlace @ 0x10002F6F1 */
    void postPlace();

    /* FUNC: BaseBlock::updateLight:liquid:wholeness:continuity: @ 0x10002F7E4 */
    void updateEnvironment(bool light = true, bool liquid = true, bool wholeness = true, bool continuity = true);

    /* FUNC: BaseBlock::updateNeighbors @ 0x1000321FF */
    void updateNeighbors();

    /* FUNC: BaseBlock::setItem:mod:forLayer: @ 0x10002EB95 */
    void setLayer(BlockLayer layer, uint16_t item, uint8_t mod);

    /* FUNC: BaseBlock::setItem:forLayer: 0x10002ECC8 */
    void setItemForLayer(BlockLayer layer, uint16_t item);

    /* FUNC: BaseBlock::itemForLayer: @ 0x10002ED83 */
    Item* getItemForLayer(BlockLayer layer) const;

    /* FUNC: BaseBlock::setMod:forLayer: @ 0x10002ED24 */
    void setModForLayer(BlockLayer layer, uint8_t mod);

    /* FUNC: BaseBlock::modForLayer: @ 0x10002EDD9 */
    uint8_t getModForLayer(BlockLayer layer) const;

    /* FUNC: BaseBlock::continuityForLayer: @ 0x10002EE01 */
    uint8_t getContinuityForLayer(BlockLayer layer) const;

    /* FUNC: BaseBlock::wholeness @ 0x1000333F0 */
    uint8_t getWholeness() const { return _wholeness; }

    /* FUNC: BaseBlock::liquidity @ 0x100033456 */
    uint16_t getLiquidity() const { return _liquidity; }

    /* FUNC: BaseBlock::setBase: @ 0x10002EE2A */
    void setBase(uint8_t base);

    /* FUNC: BaseBlock::setBack: @ 0x10002EEC0 */
    void setBack(uint16_t back);

    /* FUNC: BaseBlock::setBackMod: @ 0x10002EEE5 */
    void setBackMod(uint8_t backMod);

    /* FUNC: BaseBlock::setFront: @ 0x10002EF0A */
    void setFront(uint16_t front);

    /* FUNC: BaseBlock::setFrontMod: @ 0x10002EF2F */
    void setFrontMod(uint8_t frontMod);

    /* FUNC: BaseBlock::setLiquid: @ 0x10002F46D */
    void setLiquid(uint8_t liquid);

    /* FUNC: BaseBlock::setLiquidMod: @ 0x10002F50B */
    void setLiquidMod(uint8_t liquidMod);

    /* FUNC: BaseBlock::updateLiquid: @ 0x1000305B9 */
    void updateLiquid(bool updateNeighbors = false);

    /* FUNC: BaseBlock::updateFront @ 0x10002EF54*/
    void updateFront();

    /* FUNC: BaseBlock::updateBack @ 0x10002F289 */
    void updateBack();

    /* FUNC: BaseBlock::isOpaque @ 0x10002F6F1 */
    bool isOpaque() const;
    bool isOpaque(BlockLayer layer) const;
    bool isBackOpaque() const;
    bool isFrontOpaque() const;

    /* FUNC: BaseBlock::clearFromWorld @ 0x10003086F */
    void clearFromWorld();

    /* FUNC: BaseBlock::pushSprite:accessory: @ 0x100032381 */
    void pushSprite(MaskedSprite* sprite);

    /* FUNC: BaseBlock::recycleSprites:inLayer: @ 0x100032916 */
    void recycleSprites(BlockLayer layer = BlockLayer::NONE);

    /* FUNC: BaseBlock::recycleSpriteWithTag: @ 0x100032810 */
    void recycleSpriteWithTag(int tag);

    /* FUNC: BaseBlock::spriteWithTag: @ 0x100032791 */
    MaskedSprite* getSpriteWithTag(int tag) const;

    /* FUNC: BaseBlock::topSpriteForLayer: @ 0x100032436 */
    MaskedSprite* getTopSpriteForLayer(BlockLayer layer) const;

    /* FUNC: BaseBlock::above @ 0x10002E68E */
    BaseBlock* getAbove() const;

    /* FUNC: BaseBlock::below @ 0x10002E6C3 */
    BaseBlock* getBelow() const;

    /* FUNC: BaseBlock::left @ 0x10002E6F8 */
    BaseBlock* getLeft() const;

    /* FUNC: BaseBlock::right @ 0x10002E72D */
    BaseBlock* getRight() const;

    /* FUNC: BaseBlock::setX: @ 0x1000330F9 */
    void setX(int16_t x) { _x = x; }

    /* FUNC: BaseBlock::x @ 0x1000330E8 */
    int16_t getX() const { return _x; }

    /* FUNC: BaseBlock::setY: @ 0x10003311B */
    void setY(int16_t y) { _y = y; }

    /* FUNC: BaseBlock::y @ 0x10003310A */
    int16_t getY() const { return _y; }

    /* FUNC: BaseBlock::base @ 0x10003315A */
    uint8_t getBase() const { return _base; }

    /* FUNC: BaseBlock::liquid @ 0x10003316B */
    uint8_t getLiquid() const { return _liquid; }

    /* FUNC: BaseBlock::liquidMod @ 0x10003317C */
    uint8_t getLiquidMod() const { return _liquidMod; }

    /* FUNC: BaseBlock::back @ 0x10003318D */
    uint16_t getBack() const { return _back; }

    /* FUNC: BaseBlock::backMod @ 0x10003319E */
    uint8_t getBackMod() const { return _backMod; }

    /* FUNC: BaseBlock::front @ 0x1000331AF */
    uint16_t getFront() const { return _front; }

    /* FUNC: BaseBlock::frontMod @ 0x1000331C0 */
    uint8_t getFrontMod() const { return _frontMod; }

    /* FUNC: BaseBlock::frontNatural @ 0x1000331D1 */
    bool isFrontNatural() const { return _frontNatural; }

    /* FUNC: BaseBlock::baseItem @ 0x100033536 */
    Item* getBaseItem() const { return _baseItem; }

    /* FUNC: BaseBlock::backItem @ 0x100033558 */
    Item* getBackItem() const { return _backItem; }

    /* FUNC: BaseBlock::frontItem @ 0x10003357A */
    Item* getFrontItem() const { return _frontItem; }

    /* FUNC: BaseBlock::liquidItem @ 0x10003359C */
    Item* getLiquidItem() const { return _liquidItem; }

    /* FUNC: BaseBlock::setQueuedAt: @ 0x1000334DC */
    void setQueuedAt(double time) { _queuedAt = time; }

    /* FUNC: BaseBlock::queuedAt @ 0x1000334CA */
    double getQueuedAt() const { return _queuedAt; }

    /* FUNC: BaseBlock::setPlacing: 0x100033499 */
    void setPlacing(bool placing) { _placing = placing; }

    /* FUNC: BaseBlock::setRendering: @ 0x1000334BA */
    void setRendering(bool rendering) { _rendering = rendering; }

    /* FUNC: BaseBlock::setCurrentLightR: @ 0x10003330E */
    void setCurrentLightR(float value) { _currentLightR = value; }

    /* FUNC: BaseBlock::currentLightR @ 0x1000332FC */
    float getCurrentLightR() const { return _currentLightR; }

    /* FUNC: BaseBlock::setCurrentLightG: @ 0x100033332 */
    void setCurrentLightG(float value) { _currentLightG = value; }

    /* FUNC: BaseBlock::currentLightG @ 0x100033320 */
    float getCurrentLightG() const { return _currentLightG; }

    /* FUNC: BaseBlock::setCurrentLightB: @ 0x100033356 */
    void setCurrentLightB(float value) { _currentLightB = value; }

    /* FUNC: BaseBlock::currentLightB @ 0x100033344 */
    float getCurrentLightB() const { return _currentLightB; }

    /* FUNC: BaseBlock::setCurrentLightA: @ 0x10003337A */
    void setCurrentLightA(float value) { _currentLightA = value; }

    /* FUNC: BaseBlock::currentLightA @ 0x100033368 */
    float getCurrentLightA() const { return _currentLightA; }

    /* FUNC: BaseBlock::setCurrentLightLit: @ 0x10003339D */
    void setCurrentLightLit(bool value) { _currentLightLit = value; }

    /* FUNC: BaseBlock::currentLightLit @ 0x10003338C */
    bool isCurrentLightLit() const { return _currentLightLit; }

    // Continuity constants
    static constexpr auto CONTINUITY_TOP          = 0b00000001ui8;
    static constexpr auto CONTINUITY_RIGHT        = 0b00000010ui8;
    static constexpr auto CONTINUITY_BOTTOM       = 0b00000100ui8;
    static constexpr auto CONTINUITY_LEFT         = 0b00001000ui8;
    static constexpr auto CONTINUITY_TOP_RIGHT    = 0b00010000ui8;
    static constexpr auto CONTINUITY_BOTTOM_RIGHT = 0b00100000ui8;
    static constexpr auto CONTINUITY_BOTTOM_LEFT  = 0b01000000ui8;
    static constexpr auto CONTINUITY_TOP_LEFT     = 0b10000000ui8;
    static constexpr auto CONTINUITY_CORNERS      = 0b11110000ui8;
    static constexpr auto CONTINUITY_EDGES        = 0b00001111ui8;

private:
    /* Convenience function for grabbing neighbor blocks. */
    void getNeighbors(BaseBlock* neighbors[8]) const;

    inline static size_t sBlocksAllocated = 0;  // 0x10032EAA8

    WorldZone* _zone;                    // BaseBlock::zone @ 0x100310A58
    int16_t _x;                          // BaseBlock::x @ 0x100310A60
    int16_t _y;                          // BaseBlock::y @ 0x100310A68
    uint8_t _base;                       // BaseBlock::base @ 0x100310A80
    uint16_t _back;                      // BaseBlock::back @ 0x100310A88
    uint8_t _backMod;                    // BaseBlock::backMod @ 0x100310A90
    uint16_t _front;                     // BaseBlock::front @ 0x100310A98
    uint8_t _frontMod;                   // BaseBlock::frontMod @ 0x100310AA0
    bool _frontNatural;                  // BaseBlock::frontNatural @ 0x100310AA8
    uint8_t _liquid;                     // BaseBlock::liquid @ 0x100310AB0
    uint8_t _liquidMod;                  // BaseBlock::liquidMod @ 0x100310AB8
    Item* _baseItem;                     // BaseBlock::baseItem @ 0x100310AC0
    Item* _backItem;                     // BaseBlock::backItem @ 0x100310AC8
    Item* _frontItem;                    // BaseBlock::frontItem @ 0x100310AD0
    Item* _liquidItem;                   // BaseBlock::liquidItem @ 0x100310AD8
    double _queuedAt;                    // BaseBlock::queuedAt @ 0x100310BB8
    ax::Vector<MaskedSprite*> _sprites;  // BaseBlock::sprites @ 0x100310B58
    bool _placing;                       // BaseBlock::placing @ 0x100310A70
    bool _rendering;                     // BaseBlock::rendering @ 0x100310A78
    uint8_t _wholeness;                  // BaseBlock::wholeness @ 0x100310B18
    uint8_t _baseContinuity;             // BaseBlock::baseContinuity @ 0x100310AF0
    uint8_t _backContinuity;             // BaseBlock::backContinuity @ 0x100310AE8
    uint8_t _backModContinuity;          // BaseBlock::backModContinuity @ 0x100310B20
    uint8_t _frontContinuity;            // BaseBlock::frontContinuity @ 0x100310AE0
    uint8_t _frontModContinuity;         // BaseBlock::frontModContinuity @ 0x100310B28
    uint16_t _liquidity;                 // BaseBlock::liquidity @ 0x100310B30
    float _currentLightR;                // BaseBlock::currentLightR @ 0x100310B88
    float _currentLightG;                // BaseBlock::currentLightG @ 0x100310B90
    float _currentLightB;                // BaseBlock::currentLightB @ 0x100310B98
    float _currentLightA;                // BaseBlock::currentLightA @ 0x100310BA0
    bool _currentLightLit;               // BaseBlock::currentLightLit @ 0x100310BA8
};

}  // namespace opendw

#endif  // __BASE_BLOCK_H__
