#include "Player.h"

#include "entity/EntityAnimatedAvatar.h"
#include "graphics/WorldRenderer.h"
#include "gui/GameGui.h"
#include "network/tcp/MessageIdent.h"
#include "physics/ChipmunkBody.h"
#include "physics/ChipmunkShape.h"
#include "physics/Physical.h"
#include "util/MapUtil.h"
#include "zone/WorldZone.h"
#include "AudioManager.h"
#include "CommonDefs.h"
#include "GameConfig.h"
#include "GameManager.h"

#define MOVE_MESSAGE_INTERVAL 0.2

USING_NS_AX;

namespace opendw
{

Player::~Player()
{
    AX_SAFE_RELEASE(_avatar);
}

Player* Player::createWithGame(GameManager* game)
{
    CREATE_INIT(Player, initWithGame, game);
}

bool Player::initWithGame(GameManager* game)
{
    _game            = game;
    _entityId        = -1;
    _zoneTeleporting = false;
    _clip            = true;
    sMain            = this;
    return true;
}

void Player::preconfigure(const ValueMap& data)
{
    _admin = map_util::getBool(data, "admin");
}

void Player::configure(const ValueMap& data)
{
    // TODO: finish
    configureAvatar(data);
}

void Player::configureAvatar(const ValueMap& data)
{
    /* 0x1000293F3: Configure avatar */
    auto& appearance = map_util::getMap(data, "appearance");
    auto config      = GameConfig::getMain()->getEntityForCode(0);  // Avatar
    AX_ASSERT(config);
    _avatar = new EntityAnimatedAvatar();
    _avatar->setIsPlayerAvatar(true);  // Need to set *BEFORE* initializing!
    _avatar->initWithConfig(config, "", appearance);
    _avatar->autorelease();
    _avatar->retain();
    _avatar->setEntityId(_entityId);
    WorldZone::getMain()->getWorldRenderer()->getAnimatedCharactersNode()->addChild(_avatar, 999);

    // 0x100029566: Configure physics body
    _physical = _avatar->getPhysical();
    _physical->addBody();
    auto playerWidth  = BLOCK_SIZE * 0.8F;
    auto playerHeight = BLOCK_SIZE * 1.6F;
    auto height       = playerHeight / 6.0F;  // Segment height

    for (auto i = 0; i < 10; i++)
    {
        auto width     = i == 0 ? playerWidth * 0.9F : i == 9 ? playerWidth : playerWidth * 0.5F;
        auto x         = i == 0 ? -width * 0.5F : i == 9 || (i & 1) == 0 ? -playerWidth * 0.5F : 0.0F;
        auto y         = i == 0 ? 0.0F : (float)(i + 1 >> 1) * height;
        auto points    = std::vector<Point>{{0.0F, 0.0F}, {width, 0.0F}, {width, height}, {0.0F, height}};
        auto transform = AffineTransformMake(1.0F, 0.0F, 0.0F, 1.0F, x, y + playerHeight * 0.06F);
        auto shape     = ChipmunkPolyShape::createWithBody(_physical->getBody(), points, transform, 0.0F);
        shape->setElasticity(0.3F);
        shape->setFriction(i == 0 ? 0.5F : 0.0F);
        _physical->bindInternalShape(shape);

        if (i == 0)
        {
            _feetShape = shape;
        }
        else if (i == 9)
        {
            _headShape = shape;
        }
    }

    _physical->updateChipmunkObjects();
    _physical->setCollisionType(CollisionType::PLAYER);
    _physical->setGroup(this);
    _physical->setLayer(2);
    _physical->setRotates(false);
    auto body = _physical->getBody();

    if (_clip)
    {
        auto moment = cpMomentForBox(body->getMass(), playerWidth, playerHeight);
        body->setMoment(moment);
    }
    else
    {
        body->setType(CP_BODY_TYPE_KINEMATIC);
    }

    _physical->addToSpace();
}

void Player::begin()
{
    // TODO: finish
    _zoneTeleporting = false;
}

void Player::reset()
{
    AXLOGI("[Player] reset");
    _physical = nullptr;  // Managed by avatar, no need to release
    AX_SAFE_RELEASE_NULL(_avatar);
}

void Player::update(float deltaTime)
{
    // TODO: finish
    auto animation = _clip ? "idle-1" : "falling-1";
    _physical->getBody()->setAngle(0.0F);
    _avatar->setPosition(_physical->getPosition() + Point::UNIT_Y * BLOCK_SIZE * 1.6F * 0.06F);
    _avatar->animate(animation);
    _avatar->update(deltaTime);
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

    // TODO: use move direction
    auto multiplier = 1.0F / BLOCK_SIZE * 100.0F;
    auto position   = _physical->getPosition();
    position        = {position.x * multiplier, (position.y + BLOCK_SIZE) * -multiplier};
    auto velocity   = _physical->getVelocity();
    velocity        = {velocity.x * multiplier, velocity.y * -multiplier};
    auto target     = Vec2::ZERO;
    auto animation  = _avatar->getCurrentAnimation();
    _game->sendMessage(MessageIdent::MOVE, position.x, position.y, velocity.x, velocity.y, 1, target.x, target.y,
                       animation);
    _nextMoveMessageTime = time + MOVE_MESSAGE_INTERVAL;
}

void Player::setPosition(const Point& position)
{
    _physical->setPosition(position);
}

Point Player::getPosition() const
{
    return _physical->getPosition();
}

Point Player::getBlockPosition() const
{
    auto position = _physical->getPosition();
    return _game->getZone()->getBlockPointAtNodePoint({position.x, position.y + BLOCK_SIZE * 0.2F});
}

void Player::setClip(bool clip)
{
    if (_clip != clip)
    {
        auto body = _physical->getBody();

        if (clip)
        {
            // Recalculate body mass & moment
            body->setType(CP_BODY_TYPE_DYNAMIC);
            body->setMass(1.0F);  // NOTE: This is the default mass for everything
            body->setMoment(cpMomentForBox(body->getMass(), BLOCK_SIZE * 0.8F, BLOCK_SIZE * 1.6F));
        }
        else
        {
            body->setType(CP_BODY_TYPE_KINEMATIC);
        }

        _clip = clip;
    }
}

}  // namespace opendw
