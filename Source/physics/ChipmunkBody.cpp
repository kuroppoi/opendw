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

}  // namespace opendw
