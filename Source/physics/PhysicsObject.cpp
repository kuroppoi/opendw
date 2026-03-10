#include "PhysicsObject.h"

USING_NS_AX;

namespace opendw
{

const Vector<PhysicsBaseObject*> PhysicsBaseObject::getPhysicsObjects()
{
    return {this};
}

}  // namespace opendw
