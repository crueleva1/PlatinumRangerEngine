#pragma once

#include "GLMMeshData.h"

class ECS_OGL_API COpenGLMeshBuffer : public CGLMMeshBuffer
{
public:
    COpenGLMeshBuffer();

    COpenGLMeshBuffer(unsigned int _nPrimitiveType,
                      size_t _nVertexCount,
                      size_t _nIndicesCount,
                      glm::vec3* _pkVertex,
                      glm::vec3* _pkNormal,
                      glm::vec3* _pkTangent,
                      glm::vec3* _pkBiNormal,
                      size_t _nChannelCount,
                      glm::vec3** _ppkTextCoor,
                      glm::vec4** _ppTextColor,
                      unsigned int* _pnIndices);

    virtual ~COpenGLMeshBuffer()
    {}

    std::shared_ptr <IBuffer> CreateVertexBuffer();
    std::shared_ptr <IBuffer> CreateIndexBuffer();
    std::shared_ptr <IBound> CreateBoundBox();

    bool UpdateVertexBuffer(IBuffer* _pkBuffer, bool _bInit = false);
    bool UpdateIndexBuffer(IBuffer* _pkBuffer, bool _bInit = false);
    bool UpdateBoundBox(IBound* _pkBound);
};