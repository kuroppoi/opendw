#include "PhysicsShape.h"

#include "physics/PhysicsBody.h"
#include "physics/PhysicsSpace.h"
#include "CommonDefs.h"

USING_NS_AX;

namespace opendw
{

void PhysicsShape::addToSpace(PhysicsSpace* space)
{
    space->addShape(this);
}

void PhysicsShape::removeFromSpace(PhysicsSpace* space)
{
    space->removeShape(this);
}

void PhysicsShape::setElasticity(float elasticity)
{
    cpShapeSetElasticity(getShape(), elasticity);
}

float PhysicsShape::getElasticity() const
{
    return cpShapeGetElasticity(getShape());
}

void PhysicsShape::setFriction(float friction)
{
    cpShapeSetFriction(getShape(), friction);
}

float PhysicsShape::getFriction() const
{
    return cpShapeGetFriction(getShape());
}

void PhysicsShape::setFilter(cpShapeFilter filter)
{
    cpShapeSetFilter(getShape(), filter);
}

cpShapeFilter PhysicsShape::getFilter() const
{
    return cpShapeGetFilter(getShape());
}

PhysicsPolyShape::~PhysicsPolyShape()
{
    cpShapeDestroy(getShape());
}

PhysicsPolyShape* PhysicsPolyShape::createWithBody(PhysicsBody* body,
                                                   const std::vector<ax::Point>& points,
                                                   const AffineTransform& transform,
                                                   float radius)
{
    CREATE_INIT(PhysicsPolyShape, initWithBody, body, points, transform, radius);
}

bool PhysicsPolyShape::initWithBody(PhysicsBody* body,
                                    const std::vector<ax::Point>& points,
                                    const AffineTransform& transform,
                                    float radius)
{
    auto count    = points.size();
    auto vertices = new cpVect[count];

    for (ssize_t i = 0; i < count; i++)
    {
        auto& point = points[i];
        vertices[i] = cpv(point.x, point.y);
    }

    auto cpAffine = cpTransform(transform.a, transform.b, transform.c, transform.d, transform.tx, transform.ty);
    cpPolyShapeInit(&_shape, body->getBody(), count, vertices, cpAffine, radius);
    cpShapeSetUserData(getShape(), this);
    delete[] vertices;
    return true;
}

}  // namespace opendw
