#include "ECSVulkanPCH.h"
#include "VulkanFrameBuffer.h"
#include "VulkanCommandBuffer.h"
#include "VulkanTexture.h"
#include "VulkanTools.h"

#define MAX_ATTATCHMENT 8

void CVulkanFrameBuffer::SFrameBufferInstance::OnRelease(class CVulkanRenderer& _rkRenderer)
{
    VkDevice kDevice = _rkRenderer.GetVkDevice();
    if (m_kFrameBuffer != VK_NULL_HANDLE) {
        vkDestroyFramebuffer(kDevice, m_kFrameBuffer, nullptr);
        m_kFrameBuffer = VK_NULL_HANDLE;
    }
    if (m_kRenderPass != VK_NULL_HANDLE) {
        vkDestroyRenderPass(kDevice, m_kRenderPass, nullptr);
        m_kRenderPass = VK_NULL_HANDLE;
    }
}
//---------------------------------------------------------------------------
// Vulkan FrameBuffer
//---------------------------------------------------------------------------
CVulkanFrameBuffer::CVulkanFrameBuffer(CVulkanRenderSystem& _rkSystem, unsigned int _nWidth, unsigned int _nHeight)
    :IFrameBuffer(_nWidth, _nHeight)
    ,m_rkSystem(_rkSystem)
    ,m_bInvalidedFrameBuffer(true)
    ,m_bInvalidedRenderPass(true)
    ,m_bBegin(false)
{
    memset(m_kClearValue, 0, sizeof(m_kClearValue));
}

CVulkanFrameBuffer::~CVulkanFrameBuffer()
{
    const unsigned int nSize = _ARRAYSIZE(m_spkFrameBuffers);
    for (unsigned int nIndex = 0; nIndex < nSize; nIndex++) {
        CVulkanRenderer* pkRenderer = dynamic_cast<CVulkanRenderer*>(m_rkSystem.GetRenderer());
        pkRenderer->ReleaseResource(m_spkFrameBuffers[nIndex]);
    }
}

void CVulkanFrameBuffer::SetRenderBufferColorAttactmentCount(int _nCount)
{
    if (m_kAttachmentDesc.size() <= _nCount) {
        const unsigned int nNewSize = _nCount + 1;
        const unsigned int nOldSize = m_kAttachmentDesc.size();
        m_kAttachmentDesc.resize(nNewSize);
        for (unsigned int nIndex = nOldSize; nIndex < nNewSize; nIndex++) {
            CVulkanFrameBuffer::AttachmentDesc& kDesc = m_kAttachmentDesc[nIndex];
            kDesc.m_kAttchment = vkTools::initializers::attachmentDescInfo(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
            kDesc.m_kReference.attachment = nIndex;
            kDesc.m_kReference.layout = VK_IMAGE_LAYOUT_UNDEFINED;
        }
        m_bInvalidedRenderPass = true;
        return;
    }
    m_kAttachmentDesc.resize(_nCount);
    m_bInvalidedRenderPass = true;
}

int CVulkanFrameBuffer::GetRenderBufferColorAttachmentCount()
{
    return m_kAttachmentDesc.size();
}

bool CVulkanFrameBuffer::SetTexture(std::shared_ptr<ITexture> _rspkTexture, int _nAttachment)
{
    if (m_kAttachmentDesc.size() <= _nAttachment)
        return false;

    AttachmentDesc& rkDesc = m_kAttachmentDesc[_nAttachment];

    if (!_rspkTexture)
        return false;
    CVulkanTexture* pkVkTexure = dynamic_cast <CVulkanTexture*>(_rspkTexture.get());
    if (!pkVkTexure)
        return false;

    switch (pkVkTexure->GetTextureFormat()) {
    case gli::FORMAT_D16_UNORM:
    case gli::FORMAT_D24_UNORM:
    case gli::FORMAT_D24S8_UNORM:
    case gli::FORMAT_D32_UFLOAT:
    case gli::FORMAT_D32_UFLOAT_S8_UNORM:
        return false;
    default:
        break;
    }

    pkVkTexure->SetLevel(1);
    pkVkTexure->SetTextureData(nullptr, pkVkTexure->GetWidth(), pkVkTexure->GetHeight(), 0, pkVkTexure->GetTextureFormat());
#if 0   // Custom for other
    pkVkTexure->SetFilterMode(eFT_Min, GL_LINEAR);
    pkVkTexure->SetFilterMode(eFT_Mag, GL_LINEAR);
    pkVkTexure->SetClampMode(eCT_U, GL_CLAMP_TO_EDGE);
    pkVkTexure->SetClampMode(eCT_V, GL_CLAMP_TO_EDGE);
#endif
    if (pkVkTexure->FlushTexture() == false) // flush Texture
        return false;

    rkDesc.m_kAttchment.format = CVulkanTexture::GetVulkanTextureFormat(pkVkTexure->GetTextureFormat());

    pkVkTexure->SetImageLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    rkDesc.m_kReference.layout = pkVkTexure->GetCurrentLayout();

    rkDesc.m_spkTexture = _rspkTexture;
    return true;
}

bool CVulkanFrameBuffer::SetDepthTexture(std::shared_ptr<ITexture> _rspkDepthTextue)
{
    if (_rspkDepthTextue == nullptr)
        return false;
    CVulkanTexture* pkVkTexure = dynamic_cast <CVulkanTexture*>(_rspkDepthTextue.get());
    if (!pkVkTexure)
        return false;
    switch (pkVkTexure->GetTextureFormat()) {
    case gli::FORMAT_D16_UNORM:
    case gli::FORMAT_D24_UNORM:
    case gli::FORMAT_D24S8_UNORM:
    case gli::FORMAT_D32_UFLOAT:
    case gli::FORMAT_D32_UFLOAT_S8_UNORM:
        break;
    default:
        return false;
    }

    pkVkTexure->SetLevel(1);
    pkVkTexure->SetTextureData(nullptr, pkVkTexure->GetWidth(), pkVkTexure->GetHeight(), 0, pkVkTexure->GetTextureFormat());
#if 0   // Custom for other
    pkVkTexure->SetFilterMode(eFT_Min, GL_LINEAR);
    pkVkTexure->SetFilterMode(eFT_Mag, GL_LINEAR);
    pkVkTexure->SetClampMode(eCT_U, GL_CLAMP_TO_EDGE);
    pkVkTexure->SetClampMode(eCT_V, GL_CLAMP_TO_EDGE);
#endif
    if (pkVkTexure->FlushTexture() == false) // flush Texture
        return false;

    m_kDepthStencilAttachmentDesc.m_kAttchment.format = CVulkanTexture::GetVulkanTextureFormat(pkVkTexure->GetTextureFormat());
    pkVkTexure->SetImageLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    m_kDepthStencilAttachmentDesc.m_kReference.layout = pkVkTexure->GetCurrentLayout();

    m_kDepthStencilAttachmentDesc.m_spkTexture = _rspkDepthTextue;
    return true;
}

ITexture* CVulkanFrameBuffer::GetTexture(int _nAttachment)
{
    if (m_kAttachmentDesc.size() <= _nAttachment)
        return nullptr;
    return m_kAttachmentDesc[_nAttachment].m_spkTexture.get();
}

ITexture* CVulkanFrameBuffer::GetDepthTexture()
{
    return m_kDepthStencilAttachmentDesc.m_spkTexture.get();
}

bool CVulkanFrameBuffer::Active()
{
    std::shared_ptr <SFrameBufferInstance> spkFrameBuffer = m_spkFrameBuffers[m_kOpKey.m_nKey];
    if (m_bInvalidedFrameBuffer) {
        CVulkanRenderer* pkRenderer = dynamic_cast<CVulkanRenderer*>(m_rkSystem.GetRenderer());
        VkDevice kDeivce = pkRenderer->GetVkDevice();
        if (spkFrameBuffer == nullptr) {
            spkFrameBuffer = std::make_shared<SFrameBufferInstance>();
            m_spkFrameBuffers[m_kOpKey.m_nKey] = spkFrameBuffer;
        }
        bool bInvalid = (spkFrameBuffer->m_nAttachmentCount != m_kAttachmentDesc.size());
        if (spkFrameBuffer->IsValid() == false || bInvalid) {
            if (spkFrameBuffer->IsValid()) {
                pkRenderer->ReleaseResource(m_spkFrameBuffers[m_kOpKey.m_nKey]);
                spkFrameBuffer = std::make_shared<SFrameBufferInstance>();
                m_spkFrameBuffers[m_kOpKey.m_nKey] = spkFrameBuffer;
            }
            static VkImageView kAttachmentViews[MAX_ATTATCHMENT] = {};
            static VkAttachmentReference kAttachmentRefs[MAX_ATTATCHMENT] = {};
            static VkAttachmentDescription kAttachmentDescs[MAX_ATTATCHMENT] = {};
            static VkSubpassDescription kSubPassDesc = {};
            unsigned int nMaxLayer = 0;
            VkRenderPassCreateInfo kInfo = vkTools::initializers::renderPassCreateInfo();
            kInfo.attachmentCount = (m_kAttachmentDesc.size() < MAX_ATTATCHMENT - 1) ? m_kAttachmentDesc.size() : MAX_ATTATCHMENT - 1;
            size_t nIndex = 0;
            for (auto& kDesc : m_kAttachmentDesc) {
                std::shared_ptr <CVulkanTexture> spkTexture = std::dynamic_pointer_cast<CVulkanTexture>(kDesc.m_spkTexture);
                if (spkTexture == nullptr)
                    continue;
                kAttachmentDescs[nIndex] = kDesc.m_kAttchment;
                kAttachmentRefs[nIndex] = kDesc.m_kReference;
                kAttachmentViews[nIndex] = spkTexture->GetImageView();
                unsigned int nCurrLayerCount = spkTexture->GetLayerCount();
                nMaxLayer = (nCurrLayerCount > nMaxLayer) ? nCurrLayerCount : nMaxLayer;
                nIndex++;
            }
            kSubPassDesc.colorAttachmentCount = nIndex;
            kSubPassDesc.pColorAttachments = kAttachmentRefs;

            if (m_kDepthStencilAttachmentDesc.m_spkTexture != nullptr) {
                std::shared_ptr <CVulkanTexture> spkTexture = std::dynamic_pointer_cast<CVulkanTexture>(m_kDepthStencilAttachmentDesc.m_spkTexture);
                if (spkTexture) {
                    kAttachmentDescs[nIndex] = m_kDepthStencilAttachmentDesc.m_kAttchment;
                    kAttachmentRefs[nIndex] = m_kDepthStencilAttachmentDesc.m_kReference;
                    kAttachmentViews[nIndex] = spkTexture->GetImageView();
                    unsigned int nCurrLayerCount = spkTexture->GetLayerCount();
                    nMaxLayer = (nCurrLayerCount > nMaxLayer) ? nCurrLayerCount : nMaxLayer;
                    kInfo.attachmentCount++;
                    kSubPassDesc.pDepthStencilAttachment = &kAttachmentRefs[nIndex];
                    nIndex++;
                }
            }

            VkSubpassDependency kDependencies[2] = {};

            kDependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
            kDependencies[0].dstSubpass = 0;
            kDependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
            kDependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            kDependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
            kDependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            kDependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

            kDependencies[1].srcSubpass = 0;
            kDependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
            kDependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            kDependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
            kDependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            kDependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
            kDependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

            kInfo.pAttachments = kAttachmentDescs;
            kInfo.pSubpasses = &kSubPassDesc;
            kInfo.subpassCount = 1;
            kInfo.pDependencies = kDependencies;
            kInfo.dependencyCount = 2;
            
            VkResult eResult = vkCreateRenderPass(kDeivce, &kInfo, nullptr, &spkFrameBuffer->m_kRenderPass);
            if (eResult != VK_SUCCESS)
                return false;

            VkFramebufferCreateInfo kFBCreateInfo = vkTools::initializers::framebufferCreateInfo();
            kFBCreateInfo.attachmentCount = nIndex;
            kFBCreateInfo.pAttachments = kAttachmentViews;
            kFBCreateInfo.renderPass = spkFrameBuffer->m_kRenderPass;
            kFBCreateInfo.width = m_nViewportWidth;
            kFBCreateInfo.height = m_nViewportHeight;
            kFBCreateInfo.layers = nMaxLayer;
            eResult = vkCreateFramebuffer(kDeivce, &kFBCreateInfo, nullptr, &spkFrameBuffer->m_kFrameBuffer);
            if (eResult != VK_SUCCESS)
                return false;
            spkFrameBuffer->m_nAttachmentCount = nIndex;
        }
        m_bInvalidedFrameBuffer = false;
    }
    return spkFrameBuffer->IsValid();
}

bool CVulkanFrameBuffer::Deactive()
{
    return true;
}

void CVulkanFrameBuffer::Resize(int _nWidth, int _nHeight)
{
    int nAttachmnet = 0;
    for (auto& rkDesc : m_kAttachmentDesc) {
        auto spkTexture = rkDesc.m_spkTexture;
        if (!spkTexture)
            continue;
        bool bResize = (spkTexture->GetWidth() != _nWidth);
        bResize |= (spkTexture->GetHeight() != _nHeight);
        if (!bResize)
            continue;
        spkTexture->SetTextureDimension(_nWidth, _nHeight, 1);
        SetTexture(spkTexture, nAttachmnet);
        nAttachmnet++;
    }
    if (m_kDepthStencilAttachmentDesc.m_spkTexture) {
        bool bResize = (m_kDepthStencilAttachmentDesc.m_spkTexture->GetWidth() != _nWidth);
        bResize |= (m_kDepthStencilAttachmentDesc.m_spkTexture->GetHeight() != _nHeight);
        if (bResize) {
            m_kDepthStencilAttachmentDesc.m_spkTexture->SetTextureDimension(_nWidth, _nHeight, 1);
            SetDepthTexture(m_kDepthStencilAttachmentDesc.m_spkTexture);
        }
    }
    m_nViewportWidth = _nWidth;
    m_nViewportHeight = _nHeight;
}

int CVulkanFrameBuffer::GetWidth(int _nAttachment)
{
    if (m_kAttachmentDesc.size() <= _nAttachment) {
        return 0;
    }

    AttachmentDesc& rkAttachmentDesc = m_kAttachmentDesc [_nAttachment];
    if (rkAttachmentDesc.m_spkTexture == nullptr)
        return 0;
    return rkAttachmentDesc.m_spkTexture->GetWidth();
}

int CVulkanFrameBuffer::GetHeight(int _nAttachment)
{
    if (m_kAttachmentDesc.size() <= _nAttachment) {
        return 0;
    }

    AttachmentDesc& rkAttachmentDesc = m_kAttachmentDesc [_nAttachment];
    if (rkAttachmentDesc.m_spkTexture == nullptr)
        return 0;
    return rkAttachmentDesc.m_spkTexture->GetHeight();
}

void CVulkanFrameBuffer::SetBufferClearBit(unsigned int _nBit)
{
    CVulkanFrameBuffer::OpIndex eOp = {};
    if (_nBit & IRenderer::eBCB_Color) {
        eOp.m_cLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    }
    else if (_nBit & (IRenderer::eBCB_Depth | IRenderer::eBCB_Stencil)) {
        eOp.m_cStencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    }
    
    m_bInvalidedFrameBuffer |= (m_kOpKey.m_nKey != eOp.m_nKey);
    m_kOpKey.m_nKey = eOp.m_nKey;
}

void CVulkanFrameBuffer::SetClearColor(VkClearColorValue& _rkValue)
{
    m_kClearValue[0].color = _rkValue;
}

void CVulkanFrameBuffer::SetClearDepth(VkClearDepthStencilValue& _rkValue)
{
    m_kClearValue[1].depthStencil = _rkValue;
}

bool CVulkanFrameBuffer::BeginRenderPass(CVulkanCommandBuffer& _rkCmdBuffer)
{
    if (m_bBegin)
        return true;
    if (_rkCmdBuffer.IsBegin() == false) {
        return false;
    }
    std::shared_ptr <CVulkanFrameBuffer::SFrameBufferInstance> spkFrameBuffer = m_spkFrameBuffers[m_kOpKey.m_nKey];
    if (spkFrameBuffer == nullptr)
        return false;
    if (spkFrameBuffer->IsValid() == false)
        return false;
    VkRenderPassBeginInfo kInfo = vkTools::initializers::renderPassBeginInfo();
    if (m_kOpKey.m_cLoadOp == VK_ATTACHMENT_LOAD_OP_CLEAR) {
        kInfo.pClearValues = m_kClearValue;
        kInfo.clearValueCount++;
    }
    if (m_kOpKey.m_cStencilLoadOp == VK_ATTACHMENT_LOAD_OP_CLEAR) {
        kInfo.pClearValues = (kInfo.pClearValues) ? kInfo.pClearValues : &(m_kClearValue[1]);
        kInfo.clearValueCount++;
    }
    kInfo.framebuffer = spkFrameBuffer->m_kFrameBuffer;
    kInfo.renderPass = spkFrameBuffer->m_kRenderPass;
    kInfo.renderArea.offset.x = 0;
    kInfo.renderArea.offset.y = 0;
    kInfo.renderArea.extent.width = m_nViewportWidth;
    kInfo.renderArea.extent.height = m_nViewportHeight;
    
    vkCmdBeginRenderPass(_rkCmdBuffer.GetCmdBufferInstance(), &kInfo, VK_SUBPASS_CONTENTS_INLINE);
    spkFrameBuffer->RefCommandBuffer(&_rkCmdBuffer);
    m_bBegin = true;
    return true;
}

void CVulkanFrameBuffer::EndRenderPass(CVulkanCommandBuffer& _rkCmdBuffer)
{
    if (m_bBegin == false)
        return;

    if (_rkCmdBuffer.IsBegin() == false)
        return;

    vkCmdEndRenderPass(_rkCmdBuffer.GetCmdBufferInstance());
}