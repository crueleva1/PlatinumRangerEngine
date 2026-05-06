#include "ECSVulkanPCH.h"
#include "VulkanTexture.h"
#include "VulkanRenderSystem.h"
#include "VulkanCommandBuffer.h"
#include "VulkanSampler.h"

CVulkanTexture::SVulkanPixelData::SVulkanPixelData(CVulkanTexture& _rkOwner, short _nLevel)
    : ITexture::IPixelData(_nLevel)
    , m_rkOwner(_rkOwner)
    , m_kStageImageBuffer(VK_NULL_HANDLE)
    , m_kMem(VK_NULL_HANDLE)
{
}

CVulkanTexture::SVulkanPixelData::~SVulkanPixelData()
{
}

void CVulkanTexture::SVulkanPixelData::Allocate(size_t _nSize)
{
    CVulkanRenderSystem& rkSystem = m_rkOwner.GetRenderer();
    CVulkanRenderer* pkRenderer = dynamic_cast<CVulkanRenderer*>(rkSystem.GetRenderer());
    VkDevice kDevice = pkRenderer->GetVkDevice();
    const VkPhysicalDeviceMemoryProperties& kMemProp = pkRenderer->GetDeviceMemoryProp();
    if (m_kStageImageBuffer == VK_NULL_HANDLE) {
        VkBufferCreateInfo kBufferCreateInfo = vkTools::initializers::bufferCreateInfo(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, _nSize);
        kBufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        verify(vkCreateBuffer(kDevice, &kBufferCreateInfo, NULL, &m_kStageImageBuffer));
    }
    if (m_kMem == VK_NULL_HANDLE) {
        VkMemoryAllocateInfo kMemAllocInfo = vkTools::initializers::memoryAllocateInfo();
        VkMemoryRequirements kMemReq;
        vkGetBufferMemoryRequirements(kDevice, m_kStageImageBuffer, &kMemReq);
        kMemAllocInfo.allocationSize = kMemReq.size;
        m_nSize = kMemReq.size;

        vkTools::getMemoryType(kMemProp, kMemReq.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &kMemAllocInfo.memoryTypeIndex);
        verify(vkAllocateMemory(kDevice, &kMemAllocInfo, NULL, &m_kMem) == VK_SUCCESS);
        verify(vkBindBufferMemory(kDevice, m_kStageImageBuffer, m_kMem, 0) == VK_SUCCESS);
    }
}

void CVulkanTexture::SVulkanPixelData::SetData(const char* _pcData, size_t _nSize)
{
    CVulkanRenderSystem& rkSystem = m_rkOwner.GetRenderer();
    CVulkanRenderer* pkRenderer = dynamic_cast<CVulkanRenderer*>(rkSystem.GetRenderer());
    VkDevice kDevice = pkRenderer->GetVkDevice();
    Allocate(_nSize);
    if (m_kMem != VK_NULL_HANDLE && _pcData != nullptr) {
        char* pcData = NULL;
        verify(vkMapMemory(kDevice, m_kMem, 0, m_nSize, 0, (void**)&pcData) == VK_SUCCESS);
        memcpy(pcData, _pcData, m_nSize);
        vkUnmapMemory(kDevice, m_kMem);
        m_bDirty = true;
    }
}

char* CVulkanTexture::SVulkanPixelData::GetData()
{
    // not editable texture so return nullptr
    return nullptr;
}

size_t CVulkanTexture::SVulkanPixelData::GetSize()
{
    return m_nSize;
}

void CVulkanTexture::SVulkanPixelData::ClearData()
{
    // not editable texture
}

void CVulkanTexture::SVulkanPixelData::OnRelease(class CVulkanRenderer& _rkRenderer)
{
    VkDevice kDevice = _rkRenderer.GetVkDevice();
    if (m_kMem != VK_NULL_HANDLE) {
        vkFreeMemory(kDevice, m_kMem, nullptr);
        m_kMem = VK_NULL_HANDLE;
    }
    if (m_kStageImageBuffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(kDevice, m_kStageImageBuffer, nullptr);
        m_kStageImageBuffer = VK_NULL_HANDLE;
    }
}

bool CVulkanTexture::SVulkanPixelData::CopyBufferToImage(int nBaseLayer)
{
    if (!m_bDirty)
        return true;
    VkImage kImage = m_rkOwner.GetImage();
    if (kImage == VK_NULL_HANDLE)
        return false;
    
    CVulkanRenderSystem& rkSystem = m_rkOwner.GetRenderer();
    CVulkanRenderer* pkRenderer = dynamic_cast<CVulkanRenderer*>(rkSystem.GetRenderer());

    CVulkanCommandBuffer& rkCommandBuffer = pkRenderer->GetActiveCmdBuffer(CVulkanRenderer::eCBT_Resource);

    VkBufferImageCopy kBufferCopyRegion = {};
    kBufferCopyRegion.imageSubresource.aspectMask = m_rkOwner.GetAspectFlag();
    kBufferCopyRegion.imageSubresource.mipLevel = m_nLevel;
    kBufferCopyRegion.imageSubresource.baseArrayLayer = nBaseLayer;
    kBufferCopyRegion.imageSubresource.layerCount = 1;
    kBufferCopyRegion.imageExtent.width = m_nWidth;
    kBufferCopyRegion.imageExtent.height = m_nHeight;
    kBufferCopyRegion.imageExtent.depth = m_nDepth;

    VkCommandBuffer kCmdBuffer = rkCommandBuffer.GetCmdBufferInstance();
    VkImageLayout eOldLayout = m_rkOwner.GetCurrentLayout();
    if (eOldLayout != VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        m_rkOwner.SetImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &rkCommandBuffer);
    }

    vkCmdCopyBufferToImage(kCmdBuffer, m_kStageImageBuffer, kImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &kBufferCopyRegion);
    RefCommandBuffer(&rkCommandBuffer);
    m_bDirty = false;
    return true;
}

CVulkanTexture::SVulkanTextureInstance::SVulkanTextureInstance(VkImage _kImage)
    : m_kMainImage(_kImage)
    , m_kMem(VK_NULL_HANDLE)
    , m_bExtenal(true)
{
    memset(&m_kImageDesc, 0, sizeof(VkDescriptorImageInfo));
}

CVulkanTexture::SVulkanTextureInstance::SVulkanTextureInstance()
    : m_kMainImage(VK_NULL_HANDLE)
    , m_kMem(VK_NULL_HANDLE)
    , m_bExtenal(false)
{
    memset(&m_kImageDesc, 0, sizeof(VkDescriptorImageInfo));
}

void CVulkanTexture::SVulkanTextureInstance::OnRelease(CVulkanRenderer& _rkRenderer)
{
    VkDevice kDevice = _rkRenderer.GetVkDevice();
    if (m_bExtenal) {
        m_kMainImage = VK_NULL_HANDLE;
        m_kMem = VK_NULL_HANDLE;
        if (m_kImageDesc.imageView != VK_NULL_HANDLE) {
            vkDestroyImageView(kDevice, m_kImageDesc.imageView, nullptr);
        }
        memset(&m_kImageDesc, 0, sizeof(VkDescriptorImageInfo));
        return;
    }

    if (m_kImageDesc.imageView != VK_NULL_HANDLE) {
        vkDestroyImageView(kDevice, m_kImageDesc.imageView, nullptr);
        memset(&m_kImageDesc, 0, sizeof(VkDescriptorImageInfo));
    }
    if (m_kMem != VK_NULL_HANDLE) {
        vkFreeMemory(kDevice, m_kMem, nullptr);
        m_kMem = VK_NULL_HANDLE;
    }
    if (m_kMainImage != VK_NULL_HANDLE) {
        vkDestroyImage(kDevice, m_kMainImage, nullptr);
        m_kMainImage = VK_NULL_HANDLE;
    }
}

CVulkanTexture::CVulkanTexture(CVulkanRenderSystem& _rkSystem, unsigned int _nType)
    : CGLITexture(_nType)
    , m_rkSystem(_rkSystem)
    , m_eType((VkImageViewType)_nType)
    , m_eFormat(VK_FORMAT_UNDEFINED)
    , m_bDirty(false)
{

}

CVulkanTexture::~CVulkanTexture()
{
    CVulkanRenderer* pkRenderer = dynamic_cast<CVulkanRenderer*>(m_rkSystem.GetRenderer());
    const size_t nCount = m_kPixelDatas.size();
    for (size_t nIndex = 0; nIndex < nCount; nIndex++) {
        std::shared_ptr <CVulkanTexture::SVulkanPixelData> spkResource = std::static_pointer_cast<CVulkanTexture::SVulkanPixelData>(m_kPixelDatas[nIndex]);
        if (spkResource == nullptr)
            continue;
        pkRenderer->ReleaseResource(spkResource);
    }
    if (m_spkInstance)
        pkRenderer->ReleaseResource(m_spkInstance);
}

bool CVulkanTexture::Active(int _nStage)
{
    return true;
}

bool CVulkanTexture::BindTexture()
{
    return true;
}

bool CVulkanTexture::FlushTexture()
{
    if (m_bDirty == false) {
        SetImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        return true;
    }

    CVulkanRenderer* pkRenderer = dynamic_cast<CVulkanRenderer*>(m_rkSystem.GetRenderer());

    if (m_spkInstance && m_spkInstance->IsCreated()) {
        pkRenderer->ReleaseResource(m_spkInstance);
        m_spkInstance = std::make_shared<SVulkanTextureInstance>();
    }

    if (!m_spkInstance->m_bExtenal && m_kPixelDatas.empty())
        return false;

    m_bDirty = false;

    VkDevice kDevice = pkRenderer->GetVkDevice();
    bool bIsAttachment = ((m_kPixelDatas.size() == 1) && m_kPixelDatas[0]->GetSize() == 0);
    if (m_spkInstance->m_kMainImage == VK_NULL_HANDLE) {
        VkImageCreateInfo kCreateInfo = vkTools::initializers::imageCreateInfo();
        kCreateInfo.imageType = GetImageType();
        kCreateInfo.format = CVulkanTexture::GetVulkanTextureFormat(m_eFormat);
        kCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        kCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        kCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        kCreateInfo.extent.width = GetWidth();
        kCreateInfo.extent.height = GetHeight();
        kCreateInfo.extent.depth = GetDepth();
        kCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

        if (bIsAttachment)
            kCreateInfo.usage |= GetAttachmentUsageBit();

        kCreateInfo.mipLevels = m_kPixelDatas.size();
        if (m_eType == VK_IMAGE_VIEW_TYPE_CUBE) {
            kCreateInfo.arrayLayers = 6;
            kCreateInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
        }
        else
            kCreateInfo.arrayLayers = 1;

        VkResult eResult = vkCreateImage(kDevice, &kCreateInfo, nullptr, &m_spkInstance->m_kMainImage);
        if (eResult != VK_SUCCESS)
            return false;
    }

    if (!m_spkInstance->m_bExtenal && m_spkInstance->m_kMem == VK_NULL_HANDLE) {
        VkMemoryRequirements kRequirement = {};
        vkGetImageMemoryRequirements(kDevice, m_spkInstance->m_kMainImage, &kRequirement);

        VkMemoryAllocateInfo kAllocateInfo = {};
        kAllocateInfo.allocationSize = kRequirement.size;
        vkTools::getMemoryType(pkRenderer->GetDeviceMemoryProp(), kRequirement.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &kAllocateInfo.memoryTypeIndex);
        VkResult eResult = vkAllocateMemory(kDevice, &kAllocateInfo, nullptr, &m_spkInstance->m_kMem);
        if (eResult != VK_SUCCESS)
            return false;
        eResult = vkBindImageMemory(kDevice, m_spkInstance->m_kMainImage, m_spkInstance->m_kMem, 0);
        if (eResult != VK_SUCCESS)
            return false;
    }

    if (m_spkInstance->m_kImageDesc.imageView == VK_NULL_HANDLE) {
        VkImageViewCreateInfo kImageViewCreateInfo = vkTools::initializers::imageViewCreateInfo();
        kImageViewCreateInfo.viewType = m_eType;
        kImageViewCreateInfo.format = CVulkanTexture::GetVulkanTextureFormat(m_eFormat);
        kImageViewCreateInfo.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
        kImageViewCreateInfo.subresourceRange.aspectMask = m_eAspectFlag;
        kImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        kImageViewCreateInfo.subresourceRange.levelCount = m_kPixelDatas.size();
        kImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        kImageViewCreateInfo.subresourceRange.layerCount = (m_eType == VK_IMAGE_VIEW_TYPE_CUBE) ? 6 : 1;
        kImageViewCreateInfo.image = m_spkInstance->m_kMainImage;
        
        VkResult eResult = vkCreateImageView(kDevice, &kImageViewCreateInfo, nullptr, &m_spkInstance->m_kImageDesc.imageView);
        if (eResult != VK_SUCCESS)
            return false;
    }

    const size_t nCount = m_kPixelDatas.size();
    for (size_t nIndex = 0; nIndex < nCount; nIndex++) {
        ITexture::IPixelData* pkData = m_kPixelDatas[nIndex].get();
        CVulkanTexture::SVulkanPixelData* pkPixelData = (CVulkanTexture::SVulkanPixelData*)pkData;
        verify(pkPixelData->CopyBufferToImage(0));
    }

    SetImageLayout((bIsAttachment) ? GetAttachmentImageLayoutType(true) : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    if (IsModeChanged()) {
        CVulkanSampler* pkGlobalSampler = pkRenderer->GetGlobalSampler();
        VkSamplerCreateInfo kInfo = vkTools::initializers::samplerCreateInfo();
        switch (m_eType) {
        case VK_IMAGE_VIEW_TYPE_3D:
            kInfo.addressModeW = GetVkClampMode((EClampMode)m_anClampMode[eCT_W].get());
        case VK_IMAGE_VIEW_TYPE_2D:
            kInfo.addressModeV = GetVkClampMode((EClampMode)m_anClampMode[eCT_V].get());
            kInfo.addressModeU = GetVkClampMode((EClampMode)m_anClampMode[eCT_U].get());
        default:
            break;
        }

        GetVkFilterMode((EFilterMode)m_anFilterMode[eFT_Mag].get(), kInfo.magFilter, kInfo.mipmapMode);
        GetVkFilterMode((EFilterMode)((m_kPixelDatas.size() > 1) ? m_anFilterMode [eFT_Min].get() : m_anFilterMode [eFT_Mag].get()), kInfo.magFilter, kInfo.mipmapMode);

        kInfo.maxAnisotropy = m_nAnisotropy;

        pkGlobalSampler->SetSamplerDesc(kInfo);
        verify(pkGlobalSampler->ValidateSampler());

        m_spkInstance->m_kImageDesc.sampler = pkGlobalSampler->GetSamplerInstance();

        for (int nIndex = eCT_U; nIndex < eCT_Max; nIndex++) {
            m_anClampMode [nIndex].clearDirty();
        }
        for (int nIndex = eFT_Min; nIndex < eFT_Max; nIndex++) {
            m_anFilterMode [nIndex].clearDirty();
        }
    }

    return true;
}

bool CVulkanTexture::SetTextureData(const char* _pcData, unsigned int _nWidth, unsigned int _nHeight, unsigned int _nDepth, int _eTextureFormat, int _nLevel)
{
    bool bRet = CGLITexture::SetTextureData(_pcData, _nWidth, _nHeight, _nDepth, _eTextureFormat, _nLevel);
    if (!bRet)
        return false;
    m_bDirty = true;
    return true;
}

bool CVulkanTexture::SetTextureDimension(unsigned int _nWidth, unsigned int _nHeight, unsigned int _nDepth, int _nLevel)
{
    return CGLITexture::SetTextureData(nullptr, _nWidth, _nHeight, _nDepth, _nLevel);
}

VkFormat CVulkanTexture::GetVulkanTextureFormat(int _eFormat)
{
    gli::format eFormat = (gli::format)_eFormat;
    switch (eFormat) {
    case gli::FORMAT_R8_UNORM:
        return VK_FORMAT_R8_UNORM;
    case gli::FORMAT_RG8_UNORM:
        return VK_FORMAT_R8G8_UNORM;
    case gli::FORMAT_RGB8_UNORM:
        return VK_FORMAT_R8G8B8_UNORM;
    case gli::FORMAT_RGBA8_UNORM:
        return VK_FORMAT_R8G8B8A8_UNORM;
    case gli::FORMAT_R8_SNORM:
        return VK_FORMAT_R8_SNORM;
    case gli::FORMAT_RG8_SNORM:
        return VK_FORMAT_R8G8_SNORM;
    case gli::FORMAT_RGB8_SNORM:
        return VK_FORMAT_R8G8B8_SNORM;
    case gli::FORMAT_RGBA8_SNORM:
        return VK_FORMAT_R8G8B8A8_SNORM;
    case gli::FORMAT_RGBA4_UNORM:
        return VK_FORMAT_R4G4B4A4_UNORM_PACK16;
    case gli::FORMAT_D16_UNORM:
        return VK_FORMAT_D16_UNORM;
    case gli::FORMAT_D24_UNORM:
        return VK_FORMAT_X8_D24_UNORM_PACK32;
    case gli::FORMAT_D32_UFLOAT:
        return VK_FORMAT_D32_SFLOAT;
    case gli::FORMAT_D24S8_UNORM:
        return VK_FORMAT_D24_UNORM_S8_UINT;
    case gli::FORMAT_RGB_DXT1_UNORM:
        return VK_FORMAT_BC1_RGB_UNORM_BLOCK;
    case gli::FORMAT_RGBA_DXT1_UNORM:
        return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
    case gli::FORMAT_RGBA_DXT3_UNORM:
        return VK_FORMAT_BC2_UNORM_BLOCK;
    case gli::FORMAT_RGBA_DXT5_UNORM:
        return VK_FORMAT_BC3_UNORM_BLOCK;
    case gli::FORMAT_RGBA32_SFLOAT:
        return VK_FORMAT_R32G32B32A32_SFLOAT;
    case gli::FORMAT_RGBA16_SFLOAT:
        return VK_FORMAT_R16G16B16A16_SFLOAT;
    case gli::FORMAT_R32_SFLOAT:
        return VK_FORMAT_R32_SFLOAT;
    default:
        break;
    }
    return VK_FORMAT_UNDEFINED;
}

int CVulkanTexture::GetTextureFormatByVkFormat(VkFormat _eFormat)
{
    switch (_eFormat) {
    case VK_FORMAT_R8_UNORM:
        return gli::FORMAT_R8_UNORM;
    case VK_FORMAT_R8G8_UNORM:
        return gli::FORMAT_RG8_UNORM;
    case VK_FORMAT_R8G8B8_UNORM:
        return gli::FORMAT_RGB8_UNORM;
    case VK_FORMAT_R8G8B8A8_UNORM:
        return gli::FORMAT_RGBA8_UNORM;
    case VK_FORMAT_R8_SNORM:
        return gli::FORMAT_R8_SNORM;
    case VK_FORMAT_R8G8_SNORM:
        return gli::FORMAT_RG8_SNORM;
    case VK_FORMAT_R8G8B8_SNORM:
        return gli::FORMAT_RGB8_SNORM;
    case VK_FORMAT_R8G8B8A8_SNORM:
        return gli::FORMAT_RGBA8_SNORM;
    case VK_FORMAT_R4G4B4A4_UNORM_PACK16:
        return gli::FORMAT_RGBA4_UNORM;
    case VK_FORMAT_D16_UNORM:
        return gli::FORMAT_D16_UNORM;
    case VK_FORMAT_X8_D24_UNORM_PACK32:
        return gli::FORMAT_D24_UNORM;
    case VK_FORMAT_D32_SFLOAT:
        return gli::FORMAT_D32_UFLOAT;
    case VK_FORMAT_D24_UNORM_S8_UINT:
        return gli::FORMAT_D24S8_UNORM;
    case VK_FORMAT_BC1_RGB_UNORM_BLOCK:
        return gli::FORMAT_RGB_DXT1_UNORM;
    case VK_FORMAT_BC1_RGBA_UNORM_BLOCK:
        return gli::FORMAT_RGBA_DXT1_UNORM;
    case VK_FORMAT_BC2_UNORM_BLOCK:
        return gli::FORMAT_RGBA_DXT3_UNORM;
    case VK_FORMAT_BC3_UNORM_BLOCK:
        return gli::FORMAT_RGBA_DXT5_UNORM;
    case VK_FORMAT_R32G32B32A32_SFLOAT:
        return gli::FORMAT_RGBA32_SFLOAT;
    case VK_FORMAT_R16G16B16A16_SFLOAT:
        return gli::FORMAT_RGBA16_SFLOAT;
    case VK_FORMAT_R32_SFLOAT:
        return gli::FORMAT_R32_SFLOAT;
    default:
        break;
    }
    return gli::FORMAT_INVALID;
}

char* CVulkanTexture::GetTextureData(int _nLevel)
{
    if (m_kPixelDatas.size() <= _nLevel)
        return nullptr;
    if (m_kPixelDatas[_nLevel] == nullptr)
        return nullptr;
    return m_kPixelDatas [_nLevel]->GetData();
}

void CVulkanTexture::SetLevel(int _nLevel)
{
    const unsigned int nTotalCount = m_kPixelDatas.size();
    if (nTotalCount == _nLevel)
        return;
    if (nTotalCount < _nLevel) {
        const size_t nOldSize = nTotalCount;
        const size_t nAllocateCount = _nLevel - nOldSize;
        std::vector <std::shared_ptr <ITexture::IPixelData> > kTemp = m_kPixelDatas;
        m_kPixelDatas.resize(_nLevel);
        m_kPixelDatas = kTemp;
        for (size_t nIndex = 0; nIndex < nAllocateCount; nIndex++) {
            m_kPixelDatas [nIndex + nOldSize] = AllocatePixelData(nIndex + nOldSize);
        }
        return;
    }

    CVulkanRenderer* pkRenderer = dynamic_cast<CVulkanRenderer*>(m_rkSystem.GetRenderer());
    const unsigned int nReleaseCount = nTotalCount - _nLevel;
    for (unsigned int nIndex = _nLevel - 1; nIndex < nTotalCount; nIndex++)
    {
        std::shared_ptr <CVulkanTexture::SVulkanPixelData> spkResource = std::static_pointer_cast<CVulkanTexture::SVulkanPixelData>(m_kPixelDatas[nIndex]);
        if (spkResource == nullptr)
            continue;
        pkRenderer->ReleaseResource(spkResource);
    }
    m_kPixelDatas.resize(_nLevel);
    short nLevel = 0;
    for (auto& spkPixelData : m_kPixelDatas) {
        spkPixelData = AllocatePixelData(nLevel++);
    }
    m_bDirty = true;
}

void CVulkanTexture::SetTextureFormat(int _eTextureFormat)
{
    CGLITexture::SetTextureFormat(_eTextureFormat);
    m_eAspectFlag = vkTools::getAspectFlags(CVulkanTexture::GetVulkanTextureFormat(_eTextureFormat));
}

void CVulkanTexture::SetImageLayout(VkImageLayout eLayout, CVulkanCommandBuffer* pkCmdBuffer)
{
    if (m_spkInstance == nullptr)
        return;

    if (m_spkInstance->m_kImageDesc.imageLayout != eLayout)
        return;

    if (pkCmdBuffer == nullptr) {
        CVulkanRenderer* pkRenderer = dynamic_cast<CVulkanRenderer*>(m_rkSystem.GetRenderer());
        CVulkanCommandBuffer& rkCommandBuffer = pkRenderer->GetActiveCmdBuffer(CVulkanRenderer::eCBT_Resource);
        pkCmdBuffer = &rkCommandBuffer;
    }

    VkImageSubresourceRange subresourceRange = {};
    subresourceRange.aspectMask = m_eAspectFlag;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.levelCount = m_kPixelDatas.size();
    subresourceRange.layerCount = (m_eType == VK_IMAGE_VIEW_TYPE_CUBE) ? 6 : 1;

    vkTools::setImageLayout(pkCmdBuffer->GetCmdBufferInstance(), m_spkInstance->m_kMainImage, m_eAspectFlag, m_spkInstance->m_kImageDesc.imageLayout, eLayout, subresourceRange);
    m_spkInstance->m_kImageDesc.imageLayout = eLayout;
    m_spkInstance->RefCommandBuffer(pkCmdBuffer);
}

VkImageType CVulkanTexture::GetImageType()
{
    switch (m_eType)
    {
    case VK_IMAGE_VIEW_TYPE_1D:
        return VK_IMAGE_TYPE_1D;
        break;
    case VK_IMAGE_VIEW_TYPE_2D:
        return VK_IMAGE_TYPE_2D;
        break;
    case VK_IMAGE_VIEW_TYPE_3D:
        return VK_IMAGE_TYPE_3D;
        break;
    case VK_IMAGE_VIEW_TYPE_CUBE:
        return VK_IMAGE_TYPE_2D;
        break;
    default:
        break;
    }
    return VK_IMAGE_TYPE_MAX_ENUM;
}

VkImageUsageFlags CVulkanTexture::GetAttachmentUsageBit()
{
    VkFormat eFormat = CVulkanTexture::GetVulkanTextureFormat(m_eFormat);
    switch (eFormat) {
    case VK_FORMAT_R8_UNORM:
    case VK_FORMAT_R8G8_UNORM:
    case VK_FORMAT_R8G8B8_UNORM:
    case VK_FORMAT_R8G8B8A8_UNORM:
    case VK_FORMAT_R8_SNORM:
    case VK_FORMAT_R8G8_SNORM:
    case VK_FORMAT_R8G8B8_SNORM:
    case VK_FORMAT_R8G8B8A8_SNORM:
    case VK_FORMAT_R4G4B4A4_UNORM_PACK16:
    case VK_FORMAT_R32G32B32A32_SFLOAT:
    case VK_FORMAT_R16G16B16A16_SFLOAT:
    case VK_FORMAT_R32_SFLOAT:
        return VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    case VK_FORMAT_D16_UNORM:
    case VK_FORMAT_X8_D24_UNORM_PACK32:
    case VK_FORMAT_D32_SFLOAT:
    case VK_FORMAT_D24_UNORM_S8_UINT:
        return VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    default:
        break;
    }
    return 0;
}

VkImageLayout CVulkanTexture::GetAttachmentImageLayoutType(bool _bRead)
{
    VkFormat eFormat = CVulkanTexture::GetVulkanTextureFormat(m_eFormat);
    switch (eFormat) {
    case VK_FORMAT_R8_UNORM:
    case VK_FORMAT_R8G8_UNORM:
    case VK_FORMAT_R8G8B8_UNORM:
    case VK_FORMAT_R8G8B8A8_UNORM:
    case VK_FORMAT_R8_SNORM:
    case VK_FORMAT_R8G8_SNORM:
    case VK_FORMAT_R8G8B8_SNORM:
    case VK_FORMAT_R8G8B8A8_SNORM:
    case VK_FORMAT_R4G4B4A4_UNORM_PACK16:
    case VK_FORMAT_R32G32B32A32_SFLOAT:
    case VK_FORMAT_R16G16B16A16_SFLOAT:
    case VK_FORMAT_R32_SFLOAT:
        return (_bRead) ? VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    case VK_FORMAT_D16_UNORM:
    case VK_FORMAT_X8_D24_UNORM_PACK32:
    case VK_FORMAT_D32_SFLOAT:
    case VK_FORMAT_D24_UNORM_S8_UINT:
        return (_bRead) ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    default:
        break;
    }
    return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
}

VkSamplerAddressMode CVulkanTexture::GetVkClampMode(EClampMode _eMode)
{
    VkSamplerAddressMode nMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    switch (_eMode)
    {
    case eCM_Clamp:
        nMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        break;
    case eCM_ClampToEdge:
        nMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        break;
    case eCM_ClampToBorder:
        nMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        break;
    case eCM_MirroredRepeat:
        nMode = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
        break;
    case eCM_Repeat:
        nMode = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        break;
    case eCM_MirrorClampToEdge:
        nMode = VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
        break;
    default:
        break;
    }
    return nMode;
}

bool CVulkanTexture::GetVkFilterMode(EFilterMode _eMode, VkFilter& _eFilter, VkSamplerMipmapMode& _eMipmapMode)
{
    _eFilter = VK_FILTER_LINEAR;
    _eMipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    switch (_eMode)
    {
    case eFM_Linear:
        return true;
    case eFM_Nearest:
        _eFilter = VK_FILTER_NEAREST;
        _eMipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
        return true;
    case eFM_Linear_Mipmap_Linear:
        return true;
    case eFM_Linear_Mipmap_Nearest:
        _eFilter = VK_FILTER_LINEAR;
        _eMipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
        return true;
    case eFM_Nearest_Mipmap_Linear:
        _eFilter = VK_FILTER_NEAREST;
        return true;
    case eFM_Nearest_Mipmap_Nearest:
        _eFilter = VK_FILTER_NEAREST;
        _eMipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
        return true;
    default:
        break;
    }
    return false;
}

void CVulkanTexture::SetExtenal(VkImage _kImage)
{
    if (_kImage == VK_NULL_HANDLE) {
        return;
    }

    if (m_spkInstance) {
        CVulkanRenderer* pkRenderer = dynamic_cast<CVulkanRenderer*>(m_rkSystem.GetRenderer());
        pkRenderer->ReleaseResource(m_spkInstance);
        m_spkInstance = std::make_shared<SVulkanTextureInstance>();
    }

    m_spkInstance->m_kMainImage = _kImage;
    m_bDirty = true;
}