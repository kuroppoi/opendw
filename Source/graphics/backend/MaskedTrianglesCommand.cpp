#include "MaskedTrianglesCommand.h"

#define MASK_VERTEX_SHADER   "custom/Mask_vs"
#define MASK_FRAGMENT_SHADER "custom/Mask_fs"

#define ATTRIB_POSITION      "a_position"
#define ATTRIB_COLOR         "a_color"
#define ATTRIB_TEXCOORD      "a_texCoord"
#define ATTRIB_MASKCOORD     "a_maskCoord"

#define UNIFORM_TEXTURE      "u_texture"
#define UNIFORM_MASK         "u_maskTexture"
#define UNIFORM_TRANSFORM    "u_MVPMatrix"

USING_NS_AX;

namespace opendw
{

MaskedTrianglesCommand::~MaskedTrianglesCommand()
{
    AX_SAFE_RELEASE(_programState);
}

ProgramState* MaskedTrianglesCommand::createProgramState()
{
    // Load shader program if necessary
    if (!sProgram)
    {
        sProgram = ProgramManager::getInstance()->loadProgram(MASK_VERTEX_SHADER, MASK_FRAGMENT_SHADER);
        AX_ASSERT(sProgram);
    }

    auto programState = new ProgramState(sProgram);  // Retains program

    // Set vertex layout
    auto vertexLayout = programState->getMutableVertexLayout();
    vertexLayout->setAttrib(ATTRIB_POSITION, programState->getAttributeLocation(ATTRIB_POSITION),
                            backend::VertexFormat::FLOAT3, offsetof(Vertex, position), false);
    vertexLayout->setAttrib(ATTRIB_COLOR, programState->getAttributeLocation(ATTRIB_COLOR),
                            backend::VertexFormat::UBYTE4, offsetof(Vertex, color), true);
    vertexLayout->setAttrib(ATTRIB_TEXCOORD, programState->getAttributeLocation(ATTRIB_TEXCOORD),
                            backend::VertexFormat::FLOAT2, offsetof(Vertex, texCoord), false);
    vertexLayout->setAttrib(ATTRIB_MASKCOORD, programState->getAttributeLocation(ATTRIB_MASKCOORD),
                            backend::VertexFormat::FLOAT2, offsetof(Vertex, maskCoord), false);
    vertexLayout->setStride(sizeof(Vertex));
    return programState;
}

void MaskedTrianglesCommand::init(float globalZOrder,
                                  Texture2D* texture,
                                  Texture2D* maskTexture,
                                  const BlendFunc& blendFunc,
                                  const Triangles& triangles,
                                  const Mat4& transform,
                                  uint32_t flags)
{
    CustomCommand::init(globalZOrder, transform, flags);
    _texture     = texture->getBackendTexture();
    _maskTexture = maskTexture->getBackendTexture();
    _triangles   = triangles;
    _mv          = transform;  // Not set by CustomCommand::init

    if (_triangles.indexCount % 3 != 0)
    {
        auto count            = _triangles.indexCount;
        _triangles.indexCount = count / 3 * 3;
        AXLOGW("[MaskedTrianglesCommand] Resized indexCount from {} to {}", count, _triangles.indexCount);
    }

    if (_blendFunc != blendFunc)
    {
        auto& descriptor                       = _pipelineDescriptor.blendDescriptor;
        descriptor.blendEnabled                = true;
        descriptor.sourceRGBBlendFactor        = blendFunc.src;
        descriptor.sourceAlphaBlendFactor      = blendFunc.src;
        descriptor.destinationRGBBlendFactor   = blendFunc.dst;
        descriptor.destinationAlphaBlendFactor = blendFunc.dst;
        _blendFunc                             = blendFunc;
    }

    updateProgramState();
}

void MaskedTrianglesCommand::updateProgramState()
{
    if (!_programState)
    {
        _programState                    = createProgramState();
        _pipelineDescriptor.programState = _programState;
        _textureLocation                 = _programState->getUniformLocation(UNIFORM_TEXTURE);
        _maskLocation                    = _programState->getUniformLocation(UNIFORM_MASK);
        _mvpMatrixLocation               = _programState->getUniformLocation(UNIFORM_TRANSFORM);
    }

    auto& projection = Director::getInstance()->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
    auto mvpMatrix   = projection * _mv;
    _programState->setUniform(_mvpMatrixLocation, mvpMatrix.m, sizeof(mvpMatrix.m));
    _programState->setTexture(_textureLocation, 0, _texture);
    _programState->setTexture(_maskLocation, 1, _maskTexture);
}

void MaskedTrianglesCommand::populateBuffers()
{
    auto vertexCount = _triangles.vertexCount;
    auto indexCount  = _triangles.indexCount;

    if (vertexCount != _vertexCapacity)
    {
        createVertexBuffer(sizeof(Vertex), vertexCount, BufferUsage::DYNAMIC);
    }

    if (indexCount != _indexCapacity)
    {
        createIndexBuffer(IndexFormat::U_SHORT, indexCount, BufferUsage::DYNAMIC);
    }

    updateVertexBuffer(_triangles.vertices, sizeof(Vertex) * vertexCount);
    updateIndexBuffer(_triangles.indices, sizeof(uint16_t) * indexCount);
}

}  // namespace opendw
