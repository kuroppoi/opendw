#include "Debris.h"

#include "base/Emitter.h"
#include "base/Player.h"
#include "graphics/backend/MaskedSpriteBatchNode.h"
#include "graphics/WorldRenderer.h"
#include "physics/ChipmunkBody.h"
#include "physics/Physical.h"
#include "util/ColorUtil.h"
#include "zone/WorldZone.h"
#include "CommonDefs.h"

USING_NS_AX;

namespace opendw
{

Debris::~Debris()
{
    AX_SAFE_RELEASE(_physical);
}

Debris* Debris::createWithZone(WorldZone* zone)
{
    CREATE_INIT(Debris, initWithZone, zone);
}

bool Debris::initWithZone(WorldZone* zone)
{
    if (!MaskedSprite::initWithSpriteFrameName("debris/blob"))
    {
        return false;
    }

    _zone     = zone;
    _physical = Physical::createWithTarget(this);
    AX_SAFE_RETAIN(_physical);
    return true;
}

void Debris::update(float deltaTime)
{
    deltaTime = MIN(MAX_DELTA_TIME, deltaTime);
    MaskedSprite::update(deltaTime);

    if (!_active)
    {
        return;
    }

    if (_currentLife <= 0.0F)
    {
        auto fadeTime = _emitter ? MIN(0.5F, _emitter->getLife()) : 0.5F;
        auto time     = MIN(1.0F, -_currentLife / fadeTime);

        if (time >= 1.0F)
        {
            clear();
        }
        else
        {
            setOpacity((uint8_t)(_startOpacity - _startOpacity * time));
        }
    }

    _currentLife -= deltaTime;
}

void Debris::spawnNextClone(float /* deltaTime */)
{
    if (_emitter && _clones > 0)
    {
        _zone->getWorldRenderer()->emitParticle(_emitter, _position);
        _clones--;
    }
    else
    {
        unschedule(AX_SCHEDULE_SELECTOR(Debris::spawnNextClone));
    }
}

void Debris::onExit()
{
    if (_active)
    {
        clear();
    }

    MaskedSprite::onExit();
}

void Debris::clear()
{
    unscheduleUpdate();
    _emitter = nullptr;
    _physical->clear(AX_CALLFUNC_SELECTOR(Debris::recycle));  // Space might be locked, so defer recycling
    _active = false;
    _killOnCollide = false;
    setVisible(false);
}

void Debris::recycle()
{
    _zone->getWorldRenderer()->recycleDebris(this);
}

void Debris::spawnForSprite(MaskedSprite* sprite, const Point& position, const Vec2& velocity)
{
    AXASSERT(dynamic_cast<MaskedSpriteBatchNode*>(sprite->getParent()),
             "Target sprite must be in a MaskedSpriteBatchNode");
    auto node = static_cast<MaskedSpriteBatchNode*>(sprite->getParent());

    // Setup graphics
    setMaskFrame("masks/blob");
    auto& maskSize = getMaskRect().size;
    auto& rect     = sprite->getTextureRect();
    auto center    = rect.origin + rect.size * 0.5F - maskSize * 0.5F;
    setTextureRect({center, maskSize});
    auto scale = random(0.9F, 1.1F);
    setScale(scale);
    setColor(sprite->getColor());
    setOpacity(sprite->getOpacity());
    setOpacityModifyRGB(false);  // Already done by mask shader
    setVisible(true);

    // Setup physics
    _physical->setShapeAsCircle(BLOCK_SIZE * 0.1F * scale, Point::ZERO);  // NOTE: Originally calculated differently
    _physical->setGroup(Player::getMain());
    _physical->setCollisionType(CollisionType::DEBRIS);
    _physical->setLayer(0x100);  // TODO: category constants
    _physical->setPosition(position);
    _physical->setVelocity(velocity);
    _physical->getBody()->setGravity(true);
    _physical->addToSpace();
    setPosition(position);

    if (_parent != node)
    {
        removeFromParent();
        node->addChild(this, 30);
    }

    _currentLife  = 1.2F;
    _startOpacity = getOpacity();
    _spawns++;
    scheduleUpdate();
    _active = true;
}

void Debris::spawnParticle(Emitter* emitter, const Point& point)
{
    _emitter = emitter;  // Weak ref
    renderForEmitter(emitter);

    if (emitter->hasCollision())
    {
        auto size = MAX(_contentSize.width, _contentSize.height);
        _physical->setShapeAsCircle(size * getScale() * 0.125F, Point::ZERO);
    }
    else
    {
        _physical->addBody();
        _physical->updateChipmunkObjects();
    }

    _physical->setGroup(Player::getMain());
    _physical->setCollisionType(CollisionType::DEBRIS);
    _physical->setLayer(0x100);  // TODO: category constants
    _physical->setPosition(point);
    setPosition(point);
    moveForEmitter(emitter);
    auto effectsNode = _zone->getWorldRenderer()->getEffectsNode();

    if (_parent != effectsNode)
    {
        removeFromParent();
        effectsNode->addChild(this);
    }

    _physical->addToSpace();
    _currentLife  = clampf(_emitter->getLife(), 0.1F, 5.0F);
    _startOpacity = getOpacity();
    _spawns++;
    scheduleUpdate();
    _active = true;
}

void Debris::moveForEmitter(Emitter* emitter)
{
    auto body = _physical->getBody();

    // Determine velocity based on angle
    auto angle     = MATH_DEG_TO_RAD(emitter->getAngleBase() + emitter->getAngleRange() * rand_minus1_1() * 0.5F);
    auto velocity  = emitter->getVelocityBase() + emitter->getVelocityRange() * rand_minus1_1() * 0.5F;
    auto direction = Vec2(sinf(angle), cosf(angle));
    direction.normalize();
    body->setVelocity(direction * velocity);

    if (!emitter->hasGravity())
    {
        body->setGravity(0.0F);
    }

    // Set random angular velocity
    auto angularVelocity =
        emitter->getAngularVelocityBase() + emitter->getAngularVelocityRange() * rand_minus1_1() * 0.5F;
    body->setAngularVelocity(angularVelocity);
}

void Debris::renderForEmitter(Emitter* emitter)
{
    // Set random sprite frame
    auto& frames = emitter->getSpriteFrames();
    AX_ASSERT(!frames.empty());
    auto frame = frames[random() % frames.size()];

    if (!frame)
    {
        setVisible(false);
        return;
    }

    if (_renderMode != RenderMode::QUAD_BATCHNODE)
    {
        setTexture(frame->getTexture());
    }

    auto& size = frame->getRect().size;
    setTextureRect(frame->getRect(), frame->isRotated(), size);

    // Set random scale
    auto scale = emitter->getScaleBase() + emitter->getScaleRange() * rand_minus1_1() * 0.5F;
    setScale(scale);

    // Set random color
    auto color = color_util::randomColorRanged(emitter->getColorBase(), emitter->getColorRange());
    setColor({color.r, color.g, color.b});
    setOpacity(color.a);
    setOpacityModifyRGB(true);
    setVisible(true);
}

void Debris::onCollide(const Point& point)
{
    if (_emitter)
    {
        if (auto emitter = _emitter->getCollisionEmitter())
        {
            clear();
            _zone->getWorldRenderer()->emitParticle(emitter, point);
        }
        else if (_killOnCollide)
        {
            setCurrentLife(MIN(0.5F, _currentLife));  // Slight grace period
        }
    }
}

void Debris::setClones(ssize_t clones)
{
    if (_clones != clones)
    {
        auto selector = AX_SCHEDULE_SELECTOR(Debris::spawnNextClone);
        unschedule(selector);

        if (clones > 0)
        {
            schedule(selector);
        }

        _clones = clones;
    }
}

void Debris::setCurrentLife(float life)
{
    // Don't allow changing if already dead
    if (_active && _currentLife > 0.0F)
    {
        _currentLife = MAX(0.0F, life);
    }
}

}  // namespace opendw
