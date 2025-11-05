#include "Player.h"

#include "gui/GameGui.h"
#include "network/tcp/MessageIdent.h"
#include "zone/WorldZone.h"
#include "AudioManager.h"
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
    _game            = game;
    _entityId        = -1;
    _zoneTeleporting = false;
    sMain            = this;
    return true;
}

void Player::begin()
{
    // TODO: finish
    _zoneTeleporting = false;
}

void Player::update(float deltaTime)
{
    // TODO: finish
    sendMoveMessage();
}

void Player::teleportToZone(const std::string& id)
{
    AudioManager::getInstance()->playSfx("teleport.ogg");
    // TODO: _usedZoneTeleporter = true;
    _zoneTeleporting = true;
    _game->snapshotScreenAsSpinner(true);
    AXLOGI("===== Teleporting to zone {} =====", id);
    GameGui::getMain()->hideTeleportInterface();
    _game->sendMessage(MessageIdent::ZONE_CHANGE, id);
}

void Player::sendMoveMessage()
{
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
    _game->sendMessage(MessageIdent::MOVE, position.x, position.y, velocity.x, velocity.y, 0, target.x, target.y, 33);  // funky time
    _nextMoveMessageTime = time + MOVE_MESSAGE_INTERVAL;
}

Point Player::getBlockPosition() const
{
    // TODO: use collider position
    return _game->getZone()->getBlockPointAtNodePoint(_position * BLOCK_SIZE);
}

}  // namespace opendw
