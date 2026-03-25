#include "PhysicsDebugNode.h"

#include "physics/ChipmunkShape.h"
#include "physics/ChipmunkSpace.h"
#include "zone/WorldZone.h"

USING_NS_AX;

namespace opendw
{

static void drawBody(cpBody* body, DrawNode* drawNode)
{
    auto position = cpBodyGetPosition(body);
    auto point    = Point(position.x, position.y);
    drawNode->drawCircle(point, 5.0F, 0.0F, 5, false, Color4F::RED);
}

static void drawShape(cpShape* shape, DrawNode* drawNode)
{
    auto body      = cpShapeGetBody(shape);
    auto position  = cpBodyGetPosition(body);
    auto angle     = cpBodyGetAngle(body);
    auto transform = cpTransformRigid(position, angle);
    auto userData  = cpShapeGetUserData(shape);

    switch (shape->klass->type)
    {
    case CP_CIRCLE_SHAPE:
    {
        auto radius = cpCircleShapeGetRadius(shape);
        drawNode->drawCircle({position.x, position.y}, radius, angle, 20, false, Color4F::WHITE);
        break;
    }
    case CP_SEGMENT_SHAPE:
    {
        auto from   = cpSegmentShapeGetA(shape);
        auto to     = cpSegmentShapeGetB(shape);
        auto radius = cpSegmentShapeGetRadius(shape) * 2.0F + 1.0F;
        drawNode->drawLine({from.x, from.y}, {to.x, to.y}, Color4F::WHITE, radius);
        break;
    }
    case CP_POLY_SHAPE:
    {
        auto count  = cpPolyShapeGetCount(shape);
        auto points = new Point[count];

        for (auto i = 0; i < count; i++)
        {
            auto point = cpPolyShapeGetVert(shape, i);
            point      = cpTransformPoint(transform, point);
            points[i]  = {point.x, point.y};
        }

        drawNode->drawPoly(points, count, true, Color4F::WHITE);
        delete[] points;
        break;
    }
    }
}

void PhysicsDebugNode::draw(Renderer* renderer, const Mat4& transform, uint32_t flags)
{
    clear();
    auto zone = WorldZone::getMain();

    if (zone->getState() != WorldZone::State::ACTIVE)
    {
        AXLOGW("[PhysicsDebugNode] Warning - zone state isn't active");
        return;
    }

    auto space = zone->getSpace()->getSpace();  // cpSpace
    cpSpaceEachBody(space, (cpSpaceBodyIteratorFunc)drawBody, this);
    cpSpaceEachShape(space, (cpSpaceShapeIteratorFunc)drawShape, this);
    DrawNode::draw(renderer, transform, flags);
}

}  // namespace opendw
