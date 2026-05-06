#include "ECSVulkanPCH.h"
#include "VulkanSampler.h"
#include "VulkanRenderSystem.h"


CVulkanSampler::CVulkanSampler(CVulkanRenderer& _rkRenderer)
    :m_rkRenderer(_rkRenderer)
{
    const unsigned int nCount = sizeof(SamplerKey);
    for (unsigned int nIndex = 0; nIndex < nCount; nIndex++) {
        m_kSamplers [nIndex] = VK_NULL_HANDLE;
    }
}

CVulkanSampler::~CVulkanSampler()
{
    const unsigned int nCount = sizeof(SamplerKey);
    for (unsigned int nIndex = 0; nIndex < nCount; nIndex++) {
        vkDestroySampler(m_rkRenderer.GetVkDevice(), m_kSamplers[nIndex], nullptr);
    }
}

bool CVulkanSampler::ValidateSampler()
{
    if (m_kSamplers[m_nCurrentKey] != VK_NULL_HANDLE)
        return true;

    SamplerKey kKey;
    kKey.m_nKey = m_nCurrentKey;
    VkSamplerCreateInfo kInfo = vkTools::initializers::samplerCreateInfo();
    kInfo.minFilter = (VkFilter)kKey.m_nMinFilter;
    kInfo.magFilter = (VkFilter)kKey.m_nMagFilter;
    kInfo.mipmapMode = (VkSamplerMipmapMode)kKey.m_nMipFilter;
    kInfo.addressModeW = (VkSamplerAddressMode)kKey.m_nWAddressMode;
    kInfo.addressModeV = (VkSamplerAddressMode)kKey.m_nVAddressMode;
    kInfo.addressModeU = (VkSamplerAddressMode)kKey.m_nUAddressMode;
    VkResult eResult = vkCreateSampler(m_rkRenderer.GetVkDevice(), &kInfo, nullptr, &(m_kSamplers[m_nCurrentKey]));
    return (eResult == VK_SUCCESS);
}

void CVulkanSampler::SetSamplerDesc(const VkSamplerCreateInfo& _kInfo)
{
    SamplerKey kKey;
    kKey.m_nMinFilter = _kInfo.minFilter;
    kKey.m_nMagFilter = _kInfo.magFilter;
    kKey.m_nMipFilter = _kInfo.mipmapMode;
    kKey.m_nWAddressMode = _kInfo.addressModeW;
    kKey.m_nVAddressMode = _kInfo.addressModeV;
    kKey.m_nUAddressMode = _kInfo.addressModeU;
    m_nCurrentKey = kKey.m_nKey;
}