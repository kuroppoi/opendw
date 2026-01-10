#ifndef __META_BLOCK_H__
#define __META_BLOCK_H__

#include "axmol.h"

namespace opendw
{

class Item;

/*
 * CLASS: MetaBlock : NSObject @ 0x10031A438
 */
class MetaBlock : public ax::Object
{
public:
    static MetaBlock* createWithData(int16_t x, int16_t y, Item* item, const ax::ValueMap& metadata);

    /* FUNC: MetaBlock::initWithX:y:item:metadata: @ 0x1000F8CED */
    bool initWithData(int16_t x, int16_t y, Item* item, const ax::ValueMap& metadata);

    /* FUNC: MetaBlock::x @ 0x1000F8F9B */
    int16_t getX() const { return _x; }

    /* FUNC: MetaBlock::y @ 0x1000F8FBD */
    int16_t getY() const { return _y; }

    /* FUNC: MetaBlock::setItem: @ 0x1000F8FF0 */
    void setItem(Item* item) { _item = item; }

    /* FUNC: MetaBlock::item @ 0x1000F8FDF */
    Item* getItem() const { return _item; }

    /* FUNC: MetaBlock::setMetadata: @ 0x1000F8D7B */
    void setMetadata(const ax::ValueMap& metadata);

    /* FUNC: MetaBlock::metadata @ 0x1000F9001 */
    const ax::ValueMap& getMetadata() const { return _metadata; }

    /* FUNC: MetaBlock::playerId @ 0x1000F9012 */
    const std::string& getPlayerId() const { return _playerId; }

private:
    int16_t _x;              // MetaBlock::x @ 0x100313520
    int16_t _y;              // MetaBlock::y @ 0x100313528
    Item* _item;             // MetaBlock::item @ 0x100313530
    ax::ValueMap _metadata;  // MetaBlock::metadata @ 0x100313538
    std::string _playerId;   // MetaBlock::playerId @ 0x100313540
};

}  // namespace opendw

#endif  // __META_BLOCK_H__
