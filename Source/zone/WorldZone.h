#ifndef __WORLD_ZONE_H__
#define __WORLD_ZONE_H__

#include "axmol.h"

namespace opendw
{

class BaseBlock;
class Entity;
class EntityAnimatedAvatar;
class GameManager;
class Item;
class MetaBlock;
class SceneRenderer;
class WorldRenderer;
class WorldChunk;

enum class Biome : uint8_t
{
    TEMPERATE,
    HELL,
    ARCTIC,
    BRAIN,
    DESERT,
    // OCEAN,
    DEEP = 6,
    SPACE
};

/*
 * CLASS: WorldZone : CCNode @ 0x100316E50
 */
class WorldZone : public ax::Node
{
public:
    enum class State
    {
        INACTIVE,
        ACTIVE,
        LEAVING
    };

    typedef std::unordered_map<std::string, std::vector<std::pair<double, ax::SpriteFrame*>>> DepthGraphics;

    /* FUNC: WorldZone::main @ 0x10003F646 */
    static WorldZone* getMain() { return sMain; }

    static WorldZone* createWithGame(GameManager* game);

    /* FUNC: WorldZone::initWithGame: @ 0x10003F717 */
    bool initWithGame(GameManager* game);

    /* FUNC: WorldZone::configure: @ 0x10003FA19 */
    void configure(const ax::ValueMap& data);

    /* FUNC: WorldZone::step: @ 0x100041506 */
    void update(float deltaTime) override;

    /* FUNC: WorldZone::updateTimedStatus: @ 0x1000459C7 */
    void updateTimedStatus(float deltaTime);

    /* FUNC: WorldZone::updateStatus: @ 0x10004553F */
    void updateStatus(const ax::ValueMap& status);

    /* FUNC: WorldZone::setup @ 0x100041201 */
    void setup();

    /* FUNC: WorldZone::enter @ 0x100041284 */
    void enter();

    /* FUNC: WorldZone::begin @ 0x100041358 */
    void begin();

    /* FUNC: WorldZone::waitForBlocks @ 0x100046B02 */
    void waitForBlocks();

    /* FUNC: WorldZone::cleanup: @ 0x100045E8C */
    void cleanupChunks();

    /* FUNC: WorldZone::didLoadChunkX:y: @ 0x100046C6B */
    void removePendingChunk(int16_t x, int16_t y);

    /* FUNC: WorldZone::updateSunlightX:depth: @ 0x100046AD2 */
    void updateSunlight(int16_t x, int16_t depth);

    /* FUNC: WorldZone::sunlight @ 0x10004A219 */
    const int16_t* getSunlight() const { return _sunlight; }

    int16_t getSunlightAt(int16_t x) const;

    /* FUNC: WorldZone::registerEntity:ilk:name:details: @ 0x1000476FC */
    Entity* registerEntity(int32_t id, int32_t code, const std::string& name, const ax::ValueMap& details);

    /* FUNC: WorldZone::removeEntity:withDeath: @ 0x1000484D5 */
    void removeEntity(int32_t id, bool violent = false);

    Entity* getEntityById(int32_t id);

    /* FUNC: WorldZone::peers @ 0x10004A3B9 */
    const ax::Map<int32_t, EntityAnimatedAvatar*>& getPeers() const { return _peers; }

    ssize_t getPeerCount() const { return _peers.size(); }

    /* FUNC: WorldZone::leave @ 0x1000497AB */
    void leave();

    /* FUNC: 0x100045192 */
    BaseBlock* getBlockAt(int16_t x, int16_t y, bool allowChunkAlloc = false);

    /* FUNC: WorldZone::defaultSeed @ 0x100041497 */
    uint64_t getDefaultSeed() const;

    /* FUNC: WorldZone::worldRenderer @ 0x100004A4A8 */
    WorldRenderer* getWorldRenderer() const { return _worldRenderer; }

    /* FUNC: WorldZone::pointAtBlockX:y: @ 0x10004643C */
    ax::Point getPointAtBlock(int16_t x, int16_t y) const;

    /* FUNC: WorldZone::blockIndexAtNodePoint: 0x100046549 */
    ax::Point getBlockPointAtNodePoint(const ax::Point& point) const;

    /* FUNC: WorldZone::upperLeftScreenBlockIndex @ 0x10004685F */
    ax::Point getUpperLeftScreenBlockPoint() const;

    /* FUNC: WorldZone::lowerRightScreenBlockIndex @ 0x1000468D2 */
    ax::Point getLowerRightScreenBlockPoint() const;

    /* FUNC: WorldZone::blocksInRect: @ 0x10004664A */
    std::vector<BaseBlock*> getBlocksInRect(const ax::Rect& rect);

    /* FUNC: WorldZone::setMetaBlockX:y:item:metadata: @ 0x100046DBE */
    void setMetaBlock(int16_t x, int16_t y, Item* item, const ax::ValueMap& metadata);

    /* FUNC: WorldZone::metaBlockAtX:y: @ 0x10004717A */
    MetaBlock* getMetaBlockAt(int16_t x, int16_t y) const;

    /* FUNC: WorldZone::documentId @ 0x100049FC3 */
    const std::string& getDocumentId() const { return _documentId; }

    /* FUNC: WorldZone::name @ 0x100049F95 */
    const std::string& getZoneName() const { return _name; }

    /* FUNC: WorldZone::biome @ 0x10004A034 */
    const std::string& getBiome() const { return _biome; }

    /* FUNC: WorldZone::biomeType @ 0x10004A062 */
    Biome getBiomeType() const { return _biomeType; }

    /* FUNC: WorldZone::biomeConfig @ 0x10004A072 */
    const ax::ValueMap& getBiomeConfig() const { return _biomeConfig; }

    /* FUNC: WorldZone::depthGraphics @ 0x10004A2F3 */
    const DepthGraphics& getDepthGraphics() const { return _depthGraphics; }

    /* FUNC: WorldZone::seed @ 0x10004A012 */
    uint64_t getSeed() const { return _seed; }

    /* FUNC: WorldZone::blocksWidth @ 0x100049D33 */
    int16_t getBlocksWidth() const { return _blocksWidth; }

    /* FUNC: WorldZone::blocksHeight @ 0x100049D44 */
    int16_t getBlocksHeight() const { return _blocksHeight; }

    /* FUNC: WorldZone::chunkWidth @ 0x100049DEA */
    int16_t getChunkWidth() const { return _chunkWidth; }

    /* FUNC: WorldZone::chunkHeight @ 0x100049DFB */
    int16_t getChunkHeight() const { return _chunkHeight; }

    /* FUNC: WorldZone::chunksCount @ 0x100049E3F */
    int32_t getChunkCount() const { return _chunkCount; }

    /* FUNC: WorldZone::xChunksCount @ 0x100049E1D */
    int16_t getChunkCountX() const { return _chunkCountX; }

    /* FUNC: WorldZone::yChunksCount @ 0x100049E2E */
    int16_t getChunkCountY() const { return _chunkCountY; }

    /* FUNC: WorldZone::surfaceTop @ 0x10004A28D */
    int16_t getSurfaceTop() const { return _surfaceTop; }

    /* FUNC: WorldZone::surfaceBottom @ 0x10004A29E */
    int16_t getSurfaceBottom() const { return _surfaceBottom; }

    /* FUNC: WorldZone::dayPercent @ 0x10004A177 */
    float getDayPercent() const { return _dayPercent; }

    /* FUNC: WorldZone::daytime @ 0x10004A153 */
    float getDayTime() const { return _dayTime; }

    /* FUNC: WorldZone::acidty @ 0x10004A304 */
    float getAcidity() const { return _acidity; }

    /* FUNC: WorldZone::cloudCover @ 0x10004A1D1 */
    float getCloudCover() const { return _cloudCover; }

    /* FUNC: WorldZone::doneWaitingForBlocksAt @ 0x100049F62 */
    double getDoneWaitingForBlocksAt() const { return _doneWaitingForBlocksAt; }

    /* FUNC: WorldZone::state @ 0x10004A507 */
    State getState() const { return _state; }

    /* FUNC: WorldZone::fieldDamageBlock @ 0x10004A527 */
    MetaBlock* getFieldDamageBlock() const { return _fieldDamageBlock; }

private:
    inline static WorldZone* sMain;  // 0x10032EAC0

    /* SNIPPET: 0x10003FD44 - 0x10003FEAE */
    static Biome getBiomeForName(const std::string& name);

    GameManager* _game;                              // WorldZone::game @ 0x100310EA8
    SceneRenderer* _sceneRenderer;                   // WorldZone::sceneRenderer @ 0x1003110B8
    WorldRenderer* _worldRenderer;                   // WorldZone::worldRenderer @ 0x100310FC0
    ax::Vector<WorldChunk*> _inactiveChunks;         // WorldZone::inactiveChunks @ 0x100310EE8
    ax::Vector<WorldChunk*> _activeChunks;           // WorldZone::activeChunks @ 0x100310EE0
    std::map<int32_t, double> _pendingChunks;        // WorldZone::pendingChunks @ 0x100310EF0
    std::vector<int32_t> _cleanedChunks;             // WorldZone::cleanedChunks @ 0x100310F08
    ax::Map<int32_t, WorldChunk*> _chunks;           // WorldZone::chunks @ 0x100311088
    ax::Map<int32_t, MetaBlock*> _metaBlocks;        // WorldZone::metaBlocks @ 0x100311090
    std::map<int32_t, MetaBlock*> _fieldMetaBlocks;  // WorldZone::fieldMetaBlocks @ 0x1003110A8
    ax::Map<int32_t, Entity*> _entities;             // WorldZone::entities @ 0x100310EB8
    ax::Map<int32_t, EntityAnimatedAvatar*> _peers;  // WorldZone::peers @ 0x100310EC8
    std::string _documentId;                         // WorldZone::documentId @ 0x100311170
    std::string _name;                               // WorldZone::name @ 0x100311190
    std::string _biome;                              // WorldZone::biome @ 0x100310F98
    Biome _biomeType;                                // WorldZone::biomeType @ 0x100310FA0
    ax::ValueMap _biomeConfig;                       // WorldZone::biomeConfig @ 0x100310FC8
    DepthGraphics _depthGraphics;                    // WorldZone::depthGraphics @ 0x100310FB8
    uint64_t _seed;                                  // WorldZone::seed @ 0x1003111A0
    int16_t _blocksWidth;                            // WorldZone::blocksWidth @ 0x100310FF8
    int16_t _blocksHeight;                           // WorldZone::blocksHeight @ 0x100311000
    int32_t _blockCount;                             // WorldZone::blocksCount @ 0x100311050
    int16_t _chunkWidth;                             // WorldZone::chunkWidth @ 0x100311008
    int16_t _chunkHeight;                            // WorldZone::chunkHeight @ 0x100311010
    int16_t _chunkSize;                              // WorldZone::chunkSize @ 0x100311018
    int32_t _chunkCount;                             // WorldZone::chunksCount @ 0x100311010
    int16_t _chunkCountX;                            // WorldZone::xChunksCount @ 0x100311020
    int16_t _chunkCountY;                            // WorldZone::yChunksCount @ 0x100311028
    int16_t _surfaceTop;                             // WorldZone::surfaceTop @ 0x100311060
    int16_t _surfaceBottom;                          // WorldZone::surfaceBottom @ 0x100311068
    int16_t* _sunlight;                              // WorldZone::sunlight @ 0x100311080
    ax::ValueVector _timedStatus;                    // WorldZone::status @ 0x100311110
    bool _receivedInitialStatus;                     // WorldZone::receivedInitialStatus @ 0x100310F58
    float _dayPercent;                               // WorldZone::dayPercent @ 0x1003110E8
    float _dayTime;                                  // WorldZone::daytime @ 0x1003110E0
    float _acidity;                                  // WorldZone::acidity @ 0x100311138
    float _cloudCover;                               // WorldZone::cloudCover @ 0x100311160
    double _lastBlocksRequestAt;                     // WorldZone::lastBlocksRequestAt @ 0x100311100
    double _lastBlocksIgnoreAt;                      // WorldZone::lastBlocksIgnoreAt @ 0x100311108
    double _doneWaitingForBlocksAt;                  // WorldZone::doneWaitingForBlocksAt @ 0x1003110F8
    bool _justWaitedForBlocks;                       // WorldZone::justWaitedForBlocks @ 0x1003110F0
    MetaBlock* _fieldDamageBlock;                    // WorldZone::fieldDamageBlock @ 0x100311120
    State _state;                                    // WorldZone::state @ 0x100310F28
    bool _paused;                                    // WorldZone::paused @ 0x100310F30
};

}  // namespace opendw

#endif  // __WORLD_ZONE_H__
