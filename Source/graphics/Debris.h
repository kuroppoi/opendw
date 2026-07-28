#ifndef __DEBRIS_H__
#define __DEBRIS_H__

#include "graphics/backend/MaskedSprite.h"

namespace opendw
{

class Emitter;
class Physical;
class WorldZone;

/*
 * CLASS: Debris : BaseGameObject @ 0x100316F18
 *
 * Manages the lifecycle of a particle effect.
 */
class Debris : public MaskedSprite
{
public:
    /* FUNC: Debris::dealloc @ 0x1000555E3 */
    virtual ~Debris();

    static Debris* createWithZone(WorldZone* zone);

    /* FUNC: Debris::initWithWorldZone: @ 0x1000544A5 */
    bool initWithZone(WorldZone* zone);

    void update(float deltaTime) override;

    /* FUNC: Debris::step @ 0x100055435 */
    void spawnNextClone(float /* deltaTime */);

    /* FUNC: Debris::onExit @ 0x10005554C */
    void onExit() override;

    void clear();
    void recycle();

    /* FUNC: Debris::spawnForItem:inNode:atPosition:velocity: @ 0x10005456C */
    void spawnForSprite(MaskedSprite* sprite, const ax::Point& position, const ax::Vec2& velocity);

    /* FUNC: Debris::spawnParticle:atPosition: @ 0x100054B2C */
    void spawnParticle(Emitter* emitter, const ax::Point& position);

    /* FUNC: Debris::moveForEmitter: @ 0x100054D96 */
    void moveForEmitter(Emitter* emitter);

    /* FUNC: Debris::renderForEmitter: @ 0x100055007 */
    void renderForEmitter(Emitter* emitter);

    /* FUNC: Debris::onCollide: @ 0x1000554CB */
    void onCollide(const ax::Point& point);

    /* FUNC: Debris::setClones: @ 0x1000553E3 */
    void setClones(ssize_t clones);

    Physical* getPhysical() const { return _physical; }

    void setPoolIndex(ssize_t index) { _poolIndex = index; }
    ssize_t getPoolIndex() const { return _poolIndex; }

    bool isActive() const { return _active; }

    void setCurrentLife(float life);

    void setKillOnCollide(bool value) { _killOnCollide = value; }

private:
    WorldZone* _zone;   // Debris::zone @ 0x1003115E0
    ssize_t _spawns;    // Debris::spawns @ 0x1003115F0
    Emitter* _emitter;  // Debris::emitter @ 0x1003115F8
    ssize_t _clones;    // Debris::clones @ 0x100311608
    Physical* _physical;
    ssize_t _poolIndex;
    bool _active;
    float _currentLife;
    uint8_t _startOpacity;
    bool _killOnCollide;
};

}  // namespace opendw

#endif  // __DEBRIS_H__
