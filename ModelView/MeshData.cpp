#include "MainPCH.h"
#include "MeshData.h"
#include "AssimpGLGeometryComp.h"
#include "OpenGLRenderComponent.h"
#include <assimp/scene.h>

const std::vector<CBoneWeights::CVertexWeight>& CBoneWeights::Weights()
{
    return m_kWeights;
}

void CBoneWeights::AddWeight(float _fWeight, unsigned int _nBoneIndex)
{
    m_kWeights.emplace_back(_fWeight, _nBoneIndex);
}

unsigned int CBone::Index() const
{
    return m_nIndex;
}

void CBone::SetIndex(unsigned int _nIndex)
{
    m_nIndex = _nIndex;
}

const glm::mat4& CBone::OffsetTransform() const
{
    return m_kOffsetTransform;
}

glm::mat4 CBone::OffsetTransformMatrix() const
{
    return m_kOffsetTransform;
}

CBone::CBone(std::string& _rkName, unsigned int _nIndex, glm::mat4& _rkOffsetTransform)
    :m_kName(_rkName)
    ,m_nIndex(_nIndex)
    ,m_kOffsetTransform(_rkOffsetTransform)
{

}

CMeshData::CMeshData(CAssimpGLGeometryComp& _rkModel, aiMesh& _rkMesh)
    : COpenGLMeshBuffer()
    , m_kModel(_rkModel)
    , m_kName(_rkMesh.mName.C_Str())
{
    m_nMaterialIndex = _rkMesh.mMaterialIndex;
    switch (_rkMesh.mPrimitiveTypes)
    {
    case aiPrimitiveType_POINT:
        m_nPrimitiveType = ePrimitive_Points;
        break;
    case aiPrimitiveType_LINE:
        m_nPrimitiveType = ePrimitive_Lines;
        break;
    case aiPrimitiveType_TRIANGLE:
        m_nPrimitiveType = ePrimitive_Triangles;
        break;
    case aiPrimitiveType_POLYGON:
        m_nPrimitiveType = ePrimitive_TriangleStrip;
        break;
    default:
        m_nPrimitiveType = ePrimitive_Triangles;
        break;
    }

    // Vertices
    m_kVertices.reserve(_rkMesh.mNumVertices);
    for (unsigned int nIndex = 0; nIndex < _rkMesh.mNumVertices; nIndex++) {
        aiVector3D& rkData = _rkMesh.mVertices [nIndex];
        m_kVertices.emplace_back(rkData.x, rkData.y, rkData.z);
    }
    m_nVarCount++;

    // Normals
    if (_rkMesh.HasNormals()) {
        m_kNormals.reserve(_rkMesh.mNumVertices);
        for (unsigned int nIndex = 0; nIndex < _rkMesh.mNumVertices; nIndex++) {
            aiVector3D& rkData = _rkMesh.mNormals [nIndex];
            m_kNormals.emplace_back(rkData.x, rkData.y, rkData.z);
        }
        m_nVarCount++;
    }

    // Tangents and Binormals
    if (_rkMesh.HasTangentsAndBitangents()) {
        m_kTangents.reserve(_rkMesh.mNumVertices);
        m_kBiNormals.reserve(_rkMesh.mNumVertices);
        for (unsigned int nIndex = 0; nIndex < _rkMesh.mNumVertices; nIndex++) {
            aiVector3D& rkTangentData = _rkMesh.mTangents [nIndex];
            aiVector3D& rkBiNormaData = _rkMesh.mBitangents [nIndex];
            m_kTangents.emplace_back (rkTangentData.x, rkTangentData.y, rkTangentData.z);
            m_kBiNormals.emplace_back (rkBiNormaData.x, rkBiNormaData.y, rkBiNormaData.z);
        }
        m_nVarCount++;
        m_nVarCount++;
    }

    // Texture Coordinates
    unsigned int uvChannelCount = _rkMesh.GetNumUVChannels();
    for (unsigned int nIndex = 0; nIndex < uvChannelCount; nIndex++) {
        std::vector<glm::vec3>* pkTextureCoordinates = new std::vector<glm::vec3>();
        pkTextureCoordinates->reserve(_rkMesh.mNumVertices);
        m_kTextureCoordinates.push_back(pkTextureCoordinates);

        aiVector3D* pakTextureCoordinates = _rkMesh.mTextureCoords[nIndex];
        for (unsigned int nCount = 0; nCount < _rkMesh.mNumVertices; nCount++)
        {
            aiVector3D& rkTextureCoor = pakTextureCoordinates[nCount];
            pkTextureCoordinates->emplace_back (rkTextureCoor.x, rkTextureCoor.y, rkTextureCoor.z);
        }
        m_nVarCount++;
    }

    // Vertex Colors
    unsigned int nChannelCount = _rkMesh.GetNumColorChannels();
    for (unsigned int nIndex = 0; nIndex < nChannelCount; nIndex++) {
        std::vector<glm::vec4>* pkVertexColors = new std::vector<glm::vec4>();
        pkVertexColors->reserve(_rkMesh.mNumVertices);
        m_kVertexColors.push_back(pkVertexColors);

        aiColor4D* pakVertexColors = _rkMesh.mColors[nIndex];
        for (unsigned int nCount = 0; nCount < _rkMesh.mNumVertices; nCount++) {
            aiColor4D& rkColor = pakVertexColors [nCount];
            pkVertexColors->emplace_back (rkColor.r, rkColor.g, rkColor.b, rkColor.a);
        }
        m_nVarCount++;
    }

    // Faces (note: could pre-reserve if we limit primitive types)
    if (_rkMesh.HasFaces()) {
        m_nFaceCount = _rkMesh.mNumFaces;
        for (unsigned int nIndex = 0; nIndex < m_nFaceCount; nIndex++) {
            aiFace* pkFace = &_rkMesh.mFaces[nIndex];
        
            for (unsigned int nCount = 0; nCount < pkFace->mNumIndices; nCount++) {
                m_kIndices.push_back(pkFace->mIndices[nCount]);
            }
        }
    }
#if 1
    // Bones
    if (_rkMesh.HasBones()) {
        m_kBoneWeights.resize(_rkMesh.mNumVertices);

        for (unsigned int i = 0; i < _rkMesh.mNumBones; i++) {
            aiBone* pkMeshBone = _rkMesh.mBones[i];

            // Look up the bone in the model's hierarchy, or add it if not found.
            unsigned int nBoneIndex = 0U;
            std::string kBoneName = pkMeshBone->mName.C_Str();
            auto kBoneMappingIterator = m_kBoneIndexMapping.find(kBoneName);
            if (kBoneMappingIterator != m_kBoneIndexMapping.end())
            {
                nBoneIndex = kBoneMappingIterator->second;                    
            }
            else
            {                    
                nBoneIndex = m_kBones.size();
                glm::mat4 kOffsetMat = *(reinterpret_cast<glm::mat4*>(pkMeshBone->mOffsetMatrix[0]));
                m_kBones.emplace_back(kBoneName, nBoneIndex, kOffsetMat);
                m_kBoneIndexMapping [kBoneName] = nBoneIndex;
            }

            for (unsigned int i = 0; i < pkMeshBone->mNumWeights; i++)
            {
                aiVertexWeight vertexWeight = pkMeshBone->mWeights[i];                    
                m_kBoneWeights[vertexWeight.mVertexId].AddWeight(vertexWeight.mWeight, nBoneIndex);
            }
        }
    }
#endif
}

CMeshData::~CMeshData()
{
    for (std::vector<glm::vec3>* pkTextureCoordinates : m_kTextureCoordinates)
    {
        delete pkTextureCoordinates;
    }

    for (std::vector<glm::vec4>* pkVertexColors : m_kVertexColors)
    {
        delete pkVertexColors;
    }
}

CAssimpGLGeometryComp& CMeshData::GetModel()
{
    return m_kModel;
}

const std::string& CMeshData::GetName() const
{
    return m_kName;
}

const std::vector<glm::vec3>& CMeshData::GetVertices() const
{
    return GetVertices();
}

const std::vector<glm::vec3>& CMeshData::GetNormals() const
{
    return GetNormals();
}

const std::vector<glm::vec3>& CMeshData::GetTangents() const
{
    return GetTangents();
}

const std::vector<glm::vec3>& CMeshData::GetBiNormals() const
{
    return GetBiNormals();
}

const std::vector<std::vector<glm::vec3>*>& CMeshData::GetTextureCoordinates() const
{
    return GetTextureCoordinates();
}

const std::vector<std::vector<glm::vec4>*>& CMeshData::GetVertexColors() const
{
    return GetVertexColors();
}

unsigned int CMeshData::GetFaceCount() const
{
    return GetFaceCount();
}

const std::vector<unsigned int>& CMeshData::GetIndices() const
{
    return GetIndices();
}

const std::vector<CBoneWeights>& CMeshData::GetBoneWeights() const
{
    return m_kBoneWeights;
}

void CMeshData::XFormVertex(const glm::mat4& _rkTransform)
{
    for (glm::vec3& rkPos : m_kVertices) {
        glm::vec4 kTransPos = _rkTransform * glm::vec4(rkPos, 1.0f);
        rkPos = kTransPos;
    }
}

std::shared_ptr <CGLMMeshBuffer> CreateRectData(float nLeft, float nRight, float nTop, float nBottom)
{
    std::shared_ptr <IMeshBuffer> spkMesh = IRenderSystem::CreateRenderMeshBuffer();
    std::shared_ptr <CGLMMeshBuffer> spkGLMMesh = std::dynamic_pointer_cast <CGLMMeshBuffer>(spkMesh);
    if (spkGLMMesh == nullptr)
        return nullptr;
    const size_t nVerticeCount = 4;
    const size_t nIndicesCount = 6;
    glm::vec3 kVertices [nVerticeCount];
    glm::vec3 kTextureCoord [nVerticeCount];
    unsigned int kIndices [nIndicesCount];

    kVertices [0] = glm::vec3(nLeft, nTop, 0.0f);
    kTextureCoord [0] = glm::vec3(0.0f, 1.0f, 0.0f);
    kVertices [1] = glm::vec3(nRight, nTop, 0.0f);
    kTextureCoord [1] = glm::vec3(1.0f, 1.0f, 0.0f);
    kVertices [2] = glm::vec3(nLeft, nBottom, 0.0f);
    kTextureCoord [2] = glm::vec3(0.0f, 0.0f, 0.0f);
    kVertices [3] = glm::vec3(nRight, nBottom, 0.0f);
    kTextureCoord [3] = glm::vec3(1.0f, 0.0f, 0.0f);

    kIndices [0] = 2;
    kIndices [1] = 1;
    kIndices [2] = 0;

    kIndices [3] = 1;
    kIndices [4] = 2;
    kIndices [5] = 3;

    glm::vec3* pkTextureCoord = kTextureCoord;
    spkGLMMesh->SetMeshBuffer(ePrimitive_Triangles,
                             nVerticeCount,
                             nIndicesCount,
                             kVertices,
                             nullptr,
                             nullptr,
                             nullptr,
                             1,
                             &pkTextureCoord,
                             nullptr,
                             kIndices);
    return spkGLMMesh;
}

std::shared_ptr <CGLMMeshBuffer> CreateSphereData(float _nRadius, int _nStack, int _nSlice)
{
    std::shared_ptr <IMeshBuffer> spkMesh = IRenderSystem::CreateRenderMeshBuffer();
    std::shared_ptr <CGLMMeshBuffer> spkGLMMesh = std::dynamic_pointer_cast <CGLMMeshBuffer>(spkMesh);
    if (spkGLMMesh == nullptr)
        return nullptr;
    const int nVertices = (_nStack + 1) * (_nSlice + 1);
    const int nTriangles = _nStack * _nSlice * 2;
    const int nIndices = nTriangles * 3;

    std::vector <glm::vec3> kVertices(nVertices);
    std::vector <glm::vec3> kNormals(nVertices);
    std::vector <glm::vec3> kTextureCoords(nVertices);
    std::vector <unsigned int> kIndices(nIndices);

    const float nThetaStartRadius = DegreeToRadian(0.0f);
    const float nThetaEndRadius = DegreeToRadian(360.0f);
    const float nPhiStartRadius = DegreeToRadian(-90.0f);
    const float nPhiEndRadius = DegreeToRadian(90.0f);

    float nThetaTotal = nThetaEndRadius - nThetaStartRadius;
    float nPhiTotal = nPhiEndRadius - nPhiStartRadius;
    float nThetaStep = nThetaTotal / _nStack;
    float nPhiStep = nPhiTotal / _nSlice;

    float nYStart = 1.0f;
    float nYStep = -1.0f / _nSlice;
    float nXStart = 1.0f;
    float nXStep = -1.0f / _nStack;

    int index = 0;
    float nTheta = nThetaStartRadius;
    glm::vec3 kTextureCoord(nXStart, nYStart, 0.0f);
    int nIndex = 0;
    for (int nStackIndex = 0; nStackIndex <= _nStack; nStackIndex++) {
        float nPhi = nPhiStartRadius;
        float nSinTheta;
        float nCosTheta;
        SinCos(nTheta, nSinTheta, nCosTheta);
        for (int nSlicesIndex = 0; nSlicesIndex <= _nSlice; nSlicesIndex++) {
            float nSinPhi;
            float nCosPhi;
            SinCos(nPhi, nSinPhi, nCosPhi);
            glm::vec3 kNormal(nCosPhi * nCosTheta, nSinPhi, nCosPhi * nSinTheta);
            kNormals [nIndex] = kNormal;
            kVertices [nIndex] = (kNormal * _nRadius);
            kTextureCoords [nIndex] = kTextureCoord;
            nPhi += nPhiStep;
            kTextureCoord.y += nYStep;
            nIndex++;
        }
        nTheta += nThetaStep;
        kTextureCoord.x += nXStep;
        kTextureCoord.y = nYStart;
    }

    // build index array
    int nOffset = 0;
    nIndex = 0;

    // triangle list
    for (int nStackIndex = 0; nStackIndex < _nStack; nStackIndex++) {
        for (int nSliceIndex = 0; nSliceIndex < _nSlice; nSliceIndex++) {
            kIndices [nIndex++] = nOffset;
            kIndices [nIndex++] = nOffset + 1;
            kIndices [nIndex++] = nOffset + _nSlice + 1;

            kIndices [nIndex++] = nOffset + 1;
            kIndices [nIndex++] = nOffset + _nSlice + 2;
            kIndices [nIndex++] = nOffset + _nSlice + 1;
            nOffset++;
        }
        nOffset++;
    }

    glm::vec3* pkTextureCoord = kTextureCoords.data();

    spkGLMMesh->SetMeshBuffer(ePrimitive_Triangles,
                             nVertices,
                             nIndices,
                             kVertices.data(),
                             kNormals.data(),
                             nullptr,
                             nullptr,
                             1,
                             &pkTextureCoord,
                             nullptr,
                             kIndices.data());
    return spkGLMMesh;
}

std::shared_ptr <CGLMMeshBuffer> CreateBoxdata(const glm::vec3& _rkHalfVec)
{
    std::shared_ptr <IMeshBuffer> spkMesh = IRenderSystem::CreateRenderMeshBuffer();
    std::shared_ptr <CGLMMeshBuffer> spkGLMMesh = std::dynamic_pointer_cast <CGLMMeshBuffer>(spkMesh);
    if (spkGLMMesh == nullptr)
        return nullptr;
    const int nVertices = 8;
    const int nIndices = 36;
    const int nFace = 6;
    std::vector <glm::vec3> kVertices(nVertices);
    std::vector <glm::vec3> kTextureCoords(nVertices);
    std::vector <unsigned int> kIndices(nIndices);

    const glm::vec2 kTexVec(1.0f, 1.0f);
    const int nCount = nVertices / 2;
    for (char nIndex = 0; nIndex < nCount; nIndex++) {
        const char nIndex1 = nIndex;
        const char nIndex2 = nIndex + nCount;
        kVertices [nIndex1] = _rkHalfVec;
        kVertices [nIndex2] = -_rkHalfVec;
        kTextureCoords [nIndex1] = glm::vec3(kTexVec, 0.0);
        kTextureCoords [nIndex2] = glm::vec3(-kTexVec, 0.0);

        if (nIndex & 1) {
            kVertices [nIndex1].z *= -1;
            kVertices [nIndex2].z *= -1;
            kTextureCoords [nIndex1].x *= -1;
            kTextureCoords [nIndex2].x *= -1;
        }
        if (nIndex & 2) {
            kVertices [nIndex1].y *= -1;
            kVertices [nIndex2].y *= -1;
            kTextureCoords [nIndex1].y *= -1;
            kTextureCoords [nIndex2].y *= -1;
        }
    }

    kIndices [0] = 0; kIndices [1] = 1; kIndices [2] = 2;
    kIndices [3] = 3; kIndices [4] = 2; kIndices [5] = 1;
    kIndices [6] = 1; kIndices [7] = 4; kIndices [8] = 3;
    kIndices [9] = 6; kIndices [10] = 4; kIndices [11] = 1;
    kIndices [12] = 6; kIndices [13] = 5; kIndices [14] = 4;
    kIndices [15] = 7; kIndices [16] = 5; kIndices [17] = 6;
    kIndices [18] = 7; kIndices [19] = 2; kIndices [20] = 5;
    kIndices [21] = 0; kIndices [22] = 2; kIndices [23] = 7;
    kIndices [24] = 6; kIndices [25] = 0; kIndices [26] = 7;
    kIndices [27] = 1; kIndices [28] = 0; kIndices [29] = 6;
    kIndices [30] = 5; kIndices [31] = 3; kIndices [32] = 4;
    kIndices [33] = 2; kIndices [34] = 3; kIndices [35] = 5;

    glm::vec3* pkTextureCoord = kTextureCoords.data();

    spkGLMMesh->SetMeshBuffer(ePrimitive_Triangles,
                             nVertices,
                             nIndices,
                             kVertices.data(),
                             nullptr,
                             nullptr,
                             nullptr,
                             1,
                             &pkTextureCoord,
                             nullptr,
                             kIndices.data());
    return spkGLMMesh;
}

std::shared_ptr <CGLMMeshBuffer> CreateLineBoxdata(const glm::vec3& _rkHalfVec)
{
    std::shared_ptr <IMeshBuffer> spkMesh = IRenderSystem::CreateRenderMeshBuffer();
    std::shared_ptr <CGLMMeshBuffer> spkGLMMesh = std::dynamic_pointer_cast <CGLMMeshBuffer>(spkMesh);
    if (spkGLMMesh == nullptr)
        return nullptr;
    const int nVertices = 8;
    const int nIndices = 24;
    glm::vec3 kVertices [nVertices] = {};
    unsigned int kIndices [nIndices] = {};
    const int nCount = nVertices / 2;
    for (char nIndex = 0; nIndex < nCount; nIndex++) {
        const char nIndex1 = nIndex;
        const char nIndex2 = nIndex + nCount;
        kVertices [nIndex1] = _rkHalfVec;
        kVertices [nIndex2] = -_rkHalfVec;

        if (nIndex & 1) {
            kVertices [nIndex1].z *= -1;
            kVertices [nIndex2].z *= -1;
        }
        if (nIndex & 2) {
            kVertices [nIndex1].y *= -1;
            kVertices [nIndex2].y *= -1;
        }
    }

    kIndices [0] = 0; kIndices [1] = 1;
    kIndices [2] = 0; kIndices [3] = 2;
    kIndices [4] = 1; kIndices [5] = 3;
    kIndices [6] = 2; kIndices [7] = 3;
    kIndices [8] = 1; kIndices [9] = 6;
    kIndices [10] = 2; kIndices [11] = 5;
    kIndices [12] = 5; kIndices [13] = 4;
    kIndices [14] = 5; kIndices [15] = 7;
    kIndices [16] = 4; kIndices [17] = 3;
    kIndices [18] = 4; kIndices [19] = 6;
    kIndices [20] = 7; kIndices [21] = 6;
    kIndices [22] = 7; kIndices [23] = 0;

    spkGLMMesh->SetMeshBuffer(ePrimitive_Lines,
                             nVertices,
                             nIndices,
                             kVertices,
                             nullptr,
                             nullptr,
                             nullptr,
                             1,
                             nullptr,
                             nullptr,
                             kIndices);
    return spkGLMMesh;
}

void UpdateBoxData(CGLMMeshBuffer* pkMeshData, const glm::vec3& _rkHalfVec)
{
    if (!pkMeshData)
        return;
    const int nVertices = 8;
    std::vector <glm::vec3>& rkVertice = pkMeshData->GetVertices();
    // Make sure created by CreateBoxdata
    if (rkVertice.size() != nVertices)
        return;

    const int nCount = nVertices / 2;
    for (char nIndex = 0; nIndex < nCount; nIndex++) {
        const char nIndex1 = nIndex;
        const char nIndex2 = nIndex + nCount;
        rkVertice [nIndex1] = _rkHalfVec;
        rkVertice [nIndex2] = -_rkHalfVec;

        if (nIndex & 1) {
            rkVertice [nIndex1].z *= -1;
            rkVertice [nIndex2].z *= -1;
        }
        if (nIndex & 2) {
            rkVertice [nIndex1].y *= -1;
            rkVertice [nIndex2].y *= -1;
        }
    }
}