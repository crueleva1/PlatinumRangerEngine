#pragma once

#include "ShareModule.h"

#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#else
#ifdef __linux__
#define VK_USE_PLATFORM_XCB_KHR
#else
#define VK_USE_PLATFORM_ANDROID_KHR
#endif
#endif
#define VK_NO_PROTOTYPES

#include "vulkan.h"

// Vulkan Library Function Ptr
extern PFN_vkCreateInstance vkCreateInstance;
extern PFN_vkDestroyInstance vkDestroyInstance;
extern PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevices;
extern PFN_vkGetPhysicalDeviceFeatures vkGetPhysicalDeviceFeatures;
extern PFN_vkGetPhysicalDeviceFormatProperties vkGetPhysicalDeviceFormatProperties;
extern PFN_vkGetPhysicalDeviceImageFormatProperties vkGetPhysicalDeviceImageFormatProperties;
extern PFN_vkGetPhysicalDeviceProperties vkGetPhysicalDeviceProperties;
extern PFN_vkGetPhysicalDeviceQueueFamilyProperties vkGetPhysicalDeviceQueueFamilyProperties;
extern PFN_vkGetPhysicalDeviceMemoryProperties vkGetPhysicalDeviceMemoryProperties;
extern PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr;
extern PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr;
extern PFN_vkCreateDevice vkCreateDevice;
extern PFN_vkDestroyDevice vkDestroyDevice;
extern PFN_vkEnumerateInstanceExtensionProperties vkEnumerateInstanceExtensionProperties;
extern PFN_vkEnumerateDeviceExtensionProperties vkEnumerateDeviceExtensionProperties;
extern PFN_vkEnumerateInstanceLayerProperties vkEnumerateInstanceLayerProperties;
extern PFN_vkEnumerateDeviceLayerProperties vkEnumerateDeviceLayerProperties;
extern PFN_vkGetDeviceQueue vkGetDeviceQueue;
extern PFN_vkQueueSubmit vkQueueSubmit;
extern PFN_vkQueueWaitIdle vkQueueWaitIdle;
extern PFN_vkDeviceWaitIdle vkDeviceWaitIdle;
extern PFN_vkAllocateMemory vkAllocateMemory;
extern PFN_vkFreeMemory vkFreeMemory;
extern PFN_vkMapMemory vkMapMemory;
extern PFN_vkUnmapMemory vkUnmapMemory;
extern PFN_vkFlushMappedMemoryRanges vkFlushMappedMemoryRanges;
extern PFN_vkInvalidateMappedMemoryRanges vkInvalidateMappedMemoryRanges;
extern PFN_vkGetDeviceMemoryCommitment vkGetDeviceMemoryCommitment;
extern PFN_vkBindBufferMemory vkBindBufferMemory;
extern PFN_vkBindImageMemory vkBindImageMemory;
extern PFN_vkGetBufferMemoryRequirements vkGetBufferMemoryRequirements;
extern PFN_vkGetImageMemoryRequirements vkGetImageMemoryRequirements;
extern PFN_vkGetImageSparseMemoryRequirements vkGetImageSparseMemoryRequirements;
extern PFN_vkGetPhysicalDeviceSparseImageFormatProperties vkGetPhysicalDeviceSparseImageFormatProperties;
extern PFN_vkQueueBindSparse vkQueueBindSparse;
extern PFN_vkCreateFence vkCreateFence;
extern PFN_vkDestroyFence vkDestroyFence;
extern PFN_vkResetFences vkResetFences;
extern PFN_vkGetFenceStatus vkGetFenceStatus;
extern PFN_vkWaitForFences vkWaitForFences;
extern PFN_vkCreateSemaphore vkCreateSemaphore;
extern PFN_vkDestroySemaphore vkDestroySemaphore;
extern PFN_vkCreateEvent vkCreateEvent;
extern PFN_vkDestroyEvent vkDestroyEvent;
extern PFN_vkGetEventStatus vkGetEventStatus;
extern PFN_vkSetEvent vkSetEvent;
extern PFN_vkResetEvent vkResetEvent;
extern PFN_vkCreateQueryPool vkCreateQueryPool;
extern PFN_vkDestroyQueryPool vkDestroyQueryPool;
extern PFN_vkGetQueryPoolResults vkGetQueryPoolResults;
extern PFN_vkCreateBuffer vkCreateBuffer;
extern PFN_vkDestroyBuffer vkDestroyBuffer;
extern PFN_vkCreateBufferView vkCreateBufferView;
extern PFN_vkDestroyBufferView vkDestroyBufferView;
extern PFN_vkCreateImage vkCreateImage;
extern PFN_vkDestroyImage vkDestroyImage;
extern PFN_vkGetImageSubresourceLayout vkGetImageSubresourceLayout;
extern PFN_vkCreateImageView vkCreateImageView;
extern PFN_vkDestroyImageView vkDestroyImageView;
extern PFN_vkCreateShaderModule vkCreateShaderModule;
extern PFN_vkDestroyShaderModule vkDestroyShaderModule;
extern PFN_vkCreatePipelineCache vkCreatePipelineCache;
extern PFN_vkDestroyPipelineCache vkDestroyPipelineCache;
extern PFN_vkGetPipelineCacheData vkGetPipelineCacheData;
extern PFN_vkMergePipelineCaches vkMergePipelineCaches;
extern PFN_vkCreateGraphicsPipelines vkCreateGraphicsPipelines;
extern PFN_vkCreateComputePipelines vkCreateComputePipelines;
extern PFN_vkDestroyPipeline vkDestroyPipeline;
extern PFN_vkCreatePipelineLayout vkCreatePipelineLayout;
extern PFN_vkDestroyPipelineLayout vkDestroyPipelineLayout;
extern PFN_vkCreateSampler vkCreateSampler;
extern PFN_vkDestroySampler vkDestroySampler;
extern PFN_vkCreateDescriptorSetLayout vkCreateDescriptorSetLayout;
extern PFN_vkDestroyDescriptorSetLayout vkDestroyDescriptorSetLayout;
extern PFN_vkCreateDescriptorPool vkCreateDescriptorPool;
extern PFN_vkDestroyDescriptorPool vkDestroyDescriptorPool;
extern PFN_vkResetDescriptorPool vkResetDescriptorPool;
extern PFN_vkAllocateDescriptorSets vkAllocateDescriptorSets;
extern PFN_vkFreeDescriptorSets vkFreeDescriptorSets;
extern PFN_vkUpdateDescriptorSets vkUpdateDescriptorSets;
extern PFN_vkCreateFramebuffer vkCreateFramebuffer;
extern PFN_vkDestroyFramebuffer vkDestroyFramebuffer;
extern PFN_vkCreateRenderPass vkCreateRenderPass;
extern PFN_vkDestroyRenderPass vkDestroyRenderPass;
extern PFN_vkGetRenderAreaGranularity vkGetRenderAreaGranularity;
extern PFN_vkCreateCommandPool vkCreateCommandPool;
extern PFN_vkDestroyCommandPool vkDestroyCommandPool;
extern PFN_vkResetCommandPool vkResetCommandPool;
extern PFN_vkAllocateCommandBuffers vkAllocateCommandBuffers;
extern PFN_vkFreeCommandBuffers vkFreeCommandBuffers;
extern PFN_vkBeginCommandBuffer vkBeginCommandBuffer;
extern PFN_vkEndCommandBuffer vkEndCommandBuffer;
extern PFN_vkResetCommandBuffer vkResetCommandBuffer;
extern PFN_vkCmdBindPipeline vkCmdBindPipeline;
extern PFN_vkCmdSetViewport vkCmdSetViewport;
extern PFN_vkCmdSetScissor vkCmdSetScissor;
extern PFN_vkCmdSetLineWidth vkCmdSetLineWidth;
extern PFN_vkCmdSetDepthBias vkCmdSetDepthBias;
extern PFN_vkCmdSetBlendConstants vkCmdSetBlendConstants;
extern PFN_vkCmdSetDepthBounds vkCmdSetDepthBounds;
extern PFN_vkCmdSetStencilCompareMask vkCmdSetStencilCompareMask;
extern PFN_vkCmdSetStencilWriteMask vkCmdSetStencilWriteMask;
extern PFN_vkCmdSetStencilReference vkCmdSetStencilReference;
extern PFN_vkCmdBindDescriptorSets vkCmdBindDescriptorSets;
extern PFN_vkCmdBindIndexBuffer vkCmdBindIndexBuffer;
extern PFN_vkCmdBindVertexBuffers vkCmdBindVertexBuffers;
extern PFN_vkCmdDraw vkCmdDraw;
extern PFN_vkCmdDrawIndexed vkCmdDrawIndexed;
extern PFN_vkCmdDrawIndirect vkCmdDrawIndirect;
extern PFN_vkCmdDrawIndexedIndirect vkCmdDrawIndexedIndirect;
extern PFN_vkCmdDispatch vkCmdDispatch;
extern PFN_vkCmdDispatchIndirect vkCmdDispatchIndirect;
extern PFN_vkCmdCopyBuffer vkCmdCopyBuffer;
extern PFN_vkCmdCopyImage vkCmdCopyImage;
extern PFN_vkCmdBlitImage vkCmdBlitImage;
extern PFN_vkCmdCopyBufferToImage vkCmdCopyBufferToImage;
extern PFN_vkCmdCopyImageToBuffer vkCmdCopyImageToBuffer;
extern PFN_vkCmdUpdateBuffer vkCmdUpdateBuffer;
extern PFN_vkCmdFillBuffer vkCmdFillBuffer;
extern PFN_vkCmdClearColorImage vkCmdClearColorImage;
extern PFN_vkCmdClearDepthStencilImage vkCmdClearDepthStencilImage;
extern PFN_vkCmdClearAttachments vkCmdClearAttachments;
extern PFN_vkCmdResolveImage vkCmdResolveImage;
extern PFN_vkCmdSetEvent vkCmdSetEvent;
extern PFN_vkCmdResetEvent vkCmdResetEvent;
extern PFN_vkCmdWaitEvents vkCmdWaitEvents;
extern PFN_vkCmdPipelineBarrier vkCmdPipelineBarrier;
extern PFN_vkCmdBeginQuery vkCmdBeginQuery;
extern PFN_vkCmdEndQuery vkCmdEndQuery;
extern PFN_vkCmdResetQueryPool vkCmdResetQueryPool;
extern PFN_vkCmdWriteTimestamp vkCmdWriteTimestamp;
extern PFN_vkCmdCopyQueryPoolResults vkCmdCopyQueryPoolResults;
extern PFN_vkCmdPushConstants vkCmdPushConstants;
extern PFN_vkCmdBeginRenderPass vkCmdBeginRenderPass;
extern PFN_vkCmdNextSubpass vkCmdNextSubpass;
extern PFN_vkCmdEndRenderPass vkCmdEndRenderPass;
extern PFN_vkCmdExecuteCommands vkCmdExecuteCommands;
extern PFN_vkDestroySurfaceKHR vkDestroySurfaceKHR;
extern PFN_vkGetPhysicalDeviceSurfaceSupportKHR vkGetPhysicalDeviceSurfaceSupportKHR;
extern PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR vkGetPhysicalDeviceSurfaceCapabilitiesKHR;
extern PFN_vkGetPhysicalDeviceSurfaceFormatsKHR vkGetPhysicalDeviceSurfaceFormatsKHR;
extern PFN_vkGetPhysicalDeviceSurfacePresentModesKHR vkGetPhysicalDeviceSurfacePresentModesKHR;
extern PFN_vkCreateSwapchainKHR vkCreateSwapchainKHR;
extern PFN_vkDestroySwapchainKHR vkDestroySwapchainKHR;
extern PFN_vkGetSwapchainImagesKHR vkGetSwapchainImagesKHR;
extern PFN_vkAcquireNextImageKHR vkAcquireNextImageKHR;
extern PFN_vkQueuePresentKHR vkQueuePresentKHR;
extern PFN_vkGetPhysicalDeviceDisplayPropertiesKHR vkGetPhysicalDeviceDisplayPropertiesKHR;
extern PFN_vkGetPhysicalDeviceDisplayPlanePropertiesKHR vkGetPhysicalDeviceDisplayPlanePropertiesKHR;
extern PFN_vkGetDisplayPlaneSupportedDisplaysKHR vkGetDisplayPlaneSupportedDisplaysKHR;
extern PFN_vkGetDisplayModePropertiesKHR vkGetDisplayModePropertiesKHR;
extern PFN_vkCreateDisplayModeKHR vkCreateDisplayModeKHR;
extern PFN_vkGetDisplayPlaneCapabilitiesKHR vkGetDisplayPlaneCapabilitiesKHR;
extern PFN_vkCreateDisplayPlaneSurfaceKHR vkCreateDisplayPlaneSurfaceKHR;
extern PFN_vkCreateSharedSwapchainsKHR vkCreateSharedSwapchainsKHR;
extern PFN_vkDebugMarkerSetObjectTagEXT vkDebugMarkerSetObjectTagEXT;
extern PFN_vkDebugMarkerSetObjectNameEXT vkDebugMarkerSetObjectNameEXT;
extern PFN_vkCmdDebugMarkerBeginEXT vkCmdDebugMarkerBeginEXT;
extern PFN_vkCmdDebugMarkerEndEXT vkCmdDebugMarkerEndEXT;
extern PFN_vkCmdDebugMarkerInsertEXT vkCmdDebugMarkerInsertEXT;
#ifdef _WIN32
extern PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR vkGetPhysicalDeviceWin32PresentationSupportKHR;
extern PFN_vkCreateWin32SurfaceKHR vkCreateWin32SurfaceKHR;
#elif defined(__linux__)
extern PFN_vkCreateXcbSurfaceKHR vkCreateXcbSurfaceKHR;
extern PFN_vkGetPhysicalDeviceXcbPresentationSupportKHR vkGetPhysicalDeviceXcbPresentationSupportKHR;
#else
extern PFN_vkCreateAndroidSurfaceKHR vkCreateAndroidSurfaceKHR;
#endif

#define GetVulkanBaseProcAddress(x) x = GetAddressByType<PFN_##x>(#x);


#define VerifyAddress(x) if (!x) { assert(0); }

#define GetAndVerifyVulkanBaseAddress(x) \
        GetVulkanBaseProcAddress(x) \
        VerifyAddress(x)

#define GetVulkanInstanceProcAddress(i, x) x = (PFN_##x)vkGetInstanceProcAddr(i, #x);
#define GetAndVerifyVulkanInstanceProcAddress(i, x) \
        GetVulkanInstanceProcAddress(i, x) \
        if (!x) GetVulkanBaseProcAddress(x) \
        VerifyAddress(x)

#define GetVulkanDeviceProcAddress(d, x) x = (PFN_##x)vkGetDeviceProcAddr(d, #x);
#define GetAndVerifyVulkanDeviceProcAddress(d, x) \
        GetVulkanDeviceProcAddress(d, x) \
        VerifyAddress(x)

// Vulkan Importer
class CVulkanImporter : public CShareModule
{
    bool InitializeModule()
    {
        if (CShareModule::InitializeModule() == false)
            return false;
    
        // Load base function pointers
        GetAndVerifyVulkanBaseAddress(vkEnumerateInstanceExtensionProperties)
        GetAndVerifyVulkanBaseAddress(vkEnumerateInstanceLayerProperties)
        GetAndVerifyVulkanBaseAddress(vkCreateInstance)
        GetAndVerifyVulkanBaseAddress(vkDestroyInstance)
        GetAndVerifyVulkanBaseAddress(vkGetInstanceProcAddr)
        GetAndVerifyVulkanBaseAddress(vkGetDeviceProcAddr)
#ifdef _WIN32
        GetAndVerifyVulkanBaseAddress(vkGetPhysicalDeviceDisplayPropertiesKHR)
        GetAndVerifyVulkanBaseAddress(vkGetPhysicalDeviceDisplayPlanePropertiesKHR)
        GetAndVerifyVulkanBaseAddress(vkGetDisplayPlaneSupportedDisplaysKHR)
        GetAndVerifyVulkanBaseAddress(vkGetDisplayModePropertiesKHR)
        GetAndVerifyVulkanBaseAddress(vkCreateDisplayModeKHR)
        GetAndVerifyVulkanBaseAddress(vkGetDisplayPlaneCapabilitiesKHR)
        GetAndVerifyVulkanBaseAddress(vkCreateDisplayPlaneSurfaceKHR)
#endif
        return true;
    }

public:
    CVulkanImporter()
#ifdef _WIN32
        :CShareModule("vulkan-1.dll")
#else
        :CShareModule("libvulkan.so")
#endif
    {
    }
    
    virtual ~CVulkanImporter()
    {
    }

    bool LoadVulkanInstanceAddress(VkInstance kInstance)
    {
        if (kInstance == VK_NULL_HANDLE)
            return false;
    
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkEnumeratePhysicalDevices)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkGetPhysicalDeviceFeatures)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkGetPhysicalDeviceFormatProperties)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkGetPhysicalDeviceImageFormatProperties)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkGetPhysicalDeviceProperties)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkGetPhysicalDeviceQueueFamilyProperties)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkGetPhysicalDeviceMemoryProperties)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkGetInstanceProcAddr)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkGetDeviceProcAddr)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCreateDevice)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkDestroyDevice)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkEnumerateDeviceExtensionProperties)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkEnumerateDeviceLayerProperties)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkGetDeviceQueue)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkQueueSubmit)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkQueueWaitIdle)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkDeviceWaitIdle)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkAllocateMemory)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkFreeMemory)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkMapMemory)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkUnmapMemory)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkFlushMappedMemoryRanges)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkInvalidateMappedMemoryRanges)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkGetDeviceMemoryCommitment)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkBindBufferMemory)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkBindImageMemory)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkGetBufferMemoryRequirements)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkGetImageMemoryRequirements)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkGetImageSparseMemoryRequirements)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkGetPhysicalDeviceSparseImageFormatProperties)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkQueueBindSparse)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCreateFence)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkDestroyFence)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkResetFences)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkGetFenceStatus)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkWaitForFences)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCreateSemaphore)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkDestroySemaphore)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCreateEvent)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkDestroyEvent)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkGetEventStatus)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkSetEvent)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkResetEvent)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCreateQueryPool)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkDestroyQueryPool)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkGetQueryPoolResults)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCreateBuffer)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkDestroyBuffer)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCreateBufferView)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkDestroyBufferView)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCreateImage)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkDestroyImage)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkGetImageSubresourceLayout)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCreateImageView)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkDestroyImageView)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCreateShaderModule)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkDestroyShaderModule)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCreatePipelineCache)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkDestroyPipelineCache)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkGetPipelineCacheData)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkMergePipelineCaches)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCreateGraphicsPipelines)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCreateComputePipelines)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkDestroyPipeline)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCreatePipelineLayout)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkDestroyPipelineLayout)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCreateSampler)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkDestroySampler)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCreateDescriptorSetLayout)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkDestroyDescriptorSetLayout)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCreateDescriptorPool)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkDestroyDescriptorPool)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkResetDescriptorPool)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkAllocateDescriptorSets)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkFreeDescriptorSets)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkUpdateDescriptorSets)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCreateFramebuffer)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkDestroyFramebuffer)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCreateRenderPass)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkDestroyRenderPass)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkGetRenderAreaGranularity)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCreateCommandPool)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkDestroyCommandPool)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkResetCommandPool)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkAllocateCommandBuffers)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkFreeCommandBuffers)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkBeginCommandBuffer)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkEndCommandBuffer)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkResetCommandBuffer)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCmdBindPipeline)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCmdSetViewport)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCmdSetScissor)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCmdSetLineWidth)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCmdSetDepthBias)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCmdSetBlendConstants)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCmdSetDepthBounds)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCmdSetStencilCompareMask)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCmdSetStencilWriteMask)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCmdSetStencilReference)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCmdBindDescriptorSets)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCmdBindIndexBuffer)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCmdBindVertexBuffers)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCmdDraw)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCmdDrawIndexed)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCmdDrawIndirect)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCmdDrawIndexedIndirect)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCmdDispatch)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCmdDispatchIndirect)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCmdCopyBuffer)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCmdCopyImage)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCmdBlitImage)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCmdCopyBufferToImage)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCmdCopyImageToBuffer)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCmdUpdateBuffer)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCmdFillBuffer)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCmdClearColorImage)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCmdClearDepthStencilImage)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCmdClearAttachments)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCmdResolveImage)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCmdSetEvent)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCmdResetEvent)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCmdWaitEvents)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCmdPipelineBarrier)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCmdBeginQuery)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCmdEndQuery)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCmdResetQueryPool)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCmdWriteTimestamp)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCmdCopyQueryPoolResults)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCmdPushConstants)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCmdBeginRenderPass)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCmdNextSubpass)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCmdEndRenderPass)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCmdExecuteCommands)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkDestroySurfaceKHR)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkGetPhysicalDeviceSurfaceSupportKHR)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkGetPhysicalDeviceSurfaceCapabilitiesKHR)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkGetPhysicalDeviceSurfaceFormatsKHR)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkGetPhysicalDeviceSurfacePresentModesKHR)
#ifdef _WIN32
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCreateWin32SurfaceKHR)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkGetPhysicalDeviceWin32PresentationSupportKHR)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkDebugMarkerSetObjectTagEXT)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkDebugMarkerSetObjectNameEXT)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCmdDebugMarkerBeginEXT)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCmdDebugMarkerEndEXT)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCmdDebugMarkerInsertEXT)
#elif defined(__linux__)
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCreateXcbSurfaceKHR)
#else
        GetAndVerifyVulkanInstanceProcAddress(kInstance, vkCreateAndroidSurfaceKHR)
#endif
        return true;
    }

    bool LoadVulkanDeviceAddress(VkDevice kDevice)
    {
        if (kDevice == VK_NULL_HANDLE)
            return false;
    
        GetAndVerifyVulkanDeviceProcAddress(kDevice, vkCreateSwapchainKHR)
        GetAndVerifyVulkanDeviceProcAddress(kDevice, vkDestroySwapchainKHR)
        GetAndVerifyVulkanDeviceProcAddress(kDevice, vkGetSwapchainImagesKHR)
        GetAndVerifyVulkanDeviceProcAddress(kDevice, vkAcquireNextImageKHR)
        GetAndVerifyVulkanDeviceProcAddress(kDevice, vkQueuePresentKHR)
    
        return true;
    }
};