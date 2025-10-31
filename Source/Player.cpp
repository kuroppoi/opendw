#include "Player.h"

#include "network/tcp/Message.h"
#include "zone/WorldZone.h"
#include "CommonDefs.h"
#include "GameManager.h"

#define MOVE_MESSAGE_INTERVAL 0.2

USING_NS_AX;

namespace opendw
{

Player* Player::createWithGame(GameManager* game)
{
    CREATE_INIT(Player, initWithGame, game);
}

bool Player::initWithGame(GameManager* game)
{
    _game     = game;
    _entityId = -1;
    return true;
}

void Player::update(float deltaTime)
{
    // TODO: finish
    sendMoveMessage();
}

void Player::sendMoveMessage() {

    auto time = utils::gettime();

    // Do nothing if it's not time yet
    if (time < _nextMoveMessageTime)
    {
        return;
    }

    // TODO: use collider info + avatar info for animation
    auto position = _position * BLOCK_SIZE * (1.0F / BLOCK_SIZE * 100.0F);
    position.y *= -1.0F;
    auto velocity = Vec2::ZERO;
    auto target   = Vec2::ZERO;
    _game->sendMessage(MoveMessage(position, velocity, 0i8, target, 33)); // funky time
    _nextMoveMessageTime = time + MOVE_MESSAGE_INTERVAL;
}

Point Player::getBlockPosition() const
{
    // TODO: use collider position
    return _game->getZone()->getBlockPointAtNodePoint(_position * BLOCK_SIZE);
}

}  // namespace opendw
