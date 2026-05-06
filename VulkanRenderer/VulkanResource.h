#pragma once


class CVulkanCommandBuffer;

class CVulkanCmdBufferResource
{
    // Revision / CmdBuffer
    std::map <unsigned int, CVulkanCommandBuffer*> m_kRefCmdBuffer;
public:
    CVulkanCmdBufferResource();

    virtual ~CVulkanCmdBufferResource();

    void RefCommandBuffer(CVulkanCommandBuffer* _pkCmdBuffer);

    virtual bool IsCmdbufferAllResolved();

    virtual void OnRelease(class CVulkanRenderer& _rkRenderer) = 0;
};
