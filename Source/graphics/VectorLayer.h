#ifndef __VECTOR_LAYER_H__
#define __VECTOR_LAYER_H__

#include "axmol.h"

namespace opendw
{

/*
 * CLASS: VectorLayer : CCNode @ 0x10031A5F0
 *
 * Used to draw primitive shapes in world space.
 * Its main purpose is to draw protector field radii & static electricity between energy particles.
 */
class VectorLayer : public ax::DrawNode
{
public:
    CREATE_FUNC(VectorLayer);

    /* FUNC: VectorLayer::draw @ 0x1000FBF68 */
    void draw(ax::Renderer* renderer, const ax::Mat4& transform, uint32_t flags) override;
};

}  // namespace opendw

#endif  // __VECTOR_LAYER_H__
