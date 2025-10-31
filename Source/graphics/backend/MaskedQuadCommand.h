#ifndef __MASKED_QUAD_COMMAND_H__
#define __MASKED_QUAD_COMMAND_H__

#include "graphics/backend/MaskedTrianglesCommand.h"

namespace opendw
{

class MaskedQuadCommand : public MaskedTrianglesCommand
{
public:
    struct Quad
    {
        Vertex tl;  // Top left
        Vertex bl;  // Bottom left
        Vertex tr;  // Top right
        Vertex br;  // Bottom right
    };

    MaskedQuadCommand();
    ~MaskedQuadCommand() override;

    void init(float globalZOrder,
              ax::Texture2D* texture,
              ax::Texture2D* maskTexture,
              const ax::BlendFunc& blendFunc,
              Quad* quads,
              int quadCount,
              const ax::Mat4& modelView,
              uint32_t flags);

    void reindex(int indexCount);

private:
    inline static int sIndexCapacity = -1;
    inline static uint16_t* sIndices;
    inline static size_t sInstanceCount;

    int _indexCount = 0;
};

}  // namespace opendw

#endif  // __MASKED_QUAD_COMMAND_H__
