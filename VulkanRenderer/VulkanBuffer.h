#pragma once


#include "IRenderComponent.h"
#include "VulkanResource.h"

class CVulkanBuffer : public IBuffer
{
protected:
    struct SVulkanBufferInstance : public CVulkanCmdBufferResource
    {
        VkBuffer m_kBuffer;
        VkDeviceMemory m_kMem;
        char* m_pcMapped;
        
        SVulkanBufferInstance()
            :m_kBuffer(VK_NULL_HANDLE)
            ,m_kMem(VK_NULL_HANDLE)
            ,m_pcMapped(nullptr)
        {}

        bool IsCreated()
        {
            if (m_kBuffer == VK_NULL_HANDLE)
                return false;
            if (m_kMem == VK_NULL_HANDLE)
                return false;
            return true;
        }

        void OnRelease(class CVulkanRenderer& _rkRenderer);
    };

    class CVulkanRenderSystem& m_rkSystem;
    std::shared_ptr <SVulkanBufferInstance> m_spkBuffer;
    VkBufferUsageFlags m_eUsage;
    VkFlags m_eMemFlag;
    VkDeviceSize m_nSize;
public:
    CVulkanBuffer(class CVulkanRenderSystem& _rkSystem, VkFlags _eMemFlag, VkBufferUsageFlags _eUsage);

    virtual ~CVulkanBuffer();

    virtual int GetBufferType();

    virtual void* Map();

    virtual void UnMap();

    virtual bool Allocate(size_t _nSize);

    virtual size_t GetSize();
};

class CVulkanVertexBuffer : public TVertexBuffer<CVulkanBuffer>
{
    std::vector <std::shared_ptr <IVariableSemantic> > m_kSematics;
    size_t m_nOffset;
public:
    CVulkanVertexBuffer(class CVulkanRenderSystem& _rkSystem);
    
    virtual ~CVulkanVertexBuffer();

    virtual void SetAttrSemanticCount(short _nSize);

    virtual bool OnSetAttrSemantic(std::shared_ptr <class IVariableSemantic> _spkSemantic, size_t _nIndex);

    template <class T, class S = TVariableSemantic <T> >
    bool SetAttrSemantic(const char* _pcSemantic, size_t _nIndex, int _nElementCount, EVariableSemanticElementType _eType)
    {
        std::shared_ptr <class IVariableSemantic> spkSemantic = std::make_shared <S>(_pcSemantic, _nIndex, _nElementCount, _eType);
        return OnSetAttrSemantic(spkSemantic, _nIndex);
    }

    virtual size_t GetAttrSemanticCount();

    virtual const class IVariableSemantic* GetAttrSemantic(size_t _nIndex);

    virtual size_t GetOffset()
    {
        return m_nOffset;
    }

    virtual void SetOffset(size_t _nOffset)
    {
        m_nOffset = _nOffset;
    }

    virtual bool SetVertexBuffer(struct IMaterial*);

    virtual size_t GetAttrSemanticSize();

    virtual bool Bind();
};

class CVulkanIndiceBuffer : public TIndiceBuffer<CVulkanBuffer>
{
    VkIndexType m_eIndexType;
public:
    CVulkanIndiceBuffer(class CVulkanRenderSystem& _rkSystem);

    virtual ~CVulkanIndiceBuffer();

    virtual int GetIndicesType();

    virtual size_t GetElementSize();

    virtual bool Bind();
};

class CVulkanUniformBuffer : public CVulkanBuffer
{
    VkDescriptorBufferInfo m_kBufferInfo;
    size_t m_nBaseOffset;
    size_t m_nCurrSize;
public:
    CVulkanUniformBuffer(class CVulkanRenderSystem& _rkSystem);

    virtual ~CVulkanUniformBuffer();

    void FlushMapRange(size_t _nSize, size_t _nOffset);

    void* Map() override;

    bool RequestBuffer(size_t _nSize);

    void ResetBase()
    {
        m_nBaseOffset = 0;
    }

    template <class T>
    bool UpdateData(const T& _rkData, size_t _nOffset, bool _bFlush = false)
    {
        if (m_spkBuffer == nullptr)
            return false;

        size_t nRange = _nOffset + sizeof(_rkData);
        if (nRange > GetSize())
            return false;

        char* pcData = reinterpret_cast<char*>(Map());
        if (pcData == nullptr)
            return false;

        pcData += _nOffset;
        memcpy(pcData, &_rkData, sizeof(_rkData));
        if (_bFlush) {
            FlushMapRange(sizeof(_rkData), _nOffset);
        }
        return true;
    }

    virtual bool Bind();

    void GetDescritorInfo(VkDescriptorBufferInfo& _rkInfo);
};