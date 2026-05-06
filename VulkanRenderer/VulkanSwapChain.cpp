#include "ECSVulkanPCH.h"
#include "VulkanSwapChain.h"
#include "VulkanTools.h"
#include "VulkanFrameBuffer.h"
#include "VulkanRenderSystem.h"


//---------------------------------------------------------------------------
// Vulkan SwapChain
//---------------------------------------------------------------------------
CVulkanSwapChain::CVulkanSwapChain(CVulkanRenderSystem& _rkSystem)
    : m_rkSystem(_rkSystem)
    , m_kSwapChain(VK_NULL_HANDLE)
    , m_kSurface(VK_NULL_HANDLE)
    , m_eSwapChainDepthFormat(VK_FORMAT_D24_UNORM_S8_UINT)
    , m_nQueueNodeIndex(-1)
{
    memset(&m_spkDepthStencilView, 0, sizeof(m_spkDepthStencilView));
}

unsigned int CVulkanSwapChain::GetSupportQueueFamily(VkPhysicalDevice kPhysicalDevice)
{
    if (kPhysicalDevice == VK_NULL_HANDLE)
        return -1;

    if (!IsSurfaceInit())
        return -1;

    if (m_nQueueNodeIndex != -1)
        return m_nQueueNodeIndex; // return if selected Queue Node Index

    // Get Available Queue family properties
    unsigned int nQueueCount;
    vkGetPhysicalDeviceQueueFamilyProperties(kPhysicalDevice, &nQueueCount, NULL);
    if (!nQueueCount)
        return -1;
    std::vector <VkQueueFamilyProperties> kQueueProps(nQueueCount);
    vkGetPhysicalDeviceQueueFamilyProperties(kPhysicalDevice, &nQueueCount, &kQueueProps[0]);

    std::vector <VkBool32> kSupportPresent(nQueueCount);
    for (unsigned int nIndex = 0; nIndex < nQueueCount; nIndex++)
    {
        vkGetPhysicalDeviceSurfaceSupportKHR(kPhysicalDevice, nIndex, m_kSurface, &kSupportPresent[nIndex]);
    }
    unsigned int nGraphicsQueueNodeIndex = -1;
    unsigned int nPresentQueueNodeIndex = -1;
    for (unsigned int nIndex = 0; nIndex < nQueueCount; nIndex++)
    {
        if (kQueueProps[nIndex].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            if (nGraphicsQueueNodeIndex == -1)
            {
                nGraphicsQueueNodeIndex = nIndex;
            }

            if (kSupportPresent[nIndex] == VK_TRUE)
            {
                nGraphicsQueueNodeIndex = nIndex;
                nPresentQueueNodeIndex = nIndex;
                break;
            }
        }
    }
#if 1
    if (nPresentQueueNodeIndex == -1)
    {
        for (unsigned int nIndex = 0; nIndex < nQueueCount; ++nIndex)
        {
            if (kSupportPresent[nIndex] == VK_TRUE)
            {
                nPresentQueueNodeIndex = nIndex;
                break;
            }
        }
    }
    if (nGraphicsQueueNodeIndex == -1 || nPresentQueueNodeIndex == -1)
    {
        return -1;
    }
    // todo : Add support for separate graphics and presenting queue
    if (nGraphicsQueueNodeIndex != nPresentQueueNodeIndex) 
    {
        //Separate graphics and presenting queues are not supported yet!
        return -1;
    }
#else
    if (nGraphicsQueueNodeIndex == -1)
        return -1;
#endif
    m_nQueueNodeIndex = nGraphicsQueueNodeIndex;
    return m_nQueueNodeIndex;
}

#if defined(__ANDROID__)
bool CVulkanSwapChain::InitSurface(VkInstance kInstance, VkPhysicalDevice kPhysicalDevice, ANativeWindow* pkWindow)
#else
bool CVulkanSwapChain::InitSurface(VkInstance kInstance, VkPhysicalDevice kPhysicalDevice, void* pPlatformHandle, void* pPlatformWindow)
#endif
{
    VkResult eResult;
#if defined(__ANDROID__)
    VkAndroidSurfaceCreateInfoKHR kSurfaceCreateInfo = {};
    kSurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
    kSurfaceCreateInfo.window = pkWindow;
    eResult = vkCreateAndroidSurfaceKHR(kInstance, &kSurfaceCreateInfo, NULL, &m_kSurface);
#else
    VkWin32SurfaceCreateInfoKHR kSurfaceCreateInfo = {};
    kSurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    kSurfaceCreateInfo.hinstance = (HINSTANCE)pPlatformHandle;
    kSurfaceCreateInfo.hwnd = (HWND)pPlatformWindow;
    eResult = vkCreateWin32SurfaceKHR(kInstance, &kSurfaceCreateInfo, NULL, &m_kSurface);
#endif
    if (eResult != VK_SUCCESS)
        return false;

    // Get Support surface formats
    unsigned int nFormatCount = 0;
    eResult = vkGetPhysicalDeviceSurfaceFormatsKHR(kPhysicalDevice, m_kSurface, &nFormatCount, NULL);
    if (eResult != VK_SUCCESS)
        return false;
    if (!nFormatCount) {
        return false;
    }

    std::vector <VkSurfaceFormatKHR> kSurfaceFormats(nFormatCount);
    eResult = vkGetPhysicalDeviceSurfaceFormatsKHR(kPhysicalDevice, m_kSurface, &nFormatCount, &kSurfaceFormats[0]);
    if (eResult != VK_SUCCESS)
        return false;

    if ((nFormatCount == 1) && (kSurfaceFormats[0].format == VK_FORMAT_UNDEFINED))
    {
        m_eColorFormat = VK_FORMAT_R8G8B8A8_UNORM;
    }
    else
    {
        m_eColorFormat = kSurfaceFormats[0].format;
    }
    m_kColorSpace = kSurfaceFormats[0].colorSpace;
    return true;
}

bool CVulkanSwapChain::Create(VkDevice kDevice, VkPhysicalDevice kPhysicalDevice, VkPhysicalDeviceMemoryProperties& kMemProp, CVulkanCommandBuffer* pkCmdBuffer, unsigned int& nWidth, unsigned int& nHeight, VkPresentModeKHR _nPresetMode)
{
    VkResult eResult;
    VkSwapchainKHR kOldSwapchain = m_kSwapChain;

    VkSurfaceCapabilitiesKHR kSurfaceCap = {};
    eResult = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(kPhysicalDevice, m_kSurface, &kSurfaceCap);
    if (eResult != VK_SUCCESS)
        return false;

    VkExtent2D kSwapChainExtent = {};
    if (kSurfaceCap.currentExtent.width == 0)
    {
        kSwapChainExtent.width = nWidth;
        kSwapChainExtent.height = nHeight;
    }
    else
    {
        kSwapChainExtent = kSurfaceCap.currentExtent;
        nWidth = kSwapChainExtent.width;
        nHeight = kSwapChainExtent.height;
    }

    VkPresentModeKHR nSwapChainPresentMode = VK_PRESENT_MODE_FIFO_KHR;
    unsigned int nPresentModeCount;
    eResult = vkGetPhysicalDeviceSurfacePresentModesKHR(kPhysicalDevice, m_kSurface, &nPresentModeCount, NULL);
    if (eResult != VK_SUCCESS)
        return false;
    if (!nPresentModeCount)
        return false;

    std::vector <VkPresentModeKHR> kPresentMode(nPresentModeCount);
    eResult = vkGetPhysicalDeviceSurfacePresentModesKHR(kPhysicalDevice, m_kSurface, &nPresentModeCount, &kPresentMode[0]);
    if (eResult != VK_SUCCESS)
        return false;

    for (size_t nIndex = 0; nIndex < nPresentModeCount; nIndex++)
    {
        if (kPresentMode[nIndex] == _nPresetMode)
        {
            nSwapChainPresentMode = _nPresetMode;
            break;
        }
    }

    unsigned int nDesireNumberOfSwapChainImage = kSurfaceCap.minImageCount + 1;
    if ((kSurfaceCap.maxImageCount > 0) && (nDesireNumberOfSwapChainImage > kSurfaceCap.maxImageCount))
    {
        nDesireNumberOfSwapChainImage = kSurfaceCap.maxImageCount;
    }

    VkSurfaceTransformFlagsKHR kPreTransForm = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    if (kSurfaceCap.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
    {
        kPreTransForm = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    }
    else
    {
        kPreTransForm = kSurfaceCap.currentTransform;
    }

    VkSwapchainCreateInfoKHR kSwapChainCreateInfo = {};
    kSwapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    kSwapChainCreateInfo.pNext = NULL;
    kSwapChainCreateInfo.surface = m_kSurface;
    kSwapChainCreateInfo.minImageCount = nDesireNumberOfSwapChainImage;
    kSwapChainCreateInfo.imageFormat = m_eColorFormat;
    kSwapChainCreateInfo.imageColorSpace = m_kColorSpace;
    kSwapChainCreateInfo.imageExtent = kSwapChainExtent;
    kSwapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    kSwapChainCreateInfo.preTransform = (VkSurfaceTransformFlagBitsKHR)kPreTransForm;
    kSwapChainCreateInfo.imageArrayLayers = 1;
    kSwapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    kSwapChainCreateInfo.queueFamilyIndexCount = 0;
    kSwapChainCreateInfo.pQueueFamilyIndices = NULL;
    kSwapChainCreateInfo.presentMode = nSwapChainPresentMode;
    kSwapChainCreateInfo.oldSwapchain = kOldSwapchain;
    kSwapChainCreateInfo.clipped = true;
    kSwapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    eResult = vkCreateSwapchainKHR(kDevice, &kSwapChainCreateInfo, NULL, &m_kSwapChain);
    if (eResult != VK_SUCCESS)
        return false;

    if (kOldSwapchain != VK_NULL_HANDLE)
    {
        m_kBuffers.clear();
        vkDestroySwapchainKHR(kDevice, kOldSwapchain, NULL);
    }
    unsigned int nImageCount;
    eResult = vkGetSwapchainImagesKHR(kDevice, m_kSwapChain, &nImageCount, NULL);
    if (eResult != VK_SUCCESS)
        return false;

    std::vector <VkImage> kImages(nImageCount);
    eResult = vkGetSwapchainImagesKHR(kDevice, m_kSwapChain, &nImageCount, kImages.data());
    if (eResult != VK_SUCCESS)
        return false;

    if (!SetupDepthStencilView(kDevice, kPhysicalDevice, kMemProp, pkCmdBuffer, m_eSwapChainDepthFormat, nWidth, nHeight))
        return false;

    m_kBuffers.resize(nImageCount);
    for (unsigned int nIndex = 0; nIndex < nImageCount; nIndex++)
    {
        SSwapChainBuffer& kBuffer = m_kBuffers[nIndex];
        kBuffer.m_spkTexture = std::static_pointer_cast<CVulkanTexture>(m_rkSystem.CreateTexture(IRenderer::eTT_2D));
        kBuffer.m_spkTexture->SetExtenal(kImages[nIndex]);
        kBuffer.m_spkTexture->SetTextureFormat(CVulkanTexture::GetTextureFormatByVkFormat(m_eColorFormat));
        kBuffer.m_spkTexture->SetTextureDimension(kSwapChainExtent.width, kSwapChainExtent.height, 1);
        if (kBuffer.m_spkTexture->FlushTexture())
            return false;
        kBuffer.m_spkTexture->SetImageLayout(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, pkCmdBuffer);
        // Create FrameBuffer!
        kBuffer.m_spkFrameBuffer = std::static_pointer_cast<CVulkanFrameBuffer>(m_rkSystem.CreateFrameBuffer(IRenderer::eFBT_Genernal, kSwapChainExtent.width, kSwapChainExtent.height));
        kBuffer.m_spkFrameBuffer->SetRenderBufferColorAttactmentCount(1);
        kBuffer.m_spkFrameBuffer->SetTexture(kBuffer.m_spkTexture, 0);
        kBuffer.m_spkFrameBuffer->SetDepthTexture(m_spkDepthStencilView);
    }
    return true;
}

void CVulkanSwapChain::ClearUp(VkInstance kInstance, VkDevice kDevice)
{
    if (kInstance == VK_NULL_HANDLE)
        return;
    if (kDevice == VK_NULL_HANDLE)
        return;

    m_kBuffers.clear();
    vkDestroySwapchainKHR(kDevice, m_kSwapChain, NULL);
    m_kSwapChain = VK_NULL_HANDLE;
    vkDestroySurfaceKHR(kInstance, m_kSurface, NULL);
    m_kSurface = VK_NULL_HANDLE;

    DestroyDepthStencilView(kDevice);
}

bool CVulkanSwapChain::SetupDepthStencilView(VkDevice _kDevice, VkPhysicalDevice _kPhysicalDevice, VkPhysicalDeviceMemoryProperties& _rkMemProp, CVulkanCommandBuffer* pkCmdBuffer, VkFormat _eFormat, unsigned int _nWidth, unsigned int _nHeight)
{
    if (m_spkDepthStencilView == nullptr) {
        m_spkDepthStencilView = std::static_pointer_cast<CVulkanTexture>(m_rkSystem.CreateTexture(IRenderer::eTT_2D));
        m_spkDepthStencilView->SetTextureFormat(CVulkanTexture::GetTextureFormatByVkFormat(_eFormat));
        m_spkDepthStencilView->SetTextureDimension(_nWidth, _nHeight, 1);
        if (m_spkDepthStencilView->FlushTexture())
            return false;
    }
    return true;
#if 0
    if (m_kDepthStencilView.m_kView != VK_NULL_HANDLE)
        return true;
    VkImageCreateInfo kImageInfo = {};
    kImageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    kImageInfo.pNext = NULL;
    kImageInfo.imageType = VK_IMAGE_TYPE_2D;
    kImageInfo.format = _eFormat;
    kImageInfo.extent.width = _nWidth;
    kImageInfo.extent.height = _nHeight;
    kImageInfo.extent.depth = 1;
    kImageInfo.mipLevels = 1;
    kImageInfo.arrayLayers = 1;
    kImageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    kImageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    kImageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    kImageInfo.flags = 0;

    VkMemoryAllocateInfo kMemAllocInfo = {};
    kMemAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    kMemAllocInfo.pNext = NULL;
    kMemAllocInfo.allocationSize = 0;
    kMemAllocInfo.memoryTypeIndex = 0;

    VkImageViewCreateInfo kDepthStencilViewInfo = {};
    kDepthStencilViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    kDepthStencilViewInfo.pNext = NULL;
    kDepthStencilViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    kDepthStencilViewInfo.format = _eFormat;
    kDepthStencilViewInfo.flags = 0;
    kDepthStencilViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
    kDepthStencilViewInfo.subresourceRange.baseMipLevel = 0;
    kDepthStencilViewInfo.subresourceRange.levelCount = 1;
    kDepthStencilViewInfo.subresourceRange.baseArrayLayer = 0;
    kDepthStencilViewInfo.subresourceRange.layerCount = 1;

    VkMemoryRequirements kMemReqs;
    VkResult eResult;

    eResult = vkCreateImage(_kDevice, &kImageInfo, NULL, &m_kDepthStencilView.m_kImage);
    if (eResult != VK_SUCCESS)
        return false;
    vkGetImageMemoryRequirements(_kDevice, m_kDepthStencilView.m_kImage, &kMemReqs);
    kMemAllocInfo.allocationSize = kMemReqs.size;
    vkTools::getMemoryType(_rkMemProp, kMemReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &kMemAllocInfo.memoryTypeIndex);
    eResult = vkAllocateMemory(_kDevice, &kMemAllocInfo, NULL, &m_kDepthStencilView.m_kMem);
    if (eResult != VK_SUCCESS)
        return false;

    eResult = vkBindImageMemory(_kDevice, m_kDepthStencilView.m_kImage, m_kDepthStencilView.m_kMem, 0);
    if (eResult != VK_SUCCESS)
        return false;
    vkTools::setImageLayout(_kSetupCmdBuffer, m_kDepthStencilView.m_kImage, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

    kDepthStencilViewInfo.image = m_kDepthStencilView.m_kImage;
    eResult = vkCreateImageView(_kDevice, &kDepthStencilViewInfo, NULL, &m_kDepthStencilView.m_kView);
    if (eResult == VK_SUCCESS)
        return false;

    // Create FrameBuffer!










    return (eResult == VK_SUCCESS);
#endif
}

void CVulkanSwapChain::DestroyDepthStencilView(VkDevice _kDevice)
{
    m_spkDepthStencilView = nullptr;
}