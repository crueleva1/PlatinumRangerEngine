#pragma once

#include "VulkanTexture.h"

class CVulkanRenderSystem;
class CVulkanFrameBuffer;
//---------------------------------------------------------------------------
// Vulkan SwapChain
//---------------------------------------------------------------------------
class CVulkanSwapChain
{
    struct SSwapChainBuffer
    {
        std::shared_ptr <CVulkanTexture> m_spkTexture;
        std::shared_ptr <CVulkanFrameBuffer> m_spkFrameBuffer;
    };
    std::shared_ptr <CVulkanTexture> m_spkDepthStencilView;
    CVulkanRenderSystem& m_rkSystem;
    VkSurfaceKHR m_kSurface;
    VkSwapchainKHR m_kSwapChain;
    VkFormat m_eColorFormat;
    VkColorSpaceKHR m_kColorSpace;
    std::vector <SSwapChainBuffer> m_kBuffers;
    VkFormat m_eSwapChainDepthFormat;
    // List of available frame buffers (same as number of swap chain images)
    unsigned int m_nQueueNodeIndex;

    CVulkanSwapChain(CVulkanRenderSystem& _rkSystem);

    inline bool IsSurfaceInit()
    {
        return (m_kSurface != VK_NULL_HANDLE);
    }

    inline bool IsSwapChainCreated()
    {
        if (m_kBuffers.empty())
            return false;
        return true;
    }

    inline bool IsFrameBufferRegistered()
    {
        const size_t nSize = m_kBuffers.size();
        for (size_t nIndex = 0; nIndex < nSize; nIndex++)
        {
            if (m_kBuffers[nIndex].m_spkFrameBuffer == nullptr)
                return false;
        }
        return true;
    }

    inline CVulkanFrameBuffer* GetFrameBuffer(size_t nBufferIndex)
    {
        return m_kBuffers[nBufferIndex].m_spkFrameBuffer.get();
    }

    inline void SetDepthFormat(VkFormat kFormat)
    {
        switch (kFormat)
        {
        case VK_FORMAT_D24_UNORM_S8_UINT:
        case VK_FORMAT_D32_SFLOAT_S8_UINT:
        case VK_FORMAT_D32_SFLOAT:
        case VK_FORMAT_D16_UNORM_S8_UINT:
        case VK_FORMAT_D16_UNORM:
            break;
        default:
            return;
        }
        m_eSwapChainDepthFormat = kFormat;
    }

    unsigned int GetSupportQueueFamily(VkPhysicalDevice kPhysicalDevice);

#if defined(__ANDROID__)
    bool InitSurface(VkInstance kInstance, VkPhysicalDevice kPhysicalDevice, ANativeWindow* pkWindow);
#else
    bool InitSurface(VkInstance kInstance, VkPhysicalDevice kPhysicalDevice, void* pPlatformHandle, void* pPlatformWindow);
#endif
   
    bool Create(VkDevice kDevice, VkPhysicalDevice kPhysicalDevice, VkPhysicalDeviceMemoryProperties& kMemProp, CVulkanCommandBuffer* pkCmdBuffer, unsigned int& nWidth, unsigned int& nHeight, VkPresentModeKHR _nPresetMode = VK_PRESENT_MODE_MAILBOX_KHR);

    void ClearUp(VkInstance kInstance, VkDevice kDevice);

    bool SetupDepthStencilView(VkDevice _kDevice, VkPhysicalDevice _kPhysicalDevice, VkPhysicalDeviceMemoryProperties& _rkMemProp, CVulkanCommandBuffer* pkCmdBuffer, VkFormat _eFormat, unsigned int _nWidth, unsigned int _nHeight);

    void DestroyDepthStencilView(VkDevice kDevice);

    // Acquires the next image in the swap chain
    inline VkResult AcquireNextImage(VkDevice kDevice, VkSemaphore kPresentCompleteSemaphore, unsigned int* pCurrentBuffer)
    {
        return vkAcquireNextImageKHR(kDevice, m_kSwapChain, -1, kPresentCompleteSemaphore, (VkFence)NULL, pnCurrentBuffer);
    }

    // Present the current image to the queue
    inline VkResult QueuePresent(VkQueue kQueue, unsigned int nCurrentBuffer)
    {
        VkPresentInfoKHR kPresentInfo = {};
        kPresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        kPresentInfo.pNext = NULL;
        kPresentInfo.swapchainCount = 1;
        kPresentInfo.pSwapchains = &m_kSwapChain;
        kPresentInfo.pImageIndices = &nCurrentBuffer;
        return vkQueuePresentKHR(kQueue, &kPresentInfo);
    }

    // Present the current image to the queue
    inline VkResult QueuePresent(VkQueue kQueue, unsigned int nCurrentBuffer, VkSemaphore& kWaitSemaphore)
    {
        VkPresentInfoKHR kPresentInfo = {};
        kPresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        kPresentInfo.pNext = NULL;
        kPresentInfo.swapchainCount = 1;
        kPresentInfo.pSwapchains = &m_kSwapChain;
        kPresentInfo.pImageIndices = &nCurrentBuffer;
        if (kWaitSemaphore != VK_NULL_HANDLE)
        {
            kPresentInfo.pWaitSemaphores = &kWaitSemaphore;
            kPresentInfo.waitSemaphoreCount = 1;
        }
        return vkQueuePresentKHR(kQueue, &kPresentInfo);
    }
};