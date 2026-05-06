#include "ECSOpenGLPCH.h"
#include "OpenGLMeshData.h"
#include "OpenGLRenderComponent.h"
#include "GLMBoxBound.h"

#define USE_BUFFERDATA 1

void CopyToMappedBuffer(char* _pcBuffer, char* _pcSrc, size_t _nSize, size_t _nOffset, size_t _nStrip, size_t _nCount)
{
    for (size_t nIndex = 0; nIndex < _nCount; nIndex++) {
        char* pkDst = _pcBuffer + _nOffset + (_nStrip * nIndex);
        char* pkSrc = _pcSrc + (_nSize * nIndex);
        memcpy(pkDst, pkSrc, _nSize);
    }
}

COpenGLMeshBuffer::COpenGLMeshBuffer()
    : CGLMMeshBuffer()
{
}

COpenGLMeshBuffer::COpenGLMeshBuffer(unsigned int _nPrimitiveType,
                                     size_t _nVertexCount,
                                     size_t _nIndicesCount,
                                     glm::vec3* _pkVertex,
                                     glm::vec3* _pkNormal,
                                     glm::vec3* _pkTangent,
                                     glm::vec3* _pkBiNormal,
                                     size_t _nChannelCount,
                                     glm::vec3** _ppkTextCoor,
                                     glm::vec4** _ppTextColor,
                                     unsigned int* _pnIndices)
{
    SetMeshBuffer((EPrimitiveType)_nPrimitiveType,
                  _nVertexCount,
                  _nIndicesCount,
                  _pkVertex,
                  _pkNormal,
                  _pkTangent,
                  _pkBiNormal,
                  _nChannelCount,
                  _ppkTextCoor,
                  _ppTextColor,
                  _pnIndices);
}

std::shared_ptr <IBuffer> COpenGLMeshBuffer::CreateVertexBuffer()
{
#if USE_BUFFERDATA
    std::shared_ptr <COpenGLVertexBuffer> spkVertexBuffer = std::make_shared <COpenGLBufferDataVertexBuffer>();
#else
    std::shared_ptr <COpenGLVertexBuffer> spkVertexBuffer = std::make_shared <COpenGLVertexBuffer>();
#endif
    std::vector<glm::vec3>& rkVertices = GetVertices();
    if (rkVertices.empty())
        return nullptr;

    std::vector<glm::vec3>& rkNormals = GetNormals();
    std::vector<glm::vec3>& rkTangents = GetTangents();
    std::vector<glm::vec3>& rkBiNormals = GetBiNormals();
    std::vector<std::vector<glm::vec3>*>& rkTextureCoordinates = GetTextureCoordinates();
    std::vector<std::vector<glm::vec4>*>& rkVertexColors = GetVertexColors();
    unsigned int nFaceCount = GetFaceCount();
    size_t nVarCount = GetVarCount();
    
    spkVertexBuffer->SetAttrSemanticCount(nVarCount);
    size_t nIndex = 0;
    if (!rkVertices.empty()) {
        spkVertexBuffer->SetAttrSemantic <glm::vec3>("Vertices", nIndex++, glm::vec3().length(), eVSET_float);
    }
    if (!rkNormals.empty()) {
        spkVertexBuffer->SetAttrSemantic <glm::vec3>("Normals", nIndex++, glm::vec3().length(), eVSET_float);
    }
    if (!rkTangents.empty()) {
        spkVertexBuffer->SetAttrSemantic <glm::vec3>("Tangents", nIndex++, glm::vec3().length(), eVSET_float);
    }
    if (!rkBiNormals.empty()) {
        spkVertexBuffer->SetAttrSemantic <glm::vec3>("BiNormals", nIndex++, glm::vec3().length(), eVSET_float);
    }
    if (!rkTextureCoordinates.empty()) {
        size_t nChannel = 1;
        for (auto pkTextureCoor : rkTextureCoordinates) {
            char cBuffer [64] = { 0 };
            sprintf_s(cBuffer, sizeof(cBuffer), "Texture%zd", nChannel++);
            bool bIs2DTexture = true;
            for (auto kVec3 : *pkTextureCoor) {
                if (kVec3.z != 0) {
                    bIs2DTexture = false;
                }
            }
            if (bIs2DTexture)
                spkVertexBuffer->SetAttrSemantic <glm::vec2>(cBuffer, nIndex++, glm::vec2().length(), eVSET_float);
            else
                spkVertexBuffer->SetAttrSemantic <glm::vec3>(cBuffer, nIndex++, glm::vec3().length(), eVSET_float);
        }
    }
    if (!rkVertexColors.empty()) {
        size_t nChannel = 1;
        for (auto pkColors : rkVertexColors) {
            char cBuffer [64] = { 0 };
            sprintf_s(cBuffer, sizeof(cBuffer), "VertexColor%zd", nChannel++);
            spkVertexBuffer->SetAttrSemantic <glm::vec4>(cBuffer, nIndex++, glm::vec4().length(), eVSET_float);
        }
    }

    if (!spkVertexBuffer->Allocate(spkVertexBuffer->GetAttrSemanticSize() * rkVertices.size()))
        return nullptr;

    if (!UpdateVertexBuffer(spkVertexBuffer.get(), true))
        return nullptr;

    return spkVertexBuffer;
}

std::shared_ptr <IBuffer> COpenGLMeshBuffer::CreateIndexBuffer()
{
    std::vector<unsigned int>& rkIndices = GetIndices();
    if (rkIndices.empty())
        return nullptr;

    int nIndicesType = GL_UNSIGNED_INT;
#if USE_BUFFERDATA
    std::shared_ptr <IIndiceBuffer> spkIndicesBuffer = std::make_shared <COpenGLBufferDataIndiceBuffer>(nIndicesType);
#else
    std::shared_ptr <IIndiceBuffer> spkIndicesBuffer = std::make_shared <COpenGLIndiceBuffer>(nIndicesType);
#endif
    if (!spkIndicesBuffer->Allocate(sizeof(unsigned int) * rkIndices.size())) {
        return nullptr;
    }

    if (!UpdateIndexBuffer(spkIndicesBuffer.get(), true))
        return nullptr;

    return spkIndicesBuffer;
}

std::shared_ptr <IBound> COpenGLMeshBuffer::CreateBoundBox()
{
    std::shared_ptr <IBound> spkBound = std::make_shared <CGLMBoundBox>();
    if (!UpdateBoundBox(spkBound.get()))
        return nullptr;
    return spkBound;
}

bool COpenGLMeshBuffer::UpdateVertexBuffer(IBuffer* _pkBuffer, bool _bInit /*= false*/)
{
    if (!_pkBuffer)
        return false;

    COpenGLVertexBuffer* pkVertexBuffer = static_cast <COpenGLVertexBuffer*>(_pkBuffer);
    if (!_bInit) {
        if (pkVertexBuffer && (pkVertexBuffer->GetUseage() != GL_DYNAMIC_DRAW)) {
            if (!pkVertexBuffer->SetUsage(GL_DYNAMIC_DRAW))
                return false;
        }
    }

    std::vector<glm::vec3>& rkVertices = GetVertices();
    std::vector<glm::vec3>& rkNormals = GetNormals();
    std::vector<glm::vec3>& rkTangents = GetTangents();
    std::vector<glm::vec3>& rkBiNormals = GetBiNormals();
    std::vector<std::vector<glm::vec3>*>& rkTextureCoordinates = GetTextureCoordinates();
    std::vector<std::vector<glm::vec4>*>& rkVertexColors = GetVertexColors();
    unsigned int nFaceCount = GetFaceCount();
    size_t nVarCount = GetVarCount();

    const size_t nStrip = pkVertexBuffer->GetAttrSemanticSize();
    size_t nVertexSize = pkVertexBuffer->GetSize() / nStrip;
    if (nVertexSize < rkVertices.size())
        return false;

    void* pkMappedBuffer = _pkBuffer->Map();
    if (!pkMappedBuffer)
        return false;

    const size_t nVarSemanticCount = pkVertexBuffer->GetAttrSemanticCount();
    size_t nOffset = 0;
    bool bComplate = true;
    for (size_t nIndex = 0; nIndex < nVarSemanticCount; nIndex++) {
        const IVariableSemantic* pkSemantic = pkVertexBuffer->GetAttrSemantic(nIndex);
        std::string kName = pkSemantic->GetName();
        if (kName.find("Vertices") != std::string::npos) {
            if (pkSemantic->GetDescSize() != sizeof(glm::vec3)) {
                bComplate = false;
                break;
            }
            CopyToMappedBuffer((char*)pkMappedBuffer, (char*)rkVertices.data(), sizeof(glm::vec3), nOffset, nStrip, rkVertices.size());
            nOffset = nOffset + pkSemantic->GetDescSize();
        }
        else if (kName.find("Tangents") != std::string::npos) {
            if (pkSemantic->GetDescSize() != sizeof(glm::vec3)) {
                bComplate = false;
                break;
            }
            CopyToMappedBuffer((char*)pkMappedBuffer, (char*)rkTangents.data(), sizeof(glm::vec3), nOffset, nStrip, rkTangents.size());
            nOffset = nOffset + pkSemantic->GetDescSize();
        }
        else if (kName.find("BiNormals") != std::string::npos) {
            if (pkSemantic->GetDescSize() != sizeof(glm::vec3)) {
                bComplate = false;
                break;
            }
            CopyToMappedBuffer((char*)pkMappedBuffer, (char*)rkBiNormals.data(), sizeof(glm::vec3), nOffset, nStrip, rkBiNormals.size());
            nOffset = nOffset + pkSemantic->GetDescSize();
        }
        else if (kName.find("Normals") != std::string::npos) {
            if (pkSemantic->GetDescSize() != sizeof(glm::vec3)) {
                bComplate = false;
                break;
            }
            CopyToMappedBuffer((char*)pkMappedBuffer, (char*)rkNormals.data(), sizeof(glm::vec3), nOffset, nStrip, rkNormals.size());
            nOffset = nOffset + pkSemantic->GetDescSize();
        }
        else if (kName.find("Texture") != std::string::npos) {
            std::string kCannel = kName.substr(sizeof("Texture") - 1);
            size_t nChannel = std::stoi(kCannel);
            if ((nChannel - 1) >= rkTextureCoordinates.size()) {
                bComplate = false;
                break;
            }
            auto pkTextureCoor = rkTextureCoordinates[nChannel - 1];

            if (pkSemantic->GetDescSize() == sizeof(glm::vec3)) {
                CopyToMappedBuffer((char*)pkMappedBuffer, (char*)pkTextureCoor->data(), sizeof(glm::vec3), nOffset, nStrip, pkTextureCoor->size());
                nOffset = nOffset + pkSemantic->GetDescSize();
            }
            else if (pkSemantic->GetDescSize() == sizeof(glm::vec2)) {
                for (size_t nIndex = 0; nIndex < pkTextureCoor->size(); nIndex++) {
                    glm::vec3& rkVec3 =(*pkTextureCoor)[nIndex];
                    glm::vec2 kVec2(rkVec3.x, rkVec3.y);
                    char* pkDst =(char*)pkMappedBuffer + nOffset +(nStrip * nIndex);
                    char* pkSrc =(char*)(&kVec2);
                    memcpy(pkDst, pkSrc, sizeof(glm::vec2));
                }
                nOffset = nOffset + pkSemantic->GetDescSize();
            }
            else {
                bComplate = false;
                break;
            }
        }
        else if (kName.find("VertexColor") != std::string::npos) {
            std::string kCannel = kName.substr(sizeof("VertexColor") - 1);
            size_t nChannel = std::stoi(kCannel);
            if ((nChannel - 1) >= rkVertexColors.size()) {
                bComplate = false;
                break;
            }
            auto pkVertexColor = rkVertexColors[nChannel - 1];
            if (pkSemantic->GetDescSize() != sizeof(glm::vec4)) {
                bComplate = false;
                break;
            }
            CopyToMappedBuffer((char*)pkMappedBuffer, (char*)pkVertexColor->data(), sizeof(glm::vec4), nOffset, nStrip, pkVertexColor->size());
            nOffset = nOffset + pkSemantic->GetDescSize();
        }
        else {
            bComplate = false;
            break;
        }
    }
    _pkBuffer->UnMap();

    return bComplate;
}

bool COpenGLMeshBuffer::UpdateIndexBuffer(IBuffer* _pkBuffer, bool _bInit /*= false*/)
{
    std::vector<unsigned int>& rkIndices = GetIndices();
    if (rkIndices.empty()) {
        return false;
    }

    if (!_pkBuffer)
        return false;

    COpenGLIndiceBuffer* pkIndicesBuffer = static_cast <COpenGLIndiceBuffer*>(_pkBuffer);
    if (!_bInit) {
        if (pkIndicesBuffer && (pkIndicesBuffer->GetUsage() != GL_DYNAMIC_DRAW)) {
            if (!pkIndicesBuffer->SetUsage(GL_DYNAMIC_DRAW))
                return false;
        }
    }

    if (pkIndicesBuffer->GetIndicesType() != GL_UNSIGNED_INT) {
        return false;
    }

    size_t nVerticeCount = pkIndicesBuffer->GetSize() / pkIndicesBuffer->GetElementSize();
    if (nVerticeCount < rkIndices.size())
        return false;

    void* pkMappedBuffer = pkIndicesBuffer->Map();
    if (!pkMappedBuffer)
        return false;

    const size_t nCopyBytes = rkIndices.size() * pkIndicesBuffer->GetElementSize();
    memcpy(pkMappedBuffer, rkIndices.data(), nCopyBytes);

    _pkBuffer->UnMap();
    return true;
}

bool COpenGLMeshBuffer::UpdateBoundBox(IBound* _pkBound)
{
    CGLMBoundBox* pkBox = dynamic_cast <CGLMBoundBox*>(_pkBound);
    if (!pkBox)
        return false;
    std::vector <glm::vec3>& rkVertices = GetVertices();
    if (rkVertices.empty())
        return false;
    glm::vec3 kMax;
    glm::vec3 kMin;
    // init first point
    kMax = rkVertices[0];
    kMin = rkVertices[0];
    for (glm::vec3& rkPos : rkVertices) {
        kMax.x = glm::max(kMax.x, rkPos.x);
        kMax.y = glm::max(kMax.y, rkPos.y);
        kMax.z = glm::max(kMax.z, rkPos.z);
        kMin.x = glm::min(kMin.x, rkPos.x);
        kMin.y = glm::min(kMin.y, rkPos.y);
        kMin.z = glm::min(kMin.z, rkPos.z);
    }
    pkBox->SetBound(kMin, kMax);
    return true;
}