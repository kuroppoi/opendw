#include "ChipmunkShape.h"

#include "physics/ChipmunkBody.h"
#include "physics/ChipmunkSpace.h"
#include "CommonDefs.h"

USING_NS_AX;

namespace opendw
{

void ChipmunkShape::addToSpace(ChipmunkSpace* space)
{
    space->addShape(this);
}

void ChipmunkShape::removeFromSpace(ChipmunkSpace* space)
{
    space->removeShape(this);
}

void ChipmunkShape::setElasticity(float elasticity)
{
    cpShapeSetElasticity(getShape(), elasticity);
}

float ChipmunkShape::getElasticity() const
{
    return cpShapeGetElasticity(getShape());
}

void ChipmunkShape::setFriction(float friction)
{
    cpShapeSetFriction(getShape(), friction);
}

float ChipmunkShape::getFriction() const
{
    return cpShapeGetFriction(getShape());
}

void ChipmunkShape::setFilter(cpShapeFilter filter)
{
    cpShapeSetFilter(getShape(), filter);
}

cpShapeFilter ChipmunkShape::getFilter() const
{
    return cpShapeGetFilter(getShape());
}

ChipmunkPolyShape::~ChipmunkPolyShape()
{
    cpShapeDestroy(getShape());
}

ChipmunkPolyShape* ChipmunkPolyShape::createWithBody(ChipmunkBody* body,
                                                     const std::vector<ax::Point>& points,
                                                     const AffineTransform& transform,
                                                     float radius)
{
    CREATE_INIT(ChipmunkPolyShape, initWithBody, body, points, transform, radius);
}

bool ChipmunkPolyShape::initWithBody(ChipmunkBody* body,
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
