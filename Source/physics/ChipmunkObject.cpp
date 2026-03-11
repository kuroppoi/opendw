#include "ChipmunkObject.h"

USING_NS_AX;

namespace opendw
{

const Vector<ChipmunkBaseObject*> ChipmunkBaseObject::getChipmunkObjects()
{
    return {this};
}

}  // namespace opendw
