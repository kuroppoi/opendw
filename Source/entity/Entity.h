#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "axmol.h"

namespace opendw
{

class EntityConfig;

/*
 * CLASS: Entity : BaseGameObject @ 0x100318638
 */
class Entity : public ax::Sprite
{
public:
    /* FUNC: Entity::dealloc @ 0x1000C0CB1 */
    virtual ~Entity() override;

    static Entity* createWithConfig(EntityConfig* config, const std::string& name, const ax::ValueMap& details);

    /* FUNC: Entity::initWithConfig:name:details: @ 0x1000BB75A */
    virtual bool initWithConfig(EntityConfig* config, const std::string& name, const ax::ValueMap& details);

    /* FUNC: Entity::onExit @ 0x1000C0C10 */
    virtual void onExit() override;

    /* FUNC: Entity::step: @ 0x1000BCE54 */
    virtual void update(float deltaTime) override;

    /* FUNC: Entity::buildGraphics: @ 0x1000BE7DA */
    virtual void buildGraphics() {}

    /* FUNC: Entity::finishGraphics @ 0x1000BE7E0 */
    virtual void finishGraphics() {}

    /* FUNC: Entity::change: @ 0x1000BCB73 */
    virtual void change(const ax::ValueMap& data);

    /* FUNC: Entity::runAnimation: @ 0x1000BEA8D */
    virtual bool runAnimation(int32_t id) { return false; }

    /* FUNC: Entity::setFlipX: @ 0x1000BC034 */
    virtual void setFlippedX(bool flippedX);

    /* FUNC: Entity::isHuman @ 0x1000BE597 */
    virtual bool isHuman() const { return false; }

    /* FUNC: Entity::isAvatar @ 0x1000BE59F */
    virtual bool isAvatar() const { return false; }

    /* FUNC: Entity::setName: @ 0x1000C0E19 */
    void setEntityName(const std::string& name) { _name = name; }

    /* FUNC: Entity::name @ 0x1000C0E08 */
    const std::string& getEntityName() const { return _name; }

    /* FUNC: Entity::setEntityId: @ 0x1000C0DD5 */
    void setEntityId(int32_t entityId) { _entityId = entityId; }

    /* FUNC: Entity::entityId @ 0x1000C0DC4 */
    int32_t getEntityId() const { return _entityId; }

    /* FUNC: Entity::clientDirected @ 0x1000C0F72 */
    bool isClientDirected() const { return _clientDirected; }

    /* FUNC: Entity::createNameLabel @ 0x1000BE9A1 */
    void createNameLabel();

    /* FUNC: Entity::nameLabel @ 0x1000C0E36 */
    ax::Label* getNameLabel() const { return _nameLabel; }

    /* FUNC: Entity::setRealPosition: @ 0x1000BC321 */
    void setRealPosition(const ax::Point& position);

    /* FUNC: Entity::realPosition @ 0x1000C0F93 */
    const ax::Point& getRealPosition() const { return _realPosition; }

    /* FUNC: Entity::setRealRotation: @ 0x1000cCFED */
    void setRealRotation(float rotation) { _realRotation = rotation; }

    /* FUNC: Entity::realRotation @ 0x1000C0FDB */
    float getRealRotation() const { return _realRotation; }

    /* FUNC: Entity::isBlock @ 0x1000BE520 */
    bool isBlock() const;

protected:
    EntityConfig* _config;                   // Entity::config @ 0x1003128A0
    EntityConfig* _aggregateConfig;          // Entity::aggregateConfig @ 0x100312928
    ax::ValueMap _details;                   // Entity::details @ 0x1003128A8
    std::string _name;                       // Entity::name @ 0x100312970
    int32_t _entityId;                       // Entity::entityId @ 0x100312A08
    int32_t _currentAnimation;               // Entity::currentAnimation @ 0x1003128D8
    bool _clientDirected;                    // Entity::clientDirected @ 0x1003128C0
    ax::Label* _nameLabel;                   // Entity::nameLabel @ 0x100312960
    std::vector<EntityConfig*> _components;  // Entity::components @ 0x1003128B0
    ax::StringMap<Sprite*> _sprites;         // Entity::sprites @ 0x100312940
    ax::Point _realPosition;                 // Entity::realPosition @ 0x100312958
    float _realRotation;                     // Entity::realRotation @ 0x100312990
    bool _positioned;                        // Entity::hasBeenPositioned @ 0x100312900
    bool _alive;                             // Entity::alive @ 0x1003128F0
};

}  // namespace opendw

#endif  // __ENTITY_H__
