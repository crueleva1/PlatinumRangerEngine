#pragma once


#include "GLITexture.h"
#include "VulkanSampler.h"
#include "VulkanResource.h"

class CVulkanRenderSystem;
class CVulkanTexture 
    : public CGLITexture
{
    struct SVulkanPixelData
        : public ITexture::IPixelData
        , public CVulkanCmdBufferResource
    {
        CVulkanTexture& m_rkOwner;
        VkBuffer m_kStageImageBuffer;
        VkDeviceMemory m_kMem;
        bool m_bDirty;

        SVulkanPixelData(CVulkanTexture& _rkOwner, short _nLevel);

        virtual ~SVulkanPixelData();

        virtual void Allocate(size_t _nSize);

        virtual void SetData(const char* _pcData, size_t _nSize);

        virtual char* GetData();

        virtual size_t GetSize();

        virtual void ClearData();

        void OnRelease(class CVulkanRenderer& _rkRenderer);

        bool CopyBufferToImage(int nBaseLayer);
    };

    struct SVulkanTextureInstance
        : public CVulkanCmdBufferResource
    {
        VkDescriptorImageInfo m_kImageDesc;
        VkImage m_kMainImage;
        VkDeviceMemory m_kMem;
        bool m_bExtenal;
        
        SVulkanTextureInstance(VkImage _kImage);
        SVulkanTextureInstance();
        void OnRelease(class CVulkanRenderer& _rkRenderer);
        inline bool IsCreated()
        {
            if (m_kMainImage == VK_NULL_HANDLE)
                return false;
            if (m_kMem == VK_NULL_HANDLE)
                return false;
            if (m_kImageDesc.imageView == VK_NULL_HANDLE)
                return false;
            return true;
        }
    };
    CVulkanRenderSystem& m_rkSystem;
    std::shared_ptr <SVulkanTextureInstance> m_spkInstance;
    VkImageViewType m_eType;
    VkFormat m_eFormat;
    VkImageAspectFlags m_eAspectFlag;
    bool m_bDirty;

    inline CVulkanRenderSystem& GetRenderer()
    {
        return m_rkSystem;
    }

    virtual std::shared_ptr <ITexture::IPixelData> AllocatePixelData(short _nLevel)
    {
        return std::make_shared <CVulkanTexture::SVulkanPixelData>(*this, _nLevel);
    }

    VkImageUsageFlags GetAttachmentUsageBit();

    VkImageLayout GetAttachmentImageLayoutType(bool _bRead);
public:
    CVulkanTexture(CVulkanRenderSystem& _rkSystem, unsigned int _nType);

    virtual ~CVulkanTexture();

    virtual bool Active(int _nStage);

    virtual bool BindTexture();

    virtual bool FlushTexture();

    virtual bool SetTextureData(const char* _pcData, unsigned int _nWidth, unsigned int _nHeight, unsigned int _nDepth, int _eTextureFormat, int _nLevel = 0);

    virtual bool SetTextureDimension(unsigned int _nWidth, unsigned int _nHeight, unsigned int _nDepth, int _nLevel = 0);

    static VkFormat GetVulkanTextureFormat(int _eFormat);

    static int GetTextureFormatByVkFormat(VkFormat _eFormat);

    virtual char* GetTextureData(int _nLevel);

    virtual void SetLevel(int _nLevel);

    virtual void SetTextureFormat(int _eTextureFormat);

    inline VkImageLayout GetCurrentLayout()
    {
        if (m_spkInstance == nullptr)
            return VK_IMAGE_LAYOUT_UNDEFINED;
        return m_spkInstance->m_kImageDesc.imageLayout;
    }

    void SetImageLayout(VkImageLayout eLayout, CVulkanCommandBuffer* pkCmdBuffer = nullptr);

    inline VkImage GetImage()
    {
        if (m_spkInstance == nullptr)
            return VK_NULL_HANDLE;
        return m_spkInstance->m_kMainImage;
    }

    inline VkImageView GetImageView()
    {
        if (m_spkInstance == nullptr)
            return VK_NULL_HANDLE;
        return m_spkInstance->m_kImageDesc.imageView;
    }

    inline unsigned int GetAspectFlag()
    {
        return m_eAspectFlag;
    }

    inline unsigned int GetLayerCount()
    {
        return (m_eType == VK_IMAGE_VIEW_TYPE_CUBE) ? 6 : 1;
    }

    VkImageType GetImageType();

    VkSamplerAddressMode GetVkClampMode(EClampMode _eMode);

    bool GetVkFilterMode(EFilterMode _eMode, VkFilter& _eFilter, VkSamplerMipmapMode& _eMipmapMode);

    void SetExtenal(VkImage _kImage);

};