#pragma once

#include "VulkanBuffer.h"

class CVulkanCommandBuffer
{
    class CVulkanRenderer& m_rkSystem;
    std::shared_ptr <CVulkanUniformBuffer> m_spkGlobalUniformBuffer;
    VkCommandPool m_kRefCmdPool;
    VkCommandBuffer m_kCmdBuffer;
    VkFence m_kFence;
    unsigned int m_nRevision;
    bool m_bUseFence;
    bool m_bRecord;
    bool m_bIsBegin;
    bool m_bSubmited;
public:

    CVulkanCommandBuffer(class CVulkanRenderSystem& _rkSystem, VkCommandPool kPool);

    virtual ~CVulkanCommandBuffer();

    bool Initialize(VkCommandBufferLevel _eLevel, bool _bUseFence = false);

    inline unsigned int GetRevision()
    {
        return m_nRevision;
    }

    inline bool IsUseFence()
    {
        return m_bUseFence;
    }

    inline bool IsSubmit()
    {
        return m_bSubmited;
    }

    inline bool IsBegin()
    {
        return m_bIsBegin;
    }

    inline bool IsRecord()
    {
        return m_bRecord;
    }

    bool Begin();

    void End();

    bool Submit(VkQueue kQueue, VkSubmitInfo& kSubmitInfo, bool bUseFence, bool bWaitIdle = false);

    bool ResetCmdBuffer();

    bool IsFenceSignaled();

    bool WaitFence(uint64_t _n64TimeInNanosec);

    void ResetFence();

    bool IsCommandBufferValid();

    inline VkCommandBuffer GetCmdBufferInstance()
    {
        return m_kCmdBuffer;
    }

    inline CVulkanUniformBuffer* GetGlobalUniform()
    {
        return m_spkGlobalUniformBuffer.get();
    }
};