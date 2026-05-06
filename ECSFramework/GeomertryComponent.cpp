#include "ECSFrameworkPCH.h"
#include "GeomertryComponent.h"
#include "MaterialSystem.h"
#include "IBound.h"
#include "RenderSystem.h"

CGeomertryComponent::CGeomertryComponent()
    : m_nActiveIndex(-1)
    , m_bIsCastLight(true)
{
}

CGeomertryComponent::~CGeomertryComponent()
{

}

void CGeomertryComponent::ActiveGeometry (size_t _nIndex)
{
    m_nActiveIndex = _nIndex;
    if (m_spkRenderState) {
        m_spkRenderState->SetPrimitiveTopology((EPrimitiveType)GetPrimitiveType());
    }
}

size_t CGeomertryComponent::GetGeometryCount()
{
    return m_kGeometryDatas.size();
}

size_t CGeomertryComponent::CurrentActiveGeometry()
{
    return m_nActiveIndex;
}

IBuffer* CGeomertryComponent::GetVertexBuffer()
{
    if (m_nActiveIndex == -1)
        return nullptr;
    return m_kGeometryDatas[m_nActiveIndex].m_kVertexBuffer.get();
}

IBuffer* CGeomertryComponent::GetIndiceBuffer()
{
    if (m_nActiveIndex == -1)
        return nullptr;
    return m_kGeometryDatas[m_nActiveIndex].m_kIndexBuffer.get();
}

IRenderState* CGeomertryComponent::GetRenderState()
{
    return m_spkRenderState.get();
}

ITextureDesc* CGeomertryComponent::GetTextureDesc(size_t _nIndex)
{
    if (m_nActiveIndex == -1)
        return nullptr;
    SGeomertryBuffer& rkBuffer = m_kGeometryDatas[m_nActiveIndex];
    if (rkBuffer.m_nTextureIndex == -1)
        return nullptr;
    std::vector <ITextureDescPtr>& kDesc = m_kTextureDescripts[rkBuffer.m_nTextureIndex];
    if (kDesc.size() <= _nIndex)
        return nullptr;
    return kDesc[_nIndex].get();
}

ITextureDesc* CGeomertryComponent::GetTextureDescByName(const char* _pcName)
{
    if (m_nActiveIndex == -1)
        return nullptr;
    SGeomertryBuffer& rkBuffer = m_kGeometryDatas[m_nActiveIndex];
    if (rkBuffer.m_nTextureIndex == -1)
        return nullptr;
    std::vector <ITextureDescPtr>& kDescs = m_kTextureDescripts[rkBuffer.m_nTextureIndex];
    for (auto& rkDesc : kDescs) {
        if (strstr(rkDesc->GetName(), _pcName)) {
            return rkDesc.get();
        }
    }
    return nullptr;
}

bool CGeomertryComponent::ValidateRenderDataByName(IMatertialSystem& _rkSystem, const char* _pcName)
{
    ITextureDesc* pkDesc = GetTextureDescByName(_pcName);
    if (!pkDesc)
        return false;
    if (pkDesc->GetRenderData())
        return true;
    std::shared_ptr <ITexture> spkRenderData = _rkSystem.GetRenderData(pkDesc);
    if (!spkRenderData) {
        spkRenderData = _rkSystem.CreateRenderData(pkDesc, true);
    }
    pkDesc->SetRenderData(spkRenderData);
    return true;
}

size_t CGeomertryComponent::GetTextureDescCount()
{
    if (m_nActiveIndex == -1)
        return 0;
    if (m_kTextureDescripts.empty())
        return 0;
    if (m_kGeometryDatas.empty())
        return 0;
    return m_kTextureDescripts[m_kGeometryDatas[m_nActiveIndex].m_nTextureIndex].size();
}

int CGeomertryComponent::GetPrimitiveType()
{
    if (m_nActiveIndex == -1)
        return 0;
    if (m_kGeometryDatas.empty())
        return 0;
    return m_kGeometryDatas[m_nActiveIndex].m_ePrimitiveType;
}

void CGeomertryComponent::SetCastLighting(bool _bEnable)
{
    m_bIsCastLight = _bEnable;
}

bool CGeomertryComponent::IsCastLighting()
{
    return m_bIsCastLight;
}

void CGeomertryComponent::UpdateWorldBound(class ISceneComponent& _rkComp)
{
    if (m_kGeometryDatas.empty())
        return;

    for (auto& rkBuffer : m_kGeometryDatas) {
        IBound* pkBoundBox = rkBuffer.m_kBound.get();
        if (!pkBoundBox)
            continue;
        pkBoundBox->UpdateWorldTransform(_rkComp);
    }
}

IBound* CGeomertryComponent::GetBound()
{
    if (m_nActiveIndex == -1)
        return nullptr;
    if (m_kGeometryDatas.empty())
        return nullptr;
    return m_kGeometryDatas[m_nActiveIndex].m_kBound.get();
}

void CGeomertryComponent::PurgeBuffer(size_t _nIndex)
{
    if (m_kGeometryDatas.size() <= _nIndex)
        return;

    SGeomertryBuffer& rkBuffer = m_kGeometryDatas [_nIndex];
    rkBuffer.m_kIndexBuffer = nullptr;
    rkBuffer.m_kVertexBuffer = nullptr;
}