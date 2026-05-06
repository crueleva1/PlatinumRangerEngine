#include "ECSVulkanPCH.h"
#include "VulkanResource.h"
#include "VulkanCommandBuffer.h"


CVulkanCmdBufferResource::CVulkanCmdBufferResource()
{

}

CVulkanCmdBufferResource::~CVulkanCmdBufferResource()
{

}

void CVulkanCmdBufferResource::RefCommandBuffer(CVulkanCommandBuffer* _pkCmdBuffer)
{
    if (_pkCmdBuffer == nullptr)
        return;

    m_kRefCmdBuffer.insert(std::make_pair(_pkCmdBuffer->GetRevision(), _pkCmdBuffer));
}

bool CVulkanCmdBufferResource::IsCmdbufferAllResolved()
{
    auto kIt = m_kRefCmdBuffer.begin();
    for (; kIt != m_kRefCmdBuffer.end(); ) {
        const unsigned int& rnRevision = kIt->first;
        CVulkanCommandBuffer* pkCmdBuffer = kIt->second;
        if (rnRevision != pkCmdBuffer->GetRevision()) {
            m_kRefCmdBuffer.erase(rnRevision);
            kIt++;
            continue;
        }
        if (!pkCmdBuffer->IsRecord()) {
            m_kRefCmdBuffer.erase(rnRevision);
            kIt++;
            continue;
        }
        else if (!pkCmdBuffer->IsSubmit()) {
            ++kIt;
            continue;
        }
        else if (pkCmdBuffer->IsUseFence() && !pkCmdBuffer->IsFenceSignaled()) {
            ++kIt;
            continue;
        }
        ++kIt;
    }
    return m_kRefCmdBuffer.empty();
}