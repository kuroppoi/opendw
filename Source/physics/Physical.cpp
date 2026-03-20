#include "Physical.h"

#include "physics/ChipmunkBody.h"
#include "physics/ChipmunkShape.h"
#include "physics/ChipmunkSpace.h"
#include "util/MathUtil.h"
#include "CommonDefs.h"

USING_NS_AX;

namespace opendw
{

Physical::~Physical()
{
    AX_SAFE_RELEASE(_body);
}

Physical* Physical::createWithTarget(Object* target)
{
    CREATE_INIT(Physical, initWithTarget, target);
}

bool Physical::initWithTarget(Object* target)
{
    _target = target;
    _body   = nullptr;
    return true;
}

void Physical::addBody()
{
    if (_body)
    {
        return;
    }

    auto moment = cpMomentForBox(1.0F, 1.0F, 1.0F);
    _body       = ChipmunkBody::createWithMass(1.0F, moment);
    AX_SAFE_RETAIN(_body);
    _body->setUserData(_target);
}

void Physical::useStaticBody()
{
    AX_ASSERT(sGlobalSpace);
    _body = sGlobalSpace->getStaticBody();
    AX_SAFE_RETAIN(_body);
}

void Physical::addToSpace()
{
    AX_ASSERT(sGlobalSpace);
    sGlobalSpace->add(this);
}

void Physical::setLayer(uint32_t layer)
{
    for (auto&& object : _chipmunkObjects)
    {
        if (auto shape = dynamic_cast<ChipmunkShape*>(object))
        {
            // NOTE: It seems to do `setFilter((void*)filter, ((uint64_t)layer << 32) | layer)`
            // which I *assume* sets categories & mask to layer while leaving the group unchanged.
            auto filter       = shape->getFilter();
            filter.categories = layer;
            filter.mask       = layer;
            shape->setFilter(filter);
        }
    }
}

void Physical::setGroup(void* group)
{
    for (auto&& object : _chipmunkObjects)
    {
        if (auto shape = dynamic_cast<ChipmunkShape*>(object))
        {
            auto filter  = shape->getFilter();
            filter.group = (cpGroup)group;
            shape->setFilter(filter);
        }
    }
}

void Physical::setCollisionType(CollisionType type)
{
    for (auto&& object : _chipmunkObjects)
    {
        if (auto shape = dynamic_cast<ChipmunkShape*>(object))
        {
            shape->setCollisionType(static_cast<cpCollisionType>(type));
        }
    }
}

void Physical::setPosition(const Point& position)
{
    _body->setPosition(position);

    if (_body->getType() == CP_BODY_TYPE_STATIC)
    {
        if (auto node = dynamic_cast<Node*>(_target))
        {
            node->setPosition(getPosition());

            if (_rotates)
            {
                node->setRotation(MATH_RAD_TO_DEG(_body->getAngle()));
            }
        }
    }
}

Point Physical::getPosition() const
{
    return _body->getPosition();
}

void Physical::setVelocity(const Vec2& velocity)
{
    _body->setVelocity(velocity);
}

Vec2 Physical::getVelocity() const
{
    return _body->getVelocity();
}

void Physical::setShapeAsCircle(float radius, const Point& offset)
{
    if (!_body)
    {
        addBody();
    }

    clearShapes();
    auto shape = ChipmunkCircleShape::createWithBody(_body, radius, offset);
    shape->setUserData(_target);
    shape->setElasticity(0.3F);
    shape->setFriction(0.05F);
    bindInternalShape(shape);

    if (_body->getType() != CP_BODY_TYPE_STATIC)
    {
        auto mass   = _body->getMass();
        auto moment = cpMomentForCircle(mass, radius, radius, cpvzero);
        _body->setMoment(moment);
    }

    updateChipmunkObjects();
}

ChipmunkPolyShape* Physical::createBoxShape(const Size& size, const Point& offset)
{
    auto origin = size * 0.5F;
    auto points =
        std::vector<Point>{{-origin.x, -origin.y}, {origin.x, -origin.y}, {origin.x, origin.y}, {-origin.x, origin.y}};
    auto transform = AffineTransformMake(1.0F, 0.0F, 0.0F, 1.0F, offset.x, offset.y);
    auto shape     = ChipmunkPolyShape::createWithBody(_body, points, transform, 0.0F);
    shape->setUserData(_target);
    shape->setElasticity(0.3F);
    shape->setFriction(0.3F);
    return shape;
}

void Physical::setShapeAsBox(const Size& size, const Point& offset)
{
    if (!_body)
    {
        addBody();
    }

    clearShapes();
    auto shape = createBoxShape(size, offset);
    bindInternalShape(shape);

    if (_body->getType() != CP_BODY_TYPE_STATIC)
    {
        auto mass   = _body->getMass();
        auto moment = cpMomentForBox(mass, size.x, size.y);
        _body->setMoment(moment);
    }

    updateChipmunkObjects();
}

void Physical::addBoxShape(const Size& size, const Point& offset)
{
    if (!_body)
    {
        addBody();
    }

    auto shape = createBoxShape(size, offset);
    bindInternalShape(shape);
    updateChipmunkObjects();
}

void Physical::setShapeFromDefinition(const PhysicsDefinition& definition,
                                      const Size& size,
                                      const Point& offset,
                                      float rotation,
                                      bool flipped)
{
    if (!_body)
    {
        addBody();
    }

    clearShapes();
    auto moment = 0.0F;

    // Create shape object for each polygon in the definition
    for (auto& points : definition)
    {
        auto transform = AffineTransformMake(1.0F, 0.0F, 0.0F, 1.0F, offset.x, offset.y);

        // HACK: properly offset edge case rotations
        if (rotation != 0.0F)
        {
            auto newSize = math_util::rotateVector(size, -rotation);
            newSize.x    = abs(newSize.x);
            newSize.y    = abs(newSize.y);
            auto offset  = (newSize - size) * 0.5F;
            transform    = AffineTransformTranslate(transform, offset.x, offset.y);
        }

        // HACK: rotate around center origin
        transform = AffineTransformTranslate(transform, size.x * 0.5F, size.y * 0.5F);
        transform = AffineTransformRotate(transform, MATH_DEG_TO_RAD(-rotation));
        transform = AffineTransformTranslate(transform, -size.x * 0.5F, -size.y * 0.5F);

        if (flipped)
        {
            transform = AffineTransformScale(transform, -1.0F, 1.0F);
            transform = AffineTransformTranslate(transform, -size.x, 0.0F);
        }

        auto shape = ChipmunkPolyShape::createWithBody(_body, points, transform, 0.0F);
        shape->setUserData(_target);
        shape->setElasticity(0.3F);
        shape->setFriction(0.5F);
        bindInternalShape(shape);

        // 0x10007B730: Calculate moment and add to total
        if (_body->getType() != CP_BODY_TYPE_STATIC)
        {
            auto count    = points.size();
            auto vertices = new cpVect[count];

            for (ssize_t i = 0; i < count; i++)
            {
                auto& point = points[i];
                vertices[i] = cpv(point.x, point.y);
            }

            moment += cpMomentForPoly(_body->getMass(), count, vertices, cpv(offset.x, offset.y), 0.0F);
            delete[] vertices;
        }
    }

    if (_body->getType() != CP_BODY_TYPE_STATIC)
    {
        _body->setMoment(moment);
    }

    updateChipmunkObjects();
}

void Physical::bindInternalShape(ChipmunkShape* shape)
{
    AXASSERT(!_internalShapes.contains(shape), "Shape already bound");
    _internalShapes.pushBack(shape);
}

void Physical::clearShapes()
{
    _internalShapes.clear();
}

void Physical::updateChipmunkObjects()
{
    _chipmunkObjects.clear();

    if (_body->getType() != CP_BODY_TYPE_STATIC)
    {
        _chipmunkObjects.pushBack(_body);
    }

    for (auto&& shape : _internalShapes)
    {
        _chipmunkObjects.pushBack(shape);
    }
}

}  // namespace opendw
