#pragma once

#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/matrix.hpp>
#include "OpenGLMeshData.h"

struct aiMesh;

class ECS_MODELVIEW_API CBoneWeights
{
public:
    struct CVertexWeight
    {
        float m_fWeight;
        unsigned int m_nBoneIndex;

        CVertexWeight(float _fWeight, unsigned int _nBoneIndex)
            : m_fWeight(_fWeight), m_nBoneIndex(_nBoneIndex)
        {
        }
    };

    const std::vector<CVertexWeight>& Weights();

    void AddWeight(float _fWeight, unsigned int _nBoneIndex);

    static const unsigned int m_nMaxBoneWeightsPerVertex = 4;

private:
    std::vector<CVertexWeight> m_kWeights;
};

class ECS_MODELVIEW_API CBone
{
public:
    unsigned int Index() const;
    void SetIndex(unsigned int _nIndex);

    const glm::mat4& OffsetTransform() const;
    glm::mat4 OffsetTransformMatrix() const;

    CBone(std::string& _rkName, unsigned int _nIndex, glm::mat4& _rkOffsetTransform);

private:

    std::string m_kName;
    unsigned int m_nIndex;                    // Index into the model's bone container
    glm::mat4 m_kOffsetTransform;    // Transforms from mesh space to bone space
};

class ECS_MODELVIEW_API CMeshData : public COpenGLMeshBuffer
{
    friend class CAssimpGLGeometryComp;
public:
    virtual ~CMeshData();

    CAssimpGLGeometryComp& GetModel();
    const std::string& GetName() const;

    const std::vector<glm::vec3>& GetVertices() const;
    const std::vector<glm::vec3>& GetNormals() const;
    const std::vector<glm::vec3>& GetTangents() const;
    const std::vector<glm::vec3>& GetBiNormals() const;
    const std::vector<std::vector<glm::vec3>*>& GetTextureCoordinates() const;
    const std::vector<std::vector<glm::vec4>*>& GetVertexColors() const;
    unsigned int GetFaceCount() const;
    const std::vector<unsigned int>& GetIndices() const;
    const std::vector<CBoneWeights>& GetBoneWeights() const;

    void XFormVertex(const glm::mat4& _rkTransform);

    CMeshData(CAssimpGLGeometryComp& _rkModel, aiMesh& _rkMesh);
private:

    CAssimpGLGeometryComp& m_kModel;
    std::string m_kName;
    std::vector<CBoneWeights> m_kBoneWeights;
    std::vector<CBone> m_kBones;
    std::map <std::string, size_t> m_kBoneIndexMapping;
};

std::shared_ptr <CGLMMeshBuffer> CreateRectData(float _nLeft, float _nRight, float _nTop, float _nBottom);

std::shared_ptr <CGLMMeshBuffer> CreateSphereData(float _nRadius, int _nStack, int _nSlice);

std::shared_ptr <CGLMMeshBuffer> CreateBoxdata(const glm::vec3& _rkHalfVec);

std::shared_ptr <CGLMMeshBuffer> CreateLineBoxdata(const glm::vec3& _rkHalfVec);


void UpdateBoxData(CGLMMeshBuffer* pkMesh, const glm::vec3& _rkHalfVec);