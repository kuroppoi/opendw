#include "MaskedQuadCommand.h"

#define MAX_INDEX_CAPACITY 0x20000

USING_NS_AX;

namespace opendw
{

MaskedQuadCommand::MaskedQuadCommand()
{
    sInstanceCount++;
}

MaskedQuadCommand::~MaskedQuadCommand()
{
    if (--sInstanceCount == 0)
    {
        AXLOGD("[MaskedQuadCommand] Deleting global indices array");
        AX_SAFE_DELETE_ARRAY(sIndices);
        sIndexCapacity = -1;
    }
}

void MaskedQuadCommand::init(float globalZOrder,
                             Texture2D* texture,
                             Texture2D* maskTexture,
                             const BlendFunc& blendFunc,
                             Quad* quads,
                             int quadCount,
                             const Mat4& modelView,
                             uint32_t flags)
{
    if (quadCount * 6 > _indexCount)
    {
        reindex(quadCount * 6);
    }

    Triangles triangles(&quads->tl, sIndices, quadCount * 4, quadCount * 6);
    MaskedTrianglesCommand::init(globalZOrder, texture, maskTexture, blendFunc, triangles, modelView, flags);
}

void MaskedQuadCommand::reindex(int indexCount)
{
    if (sIndexCapacity == -1)
    {
        indexCount = MAX(2048, indexCount);
    }

    if (indexCount > sIndexCapacity)
    {
        if (indexCount == MAX_INDEX_CAPACITY)
        {
            AXLOGW("[MaskedQuadCommand] Cannot increase index capacity any further!");
            return;
        }

        indexCount = MIN(MAX_INDEX_CAPACITY, indexCount * 1.25);
        AXLOGD("[MaskedQuadCommand] Increasing index count from {} to {}", sIndexCapacity, indexCount);
        AX_SAFE_DELETE_ARRAY(sIndices);
        sIndices       = new uint16_t[indexCount];
        sIndexCapacity = indexCount;
    }

    for (auto i = 0; i < sIndexCapacity / 6; i++)
    {
        sIndices[i * 6]     = static_cast<uint16_t>(i * 4);
        sIndices[i * 6 + 1] = static_cast<uint16_t>(i * 4 + 1);
        sIndices[i * 6 + 2] = static_cast<uint16_t>(i * 4 + 2);
        sIndices[i * 6 + 3] = static_cast<uint16_t>(i * 4 + 3);
        sIndices[i * 6 + 4] = static_cast<uint16_t>(i * 4 + 2);
        sIndices[i * 6 + 5] = static_cast<uint16_t>(i * 4 + 1);
    }
}

}  // namespace opendw
