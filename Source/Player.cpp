#include "Player.h"

#include "entity/EntityAnimatedAvatar.h"
#include "graphics/WorldRenderer.h"
#include "gui/GameGui.h"
#include "input/InputManager.h"
#include "network/tcp/MessageIdent.h"
#include "physics/ChipmunkBody.h"
#include "physics/ChipmunkShape.h"
#include "physics/ChipmunkSpace.h"
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
#define JUMP_COOLDOWN         0.3
#define AGILITY_LEVEL         10  // TODO: skills
#define ENGINEERING_LEVEL     10  // TODO: skills

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
    _flyAccessory    = GameConfig::getMain()->getItemForCode(1060);  // TODO: get from inventory
    _changeIdleAt    = utils::gettime() + random(8.0F, 15.0F);
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
    auto zone     = _game->getZone();
    auto blockPos = getBlockPosition();
    auto block    = zone->getBlockAt(blockPos.x, blockPos.y);

    if (block)
    {
        _currentLiquidLevel = block->getLiquidMod();
    }

    // 0x10001C47A: Change idle animation if it is time
    if (utils::gettime() >= _changeIdleAt)
    {
        if (_idleAnimation.empty())
        {
            auto next      = _currentLiquidLevel > 4 ? std::format("swim-idle-flourish-{}", random(1, 4))
                                                     : std::format("idle-{}", random(2, 5));
            auto animation = _avatar->getMainSkeleton()->findAnimation(next);
            auto duration  = animation ? animation->getDuration() : 5.0F;
            _changeIdleAt  = utils::gettime() + duration;
            _idleAnimation = next;
        }
        else
        {
            _changeIdleAt  = utils::gettime() + random(10.0, 15.0);
            _idleAnimation = "";
        }
    }

    // 0x10001C60D: Reset idle animation if we've moved recently
    if (utils::gettime() < _game->getInputManager()->getLastInputAt() + 0.5)
    {
        _changeIdleAt  = utils::gettime() + random(8.0, 10.0);
        _idleAnimation = "";
    }

    // 0x10001C6DD: Determine current animation
    auto animation = _idleAnimation.empty() ? (_currentLiquidLevel > 3 ? "swim-idle" : "idle-1") : _idleAnimation;
    auto movement  = _destination - _physical->getPosition();
    auto speedX    = abs(movement.x);
    auto speedY    = abs(movement.y);
    auto hover     = _flyAccessory && _flyAccessory->isUsableType(UseType::HOVER);
    auto grounded  = _avatar->isGrounded();

    if (_currentLiquidLevel < 5)
    {
        // TODO: is checking wasGroundedRecently() redundant?
        if (!_avatar->wasGroundedRecently() && !grounded)
        {
            // We're mid-air
            _running = false;

            if (_flyAccessoryPower <= 0.5F)
            {
                // We're not using our jetpack
                if (hover || utils::gettime() <= _avatar->getLastGroundedAt() + 3.0 ||
                    utils::gettime() <= _lastPropelledUpwardAt + 3.333)
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
        else if (!_travelingHorizontally)
        {
            // We're standing still on solid ground
            auto velocity = _physical->getVelocity();
            velocity.x *= 0.6F;
            _physical->setVelocity(velocity);
            _running = false;
        }
        else
        {
            // We're walking on solid ground
            if (!_running)
            {
                _startedRunningAt = utils::gettime();
                _running          = true;
            }

            // TODO: implement horizontal overlap check
            auto velocity = abs(_physical->getVelocity().x);

            if (velocity >= BLOCK_SIZE * 4.0F && utils::gettime() >= _startedRunningAt + 0.25)
            {
                animation = "run";
            }
            else
            {
                animation = "walk";
            }
        }
    }
    else
    {
        // We're swimming in liquid
        _running = false;

        if (_travelingHorizontally)
        {
            animation = "swim-1";
        }
        else if (_flyAccessoryPower > 0.5F)
        {
            animation = "fly";
        }
    }

    // 0x10001D002: Apply movement
    auto flying     = false;
    auto body       = _physical->getBody();
    auto propulsion = BLOCK_SIZE * (grounded ? 0.5F : 0.2F);  // Upward motion required to fly

    if (!_clip)
    {
        animation = "falling-1";
    }
    else
    {
        if (movement.y > propulsion)
        {
            _lastPropelledUpwardAt = utils::gettime();
            flying                 = _flyAccessory != nullptr;
        }

        // 0x10001CCE6: Apply horizontal movement
        if (speedX > BLOCK_SIZE * 0.05F)
        {
            auto impulse = BLOCK_SIZE * 5.0F;

            if (grounded)
            {
                impulse *= getRunningSpeed();
            }
            else if (!flying && hover)
            {
                impulse *= 0.25F;  // We're hovering; slow horizontal movement
            }

            // TODO: awesome mode speed multiplier

            if (utils::gettime() < _startedRunningAt + 0.25)
            {
                impulse *= 0.8F;  // Running warmup
            }

            if (movement.x < 0.0F)
            {
                impulse *= -1.0F;
            }

            // Apply impulse if it doesn't exceed our current horizontal velocity
            auto velocity = _physical->getVelocity();

            if ((impulse > 0.0F && impulse > velocity.x) || (impulse < 0.0F && impulse < velocity.x))
            {
                body->applyImpulseAtLocalPoint(Vec2::UNIT_X * impulse * deltaTime * 15.0F);
            }
        }

        // 0x10001D002: Apply vertical movement
        if (movement.y <= propulsion)
        {
            // We're not propelling upwards
            if (!grounded && hover)
            {
                // We're mid-air with a zeppelin
                auto counterforce = 0.85F;
                auto mass         = body->getMass();

                if (movement.y > -10.0F)
                {
                    // TODO: steam
                    // TODO: emitter
                    auto impulse = mass * deltaTime * -3.0F;
                    body->applyImpulseAtLocalPoint(_physical->getVelocity() * impulse);
                    counterforce = 0.95F;
                }

                // Apply counterforce against gravity
                auto gravity = zone->getSpace()->getGravity();
                auto impulse = mass * deltaTime * counterforce;
                body->applyImpulseAtLocalPoint(-(gravity * impulse));
            }
            else
            {
                // TODO: implement stomping
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

            if (_currentLiquidLevel < 4)
            {
                if (!climbing)
                {
                    if (!grounded)
                    {
                        if (flying && utils::gettime() > _lastJumpedAt + 0.5)
                        {
                            useFlyAccessory(_flyAccessory, deltaTime);
                        }
                    }
                    else if (utils::gettime() > _lastJumpedAt + JUMP_COOLDOWN)
                    {
                        // We're jumping
                        body->applyImpulseAtLocalPoint(Vec2::UNIT_Y * getJumpingPower() * BLOCK_SIZE * 8.0F);
                        _lastJumpedAt          = utils::gettime();
                        _lastPropelledUpwardAt = _lastJumpedAt;
                        animation              = "fly";
                    }
                }
            }
            else if (!flying)
            {
                // We're swimming
                body->applyImpulseAtLocalPoint(Vec2::UNIT_Y * getSwimmingSpeed() * BLOCK_SIZE * deltaTime * 15.0F);
                animation = "swim-1";
            }
            else
            {
                useFlyAccessory(_flyAccessory, deltaTime);
            }
        }

        // 0x10001D8DB: Move player towards block center if moving directly upwards
        if (movement.y > BLOCK_SIZE * 0.5F && speedX < BLOCK_SIZE * 0.345F && _avatar->getHeadColliderCount() < 1)
        {
            auto velocity = _physical->getVelocity();
            velocity.x    = 0.0F;
            _physical->setVelocity(velocity);
            auto blockPos = getBlockPosition();
            auto block    = zone->getBlockAt(blockPos.x, blockPos.y);

            if (block)
            {
                auto position = _physical->getPosition();
                auto target   = block->getWorldPosition();
                position.x    = math_util::lerp(position.x, target.x, deltaTime * 3.0F);
                _physical->setPosition(position);
            }
        }

        // 0x10001DBB2: Limit velocity
        // TODO: calculation is a bit more complex for liquids
        auto velLimit = _currentLiquidLevel == 0 ? BLOCK_SIZE * 12.0F : BLOCK_SIZE * 4.0F;
        auto velocity = _physical->getVelocity();

        if (velocity.lengthSquared() > velLimit * velLimit)
        {
            _physical->setVelocity(velocity.getNormalized() * velLimit);
        }
    }

    body->setAngle(0.0F);
    _flyAccessoryPower = math_util::lerp(_flyAccessoryPower, 0.0F, deltaTime);
    _avatar->update(deltaTime);
    _avatar->setRotation(math_util::lerp(_avatar->getRotation(), 0.0F, deltaTime * 2.3125F));
    _avatar->animate(animation);

    // Update avatar position
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

    _avatar->setPosition(position);
    sendMoveMessage();
}

void Player::useFlyAccessory(Item* item, float deltaTime)
{
    // TODO: flight suppression
    // TODO: emit particles
    // TODO: track steam

    auto speedX    = _destination.x - _physical->getPosition().x;
    auto flySpeed  = getFlyingSpeed() * BLOCK_SIZE * deltaTime;
    auto direction = abs(speedX) < FLT_EPSILON ? 0.0F : speedX > 0.0F ? 1.0F : -1.0F;
    _physical->getBody()->applyImpulseAtLocalPoint({flySpeed * direction, flySpeed * 21.5F});
    auto tilt     = speedX / BLOCK_SIZE * 42.0F;
    auto rotation = math_util::lerp(_avatar->getRotation(), tilt, deltaTime * 1.75F);
    _avatar->setRotation(rotation);
    _flyAccessoryPower = math_util::lerp(_flyAccessoryPower, 1.0F, deltaTime * 2.0F);
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
    return MathUtil::lerp(1.0F, 1.65F, (float)AGILITY_LEVEL / 15.0F);
}

float Player::getClimbingSpeed() const
{
    return MathUtil::lerp(1.2F, 2.2F, (float)AGILITY_LEVEL / 15.0F);
}

float Player::getSwimmingSpeed() const
{
    return MathUtil::lerp(1.0F, 1.7F, (float)AGILITY_LEVEL / 15.0F);
}

float Player::getJumpingPower() const
{
    return MathUtil::lerp(1.0F, 1.4F, (float)AGILITY_LEVEL / 15.0F);
}

float Player::getFlyingSpeed() const
{
    // TODO: check for afterburner
    auto power = _flyAccessory->getPower();
    auto speed = MathUtil::lerp(1.0F, 1.65F, (float)ENGINEERING_LEVEL / 15.0F);
    return speed * power;
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
