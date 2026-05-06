#include "ECSVulkanPCH.h"
#include "VulkanRenderComponent.h"
#include "VulkanMaterial.h"
#include "VulkanBuffer.h"
#include "VulkanTexture.h"

VkFormat CovToVulkanType(EVariableSemanticElementType _eType, size_t _nElement)
{
    VkFormat eFormat = VK_FORMAT_UNDEFINED;
    switch (_eType)
    {
    default:
    case eVSET_None:
        break;
    case eVSET_bool:
    {
        switch (_nElement)
        {
        case 1:
        default:
            eFormat = VK_FORMAT_R32_UINT;
            break;
        case 2:
            eFormat = VK_FORMAT_R32G32_UINT;
            break;
        case 3:
            eFormat = VK_FORMAT_R32G32B32_UINT;
            break;
        case 4:
            eFormat = VK_FORMAT_R32G32B32A32_UINT;
            break;
        }
        break;
    }
    case eVSET_short:
    {
        switch (_nElement)
        {
        case 1:
        default:
            eFormat = VK_FORMAT_R16_SINT;
            break;
        case 2:
            eFormat = VK_FORMAT_R16G16_SINT;
            break;
        case 3:
            eFormat = VK_FORMAT_R16G16B16_SINT;
            break;
        case 4:
            eFormat = VK_FORMAT_R16G16B16A16_SINT;
            break;
        }
        break;
    }
    case eVSET_ushort:
    {
        switch (_nElement)
        {
        case 1:
        default:
            eFormat = VK_FORMAT_R16_UINT;
            break;
        case 2:
            eFormat = VK_FORMAT_R16G16_UINT;
            break;
        case 3:
            eFormat = VK_FORMAT_R16G16B16_UINT;
            break;
        case 4:
            eFormat = VK_FORMAT_R16G16B16A16_UINT;
            break;
        }
        break;
    }
    case eVSET_float:
    {
        switch (_nElement)
        {
        case 1:
        default:
            eFormat = VK_FORMAT_R32_SFLOAT;
            break;
        case 2:
            eFormat = VK_FORMAT_R32G32_SFLOAT;
            break;
        case 3:
            eFormat = VK_FORMAT_R32G32B32_SFLOAT;
            break;
        case 4:
            eFormat = VK_FORMAT_R32G32B32A32_SFLOAT;
            break;
        }
        break;
    }
    case eVSET_int:
    {
        switch (_nElement)
        {
        case 1:
        default:
            eFormat = VK_FORMAT_R32_SINT;
            break;
        case 2:
            eFormat = VK_FORMAT_R32G32_SINT;
            break;
        case 3:
            eFormat = VK_FORMAT_R32G32B32_SINT;
            break;
        case 4:
            eFormat = VK_FORMAT_R32G32B32A32_SINT;
            break;
        }
        break;
    }
    }
    return eFormat;
}

CVulkanRenderComponent::CVulkanRenderComponent(CVulkanRenderSystem& _rkSystem)
    :CRenderComponent()
    ,m_rkSystem(_rkSystem)
{

}

CVulkanRenderComponent::~CVulkanRenderComponent()
{

}

bool CVulkanRenderComponent::SetRenderState(IRenderState* _pkRenderState)
{
    if (_pkRenderState == nullptr)
        return false;

    CVulkanMaterial* pkMaterial = dynamic_cast<CVulkanMaterial*>(m_pkMaterial);
    if (pkMaterial == nullptr)
        return false;

    return pkMaterial->SetRenderState(_pkRenderState);
}

const IRenderState* CVulkanRenderComponent::GetRenderState() const
{
    CVulkanMaterial* pkMaterial = dynamic_cast <CVulkanMaterial*>(m_pkMaterial);
    if (pkMaterial== nullptr)
        return false;

    const CVulkanRenderState& rkRenderState = pkMaterial->GetPipelineState();
    return &rkRenderState;
}

bool CVulkanRenderComponent::ValidatePipeline(IRenderSystem& _rkSystem)
{
    if (m_pkMaterial == nullptr)
        return false;

    CVulkanMaterial* pkVulkanMat = dynamic_cast <CVulkanMaterial*>(m_pkMaterial);
    if (!pkVulkanMat)
        return false;

    if (!m_pkMaterial->LinkShader(m_pkVertexBuffer))
        return false;

    if (!m_pkVertexBuffer) {
        return false;
    }
    // SetBuffer
    if (!m_pkVertexBuffer->Bind()) {
        return false;
    }

    CVulkanVertexBuffer* pkVertexBuffer = static_cast<CVulkanVertexBuffer*>(m_pkVertexBuffer);
    if (!pkVertexBuffer->SetVertexBuffer(pkVulkanMat)) {
        return false;
    }
    if (m_pkIndexBuffer) {
        if (!m_pkIndexBuffer->Bind())
            return false;
    }

    // Update Constant
    int nTextureStage = 0;
    const size_t nConstantCount = m_pkMaterial->GetConstantCount();
    for (size_t nIndex = 0; nIndex < nConstantCount; nIndex++) {
        IConstantSemantic* pkSemantic = m_pkMaterial->GetConstant(nIndex);
        if (!pkSemantic)
            continue;

        if (!pkSemantic->isDirty()) {
            continue;
        }
        //GLint nLoc = glGetUniformLocation(pkOpenGLMat->GetProgram(), pkSemantic->GetVaribleName());
        unsigned int nSet = -1;
        unsigned int nBinding = -1;
        IVulkanUniformInterface* pkILoc = dynamic_cast <IVulkanUniformInterface*>(pkSemantic);
        if (pkILoc == nullptr)
            continue;
        nSet = pkILoc->GetDescriptorSetID();
        nBinding = pkILoc->GetDescriptorSetBinding();
        EConstantType eType = pkSemantic->GetType();
        switch (eType) {
        case eConstant_bool:
        {
            
            break;
        }
        case eConstant_short:
        case eConstant_int:
            
            break;
        case eConstant_ushort:
        case eConstant_uint:
            
            break;
        case eConstant_float:
            
            break;
        case eConstant_floatVec2:
            
            break;
        case eConstant_floatVec3:
            
            break;
        case eConstant_floatVec4:
            
            break;
        case eConstant_intVec2:
            
            break;
        case eConstant_intVec3:
            
            break;
        case eConstant_intVec4:
            
            break;
        case eConstant_Matrix2x2:
            
            break;
        case eConstant_Matrix3x3:
            
            break;
        case eConstant_Matrix4x4:
            
            break;
        case eConstant_Texture:
        {
            TConstantSemantic <ITexture*>* pkTextureSemantic = dynamic_cast <TConstantSemantic <ITexture*>*>(pkSemantic);
            if (!pkTextureSemantic)
                continue;
            ITexture* pkTexture = pkTextureSemantic->GetValue();
            if (!pkTexture) {

                continue;
            }
            if (!pkTexture->Active(nTextureStage))
                continue;
            if (!pkTexture->BindTexture())
                continue;
            nTextureStage++;
            break;
        }
        default:
        {
            continue;
        }
        }
    }
    return true;
}
