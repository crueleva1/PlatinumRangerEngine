#include "ECSVulkanPCH.h"
#include "VulkanCommandBuffer.h"
#include "VulkanRenderSystem.h"

CVulkanCommandBuffer::CVulkanCommandBuffer(CVulkanRenderSystem& _rkSystem, VkCommandPool kPool)
    : m_rkSystem(dynamic_cast<CVulkanRenderer&>(*_rkSystem.GetRenderer()))
    , m_kRefCmdPool(kPool)
    , m_kCmdBuffer(VK_NULL_HANDLE)
    , m_kFence(VK_NULL_HANDLE)
    , m_bUseFence(false)
    , m_bSubmited(false)
    , m_bRecord(false)
    , m_bIsBegin(false)
    , m_nRevision(-1)
{
    m_spkGlobalUniformBuffer = std::make_shared<CVulkanUniformBuffer>(_rkSystem);
}

CVulkanCommandBuffer::~CVulkanCommandBuffer()
{
    VkDevice kDevice = m_rkSystem.GetVkDevice();
    if (kDevice == VK_NULL_HANDLE)
        return;
    if (m_bSubmited) {
        WaitFence(60 * 1000 * 1000); // Wait 60ms
    }
    if (m_kCmdBuffer != VK_NULL_HANDLE) {
        vkFreeCommandBuffers(kDevice, m_kRefCmdPool, 1, &m_kCmdBuffer);
        m_kCmdBuffer = VK_NULL_HANDLE;
    }
    if (m_kFence != VK_NULL_HANDLE) {
        vkDestroyFence(kDevice, m_kFence, NULL);
        m_kFence = VK_NULL_HANDLE;
    }
    m_bSubmited = false;
    m_bIsBegin = false;
    m_bUseFence = false;
}

bool CVulkanCommandBuffer::Initialize(VkCommandBufferLevel _eLevel, bool _bUseFence)
{
    if (m_kCmdBuffer != VK_NULL_HANDLE)
        return true;
    if (m_kRefCmdPool == VK_NULL_HANDLE)
        return false;

    VkDevice kDevice = m_rkSystem.GetVkDevice();
    if (kDevice == VK_NULL_HANDLE)
        return false;

    VkCommandBufferAllocateInfo kCmdBufInfo = {};
    kCmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    kCmdBufInfo.commandPool = m_kRefCmdPool;
    kCmdBufInfo.level = _eLevel;
    kCmdBufInfo.commandBufferCount = 1;
    VkResult eResult = VK_SUCCESS;
    eResult = vkAllocateCommandBuffers(kDevice, &kCmdBufInfo, &m_kCmdBuffer);
    if (eResult != VK_SUCCESS)
        return false;

    if (!m_bUseFence)
        return true;

    VkFenceCreateInfo kFenceInfo = {};
    kFenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    eResult = vkCreateFence(kDevice, &kFenceInfo, NULL, &m_kFence);
    return (eResult == VK_SUCCESS);
}

bool CVulkanCommandBuffer::Begin()
{
    if (m_kCmdBuffer == VK_NULL_HANDLE)
        return false;
    if (m_bIsBegin)
        return true;
    VkCommandBufferBeginInfo kCmdBuffInfo = vkTools::initializers::commandBufferBeginInfo();
    m_bIsBegin = (vkBeginCommandBuffer(m_kCmdBuffer, &kCmdBuffInfo) == VK_SUCCESS);
    m_bRecord = true;
    return m_bIsBegin;
}

void CVulkanCommandBuffer::End()
{
    if (m_kCmdBuffer == VK_NULL_HANDLE)
        return;
    if (!m_bIsBegin)
        return;
    vkEndCommandBuffer(m_kCmdBuffer);
    m_bIsBegin = false;
}

bool CVulkanCommandBuffer::Submit(VkQueue kQueue, VkSubmitInfo& kSubmitInfo, bool bUseFence, bool bWaitIdle)
{
    if (m_bIsBegin)
        return false;
    if (m_bSubmited)
        return false;

    bUseFence &= (m_kFence != VK_NULL_HANDLE);

    VkResult eResult = vkQueueSubmit(kQueue, 1, &kSubmitInfo, (bUseFence) ? m_kFence : VK_NULL_HANDLE);
    m_bSubmited = true;
    if (eResult != VK_SUCCESS)
        return false;

    m_nRevision++; //* this revision need ref system cmd buffer creation

    if (bWaitIdle) {
        eResult = vkQueueWaitIdle(kQueue);
    }
    m_bUseFence = bUseFence;

    return (eResult == VK_SUCCESS);
}

bool CVulkanCommandBuffer::ResetCmdBuffer()
{
    if (!m_bSubmited) {
        if (IsFenceSignaled())
            ResetFence();
        return true;
    }

    if (m_bUseFence && (m_kFence != VK_NULL_HANDLE) && !IsFenceSignaled())
        return false;

    ResetFence();

    VkResult eResult = vkResetCommandBuffer(m_kCmdBuffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
    if (eResult != VK_SUCCESS)
        return false;

    m_bSubmited = false;
    m_bIsBegin = false;
    m_bUseFence = false;
    m_bRecord = false;
    return true;
}

bool CVulkanCommandBuffer::IsFenceSignaled()
{
    VkDevice kDevice = m_rkSystem.GetVkDevice();
    if (kDevice == VK_NULL_HANDLE)
        return false;
    if (kDevice == VK_NULL_HANDLE)
        return false;
    if (m_kFence == VK_NULL_HANDLE)
        return false;

    VkResult eResult = vkGetFenceStatus(kDevice, m_kFence);
    return (eResult == VK_SUCCESS);
}

bool CVulkanCommandBuffer::WaitFence(uint64_t _n64TimeInNanosec)
{
    VkDevice kDevice = m_rkSystem.GetVkDevice();
    if (kDevice == VK_NULL_HANDLE)
        return false;
    if (kDevice == VK_NULL_HANDLE)
        return false;
    if (m_kFence == VK_NULL_HANDLE)
        return false;

    VkResult eResult = vkWaitForFences(kDevice, 1, &m_kFence, true, _n64TimeInNanosec);
    return (eResult == VK_SUCCESS);
}

void CVulkanCommandBuffer::ResetFence()
{
    VkDevice kDevice = m_rkSystem.GetVkDevice();
    if (kDevice == VK_NULL_HANDLE)
        return;
    if (m_kFence == VK_NULL_HANDLE)
        return;

    verify(vkResetFences(kDevice, 1, &m_kFence) == VK_SUCCESS);
}

bool CVulkanCommandBuffer::IsCommandBufferValid()
{
    return (m_kCmdBuffer != NULL);
}