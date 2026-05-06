#include "MainPCH.h"
#include "AssimpGLGeometryComp.h"
#include "IRenderComponent.h"
#include "RenderSystem.h"
#include "MaterialSystem.h"
#include "ITexture.h"
#include "GLMBoxBound.h"
#include "TextureDesc.h"

CAssimpGLGeometryComp::CAssimpGLGeometryComp(const aiScene* pkScene, size_t _nMeshIndex, bool _bBPR, const char* _pcBasePath)
{
    aiMesh* pkMesh = pkScene->mMeshes[_nMeshIndex];
    m_spkMesh = std::make_shared <CMeshData>(*this, *pkMesh);


    static char* pcTextureMap[] =
    {
        "uDiffuse",
        "uSpecular",
        "uAmbient",
        "uEmissive",
        "uHeight",
        "uNormals",
        "uShininess",
        "uOpacity",
        "uDispalacement",
        "uLightMap",
        "uReflection",
    };

    static char* pcPBRTextureMap[] =
    {
        "uDiffuse",
        "uSpecular",
        "uMetallic",
        "uEmissive",
        "uHeight",
        "uNormals",
        "uRoughness",
        "uOpacity",
        "uDispalacement",
        "uLightMap",
        "uReflection",
    };

    char** pcCurrentMap = (_bBPR) ? pcPBRTextureMap : pcTextureMap;

    if (pkScene->HasMaterials()) {
        const size_t nMaterialCount = pkScene->mNumMaterials;
        if (nMaterialCount <= m_spkMesh->MaterialIndex())
            return;
        m_kTextureDescripts.resize(1);
        aiMaterial* pkAssimpMat = pkScene->mMaterials[m_spkMesh->MaterialIndex ()];
        std::vector <ITextureDescPtr>& rkDescs = m_kTextureDescripts[0];
        for (int nMappingType = aiTextureType_DIFFUSE; nMappingType < aiTextureType_REFLECTION; nMappingType++) {
            aiString kPath;
            unsigned int nUVIndex = 0;
            ai_real nBlend = 0;
            aiTextureOp nTextureOp = aiTextureOp_Multiply;
            aiTextureMapMode nMapMode = aiTextureMapMode_Wrap;
            aiTextureMapping nMapping;
            const int nTextureCount = pkAssimpMat->GetTextureCount((aiTextureType)nMappingType);
            for (int nIndex = 0; nIndex < nTextureCount; nIndex++) {
                if (pkAssimpMat->GetTexture ((aiTextureType)nMappingType, nIndex, &kPath, &nMapping, &nUVIndex, &nBlend, &nTextureOp, &nMapMode) != AI_SUCCESS) {
                    continue;
                }
                if (!kPath.length)
                    continue;

                std::string kFinalPath;
                if (_pcBasePath) {
                    kFinalPath = _pcBasePath;
                    kFinalPath += "\\";
                }
                kFinalPath += kPath.C_Str();

                rkDescs.emplace_back(std::make_shared <CTextureDesc>(pcCurrentMap[nMappingType - 1], kFinalPath.c_str(), "dds", IRenderer::eTT_2D));
                break;
            }
        }
    }

    std::shared_ptr <IBound> spkOrgBound = m_spkMesh->CreateBoundBox();
    CGLMBoundBox* pkBound = static_cast<CGLMBoundBox*>(spkOrgBound.get());
    const glm::vec3& kCenter = pkBound->GetCenter ();
    glm::mat4 kXForm;
    kXForm[3] = glm::vec4(kCenter, 1.0);
    kXForm = glm::inverse(kXForm);
    m_spkMesh->XFormVertex(kXForm);
    m_kGeometryDatas.resize(1);
    m_kGeometryDatas[0].m_kBound = spkOrgBound;

    m_spkRenderState = IRenderSystem::CreateDefaultRenderState();
    for (auto& rkTextureDesc : m_kTextureDescripts[0]) {
        if (strstr(rkTextureDesc->GetName(), "uOpacity")) {
            m_spkRenderState->SetDepthOpMask(false);
            m_spkRenderState->SetDepthTestEnable(true);
            m_spkRenderState->SetBlendEnable(true);
            m_spkRenderState->SetAlphaTest(true);
        }
    }
}

CAssimpGLGeometryComp::~CAssimpGLGeometryComp()
{

}

size_t CAssimpGLGeometryComp::GetGeometryCount()
{
    size_t nCount = CGLMGeomertryComponent::GetGeometryCount();
    if (!nCount) {
        CreateVertexBuffer();
        CreateIndexBuffer();
        nCount = CGLMGeomertryComponent::GetGeometryCount();
    }
    return nCount;
}

IBuffer* CAssimpGLGeometryComp::GetVertexBuffer()
{
    if (m_kGeometryDatas.empty() || m_kGeometryDatas[0].m_kVertexBuffer == nullptr) {
        CreateVertexBuffer();
    }

    return m_kGeometryDatas[CurrentActiveGeometry()].m_kVertexBuffer.get();
}

IBuffer* CAssimpGLGeometryComp::GetIndiceBuffer()
{
    if (m_kGeometryDatas.empty() || m_kGeometryDatas[0].m_kIndexBuffer == nullptr) {
        CreateIndexBuffer();
    }

    return m_kGeometryDatas[CurrentActiveGeometry()].m_kIndexBuffer.get();
}

void CAssimpGLGeometryComp::CreateVertexBuffer()
{
    SGeomertryBuffer& rkBuffer = m_kGeometryDatas[0];
    if (rkBuffer.m_kVertexBuffer != nullptr)
        return;
    rkBuffer.m_kVertexBuffer = m_spkMesh->CreateVertexBuffer();
}

void CAssimpGLGeometryComp::CreateIndexBuffer()
{
    SGeomertryBuffer& rkBuffer = m_kGeometryDatas[0];
    if (rkBuffer.m_kIndexBuffer != nullptr)
        return;
    rkBuffer.m_kIndexBuffer = m_spkMesh->CreateIndexBuffer();
}

int CAssimpGLGeometryComp::GetPrimitiveType()
{
    SGeomertryBuffer& rkBuffer = m_kGeometryDatas[0];
    if (rkBuffer.m_ePrimitiveType == -1)
        rkBuffer.m_ePrimitiveType = m_spkMesh->PrimitiveType();
    return CGLMGeomertryComponent::GetPrimitiveType();
}

ITextureDesc* CAssimpGLGeometryComp::GetTextureDesc(size_t _nIndex)
{
    SGeomertryBuffer& rkBuffer = m_kGeometryDatas[0];
    if (rkBuffer.m_nTextureIndex == -1)
        rkBuffer.m_nTextureIndex = 0;
    return CGLMGeomertryComponent::GetTextureDesc(_nIndex);
}

bool CAssimpGLGeometryComp::ValidateRenderDataByName(IMatertialSystem& _rkSystem, const char* _pcName)
{
    ITextureDesc* pkDesc = GetTextureDescByName(_pcName);
    if (!pkDesc)
        return false;
    if (pkDesc->GetRenderData())
        return true;
    std::shared_ptr <ITexture> spkRenderData = _rkSystem.GetRenderData(pkDesc);
    if (!spkRenderData) {
        spkRenderData = _rkSystem.CreateRenderData(pkDesc);
    }
    pkDesc->SetRenderData(spkRenderData);
    return true;
}

size_t CAssimpGLGeometryComp::GetTextureDescCount()
{
    SGeomertryBuffer& rkBuffer = m_kGeometryDatas[0];
    if (rkBuffer.m_nTextureIndex == -1)
        rkBuffer.m_nTextureIndex = 0;
    return CGLMGeomertryComponent::GetTextureDescCount();
}

IBound* CAssimpGLGeometryComp::GetBound()
{
    SGeomertryBuffer& rkBuffer = m_kGeometryDatas[0];
    if (rkBuffer.m_kBound == nullptr) {
        rkBuffer.m_kBound = m_spkMesh->CreateBoundBox();
    }
    return rkBuffer.m_kBound.get();
}