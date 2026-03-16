#include "ChipmunkSpace.h"

#include "physics/ChipmunkBody.h"
#include "physics/ChipmunkShape.h"

USING_NS_AX;

namespace opendw
{

ChipmunkSpace::~ChipmunkSpace()
{
    freeSpace();
    AX_SAFE_RELEASE(_staticBody);
}

bool ChipmunkSpace::initWithSpace(cpSpace* space)
{
    _space = space;
    cpSpaceSetUserData(space, this);
    _staticBody = ChipmunkBody::createWithMass(0.0F, 0.0F);
    _staticBody->retain();
    _staticBody->setType(CP_BODY_TYPE_STATIC);

    // HACK: cpSpaceSetStaticBody is not accessible for some reason, so we just inline it.
    space->staticBody             = _staticBody->getBody();
    _staticBody->getBody()->space = space;
    return true;
}

bool ChipmunkSpace::init()
{
    return initWithSpace(cpSpaceNew());
}

void ChipmunkSpace::update(float deltaTime)
{
    cpSpaceStep(_space, deltaTime);
}

void ChipmunkSpace::freeSpace()
{
    cpSpaceFree(_space);
}

void ChipmunkSpace::setGravity(const Vec2& gravity)
{
    cpSpaceSetGravity(_space, cpv(gravity.x, gravity.y));
}

void ChipmunkSpace::add(ChipmunkObject* object)
{
    if (auto base = dynamic_cast<ChipmunkBaseObject*>(object))
    {
        base->addToSpace(this);
    }
    else
    {
        for (auto&& child : object->getChipmunkObjects())
        {
            add(child);
        }

        _children.pushBack(object);
    }
}

void ChipmunkSpace::remove(ChipmunkObject* object)
{
    if (auto base = dynamic_cast<ChipmunkBaseObject*>(object))
    {
        base->removeFromSpace(this);
    }
    else
    {
        for (auto&& child : object->getChipmunkObjects())
        {
            remove(child);
        }

        _children.eraseObject(object);
    }
}

void ChipmunkSpace::addBody(ChipmunkBody* body)
{
    cpSpaceAddBody(_space, body->getBody());
    _children.pushBack(body);
}

void ChipmunkSpace::removeBody(ChipmunkBody* body)
{
    cpSpaceRemoveBody(_space, body->getBody());
    _children.eraseObject(body);
}

void ChipmunkSpace::addShape(ChipmunkShape* shape)
{
    cpSpaceAddShape(_space, shape->getShape());
    _children.pushBack(shape);
}

void ChipmunkSpace::removeShape(ChipmunkShape* shape)
{
    cpSpaceRemoveShape(_space, shape->getShape());
    _children.eraseObject(shape);
}

/* FUNC: 0x100099E71 */
static void addBodyToVector(cpBody* body, std::vector<ChipmunkBody*>& vector)
{
    vector.push_back(static_cast<ChipmunkBody*>(cpBodyGetUserData(body)));
}

std::vector<ChipmunkBody*> ChipmunkSpace::getBodies() const
{
    std::vector<ChipmunkBody*> bodies;
    cpSpaceEachBody(_space, (cpSpaceBodyIteratorFunc)addBodyToVector, &bodies);
    return bodies;
}

}  // namespace opendw
