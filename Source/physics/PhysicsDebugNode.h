#ifndef __PHYSICS_DEBUG_NODE_H__
#define __PHYSICS_DEBUG_NODE_H__

#include "axmol.h"

namespace opendw
{

class PhysicsShape;

/*
 * CLASS: ChipmunkDebugNode : CCNode @ 0x10031A640
 */
class PhysicsDebugNode : public ax::DrawNode
{
public:
    CREATE_FUNC(PhysicsDebugNode);

    /* FUNC: ChipmunkDebugNode::draw @ 0x1000FD39E */
    void draw(ax::Renderer* renderer, const ax::Mat4& transform, uint32_t flags) override;
};

}  // namespace opendw

#endif  // __PHYSICS_DEBUG_NODE_H__
