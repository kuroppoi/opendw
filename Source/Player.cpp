#include "Player.h"

#include "entity/EntityAnimatedAvatar.h"
#include "graphics/WorldRenderer.h"
#include "gui/GameGui.h"
#include "network/tcp/MessageIdent.h"
#include "physics/ChipmunkBody.h"
#include "physics/ChipmunkShape.h"
#include "physics/Physical.h"
#include "util/MapUtil.h"
#include "util/MathUtil.h"
#include "zone/BaseBlock.h"
#include "zone/WorldZone.h"
#include "AudioManager.h"
#include "CommonDefs.h"
#include "GameConfig.h"
#include "GameManager.h"
#include "Item.h"

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
        shape->setUserData(_avatar);
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
    setLookDirection(1);
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

    // 0x10001C37E: Determine current liquid level
    auto zone         = _game->getZone();
    auto blockPos     = getBlockPosition();
    auto block        = zone->getBlockAt(blockPos.x, blockPos.y);
    auto flyAccessory = GameConfig::getMain()->getItemForCode(1061);  // Onyx Jetpack
    auto isFly        = true;                                         // TODO: fly accessory
    auto isHover      = false;                                        // TODO: fly accessory
    auto isPropel     = false;                                        // TODO: fly accessory

    if (block)
    {
        _currentLiquidLevel = block->getLiquidMod();
    }

    // TODO: idle animation routine

    auto animation = "falling-1";  // No-clip animation

    if (_clip)
    {
        animation = "idle-1";

        if (_currentLiquidLevel > 3)
        {
            animation = "swim-idle";
        }

        //

        auto movement = _destination - _physical->getPosition();
        auto speedX   = abs(movement.x);
        auto speedY   = abs(movement.y);

        if (speedX < 1.0F)
        {
            speedX     = 0.0F;
            movement.x = speedX;
        }

        if (speedY < 1.0F)
        {
            speedY     = 0.0F;
            movement.y = speedY;
        }

        auto travelingHorizontally = movement.x != 0.0F;

        if (_currentLiquidLevel < 5)
        {
            if (!_avatar->wasGroundedRecently())
            {
                if (_flyAccessoryPower <= 0.5F)
                {
                    auto now = utils::gettime();

                    if (now <= _avatar->getLastGroundedAt() + 3.0 || now <= _lastPropelledUpwardAt + 3.333 || isHover ||
                        _currentLiquidLevel > 4)
                    {
                        animation = "falling-1";
                    }
                    else
                    {
                        animation = "flail";
                    }
                }
                else
                {
                    animation = "fly";
                }
            }
            else if (!travelingHorizontally)
            {
                auto velocity = _physical->getVelocity();
                velocity.x *= 0.6F;
                _physical->setVelocity(velocity);
            }
            else
            {
                animation = "walk";

                if (!_running)
                {
                    _startedRunningAt = utils::gettime();
                    _running          = true;
                }

                // TODO: implement horizontal overlap check
                auto velocity = abs(_physical->getVelocity().x);

                if (velocity > BLOCK_SIZE * 4.0F && utils::gettime() > _startedRunningAt + 0.25)
                {
                    animation = "run";
                }
            }
        }
        else if (isPropel && !travelingHorizontally)
        {
            auto velocity = abs(_physical->getVelocity().y);

            if (velocity > BLOCK_SIZE)
            {
                animation = "swim-1";
            }
        }
        else
        {
            // TODO: make sense of this dumb nonsense
            animation     = "fly";
            auto velocity = abs(_physical->getVelocity().x);

            if (velocity <= BLOCK_SIZE && !travelingHorizontally)
            {
                animation = "swim-1";
            }
        }

        // 0x10001D002: Apply movement
        if (!movement.isZero())
        {
            auto propulsion = BLOCK_SIZE * (_avatar->isGrounded() ? 0.5F : 0.2F);
            auto flight     = false;

            if (movement.y > propulsion)
            {
                _lastPropelledUpwardAt = utils::gettime();

                if (flyAccessory)
                {
                    // TODO: check for steam or afterburner
                    flight = true;
                }
            }

            if (speedX > BLOCK_SIZE * 0.05F)
            {
                auto impulse = BLOCK_SIZE * 5.0F;

                if (_avatar->isGrounded())
                {
                    impulse *= getRunningSpeed();
                }
                else if (!flight && isHover)
                {
                    impulse *= 0.25F;
                }

                // TODO: awesome mode speed multiplier

                // Running warmup
                if (utils::gettime() < _startedRunningAt + 0.25)
                {
                    impulse *= 0.8F;
                }

                // Only apply impulse if it exceeds our current horizontal velocity
                auto velocity = _physical->getVelocity();

                if ((movement.x > 0.0F && impulse > velocity.x) || (movement.x < 0.0F && -impulse < velocity.x))
                {
                    impulse = movement.x > 0.0F ? impulse : -impulse;
                    _physical->getBody()->applyImpulseAtLocalPoint(Vec2::UNIT_X * impulse * deltaTime * 15.0F);
                }
            }

            if (movement.y <= propulsion)
            {
                if (!_avatar->isGrounded())
                {
                    if (isHover)
                    {
                        // TODO: implement
                    }
                    else
                    {
                        // TODO: stomp
                    }
                }
            }
            else
            {
                // 0x10001D021: Handle climbing
                // BUGFIX: Added support for climable blocks larger than 1x1
                auto blockPos = getBlockPosition();
                auto climbing = false;

                for (auto x = 0; x < 2; x++)
                {
                    for (auto y = 0; y < 4; y++)
                    {
                        auto block = zone->getBlockAt(blockPos.x - x, blockPos.y + y);

                        if (block)
                        {
                            auto item = block->getFrontItem();

                            if (item->getWidth() >= x + 1 && item->getHeight() >= y + 1 && climbBlock(block, deltaTime))
                            {
                                animation = "climb";
                                climbing  = true;
                                goto end;
                            }
                        }
                    }
                }
            end:  // Nested loop break

                if (_avatar->isGrounded() && utils::gettime() > _lastJumpedAt + 0.3)
                {
                    _physical->getBody()->applyImpulseAtLocalPoint(Point::UNIT_Y * getJumpingPower() * BLOCK_SIZE * 8.0F);
                    animation              = "fly";
                    _lastJumpedAt          = utils::gettime();
                    _lastPropelledUpwardAt = _lastJumpedAt;
                }
            }
        }

        // TODO: figure out how terminal velocity is achieved
        auto fallSpeed = BLOCK_SIZE * -10.0F;
        auto velocity  = _physical->getVelocity();

        if (velocity.y < fallSpeed)
        {
            _physical->setVelocity({velocity.x, fallSpeed});
        }
    }

    auto target    = _physical->getPosition() + Point::UNIT_Y * BLOCK_SIZE * 1.6F * 0.06F;
    Point position = _avatar->getPosition();
    auto distance  = math_util::getDistance(target.x, target.y, position.x, position.y);

    if (distance <= BLOCK_SIZE)
    {
        MathUtil::smooth(&position.x, target.x, deltaTime, 0.01F);
        MathUtil::smooth(&position.y, target.y, deltaTime, 0.01F);
    }
    else
    {
        position = target;
    }

    _physical->getBody()->setAngle(0.0F);
    _avatar->setPosition(position);
    _avatar->animate(animation);
    _avatar->update(deltaTime);
    sendMoveMessage();
}

bool Player::climbBlock(BaseBlock* block, float deltaTime)
{
    if (!block || !block->getFrontItem()->isClimbable())
    {
        return false;
    }

    auto position = _physical->getPosition();
    auto velocity = _physical->getVelocity();
    auto offset   = Point::UNIT_X * (block->getFrontItem()->getWidth() - 1) * BLOCK_SIZE * 0.5F;
    auto target   = block->getWorldPosition() + offset;
    position.x    = math_util::lerp(position.x, target.x, deltaTime * 3.0F);
    velocity.x    = math_util::lerp(velocity.x, 0.0F, deltaTime * 10.0F);
    velocity.y    = math_util::lerp(velocity.y, getClimbingSpeed() * BLOCK_SIZE * 5.0F, deltaTime * 10.0F);
    _physical->setPosition(position);
    _physical->setVelocity(velocity);
    return true;
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

    auto multiplier = 1.0F / BLOCK_SIZE * 100.0F;
    auto position   = _physical->getPosition();
    position        = {position.x * multiplier, (position.y + BLOCK_SIZE) * -multiplier};
    auto velocity   = _physical->getVelocity();
    velocity        = {velocity.x * multiplier, velocity.y * -multiplier};
    auto target     = Vec2::ZERO;  // TODO
    auto animation  = _avatar->getCurrentAnimation();
    _game->sendMessage(MessageIdent::MOVE, position.x, position.y, velocity.x, velocity.y, _lookDirection, target.x,
                       target.y, animation);
    _nextMoveMessageTime = time + MOVE_MESSAGE_INTERVAL;
}

void Player::onFeetCollideWithBlock(BaseBlock* block)
{
    // TODO: implement
}

void Player::onFeetCollideWithEntity(Entity* entity)
{
    // TODO: implement
}

void Player::onCollideWithEntity(Entity* entity)
{
    // TODO: implement
}

float Player::getRunningSpeed() const
{
    auto agility = 5;  // TODO
    return MathUtil::lerp(1.0F, 1.65F, (float)agility / 15.0F);
}

float Player::getClimbingSpeed() const
{
    auto agility = 5;  // TODO
    return MathUtil::lerp(1.2F, 2.2F, (float)agility / 15.0F);
}

float Player::getJumpingPower() const
{
    auto agility = 5;  // TODO
    return MathUtil::lerp(1.0F, 1.4F, (float)agility / 15.0F);
}

void Player::setPosition(const Point& position)
{
    _physical->setPosition(position);
    _avatar->setPosition(position);
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

void Player::setLookDirection(int8_t direction)
{
    AX_ASSERT(direction == -1 || direction == 1);

    if (_lookDirection != direction)
    {
        _lookDirection = direction;
        _avatar->setFlippedX(direction < 0);
    }
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
