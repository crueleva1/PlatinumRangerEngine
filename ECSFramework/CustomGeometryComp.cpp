#include "ECSFrameworkPCH.h"
#include "CustomGeometryComp.h"
#include "RenderSystem.h"

CCustomGeometryComp::CCustomGeometryComp(std::shared_ptr <IMeshBuffer> _rpkMesh, ECustomModelType)
    : m_spkMesh(_rpkMesh)
{
    m_spkRenderState = IRenderSystem::CreateDefaultRenderState();
    m_kGeometryDatas.resize(1);
    std::shared_ptr <IBound> spkOrgBound = m_spkMesh->CreateBoundBox();
    m_kGeometryDatas[0].m_kBound = spkOrgBound;
    m_nActiveIndex = 0;
}

CCustomGeometryComp::~CCustomGeometryComp()
{

}

void CCustomGeometryComp::ActiveGeometry(size_t _nIndex)
{
    m_nActiveIndex = 0;
}

size_t CCustomGeometryComp::GetGeometryCount()
{
    return 1;
}

size_t CCustomGeometryComp::CurrentActiveGeometry()
{
    return 0;
}

IBuffer* CCustomGeometryComp::GetVertexBuffer()
{
    if (m_kGeometryDatas.empty() || m_kGeometryDatas[0].m_kVertexBuffer == nullptr) {
        m_kGeometryDatas [0].m_kVertexBuffer = m_spkMesh->CreateVertexBuffer();
    }

    return m_kGeometryDatas[0].m_kVertexBuffer.get();
}

IBuffer* CCustomGeometryComp::GetIndiceBuffer()
{
    if (m_kGeometryDatas.empty() || m_kGeometryDatas[0].m_kIndexBuffer == nullptr) {
        m_kGeometryDatas [0].m_kIndexBuffer =  m_spkMesh->CreateIndexBuffer();
    }

    return m_kGeometryDatas[0].m_kIndexBuffer.get();
}

IRenderState* CCustomGeometryComp::GetRenderState()
{
    return m_spkRenderState.get();
}

struct ITextureDesc* CCustomGeometryComp::GetTextureDesc(size_t _nIndex)
{
    return nullptr;
}

size_t CCustomGeometryComp::GetTextureDescCount()
{
    return 0;
}

int CCustomGeometryComp::GetPrimitiveType()
{
    SGeomertryBuffer& rkBuffer = m_kGeometryDatas [0];
    if (rkBuffer.m_ePrimitiveType == -1)
        rkBuffer.m_ePrimitiveType = m_spkMesh->PrimitiveType();
    return CGeomertryComponent::GetPrimitiveType();
}

IBound* CCustomGeometryComp::GetBound()
{
    return CGeomertryComponent::GetBound();
}

void CCustomGeometryComp::PurgeBuffer(size_t _nIndex)
{
    CGeomertryComponent::PurgeBuffer(_nIndex);
    if (m_spkMesh) {
        m_spkMesh->CleanMeshBuffer();
    }
}