#include "GLMMainPCH.h"
#include "GLMMeshData.h"
#include "Util.h"

CGLMMeshBuffer::CGLMMeshBuffer()
    : m_nVarCount(0)
    , m_nPrimitiveType(ePrimitive_Triangles)
    , m_nFaceCount(0)
    , m_nMaterialIndex(0)
{
}

CGLMMeshBuffer::~CGLMMeshBuffer()
{

}

void CGLMMeshBuffer::SetMeshBuffer(EPrimitiveType _nPrimitiveType,
                                   size_t _nVertexCount,
                                   size_t _nIndicesCount,
                                   glm::vec3* _pkVertex,
                                   glm::vec3* _pkNormal,
                                   glm::vec3* _pkTangent,
                                   glm::vec3* _pkBiNormal,
                                   size_t _nChannelCount,
                                   glm::vec3** _ppkTextCoor,
                                   glm::vec4** _ppVertexColor,
                                   unsigned int* _pnIndices)
{
    CleanMeshBuffer();

    m_nVarCount = 0;
    m_nPrimitiveType = _nPrimitiveType;
    m_nFaceCount = (_nIndicesCount > 0) ? ((unsigned int)_nIndicesCount / 3) : ((unsigned int)_nVertexCount / 3);
    m_nMaterialIndex = 0;

    if (_pkVertex) {
        m_nVarCount++;
        m_kVertices.resize(_nVertexCount);
        memcpy(m_kVertices.data(), _pkVertex, _nVertexCount * sizeof(glm::vec3));
    }
    if (_pkNormal) {
        m_nVarCount++;
        m_kNormals.resize(_nVertexCount);
        memcpy(m_kNormals.data(), _pkNormal, _nVertexCount * sizeof(glm::vec3));
    }
    if (_pkTangent && _pkBiNormal) {
        m_nVarCount++;
        m_nVarCount++;
        m_kTangents.resize(_nVertexCount);
        m_kBiNormals.resize(_nVertexCount);
        memcpy(m_kTangents.data(), _pkTangent, _nVertexCount * sizeof(glm::vec3));
        memcpy(m_kBiNormals.data(), _pkBiNormal, _nVertexCount * sizeof(glm::vec3));
    }
    if (_nChannelCount > 0) {
        if (_ppkTextCoor && *_ppkTextCoor) {
            m_kTextureCoordinates.reserve(_nChannelCount);
            for (size_t nChannelCount = 0; nChannelCount < _nChannelCount; ++nChannelCount) {
                m_nVarCount++;
                std::vector < glm::vec3 >* pkCoors = new std::vector <glm::vec3>(_nVertexCount);
                memcpy(pkCoors->data(), _ppkTextCoor [nChannelCount], _nVertexCount * sizeof(glm::vec3));
                m_kTextureCoordinates.push_back(pkCoors);
            }
        }
        if (_ppVertexColor && *_ppVertexColor) {
            m_kVertexColors.reserve(_nChannelCount);
            for (size_t nChannelCount = 0; nChannelCount < _nChannelCount; nChannelCount++) {
                m_nVarCount++;
                std::vector <glm::vec4>* pkColors = new std::vector <glm::vec4>(_nVertexCount);
                memcpy(pkColors->data(), *_ppVertexColor, _nVertexCount * sizeof(glm::vec4));
                m_kVertexColors.push_back(pkColors);
            }
        }
    }
    if (_pnIndices) {
        m_kIndices.resize(_nIndicesCount);
        memcpy(m_kIndices.data(), _pnIndices, _nIndicesCount * sizeof(unsigned int));
    }
}

void CGLMMeshBuffer::CleanMeshBuffer()
{
    m_kVertices.clear();
    m_kNormals.clear();
    m_kTangents.clear();
    m_kBiNormals.clear();
    for (auto pkTextureCrood : m_kTextureCoordinates) {
        delete pkTextureCrood;
    }
    m_kTextureCoordinates.clear();
    for (auto pkVertexColor : m_kVertexColors) {
        delete pkVertexColor;
    }
    m_kVertexColors.clear();
}

std::vector<glm::vec3>& CGLMMeshBuffer::GetVertices()
{
    return m_kVertices;
}

std::vector<glm::vec3>& CGLMMeshBuffer::GetNormals()
{
    return m_kNormals;
}
std::vector<glm::vec3>& CGLMMeshBuffer::GetTangents()
{
    return m_kTangents;
}
std::vector<glm::vec3>& CGLMMeshBuffer::GetBiNormals()
{
    return m_kBiNormals;
}

std::vector<std::vector<glm::vec3>*>& CGLMMeshBuffer::GetTextureCoordinates()
{
    return m_kTextureCoordinates;
}

std::vector<std::vector<glm::vec4>*>& CGLMMeshBuffer::GetVertexColors()
{
    return m_kVertexColors;
}

unsigned int CGLMMeshBuffer::GetFaceCount()
{
    return m_nFaceCount;
}

std::vector<unsigned int>& CGLMMeshBuffer::GetIndices()
{
    return m_kIndices;
}

size_t CGLMMeshBuffer::GetVarCount()
{
    return m_nVarCount;
}

EPrimitiveType CGLMMeshBuffer::PrimitiveType()
{
    return m_nPrimitiveType;
}

int CGLMMeshBuffer::MaterialIndex()
{
    return m_nMaterialIndex;
}