#include "ECSFrameworkPCH.h"
#include "RenderComponent.h"
#include "IMaterialComponent.h"

CRenderComponent::CRenderComponent()
    :m_pkVertexBuffer(nullptr)
    ,m_pkIndexBuffer(nullptr)
    ,m_pkMaterial(nullptr)
    ,m_bRenderable(false)
{

}

CRenderComponent::~CRenderComponent()
{

}

bool CRenderComponent::SetMaterial(IMaterial* _pkMaterial)
{
    m_pkMaterial = _pkMaterial;
    return true;
}

bool CRenderComponent::SetAttribute(IBuffer* _pkVertexBuffer, IBuffer* _pkIndicesBuffer)
{
    m_pkVertexBuffer = _pkVertexBuffer;
    m_pkIndexBuffer = _pkIndicesBuffer;
    return true;
}

bool CRenderComponent::SetRenderState(IRenderState* _pkRenderState)
{
    return true;
}

IShader* CRenderComponent::GetShader(int _eShaderType) const
{
    if (m_pkMaterial == nullptr)
        return nullptr;
    return m_pkMaterial->GetShader(_eShaderType);
}

IBuffer* CRenderComponent::GetVertexBuffer() const
{
    return m_pkVertexBuffer;
}

IBuffer* CRenderComponent::GetIndiceBuffer() const
{
    return m_pkIndexBuffer;
}

const IRenderState* CRenderComponent::GetRenderState() const
{
    return nullptr;
}

ITexture* CRenderComponent::GetTexture(size_t _nIndex) const
{
    if (m_pkMaterial == nullptr)
        return nullptr;

    return m_pkMaterial->GetTexture(_nIndex);
}

IConstantSemantic* CRenderComponent::GetConstant(size_t _nIndex)
{
    if (m_pkMaterial == nullptr)
        return nullptr;

    return m_pkMaterial->GetConstant(_nIndex);
}

IConstantSemantic* CRenderComponent::GetConstantByName(const char* _pcName)
{
    if (m_pkMaterial == nullptr)
        return nullptr;

    if (!m_pkMaterial)
        return nullptr;
    if (!_pcName)
        return nullptr;

    const size_t nCount = m_pkMaterial->GetConstantCount();
    for (size_t nIndex = 0; nIndex < nCount; nIndex++) {
        IConstantSemantic* pkConstant = m_pkMaterial->GetConstant(nIndex);
        if (!pkConstant)
            continue;
        if (strstr(pkConstant->GetVaribleName(), _pcName)) {
            return pkConstant;
        }
    }

    return nullptr;
}

size_t CRenderComponent::GetConstantCount()
{
    if (m_pkMaterial == nullptr)
        return 0;
    return m_pkMaterial->GetConstantCount();
}

bool CRenderComponent::ValidatePipeline(class IRenderer&)
{
    return true;
}