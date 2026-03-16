#ifndef __CHIPMUNK_SHAPE_H__
#define __CHIPMUNK_SHAPE_H__

#include "chipmunk/chipmunk_structs.h"

#include "physics/ChipmunkObject.h"

namespace opendw
{

class ChipmunkBody;
class ChipmunkSpace;

/*
 * CLASS: ChipmunkShape : NSObject @ 0x10031C0A8
 *
 * NOTE: Virtual functions cannot be called inside of destructors, so each subclass
 * will need to override the destructor and call `cpShapeDestroy(getShape())`.
 */
class ChipmunkShape : public ChipmunkBaseObject
{
public:
    /* FUNC: ChipmunkShape::shape @ 0x10013D2B7 */
    virtual cpShape* getShape() const = 0;

    /* FUNC: ChipmunkShape::addToSpace: @ 0x10013D7D8 */
    void addToSpace(ChipmunkSpace* space) override;

    /* FUNC: ChipmunkShape::removeFromSpace: @ 0x10013D7F3 */
    void removeFromSpace(ChipmunkSpace* space) override;

    /* FUNC: ChipmunkShape::setElasticity: @ 0x10013D4EC */
    void setElasticity(float elasticity);

    /* FUNC: ChipmunkShape::elasticity @ 0x10013D4D2 */
    float getElasticity() const;

    /* FUNC: ChipmunkShape::setFriction: @ 0x10013D532 */
    void setFriction(float friction);

    /* FUNC: ChipmunkShape::friction @ 0x10013D518 */
    float getFriction() const;

    /* FUNC: ChipmunkShape::setCollisionType: @ 0x10013D5C8 */
    void setCollisionType(cpCollisionType type);

    /* FUNC: ChipmunkShape::collisionType @ 0x10013D5AE */
    cpCollisionType getCollisionType() const;

    /* FUNC: ChipmunkShape::setFilter: @ 0x10013D609 */
    void setFilter(cpShapeFilter filter);

    /* FUNC: ChipmunkShape::filter @ 0x10013D5EF */
    cpShapeFilter getFilter() const;

    /* FUNC: ChipmunkShape::setUserData: @ 0x10013D81F */
    void setUserData(ax::Object* userData) { _userData = userData; }

    /* FUNC: ChipmunkShape::userData @ 0x10013D80E */
    ax::Object* getUserData() const { return _userData; }

protected:
    ax::Object* _userData;  // ChipmunkShape::_userData @ 0x100314580
};

/*
 * CLASS: ChipmunkCircleShape : ChipmunkShape @ 0x10031C1E8 
 */
class ChipmunkCircleShape : public ChipmunkShape
{
public:
    virtual ~ChipmunkCircleShape() override;

    /* FUNC: ChipmunkCircleShape::circleWithBody:radius:offset: @ 0x10013DCA9 */
    static ChipmunkCircleShape* createWithBody(ChipmunkBody* body, float radius, const ax::Point& offset);

    /* FUNC: ChipmunkCircleShape::initWithBody:radius:offset: @ 0x10013DD1D */
    bool initWithBody(ChipmunkBody* body, float radius, const ax::Point& offset);

    /* FUNC: ChipmunkCircleShape::radius @ 0x10013DDD0 */
    float getRadius() const;

    /* FUNC: ChipmunkCircleShape::shape @ 0x10013DD0D */
    cpShape* getShape() const override { return (cpShape*)&_shape; }

private:
    cpCircleShape _shape;  // ChipmunkCircleShape::_shape @ 0x1003145B8
};

/*
 * CLASS: ChipmunkPolyShape : ChipmunkShape @ 0x10031C288
 */
class ChipmunkPolyShape : public ChipmunkShape
{
public:
    virtual ~ChipmunkPolyShape() override;

    /* FUNC: ChipmunkPolyShape::polyWithBody:count:verts:transform:radius: @ 0x10013DF96 */
    static ChipmunkPolyShape* createWithBody(ChipmunkBody* body,
                                             const std::vector<ax::Point>& points,
                                             const ax::AffineTransform& transform,
                                             float radius);

    /* FUNC: ChipmunkPolyShape::initWithBody:count:verts:transform:radius: @ 0x10013E114 */
    bool initWithBody(ChipmunkBody* body,
                      const std::vector<ax::Point>& points,
                      const ax::AffineTransform& transform,
                      float radius);

    /* FUNC: ChipmunkPolyShape::shape @ 0x10013E104 */
    cpShape* getShape() const override { return (cpShape*)&_shape; }

private:
    cpPolyShape _shape;  // ChipmunkPolyShape::_shape @ 0x1003145C8
};

}  // namespace opendw

#endif  // __CHIPMUNK_SHAPE_H__
