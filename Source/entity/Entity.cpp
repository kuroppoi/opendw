#include "Entity.h"

#include "base/GameConfig.h"
#include "entity/EntityAnimated.h"
#include "entity/EntityAnimatedAvatar.h"
#include "entity/EntityAnimatedGhost.h"
#include "entity/EntityConfig.h"
#include "entity/EntityFramed.h"
#include "entity/MutableEntityConfig.h"
#include "graphics/WorldRenderer.h"
#include "physics/ChipmunkBody.h"
#include "physics/Physical.h"
#include "util/AxUtil.h"
#include "util/ColorUtil.h"
#include "util/MapUtil.h"
#include "util/MathUtil.h"
#include "zone/WorldZone.h"
#include "AudioManager.h"
#include "CommonDefs.h"

USING_NS_AX;

namespace opendw
{

Entity::~Entity()
{
    if (_aggregateConfig != _config)
    {
        AX_SAFE_RELEASE(_aggregateConfig);
    }

    AX_SAFE_RELEASE(_physical);
    AX_SAFE_RELEASE(_lastEmote);
}

Entity* Entity::createWithConfig(EntityConfig* config, const std::string& name, const ValueMap& details)
{
    if (!config)
    {
        return nullptr;
    }

    auto& spine       = config->getSpine();
    auto& classSuffix = config->getClassSuffix();
    std::string className;
    Entity* entity = nullptr;

    if (spine.empty())
    {
        if (classSuffix.empty())
        {
            entity = new EntityFramed();
        }
        else
        {
            className = std::format("Entity{}", classSuffix);
        }
    }
    else
    {
        // Empty class suffix will just result in EntityAnimated
        className = std::format("EntityAnimated{}", classSuffix);
    }

    // Create entity if necessary
    if (!entity)
    {
        // TODO: finish
        if (className == "EntityAnimated")
        {
            entity = new EntityAnimated();
        }
        else if (className == "EntityAnimatedAvatar")
        {
            entity = new EntityAnimatedAvatar();
        }
        else if (className == "EntityAnimatedGhost")
        {
            entity = new EntityAnimatedGhost();
        }
    }

    if (entity && entity->initWithConfig(config, name, details))
    {
        entity->autorelease();
        return entity;
    }

    return nullptr;
}

bool Entity::initWithConfig(EntityConfig* config, const std::string& name, const ValueMap& details)
{
    auto file = isHuman() ? "characters-animated+hd2.png" : "entities+hd2.png";

    if (!Sprite::initWithFile(file))
    {
        return false;
    }

    // 0x1000BB84D: Configure components
    auto& components = map_util::getArray(details, "C");

    if (components != ValueVectorNull)
    {
        for (auto& component : components)
        {
            auto code   = component.asInt();
            auto config = GameConfig::getMain()->getEntityForCode(code);

            if (config)
            {
                _components.push_back(config);
            }
        }
    }

    // 0x1000BBA12: Configure properties
    _config           = config;
    _aggregateConfig  = config;
    _details          = details;
    _clientDirected   = map_util::getBool(details, "*");
    _currentAnimation = -1;
    _alive            = true;
    _physical         = nullptr;

    // 0x1000BBB9B: Configure graphics
    auto scaleBase  = _config->getScaleBase();
    auto scaleRange = _config->getScaleRange();
    auto scale      = random(scaleBase - scaleRange * 0.5F, scaleBase + scaleRange * 0.5F);
    setScale(scale);
    setCascadeOpacityEnabled(true);
    setCascadeColorEnabled(true);
    buildGraphics();

    if (!name.empty())
    {
        setEntityName(name);
        createNameLabel();
    }

    if (!_components.empty())
    {
        auto config = new MutableEntityConfig();
        config->autorelease();
        config->retain();

        for (auto component : _components)
        {
            config->inheritFromConfig(component);
        }

        _aggregateConfig = config;
    }

    finishGraphics();
    buildPhysical();
    change(details);
    return true;
}

void Entity::buildPhysical()
{
    AX_ASSERT(!_physical);
    _physical = Physical::createWithTarget(this);
    AX_SAFE_RETAIN(_physical);

    if (_playerAvatar)
    {
        return;  // Player physics object is configured by the player object
    }

    if (isBlock())
    {
        _physical->useStaticBody();
        return;  // Shape will be created when the entity is positioned for the first time
    }

    if (_contentSize.isZero())
    {
        return;
    }

    switch (_config->getShape())
    {
    case EntityConfig::Shape::BOX:
    {
        if (isAvatar())
        {
            auto halfSize = _contentSize * 0.5F;
            auto origin   = halfSize * 0.5F;
            _physical->setShapeAsBox(halfSize, -origin);
            _physical->addBoxShape(halfSize, {origin.x, -origin.y});
            _physical->addBoxShape(halfSize, {-origin.x, origin.y});
            _physical->addBoxShape(halfSize, origin);
        }
        else
        {
            _physical->setShapeAsBox(getPhysicalSize(), getPhysicalOffset());
        }

        break;
    }
    case EntityConfig::Shape::CIRCLE:
    {
        auto size   = getPhysicalSize();
        auto radius = MAX(size.x, size.y) * getScale() * 0.25F;
        _physical->setShapeAsCircle(radius, getPhysicalOffset());
        break;
    }
    }

    // TODO: properly set layers
    if (isAvatar())
    {
        _physical->setLayer(0x8);  // Peer layer
    }
    else
    {
        _physical->setLayer(_config->hasCollision() ? 0x836 : 0x814);
    }

    if (_config->getGravity() == 0.0F)
    {
        _physical->getBody()->setType(CP_BODY_TYPE_KINEMATIC);
    }
    else
    {
        // TODO: implement
    }

    // TODO: finish
}

void Entity::playEntrySoundEffects()
{
    if (WorldZone::getMain()->getState() != WorldZone::State::ACTIVE)
    {
        return;
    }

    // 0x1000BC775: Play random entry sound
    auto& sounds = _config->getSounds();

    if (!sounds.empty())
    {
        auto& sound = sounds[random() % sounds.size()];
        auto pitch  = random(0.85F, 1.15F);
        AudioManager::getInstance()->playSfx(sound, _position, pitch, 0.5F);
    }

    // 0x1000BC81A: Play power on sounds
    auto& powerOnSounds = _config->getPowerOnSounds();

    if (!powerOnSounds.empty())
    {
        float currentDelay = 0.0F;

        for (auto& sound : powerOnSounds)
        {
            auto delay  = DelayTime::create(currentDelay);
            auto action = CallFunc::create([=]() { AudioManager::getInstance()->playSfx(sound, _position, 1.0F, 0.5F); });
            this->runAction(Sequence::createWithTwoActions(delay, action));
            currentDelay += 1.0F;
        }
    }
}

void Entity::onExit()
{
    Sprite::onExit();

    if (_nameLabel)
    {
        _nameLabel->removeFromParent();
        _nameLabel->release();
    }
}

void Entity::update(float deltaTime)
{
    if (isBlock())
    {
        // TODO: probably need to find the shortest rotation distance or something
        auto rotation = MathUtil::lerp(getRotation(), _realRotation, fminf(1.0F, deltaTime * 10.0F));
        setRotation(rotation);
    }

    // 0x1000BCF33: Randomly play an ambient sound
    auto& ambientSounds = _config->getAmbientSounds();

    if (!ambientSounds.empty() && rand_0_1() < deltaTime * 0.1F)
    {
        auto& sound = ambientSounds[random() % ambientSounds.size()];
        auto pitch  = random(0.85F, 1.15F);
        AudioManager::getInstance()->playSfx(sound, _position, pitch, 0.4F);
    }

    // 0x1000BD003: Slowly reduce emote offset
    if (_emoteOffset > 0.0F)
    {
        _emoteOffset = clampf(_emoteOffset - deltaTime, 0.0F, 999.0F);
    }

    // 0x1000BD03D: Update change color
    if (_colorize)
    {
        auto color = color_util::lerpColor(_realColor, _changeColor, deltaTime * 2.0F);
        setColor(color);
    }

    // NOTE: Originally done in EntityAnimatedHuman::step:
    if (_alive && _grounded)
    {
        _lastGroundedAt = utils::gettime();
    }
}

void Entity::updateOnscreen(float deltaTime, bool onscreen)
{
    // TODO: finish

    if (!_positioned && !_clientDirected)
    {
        return;
    }

    if (!_clientDirected)
    {
        auto offset =
            _config->isBlock() ? Vec2::ZERO : math_util::rotateVector(Vec2::UNIT_Y * -BLOCK_SIZE * 0.5F, _realRotation);
        auto& size = _config->getSize();

        if (size.width > 1.0F)
        {
            offset.x += (_contentSize.width - BLOCK_SIZE) * 0.5F;
        }

        Point targetPosition(_realPosition.x + offset.x, _realPosition.y - offset.y);
        auto distance = math_util::getDistance(_position.x, _position.y, targetPosition.x, targetPosition.y);

        // Move smoothly if the target position is nearby, otherwise move abruptly.
        if (onscreen && distance < BLOCK_SIZE * 3.0F)
        {
            auto x        = MathUtil::lerp(_position.x, targetPosition.x, fminf(1.0F, deltaTime * 3.0F));
            auto y        = MathUtil::lerp(_position.y, targetPosition.y, fminf(1.0F, deltaTime * 5.0F));
            auto rotation = MathUtil::lerp(getRotation(), _realRotation, fminf(1.0F, deltaTime * 10.0F));  // TODO
            setPosition(x, y);
            setRotation(rotation);
        }
        else
        {
            setPosition(targetPosition);
            setRotation(_realRotation);
        }
    }

    // TODO
    auto offset = Vec2::UNIT_Y * _contentSize * 0.5F;
    offset      = math_util::rotateVector(offset, -getRotation());
    _physical->setPosition(_position - offset);
    _physical->getBody()->setAngle(MATH_DEG_TO_RAD(-getRotation()));

    // TODO
    if (_nameLabel)
    {
        _nameLabel->setString(_name);
        _nameLabel->setPosition(_position.x, _position.y + _contentSize.height);
    }
}

void Entity::change(const ValueMap& data)
{
    map_util::merge(data, _details);

    // 0x1000BCBA4: Update name
    if (data.contains("n"))
    {
        setEntityName(map_util::getString(data, "n"));
    }

    // 0x1000BCCAB: Update color
    if (data.contains("e*"))
    {
        _changeColor = color_util::hexToColor(map_util::getString(data, "e*"));
        _colorize    = true;
    }

    // 0x1000BCCED: Update stealth
    if (data.contains("xs"))
    {
        _stealthy = map_util::getBool(data, "xs");
        setOpacity(_stealthy ? 32 : 255);
        auto sfx = _stealthy ? "stealth-on" : "stealth-off";
        AudioManager::getInstance()->playSfx(sfx, _position);
    }

    // TODO: name icon
    // TODO: health
    // TODO: colorization
    // TODO: shield
}

void Entity::createNameLabel()
{
    if (_nameLabel)
    {
        _nameLabel->removeFromParent();
        _nameLabel->release();
    }

    _nameLabel = Label::createWithBMFont("console-shadow.fnt", _name);
    AX_SAFE_RETAIN(_nameLabel);
    _nameLabel->setScale(BLOCK_SIZE / 100.0F);
    _nameLabel->setAnchorPoint(Point::ANCHOR_MIDDLE_BOTTOM);
}

void Entity::animateViolentDeath()
{
    // TODO: play the actual animation
    // 0x1000BF2C7: Play random death sound
    auto& deathSounds = _config->getDeathSounds();

    if (!deathSounds.empty())
    {
        auto& sound = deathSounds[random() % deathSounds.size()];
        auto pitch  = random(0.85F, 1.15F);
        AudioManager::getInstance()->playSfx(sound, _position, pitch, 0.5F);
    }
}

void Entity::emote(const std::string& text, const Color3B& color, bool quick, bool replaceLast)
{
    // TODO: onscreen
    if (replaceLast && _lastEmote)
    {
        if (_lastEmote->getParent() && utils::gettime() >= _lastEmoteAt + 0.1)
        {
            _lastEmote->stopAllActions();
            ax_util::fadeOutAndRemove(_lastEmote, 0.2F);
            _emoteOffset--;
        }
    }

    AX_SAFE_RELEASE_NULL(_lastEmote);
    _emoteOffset++;
    auto position = _position + Vec2::UNIT_Y * (_contentSize.height + _emoteOffset * 20.0F);

    if (_nameLabel)
    {
        position.y += math_util::getScaledHeight(_nameLabel);
    }

    _lastEmote = WorldRenderer::getMain()->emote(text, position, color, quick);
    AX_SAFE_RETAIN(_lastEmote);
    _lastEmoteAt = utils::gettime();
}

void Entity::setFlippedX(bool flippedX)
{
    if (_flippedX != flippedX && _config->doesFlipX())
    {
        for (auto& sprite : _sprites)
        {
            sprite.second->setFlippedX(flippedX);
        }

        _flippedX = flippedX;
    }
}

void Entity::setRealPosition(const Point& position)
{
    _realPosition = position;

    if (!_positioned)
    {
        // TODO: finish
        playEntrySoundEffects();

        if (isBlock())
        {
            setVisible(true);
            _physical->setShapeAsBox(Size::ONE * BLOCK_SIZE * 0.75F, _realPosition);
            _physical->setCollisionType(CollisionType::ENTITY);
            _physical->setLayer(0x15E);
            _physical->addToSpace();
            // TODO: sensor
        }

        setPosition(position);
        _positioned = true;
    }
}

bool Entity::wasGroundedRecently() const
{
    return utils::gettime() < _lastGroundedAt + 0.1;
}

bool Entity::isBlock() const
{
    return _config->isBlock();
}

}  // namespace opendw
