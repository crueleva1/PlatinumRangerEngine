#include "ECSVulkanPCH.h"
#include "VulkanBuffer.h"
#include "VulkanRenderSystem.h"
#include "VulkanCommandBuffer.h"
#include "VulkanMaterial.h"


void CVulkanBuffer::SVulkanBufferInstance::OnRelease(CVulkanRenderer& _rkRenderer)
{
    VkDevice kDevice = _rkRenderer.GetVkDevice();
    if (m_pcMapped) {
        vkUnmapMemory(kDevice, m_kMem);
        m_pcMapped = nullptr;
    }
    if (m_kMem != VK_NULL_HANDLE) {
        vkFreeMemory(kDevice, m_kMem, nullptr);
        m_kMem = VK_NULL_HANDLE;
    }
    if(m_kBuffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(kDevice, m_kBuffer, nullptr);
        m_kBuffer = VK_NULL_HANDLE;
    }
}

CVulkanBuffer::CVulkanBuffer(CVulkanRenderSystem& _rkSystem, VkFlags _eMemFlag, VkBufferUsageFlags _eUsage)
    :m_rkSystem(_rkSystem)
    ,m_spkBuffer(nullptr)
    ,m_eUsage(_eUsage)
    ,m_eMemFlag(_eMemFlag)
{

}

CVulkanBuffer::~CVulkanBuffer()
{
    if (m_spkBuffer != nullptr) {
        CVulkanRenderer* pkRenderer = dynamic_cast<CVulkanRenderer*>(m_rkSystem.GetRenderer());
        pkRenderer->ReleaseResource(m_spkBuffer);
    }
}

int CVulkanBuffer::GetBufferType()
{
    return m_eUsage;
}

void* CVulkanBuffer::Map()
{
    if (m_spkBuffer == nullptr)
        return nullptr;
    if (m_spkBuffer->IsCreated() == false)
        return nullptr;
    if (m_spkBuffer->m_pcMapped == nullptr) {
        CVulkanRenderer* pkRenderer = dynamic_cast<CVulkanRenderer*>(m_rkSystem.GetRenderer());
        VkDevice kDevice = pkRenderer->GetVkDevice();
        VkResult eResult = vkMapMemory(kDevice, m_spkBuffer->m_kMem, 0, m_nSize, 0, (void**)&m_spkBuffer->m_pcMapped);
        if (eResult != VK_SUCCESS)
            return nullptr;
    }
    return m_spkBuffer->m_pcMapped;
}

void CVulkanBuffer::UnMap()
{

}

bool CVulkanBuffer::Allocate(size_t _nSize)
{
    CVulkanRenderer* pkRenderer = dynamic_cast<CVulkanRenderer*>(m_rkSystem.GetRenderer());
    const VkPhysicalDeviceLimits& rkLimit = pkRenderer->GetDeviceSpec();
    const VkPhysicalDeviceMemoryProperties& rkMemProp = pkRenderer->GetDeviceMemoryProp();
    VkDevice kDevice = pkRenderer->GetVkDevice();
    if (kDevice == VK_NULL_HANDLE) {
        return false;
    }

    if (m_nSize > _nSize) {
        return true;
    }

    if (m_spkBuffer && m_spkBuffer->IsCreated()) {
        pkRenderer->ReleaseResource(m_spkBuffer);
        m_spkBuffer = std::make_shared<SVulkanBufferInstance>();
    }

    if (m_spkBuffer->m_kBuffer == VK_NULL_HANDLE) {
        VkBufferCreateInfo kInfo = vkTools::initializers::bufferCreateInfo(m_eUsage, _nSize);
        VkResult eResult = vkCreateBuffer(kDevice, &kInfo, nullptr, &m_spkBuffer->m_kBuffer);
        if (eResult != VK_SUCCESS)
            return false;
    }
    if (m_spkBuffer->m_kMem == VK_NULL_HANDLE) {
        VkMemoryRequirements kRequire = {};
        VkMemoryAllocateInfo kAlloc = {};
        kAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        vkGetBufferMemoryRequirements(kDevice, m_spkBuffer->m_kBuffer, &kRequire);
        kAlloc.allocationSize = kRequire.size;
        if (vkTools::getMemoryType(rkMemProp, kRequire.memoryTypeBits, m_eMemFlag, &kAlloc.memoryTypeIndex) == false)
            return false;
        VkResult eResult = vkAllocateMemory(kDevice, &kAlloc, nullptr, &m_spkBuffer->m_kMem);
        if (eResult != VK_SUCCESS)
            return false;
        eResult = vkBindBufferMemory(kDevice, m_spkBuffer->m_kBuffer, m_spkBuffer->m_kMem, 0);
        if (eResult != VK_SUCCESS)
            return false;
    }
    return true;
}

size_t CVulkanBuffer::GetSize()
{
    return (size_t)m_nSize;
}

CVulkanVertexBuffer::CVulkanVertexBuffer(class CVulkanRenderSystem& _rkSystem)
    :TVertexBuffer(_rkSystem, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT)
{

}

CVulkanVertexBuffer::~CVulkanVertexBuffer()
{

}

void CVulkanVertexBuffer::SetAttrSemanticCount(short _nSize)
{
    m_kSematics.resize(_nSize);
}

bool CVulkanVertexBuffer::OnSetAttrSemantic(std::shared_ptr <class IVariableSemantic> _spkSemantic, size_t _nIndex)
{
    if (m_kSematics.size() <= _nIndex)
        return false;

    m_kSematics[_nIndex] = _spkSemantic;
    return true;
}

size_t CVulkanVertexBuffer::GetAttrSemanticCount()
{
    return m_kSematics.size();
}

const IVariableSemantic* CVulkanVertexBuffer::GetAttrSemantic(size_t _nIndex)
{
    return m_kSematics [_nIndex].get();
}

bool CVulkanVertexBuffer::SetVertexBuffer(struct IMaterial* pkMaterial)
{
    CVulkanMaterial* pkVkMaterial = dynamic_cast<CVulkanMaterial*>(pkMaterial);
    if (pkVkMaterial == nullptr)
        return false;

    return true;
}

size_t CVulkanVertexBuffer::GetAttrSemanticSize()
{
    return m_kSematics.size();
}

bool CVulkanVertexBuffer::Bind()
{
    if (m_spkBuffer == nullptr)
        return false;
    CVulkanRenderer* pkRenderer = dynamic_cast<CVulkanRenderer*>(m_rkSystem.GetRenderer());
    VkDevice kDevice = pkRenderer->GetVkDevice();
    if (kDevice == VK_NULL_HANDLE)
        return false;

    CVulkanCommandBuffer& rkActiveCmdBuffer = pkRenderer->GetActiveCmdBuffer(CVulkanRenderer::eCBT_Draw);
    if (rkActiveCmdBuffer.IsCommandBufferValid() == false)
        return false;
    
    vkCmdBindVertexBuffers(rkActiveCmdBuffer.GetCmdBufferInstance(), 0, 1, &m_spkBuffer->m_kBuffer, &m_nOffset);

    return true;
}

CVulkanIndiceBuffer::CVulkanIndiceBuffer(class CVulkanRenderSystem& _rkSystem)
    :TIndiceBuffer(_rkSystem, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, VK_BUFFER_USAGE_INDEX_BUFFER_BIT)
    ,m_eIndexType(VK_INDEX_TYPE_UINT16)
{

}

CVulkanIndiceBuffer::~CVulkanIndiceBuffer()
{

}

int CVulkanIndiceBuffer::GetIndicesType()
{
    return m_eIndexType;
}

size_t CVulkanIndiceBuffer::GetElementSize()
{
    size_t nSize = 2;
    switch (m_eIndexType) {
    case VK_INDEX_TYPE_UINT16:
        nSize = 2;
        break;
    case VK_INDEX_TYPE_UINT32:
        nSize = 4;
        break;
    default:
        break;
    }
    return nSize;
}

bool CVulkanIndiceBuffer::Bind()
{
    if (m_spkBuffer == nullptr)
        return false;
    CVulkanRenderer* pkRenderer = dynamic_cast<CVulkanRenderer*>(m_rkSystem.GetRenderer());
    VkDevice kDevice = pkRenderer->GetVkDevice();
    if (kDevice == VK_NULL_HANDLE)
        return false;

    CVulkanCommandBuffer& rkActiveCmdBuffer = pkRenderer->GetActiveCmdBuffer(CVulkanRenderer::eCBT_Draw);
    if (rkActiveCmdBuffer.IsCommandBufferValid() == false)
        return false;

    vkCmdBindIndexBuffer(rkActiveCmdBuffer.GetCmdBufferInstance(), m_spkBuffer->m_kBuffer, 0, m_eIndexType);
    return true;
}

CVulkanUniformBuffer::CVulkanUniformBuffer(CVulkanRenderSystem& _rkSystem)
    :CVulkanBuffer(_rkSystem, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
{
    memset(&m_kBufferInfo, 0, sizeof(VkDescriptorBufferInfo));
}

CVulkanUniformBuffer::~CVulkanUniformBuffer()
{

}

void CVulkanUniformBuffer::FlushMapRange(size_t _nSize, size_t _nOffset)
{
    if (m_spkBuffer == nullptr)
        return;
    CVulkanRenderer* pkRenderer = dynamic_cast<CVulkanRenderer*>(m_rkSystem.GetRenderer());
    VkDevice kDevice = pkRenderer->GetVkDevice();
    if (kDevice == VK_NULL_HANDLE)
        return;
    VkMappedMemoryRange kRange = {};
    kRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    kRange.memory = m_spkBuffer->m_kMem;
    kRange.offset = m_nBaseOffset + _nOffset;
    kRange.size = _nSize;
    vkFlushMappedMemoryRanges(kDevice, 1, &kRange);
}

void* CVulkanUniformBuffer::Map()
{
    char* pvData = reinterpret_cast<char*>(CVulkanBuffer::Map());
    return pvData + m_nBaseOffset;
}

bool CVulkanUniformBuffer::RequestBuffer(size_t _nSize)
{
    CVulkanRenderer* pkRenderer = dynamic_cast<CVulkanRenderer*>(m_rkSystem.GetRenderer());
    const VkPhysicalDeviceLimits& kLimit = pkRenderer->GetDeviceSpec();
    size_t nTargetSize = m_nBaseOffset + _nSize;
    if (nTargetSize <= GetSize())
    {
        m_nBaseOffset += m_nCurrSize;
        m_nCurrSize = vkAlgin(_nSize, kLimit.minUniformBufferOffsetAlignment); 
        return true;
    }
    
    if (Allocate(nTargetSize) == false)
        return false;

    m_nBaseOffset += m_nCurrSize;
    m_nCurrSize = vkAlgin(_nSize, kLimit.minUniformBufferOffsetAlignment);
    return true;
}

bool CVulkanUniformBuffer::Bind()
{
    if (m_spkBuffer == nullptr)
        return false;
    
    m_kBufferInfo.buffer = m_spkBuffer->m_kBuffer;
    m_kBufferInfo.offset = 0; // VK_KHR_push_descriptor will need
    m_kBufferInfo.range = VK_WHOLE_SIZE;
    return true;
}

void CVulkanUniformBuffer::GetDescritorInfo(VkDescriptorBufferInfo& _rkInfo)
{
    memcpy(&_rkInfo, &m_kBufferInfo, sizeof(VkDescriptorBufferInfo));
}