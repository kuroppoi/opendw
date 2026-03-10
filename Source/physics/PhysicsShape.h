#ifndef __PHYSICS_SHAPE_H__
#define __PHYSICS_SHAPE_H__

#include "chipmunk/chipmunk_structs.h"

#include "physics/PhysicsObject.h"

namespace opendw
{

class PhysicsBody;

/*
 * CLASS: ChipmunkShape : NSObject @ 0x10031C0A8
 *
 * NOTE: Virtual functions cannot be called inside of destructors, so each subclass
 * will need to override the destructor and call `cpShapeDestroy(getShape())`.
 */
class PhysicsShape : public PhysicsBaseObject
{
public:
    /* FUNC: ChipmunkShape::shape @ 0x10013D2B7 */
    virtual cpShape* getShape() const = 0;

    /* FUNC: ChipmunkShape::addToSpace: @ 0x10013D7D8 */
    void addToSpace(PhysicsSpace* space) override;

    /* FUNC: ChipmunkShape::removeFromSpace: @ 0x10013D7F3 */
    void removeFromSpace(PhysicsSpace* space) override;

    /* FUNC: ChipmunkShape::setElasticity: @ 0x10013D4EC */
    void setElasticity(float elasticity);

    /* FUNC: ChipmunkShape::elasticity @ 0x10013D4D2 */
    float getElasticity() const;

    /* FUNC: ChipmunkShape::setFriction: @ 0x10013D532 */
    void setFriction(float friction);

    /* FUNC: ChipmunkShape::friction @ 0x10013D518 */
    float getFriction() const;

    /* FUNC: ChipmunkShape::setFilter: @ 0x10013D609 */
    void setFilter(cpShapeFilter filter);

    /* FUNC: ChipmunkShape::filter @ 0x10013D5EF */
    cpShapeFilter getFilter() const;

    /* FUNC: ChipmunkShape::setUserData: @ 0x10013D81F */
    void setUserData(void* userData) { _userData = userData; }

    /* FUNC: ChipmunkShape::userData @ 0x10013D80E */
    void* getUserData() const { return _userData; }

protected:
    void* _userData;  // ChipmunkShape::_userData @ 0x100314580
};

/*
 * CLASS: ChipmunkPolyShape : ChipmunkShape @ 0x10031C288
 */
class PhysicsPolyShape : public PhysicsShape
{
public:
    virtual ~PhysicsPolyShape() override;

    /* FUNC: ChipmunkPolyShape::polyWithBody:count:verts:transform:radius: @ 0x10013DF96 */
    static PhysicsPolyShape* createWithBody(PhysicsBody* body,
                                            const std::vector<ax::Point>& points,
                                            const ax::AffineTransform& transform,
                                            float radius);

    /* FUNC: ChipmunkPolyShape::initWithBody:count:verts:transform:radius: @ 0x10013E114 */
    bool initWithBody(PhysicsBody* body,
                      const std::vector<ax::Point>& points,
                      const ax::AffineTransform& transform,
                      float radius);

    /* FUNC: ChipmunkPolyShape::shape @ 0x10013E104 */
    cpShape* getShape() const override { return (cpShape*)&_shape; }

private:
    cpPolyShape _shape;  // ChipmunkPolyShape::_shape @ 0x1003145C8
};

}  // namespace opendw

#endif  // __PHYSICS_SHAPE_H__
