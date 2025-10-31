#ifndef __GAME_CONFIG_H__
#define __GAME_CONFIG_H__

#include "axmol.h"

namespace opendw
{

class Item;

/*
 * CLASS: Config : NSObject @ 0x100316EF0
 */
class GameConfig : public ax::Object
{
public:
    typedef std::vector<ax::SpriteFrame*> SpriteList;
    typedef std::vector<std::vector<std::string>> DecayMaskMap;
    typedef std::map<std::string, SpriteList> DecayMaterialMap;

    static GameConfig* createWithData(const ax::ValueMap& data);

    /* FUNC: Config::initWithDictionary: @ 0x10004E60C */
    bool initWithData(const ax::ValueMap& data);

    /* FUNC: Config::registerItemNamed:config: @ 0x100051AD0 */
    Item* registerItem(const std::string& name, const ax::ValueMap& data);

    /* FUNC: Config::itemForName: @ 0x100051B93 */
    Item* getItemForName(const std::string& name) const;

    /* FUNC: Config::itemForCode: @ 0x100051BB0 */
    Item* getItemForCode(uint16_t code) const;

    /* FUNC: Config::loadBiome: @ 0x10005296D */
    void loadBiome(const std::string& biome);

    /* FUNC: Config::biomeConfig: @ 0x100052922 */
    const ax::ValueMap& getBiomeConfig(const std::string& biome) const;

    /* FUNC: Config::currentBiomeFrame: @ 0x100052AD3 */
    ax::SpriteFrame* getCurrentBiomeFrame(const std::string& frame) const;

    /* FUNC: Config::singleDecayMasks @ 0x10005236D */
    const DecayMaskMap& getSingleDecayMasks() const { return _singleDecayMasks; }

    /* FUNC: Config::singleDecayForMaterial: @ 0x1000523E9 */
    const SpriteList& getSingleDecayForMaterial(const std::string& material) const;

    /* FUNC: Config::data @ 0x100052EA8 */
    const ax::ValueMap& getData() const { return _data; }

private:
    inline static SpriteList sEmptySpriteList;

    ax::ValueMap _data;                       // Config::data @ 0x100311540
    ax::StringMap<Item*> _itemsByName;        // Config::itemsByName @ 0x100311568
    ax::Map<uint16_t, Item*> _itemsByCode;    // Config::itemsByCode @ 0x100311578
    ax::ValueMap _currentBiomeConfig;         // Config::currentBiomeConfig @ 0x1003115D8
    DecayMaskMap _singleDecayMasks;           // Config::singleDecay @ 0x100311598
    DecayMaterialMap _singleDecayByMaterial;  // Config::singleDecayByMaterial @ 0x1003115A8
    uint16_t _maxItemCode = 0;
};

}  // namespace opendw

#endif  // __GAME_CONFIG_H__
