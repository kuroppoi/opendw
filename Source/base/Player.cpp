#include "Player.h"

#include "base/GameConfig.h"
#include "base/InventoryItem.h"
#include "base/Item.h"
#include "base/Recipe.h"
#include "entity/EntityAnimatedAvatar.h"
#include "entity/EntityConfig.h"
#include "event/EventNames.h"
#include "graphics/WorldRenderer.h"
#include "gui/widget/ItemContainer.h"
#include "gui/GameGui.h"
#include "input/InputManager.h"
#include "network/tcp/MessageIdent.h"
#include "physics/ChipmunkBody.h"
#include "physics/ChipmunkShape.h"
#include "physics/ChipmunkSpace.h"
#include "physics/Physical.h"
#include "util/AxUtil.h"
#include "util/MapUtil.h"
#include "util/MathUtil.h"
#include "zone/BaseBlock.h"
#include "zone/WorldZone.h"
#include "AudioManager.h"
#include "CommonDefs.h"
#include "GameManager.h"

#define MOVE_MESSAGE_INTERVAL  0.2
#define JUMP_COOLDOWN          0.3
#define STEAM_RESTORE_COOLDOWN 0.5
#define BASE_HEALTH            5.0F
#define BASE_MAX_STEAM         20.0F
#define MAX_SKILL_LEVEL        15

USING_NS_AX;

namespace opendw
{

Player::~Player()
{
    AX_SAFE_RELEASE(_avatar);
    AX_SAFE_RELEASE(_miningBlock);
}

Player* Player::createWithGame(GameManager* game)
{
    CREATE_INIT(Player, initWithGame, game);
}

bool Player::initWithGame(GameManager* game)
{
    _game     = game;
    _username = "Unknown player";
    _entityId = -1;
    _health   = 5.0F;
    _clip     = true;
    sMain     = this;
    return true;
}

void Player::preconfigure(const ValueMap& data)
{
    _admin = map_util::getBool(data, "admin");
}

void Player::configure(const ValueMap& data)
{
    // TODO: finish
    _playerId = map_util::getString(data, "id");
    configureAvatar(data);
    setSteam(getMaxSteam());
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
    _zoneTeleporting   = false;
    _flyAccessoryPower = 0.0F;
    _changeIdleAt      = utils::gettime() + random(8.0F, 15.0F);
    setLookDirection(1);
}

void Player::reset()
{
    AXLOGI("[Player] reset");
    _physical = nullptr;  // Managed by avatar, no need to release
    AX_SAFE_RELEASE_NULL(_avatar);
    _inventory.clear();
    _cachedAccessoryItems.clear();
    _cachedHiddenItems.clear();
    _skills.clear();
    _cachedAdjustedSkills.clear();
    _flyAccessory     = nullptr;
    _stompAccessory   = nullptr;
    _activeHotbarItem = nullptr;
    _activeShieldItem = nullptr;
    _activeHotbarSlot = 0;
    AX_SAFE_RELEASE_NULL(_miningBlock);
    _miningAttemptBlock = nullptr;
    _lastPlacedBlock    = nullptr;
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
        // NOTE: Potential bug?
        // It fetches the exact same block twice and stores the sum of both liquid mods.
        // Perhaps it was supposed to get the block right above it where the player's head would be?
        _currentLiquidLevel = block->getLiquidMod() * 2;
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
    auto body      = _physical->getBody();

    if (!_clip)
    {
        animation = "falling-1";
    }
    else
    {
        auto movement = _destination - _physical->getPosition();
        auto speedX   = abs(movement.x);
        auto speedY   = abs(movement.y);
        auto hover    = _flyAccessory && _flyAccessory->isUsableType(UseType::HOVER);
        auto grounded = _avatar->isGrounded();

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
                // NOTE: This is an approximation and isn't going to be 100% accurate
                auto velocity = _physical->getVelocity();
                velocity.x    = math_util::lerp(velocity.x, 0.0F, deltaTime * 24.0F);
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

                if (utils::gettime() > _avatar->getLastFootstepSoundAt() + 0.3)
                {
                    _avatar->playWalkSfx(true);
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
        _stomping             = false;
        auto flying           = false;
        auto propulsion       = BLOCK_SIZE * (grounded ? 0.5F : 0.2F);  // Upward motion required to fly
        auto liquidResistance = 1.0F;

        if (movement.y > propulsion)
        {
            _lastPropelledUpwardAt = utils::gettime();
            flying = _flyAccessory != nullptr && (hasSteam() || hasAfterburner());
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

            // Apply impulse if it exceeds our current horizontal velocity
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
                    // TODO: emitter
                    auto impulse = mass * deltaTime * -3.0F;
                    body->applyImpulseAtLocalPoint(_physical->getVelocity() * impulse);
                    setSteam(_steam - _flyAccessory->getRate() * 0.2F * deltaTime / getSteamEfficiency());
                    counterforce = 0.95F;
                }

                // Apply counterforce against gravity
                auto gravity = zone->getSpace()->getGravity();
                auto impulse = mass * deltaTime * counterforce;
                body->applyImpulseAtLocalPoint(-(gravity * impulse));
            }
            else if (!grounded && _stompAccessory && movement.y <= BLOCK_SIZE * -0.75F && hasSteam() &&
                     useStompAccessory(_stompAccessory, deltaTime))
            {
                animation = "stomp-1";
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
                animation        = "swim-1";
                liquidResistance = 0.9F;
            }
            else
            {
                useFlyAccessory(_flyAccessory, deltaTime);
                liquidResistance = 0.5F;
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

        // 0x10001EEB6: Apply conveyor belt movement
        // NOTE: Normally done after limiting velocity, but that creates issues for us...
        if (grounded)
        {
            auto item = _avatar->getFeetItem();

            if (item && item->isUsableType(UseType::MOVE))
            {
                auto direction = _avatar->getFeetMod() == 0 ? 1.0F : -1.0F;
                auto impulse   = BLOCK_SIZE * item->getPower() * 20.0F * deltaTime * direction;
                body->applyImpulseAtLocalPoint(Vec2::UNIT_X * impulse);
            }
        }

        // 0x10001DBB2: Limit velocity
        auto velLimit = BLOCK_SIZE * 12.0F;
        auto velocity = _physical->getVelocity();

        if (_currentLiquidLevel > 0)
        {
            auto minSpeed = math_util::lerp(velLimit, velLimit * 0.2F, liquidResistance) * getSwimmingSpeed();
            velLimit      = math_util::lerp(velLimit, minSpeed, _currentLiquidLevel / 6.0F);
        }

        if (_stomping)
        {
            velLimit *= _stompAccessory->getPower() + 0.05F;
        }

        if (velocity.lengthSquared() > velLimit * velLimit)
        {
            _physical->setVelocity(velocity.getNormalized() * velLimit);
        }
    }

    // 0x10001DDF2: Apply entity obstruction
    if (_entitySlow > 0.01F)
    {
        auto scalar = clampf(1.0F - _entitySlow, 0.1F, 1.0F);
        _physical->setVelocity(_physical->getVelocity() * scalar);
    }

    _entitySlow = clampf(_entitySlow - deltaTime, 0.0F, 1.0F);
    body->setAngle(0.0F);
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

    // Update fly accessory power & play sound effect
    MathUtil::smooth(&_flyAccessoryPower, 0.0F, deltaTime, 1.0F);
    AudioManager::getInstance()->setAutoLoopLayer("jetpack", _flyAccessoryPower, _flyAccessoryPower);

    // 0x10001E882: Slowly restore steam if we haven't used it recently
    // TODO: awesome mode
    if (utils::gettime() > _lastUsedSteamAt + STEAM_RESTORE_COOLDOWN)
    {
        setSteam(_steam + deltaTime);
    }

    _mining = false;
    updateInventory();  // Failsafe
}

bool Player::checkHorizontalOverlap(float distance)
{
    if (!_clip)
    {
        return false;
    }

    auto velocity = _physical->getVelocity();

    if (velocity.x == 0.0F)
    {
        return false;
    }

    auto position  = getPosition();
    auto direction = velocity.x < 0.0F ? -1.0F : 1.0F;  // Horizontal movement direction
    auto x         = position.x + BLOCK_SIZE * 0.8F * direction * distance;
    auto start     = Point(x, position.y + BLOCK_SIZE * 1.6F);
    auto end       = Point(x, position.y + BLOCK_SIZE * 1.6F * 0.1F);
    auto filter    = cpShapeFilterNew(CP_NO_GROUP, 0x400, 0x400);  // Block layer filter
    return _game->getZone()->getSpace()->testSegmentQuery(start, end, 0.0F, filter);
}

void Player::stopIfHorizontalOverlap()
{
    if (checkHorizontalOverlap(0.5F))
    {
        auto velocity = _physical->getVelocity();
        _physical->setVelocity({0.0F, velocity.y});
    }
}

void Player::useFlyAccessory(Item* item, float deltaTime)
{
    // TODO: flight suppression
    // TODO: emit particles
    auto speedX    = _destination.x - _physical->getPosition().x;
    auto flySpeed  = getFlyingSpeed() * BLOCK_SIZE * deltaTime;
    auto direction = abs(speedX) < FLT_EPSILON ? 0.0F : speedX > 0.0F ? 1.0F : -1.0F;
    _physical->getBody()->applyImpulseAtLocalPoint({flySpeed * direction, flySpeed * 21.5F});
    auto tilt     = speedX / BLOCK_SIZE * 42.0F;
    auto rotation = math_util::lerp(_avatar->getRotation(), tilt, deltaTime * 1.75F);
    _avatar->setRotation(rotation);

    if (!hasAfterburner())
    {
        setSteam(_steam - item->getRate() * deltaTime / getSteamEfficiency());
    }

    _flyAccessoryPower = math_util::lerp(_flyAccessoryPower, 1.0F, deltaTime * 2.0F);
}

bool Player::useStompAccessory(Item* item, float deltaTime)
{
    if (utils::gettime() < _lastStompedAt + item->getAttackInterval())
    {
        return false;
    }

    // TODO: emit particles
    auto speedX     = _destination.x - _physical->getPosition().x;
    auto stompSpeed = BLOCK_SIZE * deltaTime;
    auto direction  = abs(speedX) < FLT_EPSILON ? 0.0F : speedX > 0.0F ? 1.0F : -1.0F;
    _physical->getBody()->applyImpulseAtLocalPoint(
        {stompSpeed * direction * 0.33F, item->getPower() * stompSpeed * -21.5F * 8.0F});
    auto rotation = math_util::lerp(_avatar->getRotation(), 0.0F, deltaTime * 4.0F);
    _avatar->setRotation(rotation);
    setSteam(_steam - item->getRate() * deltaTime / getSteamEfficiency());
    _stomping = true;
    return true;
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

void Player::emote(const std::string& text, const ax::Color3B& color, bool quick, bool replaceLast)
{
    _avatar->emote(text, color, quick, replaceLast);
}

void Player::playHurtSound(bool heavy)
{
    // TODO: male/female setting
    auto variant = std::format("male.{}", heavy ? "heavy" : "light");
    AudioManager::getInstance()->playSfx("ouch", variant, 0.0F, 0.0F, 0.5F);
}

void Player::teleportToZone(const std::string& id)
{
    AudioManager::getInstance()->clearLoopLayers();
    AudioManager::getInstance()->playSfx("teleport", 1.0F, 0.0F, 0.5F);
    // TODO: _usedZoneTeleporter = true;
    _zoneTeleporting = true;
    _game->snapshotScreenAsSpinner(true);
    AXLOGI("===== Teleporting to zone {} =====", id);
    GameGui::getMain()->hideTeleportInterface();
    _game->sendMessage(MessageIdent::ZONE_CHANGE, id);
}

void Player::respawn()
{
    if (!isRespawning())
    {
        if (isDead())
        {
            auto message = "Respawning..."s;
            _game->getEventDispatcher()->dispatchCustomEvent(events::kDeathMessageChanged, &message);
        }

        _respawnStartedAt = utils::gettime();
        AudioManager::getInstance()->playSfx("respawn");
        ax_util::scheduleOnce([this](float) { sendRespawnMessage(); }, this, 0.3334F, "respawn");
    }
}

void Player::sendRespawnMessage()
{
    _game->sendMessage(MessageIdent::RESPAWN, 0);
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
    Point target    = {_target.x * multiplier, _target.y * -multiplier};
    auto animation  = _avatar->getCurrentAnimation();
    _game->sendMessage(MessageIdent::MOVE, position.x, position.y, velocity.x, velocity.y, _lookDirection, target.x,
                       target.y, animation);
    _nextMoveMessageTime = time + MOVE_MESSAGE_INTERVAL;
}

int64_t Player::getMaxRecipeQuantity(Recipe* recipe)
{
    int64_t result = -1;

    for (auto& ingredient : recipe->getIngredients())
    {
        auto owned = getInventoryQuantity(ingredient.item);

        if (owned < 1)
        {
            return 0;
        }

        auto quantity = owned / ingredient.quantity;
        result = result == -1 ? quantity : MIN(result, quantity);
    }

    return result;
}

bool Player::canMakeRecipe(Recipe* recipe)
{
    return isSkilledToCraft(recipe->getItem()) && getMaxRecipeQuantity(recipe) > 0;
}

bool Player::makeRecipe(Recipe* recipe)
{
    if (!canMakeRecipe(recipe))
    {
        return false;
    }

    auto item = recipe->getItem();

    // If workshopping, the server will perform the workshop checks and send an inventory message,
    // so we will not update the inventory ourselves in that case.
    if (recipe->getHelpers().empty())
    {
        for (auto& ingredient : recipe->getIngredients())
        {
            addInventory(ingredient.item, -ingredient.quantity);
        }

        addInventory(item, recipe->getQuantity());
    }

    // TODO: track statistic, fire event
    _game->sendMessage(MessageIdent::CRAFT, item->getCode());
    return true;
}

void Player::sendInventoryUseMessage(Item* item, bool secondary, bool onlyIfAllowed)
{
    if (onlyIfAllowed && item && item->isGun() && !hasSteam())
    {
        return;
    }

    auto type   = secondary ? 1 : 0;
    auto code   = item ? item->getCode() : 0;
    auto status = item ? 1 : 2;
    _game->sendMessage(MessageIdent::INVENTORY_USE, type, code, status);
}

bool Player::useActiveHotbarItem(const Point& point)
{
    return useInventoryItem(_activeHotbarItem, point);
}

bool Player::useInventoryItem(InventoryItem* invItem, const Point& point)
{
    auto result     = false;
    auto mining     = false;
    Item* usingItem = nullptr;

    if (_avatar->isAlive() && !point.isZero())
    {
        if (invItem)
        {
            auto item = invItem->getItem();
            usingItem = item;

            if (item->isPlaceable())
            {
                result    = tryToPlaceInventoryItem(invItem, point);
                usingItem = nullptr;
            }
            else if (item->isMiningTool())
            {
                result = tryToMineBlockAtNodePoint(point, invItem);
                mining = true;
            }
            else if (item->isTool())
            {
                // TODO: implement other tools
                result = true;
                _avatar->animateTool(point);
            }
            else if (item->getAction() == Item::Action::SHIELD)
            {
                // TODO: implement shields
            }
        }
        else
        {
            // Mining with bare hands
            result = tryToMineBlockAtNodePoint(point, invItem);
            mining = true;
        }
    }

    // TODO: secondary
    setUsingPrimaryItem(usingItem);

    if (!mining)
    {
        _avatar->setTargetItem(nullptr);

        if (_miningBlock)
        {
            _miningBlock->cancelMining();
        }
    }

    return result;
}

BaseBlock* Player::tryToUseBlockAtNodePoint(const Point& point)
{
    if (_avatar->isAlive() && canDigAt(point))
    {
        auto zone  = _game->getZone();
        auto block = zone->getBlockAtNodePoint(point);

        if (block)
        {
            block = zone->findReachableBlock(block->getX(), block->getY(), BlockLayer::FRONT, true);

            if (block)
            {
                block->useLayer(block->getTopUsableLayer());
            }
        }
    }

    return nullptr;
}

BaseBlock* Player::tryToMineBlockAtNodePoint(const Point& point, InventoryItem* invItem)
{
    BaseBlock* target = nullptr;
    Item* targetItem  = nullptr;
    auto layer        = BlockLayer::NONE;
    auto zone         = _game->getZone();

    // 0x100024487: Find block to mine at target point
    if (canDigAt(point))
    {
        if (auto block = zone->getBlockAtNodePoint(point))
        {
            auto x = block->getX();
            auto y = block->getY();

            if (target = zone->findReachableBlock(x, y, BlockLayer::FRONT))
            {
                targetItem = target->getFrontItem();
                layer      = BlockLayer::FRONT;
            }
            else if (target = zone->findReachableBlock(x, y, BlockLayer::BACK))
            {
                targetItem = target->getBackItem();
                layer      = BlockLayer::BACK;
            }
        }
    }

    _avatar->setTargetItem(targetItem);  // BUGFIX: Don't replay previous audio when mining air
    auto item    = invItem ? invItem->getItem() : nullptr;
    auto digging = item && item->getAction() == Item::Action::DIG && targetItem && targetItem->isDiggable();

    // 0x1000245AD: Update mining state (allow bare-handed mining)
    if (!item || item->isTool())
    {
        _mining = true;
    }

    // 0x1000245F2: Animate tool
    if (_mining)
    {
        _avatar->animateTool();
    }

    // TODO: generate mining debris

    // 0x100024695: Process mining attempt
    auto toolSwung = true;

    if (utils::gettime() >= _lastMiningAttemptAt + 0.1 && utils::gettime() < _avatar->getLastSwungToolAt() + 0.1)
    {
        _lastMiningAttemptAt = utils::gettime();

        if (_miningAttemptBlock != target)
        {
            _miningAttempts     = 1;
            _miningAttemptBlock = target;  // Weak ref
        }
        else
        {
            _miningAttempts++;
        }
    }
    else
    {
        toolSwung = false;
    }

    // Return here if there is no valid mining target
    if (!target)
    {
        if (_miningBlock)
        {
            _miningBlock->cancelMining();
        }

        return nullptr;
    }

    // 0x1000247F5: Check protection
    if (!digging && target->isProtectedByField())
    {
        // TODO: show protective field hint

        if (toolSwung)
        {
            auto sprite = target->getMainSpriteForLayer(layer);
            zone->getWorldRenderer()->glowSprite(sprite);

            // 0x100024892: Show protected field alert
            if (_miningAttempts == 3)
            {
                std::string alert = "This block is protected by a force field";

                if (zone->isProtected() && !zone->isMember())
                {
                    auto& reason = zone->getProtectedReason();
                    alert        = reason.empty() ? "This world is protected" : reason;
                }

                alert = std::format("{}{}", alert,
                                    targetItem->isDiggable() ? ", but you can use a shovel to dig through it." : ".");
                Value data(alert);
                _game->notify(NotificationType::ALERT, data);
            }
        }

        return target;
    }

    // 0x100024960: Check mining skill
    if (!isSkilledToMine(targetItem))
    {
        // TODO: generate debris

        // 0x100024A42: Show unskilled alert
        if (_miningAttempts == 3)
        {
            auto alert = std::format("You need a higher {} skill to mine this block.", targetItem->getMiningSkill());
            Value data(alert);
            _game->notify(NotificationType::ALERT, data);
        }

        return target;
    }

    // TODO: check mining suppression

    // 0x100024AB2: Begin mining the block
    if (_miningBlock != target || _miningLayer != layer)
    {
        if (_miningBlock)
        {
            _miningBlock->cancelMining();
        }

        if (item && item->getAction() == Item::Action::SMASH && targetItem->getModType() == ModType::DECAY)
        {
            auto x   = target->getX();
            auto y   = target->getY();
            auto mod = target->getModForLayer(layer);
            target->setModForLayer(layer, MAX(2, MIN(4, mod + 1)));
            _game->sendMessage(MessageIdent::BLOCK_MINE, x, y, static_cast<uint8_t>(layer) - 1, targetItem->getCode(), 0);
        }
        else
        {
            target->startMining(layer, item);
        }
    }

    setMiningBlock(target);
    setMiningLayer(layer);
    return target;
}

bool Player::tryToPlaceInventoryItem(InventoryItem* invItem, const Point& point)
{
    // Check cooldown
    if (utils::gettime() < _nextAllowedPlaceTime)
    {
        return false;
    }

    // Check item quantity
    if (!invItem || invItem->getQuantity() <= 0)
    {
        return false;
    }

    auto block = _game->getZone()->getBlockAtNodePoint(point);
    auto item  = invItem->getItem();

    if (block && canPlaceItem(item, block))
    {
        // 0x10002546F: Check placing skill
        if (!isSkilledToPlace(item))
        {
            auto alert = std::format("You need {} skill level {} to place that.", item->getPlacingSkill(), item->getPlacingSkillLevel());
            Value data(alert);
            _game->notify(NotificationType::ALERT, data);
            _nextAllowedPlaceTime = utils::gettime() + 1.0;
            return false;
        }

        // TODO: check biome restriction
        // TODO: check placing cost (only used by butler bots)
        // TODO: check spacing

        auto x     = block->getX();
        auto y     = block->getY();
        auto layer = item->getLayer();
        auto code  = item->getCode();
        auto mod   = layer == BlockLayer::LIQUID ? 5 : item->getPlaceMod();

        // TODO: handle rotation
        if (item->isMirrorable() && _lookDirection == -1)
        {
            mod = 4;
        }

        block->setLayer(layer, code, mod);
        invItem->setQuantity(invItem->getQuantity() - 1);
        _game->sendMessage(MessageIdent::BLOCK_PLACE, x, y, static_cast<uint8_t>(layer) - 1, code, mod);
        AudioManager::getInstance()->playButtonSfx();
        _lastPlacedBlock = block;  // Weak ref
        // TODO: fire event
        return true;
    }

    if (block != _lastPlacedBlock)
    {
        AudioManager::getInstance()->playSfx("error", 1.0F, 0.0F, 0.3F);
    }

    _nextAllowedPlaceTime = utils::gettime() + 0.1;
    return false;
}

bool Player::canDigAt(const Point& point) const
{
    auto zone   = _game->getZone();
    auto origin = zone->getBlockAtNodePoint(getPhysicalCenter());
    auto target = zone->getBlockAtNodePoint(point);

    if (!origin || !target)
    {
        return false;
    }

    auto distance = (int)math_util::getDistance(origin->getX(), origin->getY(), target->getX(), target->getY());

    if (distance > 2)
    {
        return false;  // Too far away
    }

    if (distance < 2)
    {
        return true;
    }

    // Shitty ray cast
    for (auto i = 0; i < 4; i++)
    {
        auto offset = (i % 2) ? 1 : -1;
        auto y      = i >= 2;  // Whether we're checking the Y axis

        if ((y ? origin->getY() : origin->getX()) + offset * 2 == (y ? target->getY() : target->getX()))
        {
            auto block = zone->getBlockAt(origin->getX() + (y ? 0 : offset), origin->getY() + (y ? offset : 0));

            if (!block || block->isWhole())
            {
                return false;  // Target is obstructed
            }
        }
    }

    return true;  // No obstructions
}

bool Player::canPlaceItem(Item* item, BaseBlock* block)
{
    // 0x100027AA6: Check protection
    if (!item->canPlaceInField() && block->isProtectedByField())
    {
        return false;
    }

    // 0x100027ABE: Check reach
    if (item->hasReach() && !canDigAt(block->getWorldPosition()))
    {
        return false;
    }

    // 0x100027AFB: Check placing range
    auto position = getBlockPosition();
    auto distance = math_util::getDistance(position.x, position.y, block->getX(), block->getY());

    if (distance >= getPlacingRange())
    {
        return false;
    }

    // 0x100027B37: Check if block is occupied
    auto width  = MAX(1, item->getWidth());
    auto height = MAX(1, item->getHeight());

    if (item->isTileable() || (width == 1 && height == 1))
    {
        return block->canPlace(item);
    }

    for (auto x = block->getX(); x < block->getX() + width; x++)
    {
        for (auto y = block->getY(); y > block->getY() - height; y--)
        {
            auto target = _game->getZone()->getBlockAt(x, y);

            if (!target || !target->canPlace(item))
            {
                return false;
            }
        }
    }

    return true;
}

void Player::slowForEntity(Entity* entity)
{
    auto obstruction = entity->getConfig()->getObstruction();

    if (obstruction > _entitySlow)
    {
        _entitySlow = obstruction;
    }
}

void Player::onFeetCollideWithBlock(BaseBlock* block)
{
    // TODO: finish

    if (_clip)
    {
        _avatar->walkOnBlock(block);

        // Generate stomp effect if stomping
        if (_stomping && _currentLiquidLevel < 3)
        {
            auto worldRenderer = _game->getZone()->getWorldRenderer();
            worldRenderer->generateEffect("bomb-stomp", _stompAccessory->getPower() * 2.0F, _avatar->getPosition());
            _lastStompedAt = utils::gettime();
        }
    }
}

void Player::onFeetCollideWithEntity(Entity* entity)
{
    // TODO: implement
}

void Player::onCollideWithEntity(Entity* entity)
{
    // TODO: implement
}

float Player::getRunningSpeed()
{
    return MathUtil::lerp(1.0F, 1.65F, getNormalizedSkill(kAgilitySkill));
}

float Player::getClimbingSpeed()
{
    return MathUtil::lerp(1.2F, 2.2F, getNormalizedSkill(kAgilitySkill));
}

float Player::getSwimmingSpeed()
{
    return MathUtil::lerp(1.0F, 1.7F, getNormalizedSkill(kAgilitySkill));
}

float Player::getJumpingPower()
{
    return MathUtil::lerp(1.0F, 1.4F, getNormalizedSkill(kAgilitySkill));
}

float Player::getFlyingSpeed()
{
    auto power = _flyAccessory->getPower();
    auto speed = MathUtil::lerp(1.0F, 1.65F, getNormalizedSkill(kEngineeringSkill));

    if (hasAfterburner())
    {
        speed *= 1.25F;
    }

    return speed * power;
}

float Player::getPlacingRange()
{
    auto range = MathUtil::lerp(4.0F, 12.0F, getNormalizedSkill(kBuildingSkill));

    if (hasAccessoryWithUse(UseType::BUILDING_EXTENSION))
    {
        range *= 2.0F;
    }

    return range;
}

float Player::getMiningSpeed()
{
    return MathUtil::lerp(1.0F, 1.75F, getNormalizedSkill(kMiningSkill));
}

float Player::getZoomModifier()
{
    auto perception = getAdjustedSkill(kPerceptionSkill);
    return MathUtil::lerp(0.0F, 1.0F, (float)(perception - 4) / 11.0F);
}

bool Player::canSeeProtectorRanges()
{
    return getAdjustedSkill(kPerceptionSkill) > 2;
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

Point Player::getPhysicalCenter() const
{
    return getPosition() + Vec2::UNIT_Y * _avatar->getContentSize().height * 0.5F;
}

bool Player::isGrounded() const
{
    return _avatar->isGrounded();
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

void Player::setHealth(float health)
{
    auto maxHealth = getMaxHealth();
    auto clamped   = clampf(health, 0.0F, maxHealth);

    if (_health == clamped)
    {
        return;
    }

    auto dispatcher = Director::getInstance()->getEventDispatcher();
    float data[]    = {_health, clamped, maxHealth};  // 0 = old health, 1 = new health, 2 = max health
    dispatcher->dispatchCustomEvent(events::kPlayerHealthChanged, &data);
    _health = clamped;
    _avatar->setAlive(isAlive());

    if (isAlive())
    {
        return;
    }

    // TODO: death message changes if we were killed by another player
    auto deathMessage = std::format("You have died. {}", GameGui::getMain()->getRespawnMessage());
    dispatcher->dispatchCustomEvent(events::kDeathMessageChanged, &deathMessage);
    dispatcher->dispatchCustomEvent(events::kPlayerDeathEvent);
}

float Player::getMaxHealth()
{
    auto stamina = MAX(0, MIN(getAdjustedSkill(kStaminaSkill), 10));
    return BASE_HEALTH + (stamina - (stamina == 10 ? 0 : 1)) * 0.5F;
}

void Player::setSteam(float steam)
{
    auto clamped = clampf(steam, 0.0F, getMaxSteam());

    if (_steam == clamped)
    {
        return;
    }

    auto eventDispatcher = Director::getInstance()->getEventDispatcher();

    if (clamped < _steam)
    {
        _lastUsedSteamAt = utils::gettime();

        if (clamped == 0.0F && getAdjustedSkill(kEngineeringSkill) < 10)
        {
            _steamCooldownAt = utils::gettime() + getSteamCooldownDuration();
            // TODO: sends inventory use message?
            eventDispatcher->dispatchCustomEvent(events::kSteamCooldownBegan);
        }
    }

    if (_steamCooldownAt > 0.0 && utils::gettime() > _steamCooldownAt)
    {
        _steamCooldownAt = 0.0;
        // TODO: sends inventory use message with primary item
        eventDispatcher->dispatchCustomEvent(events::kSteamCooldownEnded);
    }

    _steam = clamped;
    eventDispatcher->dispatchCustomEvent(events::kSteamChanged, &clamped);
}

bool Player::hasSteam() const
{
    return _steam > 0.0F && utils::gettime() > _steamCooldownAt;
}

float Player::getMaxSteam() const
{
    // Find highest steam bonus accessory
    float bonus = 0.0F;

    for (auto item : _cachedAccessoryItems)
    {
        if (item->isUsableType(UseType::STEAM_BONUS))
        {
            auto& value = map_util::getValue(item->getData(), "bonus");
#if _AX_DEBUG  // Bypass assertion in debug mode
            auto type = value.getType();

            if (type != Value::Type::VECTOR && type != Value::Type::MAP && type != Value::Type::INT_KEY_MAP)
#endif
            {
                bonus = MAX(bonus, value.asFloat());
            }
        }
    }

    return BASE_MAX_STEAM + bonus;
}

float Player::getSteamEfficiency()
{
    return math_util::lerp(1.0F, 1.5F, getNormalizedSkill(kEngineeringSkill));
}

float Player::getSteamCooldownDuration()
{
    auto engineering = getAdjustedSkill(kEngineeringSkill);
    return math_util::lerp(4.0F, 1.0F, (float)engineering / 10.0F);
}

void Player::setSkill(const std::string& name, int32_t level)
{
    _cachedAdjustedSkills.erase(name);
    _skills[name] = level;

    // Stamina determines max health & accessory slots, so we should update those immediately.
    if (name == kStaminaSkill)
    {
        auto maxHealth = getMaxHealth();

        if (_health > maxHealth)
        {
            setHealth(maxHealth);
        }

        updateAccessories();
    }

    if (_game->getZone()->getState() == WorldZone::State::ACTIVE)
    {
        // TODO: pass info
        _game->getEventDispatcher()->dispatchCustomEvent(events::kPlayerSkillChanged);
    }
}

int32_t Player::getSkill(const std::string& name) const
{
    auto it = _skills.find(name);

    if (it != _skills.end())
    {
        return (*it).second;
    }

    return 0;
}

int32_t Player::getAdjustedSkill(const std::string& name)
{
    auto it = _cachedAdjustedSkills.find(name);

    if (it != _cachedAdjustedSkills.end())
    {
        return (*it).second;
    }

    auto bonus = getSkillBonus(name);
    auto level = MAX(1, MIN(MAX_SKILL_LEVEL, getSkill(name) + bonus));
    _cachedAdjustedSkills[name] = level;
    return level;
}

float Player::getNormalizedSkill(const std::string& name)
{
    return (float)getAdjustedSkill(name) / MAX_SKILL_LEVEL;
}

int32_t Player::getSkillBonus(const std::string& name) const
{
    auto accessoryBonus = getHighestSkillBonus(name, _cachedAccessoryItems);
    auto hiddenBonus    = getHighestSkillBonus(name, _cachedHiddenItems);
    return accessoryBonus + hiddenBonus;
}

int32_t Player::getHighestSkillBonus(const std::string& name, const std::vector<Item*>& items) const
{
    int32_t result = 0;

    for (auto item : items)
    {
        if (item->isUsableType(UseType::SKILL_BONUS))
        {
            auto bonus = map_util::getInt32(item->getData(), std::format("bonus.{}", name));

            if (bonus > result)
            {
                result = bonus;
            }
        }
    }

    return result;
}

bool Player::isSkilledToMine(Item* item)
{
    auto level = item->getMiningSkillLevel();
    return level == 0 || getAdjustedSkill(item->getMiningSkill()) >= level;
}

bool Player::isSkilledToPlace(Item* item)
{
    auto level = item->getPlacingSkillLevel();
    return level == 0 || getAdjustedSkill(item->getPlacingSkill()) >= level;
}

bool Player::isSkilledToCraft(Item* item)
{
    auto level = item->getCraftingSkillLevel();
    return level == 0 || getAdjustedSkill(item->getCraftingSkill()) >= level;
}

int64_t Player::getMaxAccessories()
{
    // NOTE: Don't use adjusted skill to calculate
    auto bonus = hasInventory("accessories/toolbelt") ? 5 : 0;
    return (int64_t)5 + MAX(1, MIN(10, getSkill(kStaminaSkill))) + bonus;
}

static bool compareInventoryItemBySlot(InventoryItem* a, InventoryItem* b)
{
    return b->getSlot() > a->getSlot();
}

void Player::updateAccessories(bool defer)
{
    if (defer)
    {
        _shouldUpdateAccessories = true;
        return;
    }

    _cachedAdjustedSkills.clear();  // Accessory skill bonuses must be recalculated
    _cachedAccessoryItems.clear();
    _cachedHiddenItems.clear();
    _flyAccessory   = nullptr;
    _stompAccessory = nullptr;

    // Find accessories & hidden items
    std::vector<InventoryItem*> accessoryItems;
    std::vector<InventoryItem*> hiddenItems;
    auto maxAccessories = getMaxAccessories();

    for (auto& entry : _inventory)
    {
        auto& invItem = entry.second;

        switch (invItem->getContainer())
        {
        case ContainerType::ACCESSORY:
            if (invItem->getSlot() < maxAccessories)
            {
                accessoryItems.push_back(invItem);
            }
            break;
        case ContainerType::HIDDEN:
            hiddenItems.push_back(invItem);
            break;
        }
    }

    // Sort items by slot & transform vectors
    std::sort(accessoryItems.begin(), accessoryItems.end(), compareInventoryItemBySlot);
    _cachedAccessoryItems.resize(accessoryItems.size());
    std::transform(accessoryItems.begin(), accessoryItems.end(), _cachedAccessoryItems.begin(),
                   [](InventoryItem* item) { return item->getItem(); });
    _cachedHiddenItems.resize(hiddenItems.size());
    std::sort(hiddenItems.begin(), hiddenItems.end(), compareInventoryItemBySlot);
    std::transform(hiddenItems.begin(), hiddenItems.end(), _cachedHiddenItems.begin(),
                   [](InventoryItem* item) { return item->getItem(); });

    // Update flying accessory & shield
    for (auto item : _cachedAccessoryItems)
    {
        // Check if flying accessory
        if (item->isUsableType(UseType::FLY) || item->isUsableType(UseType::PROPEL) ||
            item->isUsableType(UseType::HOVER))
        {
            _flyAccessory = item;
        }

        // Check if shield
        if (item->getAction() == Item::Action::SHIELD)
        {
            _activeShieldItem = getInventory(item);
        }
    }

    // Update stomping accessory
    for (auto item : _cachedHiddenItems)
    {
        if (item->getAction() == Item::Action::EXOLEG)
        {
            _stompAccessory = item;
        }
    }

    // Equip flying accessory on avatar
    auto uniform = _flyAccessory ? _flyAccessory->getCode() : 0;
    auto details = map_util::mapOf("u", uniform);
    _avatar->change(details);

    // Update health if new max health is less than current health
    auto maxHealth = getMaxHealth();

    if (maxHealth < _health)
    {
        setHealth(maxHealth);
    }

    // Fire accessories changed event
    _game->getEventDispatcher()->dispatchCustomEvent(events::kPlayerAccessoriesChanged, this);
    _shouldUpdateAccessories = false;
}

bool Player::hasAccessory(const std::string& name) const
{
    for (auto item : _cachedAccessoryItems)
    {
        if (item->getName() == name)
        {
            return true;
        }
    }

    return false;
}

bool Player::hasAccessoryWithUse(UseType use) const
{
    for (auto item : _cachedAccessoryItems)
    {
        if (item->isUsableType(use))
        {
            return true;
        }
    }

    return false;
}

bool Player::hasAfterburner() const
{
    return hasAccessoryWithUse(UseType::AFTERBURNER);
}

InventoryItem* Player::setInventory(Item* item, int64_t quantity)
{
    auto invItem = getInventory(item, true);
    invItem->setQuantity(quantity);
    return invItem;
}

InventoryItem* Player::setInventory(Item* item, int64_t quantity, ContainerType container, int64_t slot)
{
    auto code = item->getCode();
    auto it   = _inventory.find(code);
    InventoryItem* result;

    if (it == _inventory.end())
    {
        // Create new inventory item
        // BUGFIX: Show quantity notification for newly obtained items
        if (quantity > 0 && _game->getZone()->getState() == WorldZone::State::ACTIVE)
        {
            result = InventoryItem::createWithItem(item, 0, container, slot);
            _inventory.insert(code, result);
            result->setQuantity(quantity);
        }
        else
        {
            result = InventoryItem::createWithItem(item, quantity, container, slot);
            _inventory.insert(code, result);
            result->update();
        }
    }
    else
    {
        // Update existing inventory item
        result = (*it).second;
        result->setQuantity(quantity);
        result->moveToContainer(container, slot);
    }

    if (container == ContainerType::HOTBAR && _activeHotbarSlot == slot)
    {
        updateActiveHotbarItem();
    }

    return result;
}

InventoryItem* Player::addInventory(Item* item, int64_t quantity)
{
    auto invItem = getInventory(item, true);
    invItem->setQuantity(invItem->getQuantity() + quantity);
    return invItem;
}

InventoryItem* Player::getInventory(Item* item, bool allowAlloc)
{
    auto code = item->getCode();
    auto it   = _inventory.find(code);

    if (it != _inventory.end())
    {
        return (*it).second;
    }

    if (!allowAlloc)
    {
        return nullptr;
    }

    auto result = InventoryItem::createWithItem(item, 0, ContainerType::NONE);
    _inventory.insert(code, result);
    result->update();
    return result;
}

InventoryItem* Player::getInventory(const std::string& name)
{
    auto item = _game->getConfig()->getItemForName(name);
    return item ? getInventory(item) : nullptr;
}

bool Player::hasInventory(const std::string& name)
{
    auto invItem = getInventory(name);
    return invItem && invItem->getQuantity() > 0;
}

int64_t Player::getInventoryQuantity(Item* item)
{
    auto invItem = getInventory(item);
    return invItem ? invItem->getQuantity() : 0;
}

int64_t Player::getNextInventorySlot(ContainerType type)
{
    if (type == ContainerType::INVENTORY)
    {
        return 0;
    }

    auto container = GameGui::getMain()->getItemContainerForType(type);
    return container ? container->getNextAvailableSlot() : -1;
}

InventoryItem* Player::getInventoryItem(ContainerType container, int64_t slot, int64_t category)
{
    // It's faster than it looks... probably
    for (auto& entry : _inventory)
    {
        auto item = entry.second;

        if (item->getContainer() == container && item->getSlot() == slot && item->getCategory() == category)
        {
            return item;
        }
    }

    return nullptr;
}

static bool compareInventoryItemByOrder(InventoryItem* a, InventoryItem* b)
{
    return b->getItem()->getInventoryPosition().slot > a->getItem()->getInventoryPosition().slot;
}

void Player::arrangeInventory(Item* item, bool defer)
{
    arrangeInventory(item->getInventoryPosition().category, defer);
}

void Player::arrangeInventory(int64_t category, bool defer)
{
    if (defer)
    {
        _categoriesToArrange.insert(category);
        return;
    }

    // Create a vector of all inventory items in the target category
    std::vector<InventoryItem*> itemsInCategory;

    for (auto& entry : _inventory)
    {
        if (entry.second->isInInventory() && entry.second->getItem()->getInventoryPosition().category == category)
        {
            itemsInCategory.push_back(entry.second);
        }
    }

    // Sort items in ascending slot order
    std::sort(itemsInCategory.begin(), itemsInCategory.end(), compareInventoryItemByOrder);

    // Update position based on index in the sorted vector
    for (ssize_t i = 0; i < itemsInCategory.size(); i++)
    {
        itemsInCategory[i]->setPosition(i, category);
    }
}

void Player::updateInventory()
{
    // Rearrange dirty categories
    if (!_categoriesToArrange.empty())
    {
        for (auto category : _categoriesToArrange)
        {
            arrangeInventory(category);
        }

        _categoriesToArrange.clear();
    }

    // Update accessories if marked as dirty
    if (_shouldUpdateAccessories)
    {
        updateAccessories();  // Clears flag as well
    }
}

void Player::updateActiveHotbarItem()
{
    _activeHotbarItem = getInventoryItem(ContainerType::HOTBAR, _activeHotbarSlot);
    auto item = _activeHotbarItem ? _activeHotbarItem->getItem() : nullptr;
    int16_t code;

    if (item && item->isTool())
    {
        _avatar->setToolItem(item);
        code = item->getCode();
    }
    else
    {
        _avatar->setToolItem(nullptr);
        code = 0;
    }

    _game->sendMessage(MessageIdent::INVENTORY_USE, 0, code, 0);  // Primary, Item, Select
    auto eventDispatcher = Director::getInstance()->getEventDispatcher();
    eventDispatcher->dispatchCustomEvent(events::kActiveHotbarItemChanged, item);
}

void Player::setActiveHotbarSlot(int64_t slot)
{
    _activeHotbarSlot = slot < 0 ? kHotbarItemCount - 1 : slot >= kHotbarItemCount ? 0 : slot;
    updateActiveHotbarItem();
    GameGui::getMain()->updateHotbar();
}

void Player::setUsingPrimaryItem(Item* item)
{
    if (_usingPrimaryItem != item)
    {
        _usingPrimaryItem = item;
        sendInventoryUseMessage(item);
    }
}

void Player::setUsingSecondaryItem(Item* item)
{
    if (_usingSecondaryItem != item)
    {
        _usingSecondaryItem = item;
        sendInventoryUseMessage(item, true);
    }
}

void Player::setMiningBlock(BaseBlock* block)
{
    if (_miningBlock != block)
    {
        if (_miningBlock)
        {
            AX_SAFE_RELEASE(_miningBlock);
        }

        _miningBlock = block;
        AX_SAFE_RETAIN(_miningBlock);
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
