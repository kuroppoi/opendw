#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "axmol.h"

namespace opendw
{

class BaseBlock;
class ChipmunkShape;
class Entity;
class EntityAnimatedAvatar;
class GameManager;
class InventoryItem;
class Item;
class Physical;
enum class BlockLayer : uint8_t;
enum class ContainerType;

/*
 * CLASS: Player : NSObject @ 0x100316AE0
 */
class Player : public ax::Object
{
public:
    /* FUNC: Player::dealloc @ 0x10002D77C */
    virtual ~Player() override;

    /* FUNC: Player::main @ 0x10001BA34 */
    static Player* getMain() { return sMain; }

    static Player* createWithGame(GameManager* game);

    /* FUNC: Player::initWithGame: @ 0x10001BB05 */
    bool initWithGame(GameManager* game);

    /* FUNC: Player::preconfigure: @ 0x100028C9D */
    void preconfigure(const ax::ValueMap& data);

    /* FUNC: Player::configure: @ 0x100028D23 */
    void configure(const ax::ValueMap& data);

    /* FUNC: Player::configureAvatar: @ 0x1000293CB */
    void configureAvatar(const ax::ValueMap& data);

    /* FUNC: Player::begin @ 0x10001C103 */
    void begin();

    /* FUNC: Player::reset @ 0x10002D6A1 */
    void reset();

    /* FUNC: Player::step: @ 0x10001C351 */
    void update(float deltaTime);

    /* FUNC: Player::useFlyAccessory:delta: @ 0x10001FE17 */
    void useFlyAccessory(Item* item, float deltaTime);

    /* FUNC: Player::climbBlock:delta: @ 0x100020A28 */
    bool climbBlock(BaseBlock* block, float deltaTime);

    /* FUNC: Player::playHurtSound: @ 0x100028811 */
    void playHurtSound(bool heavy = false);

    /* FUNC: Player::teleportToZone: @ 0x100029DE0 */
    void teleportToZone(const std::string& id);

    /* FUNC: Player::respawn @ 0x10002CDEF */
    void respawn();

    /* FUNC: Player::sendRespawnCommand @ 0x10002CEE2 */
    void sendRespawnMessage();

    /* FUNC: Player::isRespawning @ 0x10002CDD1 */
    bool isRespawning() const { return _respawnStartedAt > 0.0; }

    /* FUNC: Player::playerDidRespawn @ 0x10002CECD */
    void onPlayerRespawned() { _respawnStartedAt = 0.0; }

    /* FUNC: Player::sendMoveCommand @ 0x1000232BC */
    void sendMoveMessage();

    /* FUNC: Player::sendInventoryUseMessage:onlyIfAllowed: @ 0x100025190 */
    void sendInventoryUseMessage(Item* item, bool secondary = false, bool onlyIfAllowed = true);

    /* FUNC: Player::useActivePrimaryItemAtPoint: @ 0x1000E9515 */
    bool useActiveHotbarItem(const ax::Point& point);

    /* FUNC: Player::useItem:atPoint:fromContainer: @ 0x10002365D */
    bool useInventoryItem(InventoryItem* invItem, const ax::Point& point);

    /* FUNC: Player::tryToMineBlockAtNodePoint:withItem: @ 0x100024470 */
    BaseBlock* tryToMineBlockAtNodePoint(const ax::Point& point, InventoryItem* invItem);

    /* FUNC: Player::tryToPlaceInventoryItem:atPoint: @ 0x100025346 */
    bool tryToPlaceInventoryItem(InventoryItem* invItem, const ax::Point& point);

    /* FUNC: Player::canDigAt: @ 0x100027608 */
    bool canDigAt(const ax::Point& point) const;

    /* FUNC: Player::canPlaceItem:atBlock: @ 0x100027A8C */
    bool canPlaceItem(Item* item, BaseBlock* block) const;

    /* FUNC: Player::didFeetCollideWithBlock: @ 0x10002A33C */
    void onFeetCollideWithBlock(BaseBlock* block);

    /* FUNC: Player::didFeetCollideWithEntity: @ 0x10002A481 */
    void onFeetCollideWithEntity(Entity* entity);

    /* FUNC: Player::didCollideWithEntity: @ 0x10002A5A3 */
    void onCollideWithEntity(Entity* entity);

    /* FUNC: Player::runningSpeed @ 0x10002C89B */
    float getRunningSpeed() const;

    /* FUNC: Player::climbingSpeed @ 0x10002C8D6 */
    float getClimbingSpeed() const;

    /* FUNC: Player::swimmingSpeed @ 0x10002C911 */
    float getSwimmingSpeed() const;

    /* FUNC: Player::jumpingPower @ 0x10002C94C */
    float getJumpingPower() const;

    /* FUNC: Player::flyingSpeed @ 0x10002C987 */
    float getFlyingSpeed() const;

    /* FUNC: Player::placingRange @ 0x10002CB2E */
    float getPlacingRange() const;

    /* FUNC: Player::miningSpeed @ 0x10002CBAB */
    float getMiningSpeed() const;

    /* FUNC: Player::playerId @ 0x10002D9A3 */
    const std::string& getPlayerId() const { return _playerId; }

    /* FUNC: Player::setUsername: @ 0x10002D958 */
    void setUsername(const std::string& username) { _username = username; }

    /* FUNC: Player::username @ 0x10002D947 */
    const std::string& getUsername() const { return _username; }

    /* FUNC: Player::setEntityId: @ 0x1000C0DD5 */
    void setEntityId(int32_t entityId) { _entityId = entityId; }

    /* FUNC: Player::entityId @ 1000C0DC4 */
    int32_t getEntityId() const { return _entityId; }

    void setPosition(const ax::Point& position);
    ax::Point getPosition() const;

    /* FUNC: Player::blockPosition @ 0x100028B15 */
    ax::Point getBlockPosition() const;

    /* FUNC: Player::physicalCenter @ 0x100028C24 */
    ax::Point getPhysicalCenter() const;

    /* FUNC: Player::avatar @ 0x10002DFAC */
    EntityAnimatedAvatar* getAvatar() const { return _avatar; }

    /* FUNC: Player::setDestination: @ 0x10002DAEB */
    void setDestination(const ax::Point& destination) { _destination = destination; }

    /* FUNC: Player::physical @ 0x10002DF57 */
    Physical* getPhysical() const { return _physical; }

    /* FUNC: Player::feetShape @ 0x10002DF68 */
    ChipmunkShape* getFeetShape() const { return _feetShape; }

    /* FUNC: Player::headShape @ 0x10002DF79 */
    ChipmunkShape* getHeadShape() const { return _headShape; }

    /* FUNC: Player::setLookDirection: @ 0x100028A78 */
    void setLookDirection(int8_t direction);

    /* FUNC: Player::lookDirection @ 0x10002DCB4 */
    int8_t getLookDirection() const { return _lookDirection; }

    /* FUNC: Player::setHealth: @ 0x100020E0A */
    void setHealth(float health);

    /* FUNC: Player::health @ 0x10002DB81 */
    float getHealth() const { return _health; }

    /* FUNC: Player::maxHealth @ 0x10002CBE6 */
    float getMaxHealth() const;

    bool isAlive() const { return _health > 0.0F; }
    bool isDead() const { return !isAlive(); }

    /* FUNC: Player::setSteam: @ 0x100021132 */
    void setSteam(float steam);

    /* FUNC: Player::hasSteam @ 0x10002C559 */
    bool hasSteam() const;

    /* FUNC: Player::steam @ 0x10002DB93*/
    float getSteam() const { return _steam;}

    /* FUNC: Player::maxSteam @ 0x10002C5A7 */
    float getMaxSteam() const;

    /* FUNC: Player::steamEfficiency @ 0x10002CC6F */
    float getSteamEfficiency() const;

    /* FUNC: Player::steamCooldownDuration @ 0x10002CCAA */
    float getSteamCooldownDuration() const;

    /* FUNC: Player::setInventory:count: @ 0x100021727 */
    InventoryItem* setInventory(Item* item, int64_t quantity);

    /* FUNC: Player::setInventory:count:container:position: @ 0x100021766 */
    InventoryItem* setInventory(Item* item, int64_t quantity, ContainerType container, int64_t slot);

    /* FUNC: Player::addInventory:count: @ 0x100021970 */
    InventoryItem* addInventory(Item* item, int64_t quantity);

    /* FUNC: Player::getInventory: @ 0x1000219C2 */
    InventoryItem* getInventory(Item* item);

    /* FUNC: Player::inventoryItemInContainer:position: @ 0x100022801 */
    InventoryItem* getInventoryItem(ContainerType container, int64_t slot, int64_t category = 0);

    /* FUNC: Player::arrangeInventory: @ 0x10002106 */
    void arrangeInventory(Item* item);
    void arrangeInventory(int64_t category);

    /* FUNC: Player::updateActiveItem: @ 0x100023018 */
    void updateActiveHotbarItem();

    /* FUNC: Player::activePrimaryInventoryItem @ 0x10002DA2B */
    InventoryItem* getActiveHotbarItem() const { return _activeHotbarItem; }

    /* FUNC: Player::setPrimaryHotbarIndex: @ 0x1000229A9 */
    void setActiveHotbarSlot(int64_t slot);

    /* FUNC: Player::primaryHotbarIndex @ 0x10002DA09 */
    int64_t getActiveHotbarSlot() const { return _activeHotbarSlot; }

    /* FUNC: Player::setUsingPrimaryItem: @ 0x10002DA80 */
    void setUsingPrimaryItem(Item* item);

    /* FUNC: Player::setUsingSecondaryItem: @ 0x10002DAA1 */
    void setUsingSecondaryItem(Item* item);

    /* FUNC: Player::setIsZoneTeleporting: @ 0x10002DD39 */
    void setZoneTeleporting(bool value) { _zoneTeleporting = value; }

    /* FUNC: Player::isZoneTeleporting @ 0x10002DD28 */
    bool isZoneTeleporting() const { return _zoneTeleporting; }

    /* FUNC: Player::setIsTravelingHorizontally: @ 0x10002DCF5 */
    void setTravelingHorizontally(bool value) { _travelingHorizontally = value; }

    /* FUNC: Player::setMiningBlock: @ 0x10002DB14 */
    void setMiningBlock(BaseBlock* block);

    /* FUNC: Player::miningBlock @ 0x10002DB03 */
    BaseBlock* getMiningBlock() const { return _miningBlock; }

    /* FUNC: Player::setMiningLayer: @ 0x10002DB41 */
    void setMiningLayer(BlockLayer layer) { _miningLayer = layer; }

    /* FUNC: Player::miningLayer @ 0x10002DB31 */
    BlockLayer getMiningLayer() const { return _miningLayer; }

    /* FUNC: Player::admin @ 0x10002DED1 */
    bool isAdmin() const { return _admin; }

    /* FUNC: Player::setClip: @ 0x100021465 */
    void setClip(bool clip);

    /* FUNC: Player::clip @ 0x100310660 */
    bool getClip() const { return _clip; }

    static const int kHotbarItemCount = 10;

private:
    inline static Player* sMain;  // 10032EA98

    GameManager* _game;                           // Player::game @ 0x100310630
    std::string _playerId;                        // Player::playerId @ 0x100310968
    std::string _username;                        // Player::username @ 0x100310998
    int32_t _entityId;                            // Player::entityId @ 0x100310638
    EntityAnimatedAvatar* _avatar;                // Player::avatar @ 0x100310718
    ax::Point _destination;                       // Player::destination @ 0x100310760
    Physical* _physical;                          // Player::physical @ 0x100310768
    ChipmunkShape* _feetShape;                    // Player::feetShape @ 0x1003109A0
    ChipmunkShape* _headShape;                    // Player::headShape @ 0x1003109A8
    int8_t _lookDirection;                        // Player::lookDirection @ 0x100310720
    uint8_t _currentLiquidLevel;                  // Player::currentLiquidLevel @ 0x100310740
    double _changeIdleAt;                         // Player::changeIdleAt @ 0x100310710
    std::string _idleAnimation;                   // Player::currentIdleAnimation @ 0x100310750
    double _respawnStartedAt;                     // Player::respawnStartedAt @ 0x100310830
    double _nextMoveMessageTime;                  // Player::nextMoveMessageTime @ 0x1003108C8
    double _startedRunningAt;                     // Player::startedRunningAt @ 0x100310788
    double _lastPropelledUpwardAt;                // Player::lastPropelledUpwardAt @ 0x1003107A0
    double _lastJumpedAt;                         // Player::lastJumpedAt @ 0x1003107A8
    Item* _flyAccessory;                          // Player::flyAccessory @ 0x100310778
    float _flyAccessoryPower;                     // Player::flyAccessoryPower @ 0x100310798
    float _health;                                // Player::health @ 0x1003106A8
    float _steam;                                 // Player::steam @ 0x100310890
    double _lastUsedSteamAt;                      // Player::lastSteamedAt @ 0x100310828
    double _steamCooldownAt;                      // Player::steamCooldownAt @ 0x100310898
    ax::Map<int16_t, InventoryItem*> _inventory;  // Player::inventory @ 0x100310670
    InventoryItem* _activeHotbarItem;             // Player::activePrimaryInventoryItem @ 0x1003107E8
    int64_t _activeHotbarSlot;                    // Player::primaryHotbarIndex @ 0x1003106A0
    Item* _usingPrimaryItem;                      // Player::usingPrimaryItem @ 0x100310808
    Item* _usingSecondaryItem;                    // Player::usingSecondaryItem @ 0x100310810
    bool _zoneTeleporting;                        // Player::isZoneTeleporting @ 0x1003106F8
    bool _travelingHorizontally;                  // Player::isTravelingHorizontally @ 0x100310780
    bool _mining;                                 // Player::isMining @ 0x100310848
    BaseBlock* _miningBlock;                      // Player::miningBlock @ 0x1003108E0
    BlockLayer _miningLayer;                      // Player::miningLayer @ 0x100310908
    double _lastMiningAttemptAt;                  // Player::lastMiningAttemptAt @ 0x1003108E8
    BaseBlock* _miningAttemptBlock;               // Player::miningAttemptBlock @ 0x1003108F0
    int _miningAttempts;                          // Player::miningAttempts @ 0x1003108F8
    double _nextAllowedPlaceTime;                 // Player::nextAllowedPlaceTime @ 0x100310910
    BaseBlock* _lastPlacedBlock;                  // Player::lastPlacedAt @ 0x100310918
    bool _admin;                                  // Player::admin @ 0x100310958
    bool _clip;                                   // Player::clip @ 0x100310660
    bool _running;
};

}  // namespace opendw

#endif  // __PLAYER_H__
