#include "GameConfig.h"

#include "entity/EntityConfig.h"
#include "util/MapUtil.h"
#include "CommonDefs.h"
#include "Item.h"

USING_NS_AX;

namespace opendw
{

GameConfig* GameConfig::createWithData(const ValueMap& data)
{
    CREATE_INIT(GameConfig, initWithData, data);
}

bool GameConfig::initWithData(const ValueMap& data)
{
    AXASSERT(_data.empty(), "Reinitialization is not allowed");
    auto start = utils::gettime();
    _data      = data;

    // 0x10004ED08: Configure items
    size_t itemCount = 0;
    auto itemStart   = utils::gettime();
    auto& items      = map_util::getMap(_data, "items");
    std::vector<Item*> tempItems;
    tempItems.reserve(items.size());
    _itemsByName.reserve(items.size());
    _itemsByCode.reserve(items.size());

    for (auto& entry : items)
    {
        auto item = registerItem(entry.first, entry.second.asValueMap());
        tempItems.push_back(item);
        _maxItemCode = MAX(_maxItemCode, item->getCode());
        itemCount++;
    }

    // 0x10004F326: Configure change items
    for (auto& item : tempItems)
    {
        // 0x10004F160: Configure use change item
        auto& parentData   = item->getData();
        ValueMap useChange = map_util::getMap(parentData, "use.change");  // EXPLICIT: Create copy

        if (useChange != ValueMapNull)
        {
            useChange.insert(parentData.begin(), parentData.end());
            useChange["code"] = _maxItemCode++;
            auto name         = map_util::getString(useChange, "name");
            AX_ASSERT(!name.empty());
            auto child = registerItem(name, useChange);
            child->setParentItem(item);
            item->setUseChangeItem(child);
        }

        // 0x10004F5DD: Configure regular change items
        auto& change = map_util::getArray(parentData, "change");

        if (!change.empty())
        {
            std::vector<Item*> changeItems;
            changeItems.reserve(change.size());
            auto nameSuffix = 1;

            for (auto& element : change)
            {
                ValueMap data = element.asValueMap();  // EXPLICIT: Create copy

                // 0x10004F79C: Inherit parent config if necessary
                if (map_util::getBool(data, "inherit"))
                {
                    data.insert(parentData.begin(), parentData.end());

                    // 0x10004F941: Define sprite name if necessary
                    if (!data.contains("sprite"))
                    {
                        data["sprite"] = item->getName();
                    }
                }

                data["code"] = _maxItemCode++;
                auto name =
                    map_util::getString(data, "name", std::format("{}-change-{}", item->getName(), nameSuffix++));
                auto child = registerItem(name, data);
                child->setParentItem(item);
                changeItems.push_back(child);
                itemCount++;
            }

            item->setChangeItems(changeItems);
        }
    }

    AXLOGI("[GameConfig] Configured {} items in {:.2f}s", itemCount, utils::gettime() - itemStart);

    // 0x10004FDBD: Configure entities
    auto entityStart = utils::gettime();
    auto& entities   = map_util::getMap(data, "entities");

    for (auto& entry : entities)
    {
        auto& name  = entry.first;
        auto entity = EntityConfig::createWithData(entry.second.asValueMap());
        auto code   = entity->getCode();
        _entitiesByName.insert(name, entity);

        if (code != -1)
        {
            _entitiesByCode.insert(code, entity);
        }
    }

    AXLOGI("[GameConfig] Configured {} entities in {:.2f}s", _entitiesByName.size(), utils::gettime() - entityStart);

    // 0x100050038: Configure decay masks
    auto& singleDecay = map_util::getArray(data, "decay.single");
    _singleDecayMasks.reserve(singleDecay.size());

    for (auto& element : singleDecay)
    {
        auto& frames = element.asValueVector();
        std::vector<std::string> result;

        for (auto& frame : frames)
        {
            result.push_back(frame.asString());
        }

        _singleDecayMasks.push_back(result);
    }

    // 0x100050393: Configure material decay masks
    auto& materialDecay = map_util::getMap(data, "decay.materials");
    auto cache          = SpriteFrameCache::getInstance();

    for (auto& entry : materialDecay)
    {
        auto& material = entry.first;
        auto& sprites  = entry.second.asValueVector();
        SpriteList result;

        for (auto& sprite : sprites)
        {
            std::string name  = "";
            size_t count     = 1;

            if (sprite.getType() == Value::Type::VECTOR)
            {
                auto& array = sprite.asValueVector();
                AX_ASSERT(array.size() >= 2);
                name  = array[0].asString();
                count = array[1].asUint64();
            }
            else
            {
                name = sprite.asString();
            }

            // TODO: not an accurate reimplementation, see 0x10005069A
            for (size_t i = 0; i < count; i++)
            {
                auto frame = cache->getSpriteFrameByName(std::format("decay/{}-{}", name, i + 1));

                if (frame)
                {
                    result.push_back(frame);
                }
            }
        }

        _singleDecayByMaterial[material] = result;
    }

    AXLOGI("[GameConfig] Configuration took {:.2f}s", utils::gettime() - start);
    sMain = this;
    return true;
}

Item* GameConfig::registerItem(const std::string& name, const ValueMap& data)
{
    auto item = Item::createWithManager(this, data, name);
    auto code = item->getCode();
    _itemsByName.insert(name, item);
    _itemsByCode.insert(code, item);
    return item;
}

Item* GameConfig::getItemForName(const std::string& name) const
{
    auto it = _itemsByName.find(name);
    return it == _itemsByName.end() ? nullptr : (*it).second;
}

Item* GameConfig::getItemForCode(uint16_t code) const
{
    auto it = _itemsByCode.find(code);
    return it == _itemsByCode.end() ? nullptr : (*it).second;
}

EntityConfig* GameConfig::getEntityForName(const std::string& name) const
{
    auto it = _entitiesByName.find(name);
    return it == _entitiesByName.end() ? nullptr : (*it).second;
}

EntityConfig* GameConfig::getEntityForCode(int32_t code) const
{
    auto it = _entitiesByCode.find(code);
    return it == _entitiesByCode.end() ? nullptr : (*it).second;
}

void GameConfig::loadBiome(const std::string& biome)
{
    auto& biomeConfig = getBiomeConfig(biome);
    AX_ASSERT(!biomeConfig.empty());  // Bug check
    _currentBiomeConfig = biomeConfig;

    for (auto& item : _itemsByName)
    {
        item.second->processSprites();
    }
}

const ValueMap& GameConfig::getBiomeConfig(const std::string& biome) const
{
    auto path = std::format("biomes.{}", biome == "temperate" ? "plain" : biome);
    return map_util::getMap(_data, path);
}

SpriteFrame* GameConfig::getCurrentBiomeFrame(const std::string& frame) const
{
    auto cache = SpriteFrameCache::getInstance();
    auto path  = std::format("items.{}", frame);
    return cache->findFrame(
        map_util::getString(_currentBiomeConfig, path, frame));  // Use findFrame to prevent console spam
}

const GameConfig::SpriteList& GameConfig::getSingleDecayForMaterial(const std::string& material) const
{
    auto it = _singleDecayByMaterial.find(material);

    if (it != _singleDecayByMaterial.end())
    {
        return (*it).second;
    }

    return sEmptySpriteList;
}

}  // namespace opendw
