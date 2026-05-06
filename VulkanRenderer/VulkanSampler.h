#pragma once


class CVulkanRenderSystem;
class CVulkanSampler
{
    union SamplerKey
    {
        unsigned short m_nKey;
        struct
        {
            unsigned short m_nMinFilter : 1;
            unsigned short m_nMagFilter : 1;
            unsigned short m_nMipFilter : 1;
            unsigned short m_nWAddressMode : 3;
            unsigned short m_nVAddressMode : 3;
            unsigned short m_nUAddressMode : 3;
        };
    };
    // this may be waste size
    VkSampler m_kSamplers[sizeof(SamplerKey)];
    class CVulkanRenderer& m_rkRenderer;
    unsigned short m_nCurrentKey;

public:

    CVulkanSampler(CVulkanRenderer& _rkRenderer);

    virtual ~CVulkanSampler();

    bool ValidateSampler();

    inline VkSampler GetSamplerInstance()
    {
        return m_kSamplers[m_nCurrentKey];
    }

    void SetSamplerDesc(const VkSamplerCreateInfo& _kInfo);
};