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

    for (auto handler : _collisionHandlers)
    {
        AX_SAFE_DELETE(handler);
    }
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

Vec2 ChipmunkSpace::getGravity() const
{
    auto gravity = cpSpaceGetGravity(_space);
    return Vec2(gravity.x, gravity.y);
}

/* FUNC: 0x100098F73 */
static bool onCollisionBegin(cpArbiter* arbiter, cpSpace* /*space*/, ChipmunkSpace::CollisionHandler* handler)
{
    return handler->beginFunc(arbiter, handler->space);
}

/* FUNC: 0x100098F95 */
static bool onCollisionPreSolve(cpArbiter* arbiter, cpSpace* /*space*/, ChipmunkSpace::CollisionHandler* handler)
{
    return handler->preSolveFunc(arbiter, handler->space);
}

/* FUNC: 0x100098FB7 */
static void onCollisionPostSolve(cpArbiter* arbiter, cpSpace* /*space*/, ChipmunkSpace::CollisionHandler* handler)
{
    handler->postSolveFunc(arbiter, handler->space);
}

/* FUNC: 0x100098FD3 */
static void onCollisionSeparate(cpArbiter* arbiter, cpSpace* /*space*/, ChipmunkSpace::CollisionHandler* handler)
{
    handler->separateFunc(arbiter, handler->space);
}

void ChipmunkSpace::addCollisionHandler(cpCollisionType typeA,
                                        cpCollisionType typeB,
                                        CollisionBegin begin,
                                        CollisionPreSolve preSolve,
                                        CollisionPostSolve postSolve,
                                        CollisionSeparate separate)
{
    auto handler = cpSpaceAddCollisionHandler(_space, typeA, typeB);

    if (begin)
    {
        handler->beginFunc = (cpCollisionBeginFunc)onCollisionBegin;
    }

    if (preSolve)
    {
        handler->preSolveFunc = (cpCollisionPreSolveFunc)onCollisionPreSolve;
    }

    if (postSolve)
    {
        handler->postSolveFunc = (cpCollisionPostSolveFunc)onCollisionPostSolve;
    }

    if (separate)
    {
        handler->separateFunc = (cpCollisionSeparateFunc)onCollisionSeparate;
    }

    auto data = new CollisionHandler{this, begin, preSolve, postSolve, separate};
    _collisionHandlers.push_back(data);
    handler->userData = data;
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
