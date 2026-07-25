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
    _emitter = nullptr;
    _physical->clear();
    _zone->getWorldRenderer()->recycleDebris(this);
    _active = false;
    setVisible(false);
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

    // Create lifecycle actions
    auto delayTime = DelayTime::create(1.2F);
    auto fadeTo    = FadeTo::create(0.5F, 0);
    auto callFunc  = CallFunc::create([this]() { clear(); });
    auto sequence  = Sequence::create({delayTime, fadeTo, callFunc});
    runAction(sequence);
    _spawns++;
    _active = true;
}

void Debris::spawnParticle(Emitter* emitter, const Point& point)
{
    _emitter = emitter;  // Weak ref
    renderForEmitter(emitter);

    if (emitter->hasCollision())
    {
        auto size = MIN(_contentSize.width, _contentSize.height);
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
    auto life      = clampf(_emitter->getLife(), 0.1F, 5.0F);
    auto delayTime = DelayTime::create(life);
    auto fadeTo    = FadeTo::create(0.5F, 0);
    auto callFunc  = CallFunc::create([this]() { clear(); });
    auto sequence  = Sequence::create({delayTime, fadeTo, callFunc});
    runAction(sequence);
    _spawns++;
    _active = true;
}

void Debris::moveForEmitter(Emitter* emitter)
{
    auto body = _physical->getBody();

    // Set random angle
    auto angle = MATH_DEG_TO_RAD(emitter->getAngleBase() + emitter->getAngleRange() * rand_minus1_1() * 0.5F);
    body->setAngle(angle);

    // Determine velocity based on angle
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

}  // namespace opendw
