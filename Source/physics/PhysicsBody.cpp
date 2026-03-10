#include "PhysicsBody.h"

#include "physics/PhysicsSpace.h"
#include "CommonDefs.h"

USING_NS_AX;

namespace opendw
{

PhysicsBody::~PhysicsBody()
{
    cpBodyDestroy(&_body);
}

PhysicsBody* PhysicsBody::createWithMass(float mass, float moment)
{
    CREATE_INIT(PhysicsBody, initWithMass, mass, moment);
}

bool PhysicsBody::initWithMass(float mass, float moment)
{
    cpBodyInit(&_body, mass, moment);
    cpBodySetUserData(&_body, this);
    return true;
}

void PhysicsBody::addToSpace(PhysicsSpace* space)
{
    space->addBody(this);
}

void PhysicsBody::removeFromSpace(PhysicsSpace* space)
{
    space->removeBody(this);
}

void PhysicsBody::setType(cpBodyType type)
{
    cpBodySetType(&_body, type);
}

cpBodyType PhysicsBody::getType() const
{
    return cpBodyGetType(getBody());
}

void PhysicsBody::setMass(float mass)
{
    cpBodySetMass(&_body, mass);
}

float PhysicsBody::getMass() const
{
    return cpBodyGetMass(&_body);
}

void PhysicsBody::setMoment(float moment)
{
    cpBodySetMoment(&_body, moment);
}

float PhysicsBody::getMoment() const
{
    return cpBodyGetMoment(&_body);
}

}  // namespace opendw
