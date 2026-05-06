#pragma once

#include "GLSLMaterial.h"
#include "RenderState.h"
#include "VulkanResource.h"

class IVulkanUniformInterface
{
    int m_nBasicType;
    short m_nDescriptorSetID;
    short m_nDescriptorSetBinding;
public:
    virtual ~IVulkanUniformInterface()
    {
    }

    inline void SetDescriptorSetID(short _nID)
    {
        m_nDescriptorSetID = _nID;
    }

    inline void SetDescriptorSetBinding(short _nBinding)
    {
        m_nDescriptorSetBinding = _nBinding;
    }

    inline void SetBasicType(int _nType)
    {
        m_nBasicType = _nType;
    }

    inline short GetDescriptorSetID() const
    {
        return m_nDescriptorSetID;
    }

    inline short GetDescriptorSetBinding() const
    {
        return m_nDescriptorSetBinding;
    }

    inline int GetBasicType() const
    {
        return m_nBasicType;
    }
};

class CVulkanGenericStructSematic
    : public CGenericStructSematic
    , public IVulkanUniformInterface
{
public:
    CVulkanGenericStructSematic(const char* _pcDesc, const char* _pcVariableName, size_t _nStructSize)
        : CGenericStructSematic(_pcDesc, _pcVariableName, _nStructSize)
    {
    }

    virtual ~CVulkanGenericStructSematic()
    {
    }
};
class CVulkanShader : public CGLSLShader
{
    std::vector <uint32_t> m_kSpv;
    class CVulkanRenderer& m_rkSystem;
    VkShaderModule m_kShader;
public:
    CVulkanShader(class CVulkanRenderSystem& _rkSystem, EShaderType);

    virtual ~CVulkanShader();

    virtual bool SetSource(const char* _pcCode);

    virtual bool Compile();

    inline VkShaderModule GetShader()
    {
        if (m_bError)
            return 0;
        return m_kShader;
    }
};

class CVulkanRenderState : public CRenderState
{
    struct SVulkanState
    {
        // PrimitiveTopology
        VkPipelineInputAssemblyStateCreateInfo m_kInputAssemblyState;
        // BlendState
        VkPipelineColorBlendAttachmentState m_kBlendState;
        // DepthStencilState
        VkPipelineDepthStencilStateCreateInfo m_kDepthStencilState;
        // Rasterization
        VkPipelineRasterizationStateCreateInfo m_kRasterizationState;
        // RenderPass Key
        VkRenderPass m_kRenderPass;
        // SubPass index
        unsigned char m_nSubpassIndex;
    };
    SVulkanState m_kState;
    unsigned int m_nStateHash;
    mutable bool m_bDirty;
public:
    CVulkanRenderState();

    virtual ~CVulkanRenderState();

    virtual bool SetPrimitiveTopology(EPrimitiveType _ePrimitiveType);

    virtual bool SetPolygonMode(EPolygonMode _eMode);

    virtual bool SetCullMode(ECullMode _eMode);

    virtual bool SetFrontFace(EFrontFace _eMode);

    virtual void SetDepthOpMask(bool _bEnable);

    virtual bool SetDepthCompareType(EDepthTestOp _eType);

    virtual void SetBlendEnable(bool _bEnable);

    virtual bool SetBlendFactor(EBlendFactor _eFactor, bool _bSrc, bool _bAlpha);

    virtual bool SetBlendOp(EBlendFuncOp _eOp, bool _bAlpha);

    virtual void SetDepthTestEnable(bool _bEnable);

    virtual void SetAlphaTest(bool _bEnable);

    virtual void SetCullFaceEnable(bool _bEnable);

    virtual bool SetAlphaTestRef(float _fRef);

    virtual bool SetAlphaTestMode(EAlphaTestOp _eMode);

    // PrimitiveTopology
    const VkPipelineInputAssemblyStateCreateInfo& GetInputAssemblyState() const
    {
        return m_kState.m_kInputAssemblyState;
    }
    // BlendState
    const VkPipelineColorBlendAttachmentState& GetBlendState() const
    {
        return m_kState.m_kBlendState;
    }
    // DepthStencilState
    const VkPipelineDepthStencilStateCreateInfo& GetDepthStencilState() const
    {
        return m_kState.m_kDepthStencilState;
    }
    // Rasterization
    const VkPipelineRasterizationStateCreateInfo& GetRasterizationState() const
    {
        return m_kState.m_kRasterizationState;
    }

    void CalculateStateHash();

    bool IsDataModified() const;

    inline unsigned int GetStateHash() const
    {
        return m_nStateHash;
    }

    inline void SetRenderPass(VkRenderPass _kRenderPass)
    {
        m_bDirty |= (m_kState.m_kRenderPass != _kRenderPass);
        m_kState.m_kRenderPass = _kRenderPass;
    }

    inline VkRenderPass GetRenderPass()
    {
        return m_kState.m_kRenderPass;
    }

    inline unsigned char GetSubpass()
    {
        return m_kState.m_nSubpassIndex;
    }

    inline void SetSubpass(unsigned char _nSubpass)
    {
        m_bDirty |= (m_kState.m_nSubpassIndex != _nSubpass);
        m_kState.m_nSubpassIndex = _nSubpass;
    }
};

class CVulkanDescriptorPool
{
    VkDescriptorPool m_kInstance;
    size_t m_nAllsocateSize;
    const size_t m_nPoolSize;
public:
    CVulkanDescriptorPool(size_t _nPoolSize);

    virtual ~CVulkanDescriptorPool();

    bool Allocate(size_t _nSize);

    inline VkDescriptorPool GetInstance()
    {
        return m_kInstance;
    }
};

struct CVulkanDescriptorSet : public CVulkanCmdBufferResource
{
    std::shared_ptr <CVulkanDescriptorPool> m_spkRefDescriptPool;
    VkDescriptorSet  m_akDescriptSet[eShader_Max];
    unsigned int m_nDrawRevision;
public:
    CVulkanDescriptorSet(std::shared_ptr <CVulkanDescriptorPool> spkPool);

    virtual ~CVulkanDescriptorSet();

    inline bool IsCreated(unsigned int nLayoutCount)
    {
        for (unsigned int nIndex = 0; nIndex < nLayoutCount; nIndex++)
        {
            if (m_akDescriptSet[nIndex] == VK_NULL_HANDLE)
                return false;
        }
        return true;
    }

    virtual void OnRelease(class CVulkanRenderer& _rkRenderer);

    void Reset(VkDevice kDevice);

    bool CreateSet(VkDevice kDevice, VkDescriptorSetLayout* pkLayout, unsigned int nLayoutSize);
};

class CVulkanMaterial : public CGLSLMaterial
{
    class CVulkanRenderer& m_rkRenderer;
    
    struct SVulkanPipelineInstance : public CVulkanCmdBufferResource
    {
        VkPipeline m_kPipeline;
        unsigned int m_nHash;
        SVulkanPipelineInstance()
            :m_kPipeline(VK_NULL_HANDLE)
            ,m_nHash(UINT_MAX)
        {}

        virtual void OnRelease(class CVulkanRenderer& _rkRenderer);
    };

    struct SVulkanPipelineLayoutInstance : public CVulkanCmdBufferResource
    {
        VkDescriptorSetLayout m_kDescSetLayout[eShader_Max];
        VkPipelineLayout m_kLayout;

        SVulkanPipelineLayoutInstance()
            :m_kLayout(VK_NULL_HANDLE)
        {
            memset(m_kDescSetLayout, 0, sizeof(m_kDescSetLayout));
        }

        virtual void OnRelease(class CVulkanRenderer& _rkRenderer);
    };

    CVulkanRenderState m_kRenderState;
    
    std::map <unsigned int, std::shared_ptr <SVulkanPipelineInstance> > m_kPipelines;
#if 0
    std::vector <std::shared_ptr <CVulkanDescriptorPool> > m_kDescSetPool;
    CRecyclePool <std::shared_ptr <CVulkanDescriptorSet>, unsigned int> m_kDescSetAllocator;
#endif
    std::shared_ptr <SVulkanPipelineInstance> m_spkInstance;
    std::shared_ptr <SVulkanPipelineInstance> m_spkBaseInstance;
    std::shared_ptr <SVulkanPipelineLayoutInstance> m_spkPipelineLayout;

    size_t m_nStageUniformBufferSize[eShader_Max];

    bool ValidPipeline();

    bool ValidPipelineLayout();
public:
    CVulkanMaterial(class CVulkanRenderSystem& _rkRenderer, class CGLSLMaterialSystem& _rkSystem);

    virtual ~CVulkanMaterial();

    virtual bool LinkShader(IBuffer* _pkAttr);
    
    bool SetRenderState(IRenderState* _pkState);

    const CVulkanRenderState& GetPipelineState() const
    {
        return m_kRenderState;
    }

    void SetSubpass(unsigned char _nSubpass);

    inline size_t GetStageUBOSize(int _nStage)
    {
        if (_nStage >= eShader_Max)
            return 0;
        if (_nStage < eShader_Vertex)
            return 0;
        return m_nStageUniformBufferSize[_nStage];
    }
};