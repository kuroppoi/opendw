#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "axmol.h"

namespace opendw
{

class GameManager;

/*
 * CLASS: Player : NSObject @ 0x100316AE0
 */
class Player : public ax::Object
{
public:
    /* FUNC: Player::main @ 0x10001BA34 */
    static Player* getMain() { return sMain; }

    static Player* createWithGame(GameManager* game);

    /* FUNC: Player::initWithGame: @ 0x10001BB05 */
    bool initWithGame(GameManager* game);

    /* FUNC: Player::preconfigure: @ 0x100028C9D */
    void preconfigure(const ax::ValueMap& data);

    /* FUNC: Player::configure: @ 0x100028D23 */
    void configure(const ax::ValueMap& data);

    /* FUNC: Player::begin @ 0x10001C103 */
    void begin();

    /* FUNC: Player::step: @ 0x10001C351 */
    void update(float deltaTime);

    /* FUNC: Player::teleportToZone: @ 0x100029DE0 */
    void teleportToZone(const std::string& id);

    /* FUNC: Player::sendMoveCommand @ 0x1000232BC */
    void sendMoveMessage();

    /* FUNC: Player::setEntityId: @ 0x1000C0DD5 */
    void setEntityId(int32_t entityId) { _entityId = entityId; }

    /* FUNC: Player::entityId @ 1000C0DC4 */
    int32_t getEntityId() const { return _entityId; }

    // TODO: use collider position
    void setPosition(const ax::Point& position) { _position = position; }

    // TODO: use collider position
    const ax::Point& getPosition() const { return _position; }

    /* FUNC: Player::blockPosition @ 0x100028B15 */
    ax::Point getBlockPosition() const;

    /* FUNC: Player::setIsZoneTeleporting: @ 0x10002DD39 */
    void setZoneTeleporting(bool value) { _zoneTeleporting = value; }

    /* FUNC: Player::isZoneTeleporting @ 0x10002DD28 */
    bool isZoneTeleporting() const { return _zoneTeleporting; }

    /* FUNC: Player::admin @ 0x10002DED1 */
    bool isAdmin() const { return _admin; }

private:
    inline static Player* sMain;  // 10032EA98

    GameManager* _game;           // Player::game @ 0x100310630
    int32_t _entityId;            // Player::entityId @ 0x100310638
    ax::Point _position;          // TODO: use collider position
    double _nextMoveMessageTime;  // Player::nextMoveMessageTime @ 0x1003108C8
    bool _zoneTeleporting;        // Player::isZoneTeleporting @ 0x1003106F8
    bool _admin;                  // Player::admin @ 0x100310958
};

}  // namespace opendw

#endif  // __PLAYER_H__
