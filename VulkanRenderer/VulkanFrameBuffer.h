#pragma once

#include "VulkanRenderSystem.h"
#include "VulkanResource.h"



//---------------------------------------------------------------------------
// Vulkan FrameBuffer
//---------------------------------------------------------------------------
class CVulkanFrameBuffer 
    : public IFrameBuffer
{
    struct AttachmentDesc
    {
        VkAttachmentReference m_kReference;
        VkAttachmentDescription m_kAttchment;
        std::shared_ptr<ITexture> m_spkTexture;
    };
    std::vector <AttachmentDesc> m_kAttachmentDesc;
    AttachmentDesc m_kDepthStencilAttachmentDesc;

    union OpIndex
    {
        unsigned char m_nKey;
        struct
        {
            unsigned char m_cLoadOp  : 2;
            unsigned char m_cStoreOp : 1;
            unsigned char m_cStencilLoadOp : 1;
            unsigned char m_cStencilStoreOp : 2;
        };
    };

    struct SFrameBufferInstance : public CVulkanCmdBufferResource
    {
        VkFramebuffer m_kFrameBuffer;
        VkRenderPass m_kRenderPass;
        unsigned int m_nAttachmentCount;
        
        SFrameBufferInstance()
            :m_kFrameBuffer(VK_NULL_HANDLE)
            ,m_kRenderPass(VK_NULL_HANDLE)
            ,m_nAttachmentCount(0)
        {}
        
        inline bool IsValid()
        {
            if (m_kRenderPass == VK_NULL_HANDLE)
                return false;
            if (m_kFrameBuffer == VK_NULL_HANDLE)
                return false;
            return true;
        }

        void OnRelease(class CVulkanRenderer&);
    };

    std::shared_ptr <SFrameBufferInstance> m_spkFrameBuffers[sizeof(OpIndex) << 6];

    CVulkanRenderSystem& m_rkSystem;
    OpIndex m_kOpKey;
    VkClearValue m_kClearValue[2];
    bool m_bInvalidedFrameBuffer;
    bool m_bInvalidedRenderPass;
    bool m_bBegin;
public:
    CVulkanFrameBuffer(CVulkanRenderSystem& _rkRenderSystem, unsigned int _nWidth, unsigned int _nHeight);

    virtual ~CVulkanFrameBuffer();

    virtual void SetRenderBufferColorAttactmentCount(int _nCount);

    virtual int GetRenderBufferColorAttachmentCount();

    virtual bool SetTexture(std::shared_ptr<ITexture> _rspkTexture, int _nAttachment);

    virtual bool SetDepthTexture(std::shared_ptr<ITexture> _rspkDepthTextue);

    virtual ITexture* GetTexture(int _nAttachment);

    virtual ITexture* GetDepthTexture();

    virtual bool Active();

    virtual bool Deactive();

    virtual void Resize(int _nWidth, int _nHeight);

    virtual int GetWidth(int _nAttachment);

    virtual int GetHeight(int _nAttachment);

    virtual void SetBufferClearBit(unsigned int _nBit);

    void SetClearColor(VkClearColorValue& _rkValue);

    void SetClearDepth(VkClearDepthStencilValue& _rkValue);

    bool BeginRenderPass(CVulkanCommandBuffer& _rkCmdBuffer);

    void EndRenderPass(CVulkanCommandBuffer& _rkCmdBuffer);
};