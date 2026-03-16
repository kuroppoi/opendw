#include "ChipmunkBody.h"

#include "physics/ChipmunkSpace.h"
#include "CommonDefs.h"

USING_NS_AX;

namespace opendw
{

ChipmunkBody::~ChipmunkBody()
{
    cpBodyDestroy(&_body);
}

ChipmunkBody* ChipmunkBody::createWithMass(float mass, float moment)
{
    CREATE_INIT(ChipmunkBody, initWithMass, mass, moment);
}

bool ChipmunkBody::initWithMass(float mass, float moment)
{
    cpBodyInit(&_body, mass, moment);
    cpBodySetUserData(&_body, this);
    return true;
}

void ChipmunkBody::addToSpace(ChipmunkSpace* space)
{
    space->addBody(this);
}

void ChipmunkBody::removeFromSpace(ChipmunkSpace* space)
{
    space->removeBody(this);
}

void ChipmunkBody::setType(cpBodyType type)
{
    cpBodySetType(&_body, type);
}

cpBodyType ChipmunkBody::getType() const
{
    return cpBodyGetType(getBody());
}

void ChipmunkBody::setMass(float mass)
{
    cpBodySetMass(&_body, mass);
}

float ChipmunkBody::getMass() const
{
    return cpBodyGetMass(&_body);
}

void ChipmunkBody::setMoment(float moment)
{
    cpBodySetMoment(&_body, moment);
}

float ChipmunkBody::getMoment() const
{
    return cpBodyGetMoment(&_body);
}

void ChipmunkBody::setPosition(const Point& position)
{
    cpBodySetPosition(&_body, cpv(position.x, position.y));
}

Point ChipmunkBody::getPosition() const
{
    auto position = cpBodyGetPosition(&_body);
    return Vec2(position.x, position.y);
}

void ChipmunkBody::setVelocity(const Vec2& velocity)
{
    cpBodySetVelocity(&_body, cpv(velocity.x, velocity.y));
}

Vec2 ChipmunkBody::getVelocity() const
{
    auto velocity = cpBodyGetVelocity(&_body);
    return Vec2(velocity.x, velocity.y);
}

void ChipmunkBody::setAngle(float angle)
{
    cpBodySetAngle(&_body, angle);
}

float ChipmunkBody::getAngle() const
{
    return cpBodyGetAngle(&_body);
}

}  // namespace opendw
