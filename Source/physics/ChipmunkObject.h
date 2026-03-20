#ifndef __CHIPMUNK_OBJECT_H__
#define __CHIPMUNK_OBJECT_H__

#include "axmol.h"

namespace opendw
{

class ChipmunkBaseObject;
class ChipmunkSpace;

/*
 * PROTOCOL: ChipmunkObject @ 0x1003207A0
 */
class ChipmunkObject : public ax::Object
{
public:
    virtual const ax::Vector<ChipmunkBaseObject*> getChipmunkObjects() = 0;
};

/*
 * PROTOCOL: ChipmunkBaseObject @ 0x100320A40
 */
class ChipmunkBaseObject : public ChipmunkObject
{
public:
    virtual void addToSpace(ChipmunkSpace* space)      = 0;
    virtual void removeFromSpace(ChipmunkSpace* space) = 0;

    /* By default it will return a vector containing only itself. */
    const ax::Vector<ChipmunkBaseObject*> getChipmunkObjects() override;
};

}  // namespace opendw

#endif  // __CHIPMUNK_OBJECT_H__
