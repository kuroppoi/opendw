#include "Entity.h"

#include "entity/EntityAnimated.h"
#include "entity/EntityAnimatedAvatar.h"
#include "entity/EntityAnimatedGhost.h"
#include "entity/EntityConfig.h"
#include "entity/EntityFramed.h"
#include "entity/MutableEntityConfig.h"
#include "util/MapUtil.h"
#include "util/MathUtil.h"
#include "CommonDefs.h"
#include "GameConfig.h"

USING_NS_AX;

namespace opendw
{

Entity::~Entity()
{
    if (_aggregateConfig != _config)
    {
        AX_SAFE_RELEASE(_aggregateConfig);
    }
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

    // 0x1000BBB9B: Configure graphics
    auto scaleBase    = _config->getScaleBase();
    auto scaleRange   = _config->getScaleRange();
    auto scale        = random(scaleBase - scaleRange * 0.5F, scaleBase + scaleRange * 0.5F);
    setScale(scale);
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
    change(details);
    return true;
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
    // TODO: finish
    auto offset =
        _config->isBlock() ? Vec2::ZERO : math_util::rotateVector(Vec2::UNIT_Y * -BLOCK_SIZE * 0.5F, _realRotation);
    auto& size  = _config->getSize();

    if (size.width > 1.0F)
    {
        offset.x += (_contentSize.width - BLOCK_SIZE) * 0.5F;
    }

    auto x        = MathUtil::lerp(_position.x, _realPosition.x + offset.x, fminf(1.0F, deltaTime * 3.0F));
    auto y        = MathUtil::lerp(_position.y, _realPosition.y - offset.y, fminf(1.0F, deltaTime * 5.0F));
    auto rotation = MathUtil::lerp(getRotation(), _realRotation, fminf(1.0F, deltaTime * 10.0F));  // TODO
    setPosition(x, y);
    setRotation(rotation);

    // TODO
    if (_nameLabel)
    {
        _nameLabel->setString(_name);
        _nameLabel->setPosition(x, y + _contentSize.height);
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

    // TODO: name icon
    // TODO: health
    // TODO: colorization
    // TODO: stealth
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
        // TODO: implement
        setPosition(position);
        _positioned = true;
    }
}

bool Entity::isBlock() const
{
    return _config->isBlock();
}

}  // namespace opendw
