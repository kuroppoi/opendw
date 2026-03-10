#ifndef __PHYSICS_OBJECT_H__
#define __PHYSICS_OBJECT_H__

#include "axmol.h"

namespace opendw
{

class PhysicsBaseObject;
class PhysicsSpace;

/*
 * PROTOCOL: ChipmunkObject @ 0x1003207A0
 */
class PhysicsObject : public ax::Object
{
public:
    virtual const ax::Vector<PhysicsBaseObject*> getPhysicsObjects() = 0;
};

/*
 * PROTOCOL: ChipmunkBaseObject @ 0x100320A40
 */
class PhysicsBaseObject : public PhysicsObject
{
public:
    virtual void addToSpace(PhysicsSpace* space)      = 0;
    virtual void removeFromSpace(PhysicsSpace* space) = 0;

    /* By default it will return a vector containing only itself. */
    const ax::Vector<PhysicsBaseObject*> getPhysicsObjects() override;
};

}  // namespace opendw

#endif  // __PHYSICS_OBJECT_H__
