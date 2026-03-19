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
class Physical;

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

    /* FUNC: Player::climbBlock:delta: @ 0x100020A28 */
    bool climbBlock(BaseBlock* block, float deltaTime);

    /* FUNC: Player::teleportToZone: @ 0x100029DE0 */
    void teleportToZone(const std::string& id);

    /* FUNC: Player::sendMoveCommand @ 0x1000232BC */
    void sendMoveMessage();

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

    /* FUNC: Player::jumpingPower @ 0x10002C94C */
    float getJumpingPower() const;

    /* FUNC: Player::setEntityId: @ 0x1000C0DD5 */
    void setEntityId(int32_t entityId) { _entityId = entityId; }

    /* FUNC: Player::entityId @ 1000C0DC4 */
    int32_t getEntityId() const { return _entityId; }

    void setPosition(const ax::Point& position);
    ax::Point getPosition() const;

    /* FUNC: Player::blockPosition @ 0x100028B15 */
    ax::Point getBlockPosition() const;

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

    /* FUNC: Player::setIsZoneTeleporting: @ 0x10002DD39 */
    void setZoneTeleporting(bool value) { _zoneTeleporting = value; }

    /* FUNC: Player::isZoneTeleporting @ 0x10002DD28 */
    bool isZoneTeleporting() const { return _zoneTeleporting; }

    /* FUNC: Player::admin @ 0x10002DED1 */
    bool isAdmin() const { return _admin; }

    /* FUNC: Player::setClip: @ 0x100021465 */
    void setClip(bool clip);

    /* FUNC: Player::clip @ 0x100310660 */
    bool getClip() const { return _clip; }

private:
    inline static Player* sMain;  // 10032EA98

    GameManager* _game;             // Player::game @ 0x100310630
    int32_t _entityId;              // Player::entityId @ 0x100310638
    EntityAnimatedAvatar* _avatar;  // Player::avatar @ 0x100310718
    ax::Point _destination;         // Player::destination @ 0x100310760
    Physical* _physical;            // Player::physical @ 0x100310768
    ChipmunkShape* _feetShape;      // Player::feetShape @ 0x1003109A0
    ChipmunkShape* _headShape;      // Player::headShape @ 0x1003109A8
    int8_t _lookDirection;          // Player::lookDirection @ 0x100310720
    uint8_t _currentLiquidLevel;    // Player::currentLiquidLevel @ 0x100310740
    double _nextMoveMessageTime;    // Player::nextMoveMessageTime @ 0x1003108C8
    double _startedRunningAt;       // Player::startedRunningAt @ 0x100310788
    double _lastPropelledUpwardAt;  // Player::lastPropelledUpwardAt @ 0x1003107A0
    double _lastJumpedAt;           // Player::lastJumpedAt @ 0x1003107A8
    float _flyAccessoryPower;       // Player::flyAccessoryPower @ 0x100310798
    bool _zoneTeleporting;          // Player::isZoneTeleporting @ 0x1003106F8
    bool _admin;                    // Player::admin @ 0x100310958
    bool _clip;                     // Player::clip @ 0x100310660
    bool _running;
};

}  // namespace opendw

#endif  // __PLAYER_H__
