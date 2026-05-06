#pragma once

#include <vector>
#include "RenderSystem.h"
#include "RenderState.h"


class ECSGLM_API CGLMMeshBuffer : public IMeshBuffer
{
protected:
    std::vector<glm::vec3> m_kVertices;
    std::vector<glm::vec3> m_kNormals;
    std::vector<glm::vec3> m_kTangents;
    std::vector<glm::vec3> m_kBiNormals;
    std::vector<std::vector<glm::vec3>*> m_kTextureCoordinates;
    std::vector<std::vector<glm::vec4>*> m_kVertexColors;
    std::vector<unsigned int> m_kIndices;
    unsigned int m_nFaceCount;
    EPrimitiveType m_nPrimitiveType;
    int m_nMaterialIndex;
    size_t m_nVarCount;
public:
    CGLMMeshBuffer();

    virtual ~CGLMMeshBuffer();

    void SetMeshBuffer(EPrimitiveType _nPrimitiveType,
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

    void CleanMeshBuffer();

    virtual std::vector<glm::vec3>& GetVertices();
    virtual std::vector<glm::vec3>& GetNormals();
    virtual std::vector<glm::vec3>& GetTangents();
    virtual std::vector<glm::vec3>& GetBiNormals();
    virtual std::vector<std::vector<glm::vec3>*>& GetTextureCoordinates();
    virtual std::vector<std::vector<glm::vec4>*>& GetVertexColors();
    virtual unsigned int GetFaceCount();
    virtual std::vector<unsigned int>& GetIndices();
    virtual size_t GetVarCount();
    virtual EPrimitiveType PrimitiveType();
    virtual int MaterialIndex();

};