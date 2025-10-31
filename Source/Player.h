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
    static Player* createWithGame(GameManager* game);

    /* FUNC: Player::initWithGame: @ 0x10001BB05 */
    bool initWithGame(GameManager* game);

    /* FUNC: Player::step: @ 0x10001C351 */
    void update(float deltaTime);

    /* FUNC: Player::sendMoveCommand @ 0x1000232BC */
    void sendMoveMessage();

    // TODO: use collider position
    void setPosition(const ax::Point& position) { _position = position; }

    // TODO: use collider position
    const ax::Point& getPosition() const { return _position; }

    /* FUNC: Player::blockPosition @ 0x100028B15 */
    ax::Point getBlockPosition() const;

private:
    GameManager* _game;           // Player::game @ 0x100310630
    int32_t _entityId;            // Player::entityId @ 0x100310638
    ax::Point _position;          // TODO: use collider position
    double _nextMoveMessageTime;  // Player::nextMoveMessageTime @ 0x1003108C8
};

}  // namespace opendw

#endif  // __PLAYER_H__
