#include "PhysicsSpace.h"

#include "physics/PhysicsBody.h"
#include "physics/PhysicsShape.h"

USING_NS_AX;

namespace opendw
{

PhysicsSpace::~PhysicsSpace()
{
    freeSpace();
    AX_SAFE_RELEASE(_staticBody);
}

bool PhysicsSpace::initWithSpace(cpSpace* space)
{
    _space = space;
    cpSpaceSetUserData(space, this);
    _staticBody = PhysicsBody::createWithMass(0.0F, 0.0F);
    _staticBody->retain();
    _staticBody->setType(CP_BODY_TYPE_STATIC);

    // HACK: cpSpaceSetStaticBody is not accessible for some reason, so we just inline it.
    space->staticBody             = _staticBody->getBody();
    _staticBody->getBody()->space = space;
    return true;
}

bool PhysicsSpace::init()
{
    return initWithSpace(cpSpaceNew());
}

void PhysicsSpace::update(float deltaTime)
{
    cpSpaceStep(_space, deltaTime);
}

void PhysicsSpace::freeSpace()
{
    cpSpaceFree(_space);
}

void PhysicsSpace::setGravity(const Vec2& gravity)
{
    cpSpaceSetGravity(_space, cpv(gravity.x, gravity.y));
}

void PhysicsSpace::add(PhysicsObject* object)
{
    if (auto base = dynamic_cast<PhysicsBaseObject*>(object))
    {
        base->addToSpace(this);
    }
    else
    {
        for (auto&& child : object->getPhysicsObjects())
        {
            add(child);
        }

        _children.pushBack(object);
    }
}

void PhysicsSpace::remove(PhysicsObject* object)
{
    if (auto base = dynamic_cast<PhysicsBaseObject*>(object))
    {
        base->removeFromSpace(this);
    }
    else
    {
        for (auto&& child : object->getPhysicsObjects())
        {
            remove(child);
        }

        _children.eraseObject(object);
    }
}

void PhysicsSpace::addBody(PhysicsBody* body)
{
    cpSpaceAddBody(_space, body->getBody());
    _children.pushBack(body);
}

void PhysicsSpace::removeBody(PhysicsBody* body)
{
    cpSpaceRemoveBody(_space, body->getBody());
    _children.eraseObject(body);
}

void PhysicsSpace::addShape(PhysicsShape* shape)
{
    cpSpaceAddShape(_space, shape->getShape());
    _children.pushBack(shape);
}

void PhysicsSpace::removeShape(PhysicsShape* shape)
{
    cpSpaceRemoveShape(_space, shape->getShape());
    _children.eraseObject(shape);
}

}  // namespace opendw
