#ifndef __MASKED_TRIANGLES_COMMAND_H__
#define __MASKED_TRIANGLES_COMMAND_H__

#include "axmol.h"

namespace opendw
{

class MaskedTrianglesCommand : public ax::CustomCommand
{
public:
    struct Vertex
    {
        ax::Vec3 position;
        ax::Color4B color;
        ax::Tex2F texCoord;
        ax::Tex2F maskCoord;
    };

    struct Triangles
    {
        Vertex* vertices;
        uint16_t* indices;
        int vertexCount;
        int indexCount;
    };

    virtual ~MaskedTrianglesCommand() override;

    static ax::backend::ProgramState* createProgramState();

    void init(float globalZOrder,
              ax::Texture2D* texture,
              ax::Texture2D* maskTexture,
              const ax::BlendFunc& blendFunc,
              const Triangles& triangles,
              const ax::Mat4& transform,
              uint32_t flags);

    void updateProgramState();
    void populateBuffers();

private:
    inline static ax::Program* sProgram;

    Triangles _triangles;
    ax::backend::TextureBackend* _texture;
    ax::backend::TextureBackend* _maskTexture;
    ax::backend::UniformLocation _textureLocation;
    ax::backend::UniformLocation _maskLocation;
    ax::backend::UniformLocation _mvpMatrixLocation;
    ax::ProgramState* _programState = nullptr;
    ax::BlendFunc _blendFunc;
};

}  // namespace opendw

#endif  // __MASKED_TRIANGLES_COMMAND_H__
