#include "ECSVulkanPCH.h"
#include "VulkanMaterial.h"
#include "VulkanRenderSystem.h"
#include "VulkanRenderComponent.h"
#include "VulkanMaterialSystem.h"
#include "VulkanCommandBuffer.h"
#include "VulkanBuffer.h"
#include "Crc32.h"

CVulkanShader::CVulkanShader(CVulkanRenderSystem& _rkSystem, EShaderType _eType)
    :CGLSLShader(_eType, EShMsgSpvRules | EShMsgVulkanRules)
    ,m_rkSystem(dynamic_cast<CVulkanRenderer&>(*_rkSystem.GetRenderer()))
{

}

CVulkanShader::~CVulkanShader()
{

}

bool CVulkanShader::SetSource(const char* _pcCode)
{
    if (!CGLSLShader::SetSource(_pcCode))
        return false;

    return true;
}

bool CVulkanShader::Compile()
{
    if (!CGLSLShader::Compile())
        return false;
    
    const glslang::TIntermediate& kIntermediate = m_kShaderParser.GetTIntermediate();
    glslang::GlslangToSpv(kIntermediate, m_kSpv);
    if (m_kSpv.empty())
        return false;
    VkDevice kDevice = m_rkSystem.GetVkDevice();
    if (kDevice == VK_NULL_HANDLE)
        return false;
    VkShaderModuleCreateInfo kInfo = {};
    kInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    kInfo.codeSize = m_kSpv.size() * sizeof(uint32_t);
    kInfo.pCode = m_kSpv.data();
    VkResult eResult = vkCreateShaderModule(kDevice, &kInfo, nullptr, &m_kShader);
    return (eResult == VK_SUCCESS);
}

CVulkanRenderState::CVulkanRenderState()
    :CRenderState()
    ,m_bDirty(true)
{
    memset(&m_kState, 0, sizeof(m_kState));
    m_kState.m_kInputAssemblyState = vkTools::initializers::pipelineInputAssemblyStateCreateInfo(VK_PRIMITIVE_TOPOLOGY_POINT_LIST, 0, false);
    m_kState.m_kBlendState = vkTools::initializers::pipelineColorBlendAttachmentState(0xf, false);
    m_kState.m_kBlendState.alphaBlendOp = VK_BLEND_OP_ADD;
    m_kState.m_kBlendState.colorBlendOp = VK_BLEND_OP_ADD;
    m_kState.m_kBlendState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
    m_kState.m_kBlendState.srcColorBlendFactor = VK_BLEND_FACTOR_DST_ALPHA;
    m_kState.m_kBlendState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    m_kState.m_kBlendState.srcAlphaBlendFactor = VK_BLEND_FACTOR_DST_ALPHA;
    m_kState.m_kRasterizationState = vkTools::initializers::pipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE, 0);
    m_kState.m_kDepthStencilState = vkTools::initializers::pipelineDepthStencilStateCreateInfo(VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS_OR_EQUAL);
    m_kState.m_kRenderPass = VK_NULL_HANDLE;
}

CVulkanRenderState::~CVulkanRenderState()
{

}

bool CVulkanRenderState::SetPrimitiveTopology(EPrimitiveType _ePrimitiveType)
{
    if (!CRenderState::SetPrimitiveTopology(_ePrimitiveType))
        return false;

    switch (_ePrimitiveType)
    {
    case ePrimitive_Points:
        m_kState.m_kInputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
        break;
    case ePrimitive_Lines:
        m_kState.m_kInputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
        break;
    case ePrimitive_LineStrip:
        m_kState.m_kInputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
        break;
    case ePrimitive_Triangles:
        m_kState.m_kInputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        break;
    case ePrimitive_TriangleStrip:
        m_kState.m_kInputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
        break;
    case ePrimitive_TriangleFan:
        m_kState.m_kInputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
        break;
    }
    return true;
}

bool CVulkanRenderState::SetPolygonMode(EPolygonMode _eMode)
{
    if (!CRenderState::SetPolygonMode(_eMode))
        return false;

    switch (_eMode)
    {
        case ePM_Point:
            m_kState.m_kRasterizationState.polygonMode = VK_POLYGON_MODE_POINT;
            break;
        case ePM_Line:
            m_kState.m_kRasterizationState.polygonMode = VK_POLYGON_MODE_LINE;
            break;
        case ePM_Fill:
            m_kState.m_kRasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
            break;
        default:
            return false;
    }

    return true;
}

bool CVulkanRenderState::SetCullMode(ECullMode _eMode)
{
    if (!CRenderState::SetCullMode(_eMode))
        return false;

    switch (_eMode)
    {
        case eCM_Front:
            m_kState.m_kRasterizationState.cullMode = VK_CULL_MODE_FRONT_BIT;
            break;
        case eCM_Back:
            m_kState.m_kRasterizationState.cullMode = VK_CULL_MODE_BACK_BIT;
            break;
        case eCM_FrontAndBack:
            m_kState.m_kRasterizationState.cullMode = VK_CULL_MODE_FRONT_AND_BACK;
            break;
        default:
            return false;
    }
    return true;
}

bool CVulkanRenderState::SetFrontFace(EFrontFace _eMode)
{
    if (!CRenderState::SetFrontFace(_eMode))
        return false;
    switch (_eMode)
    {
        case eFF_ClockWise:
            m_kState.m_kRasterizationState.frontFace = VK_FRONT_FACE_CLOCKWISE;
            break;
        case eFF_CounterClockWise:
            m_kState.m_kRasterizationState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
            break;
        default:
            return false;
    }
    return true;
}

void CVulkanRenderState::SetDepthOpMask(bool _bEnable)
{
    CRenderState::SetDepthOpMask(_bEnable);
    m_kState.m_kDepthStencilState.depthWriteEnable = _bEnable;
}

bool CVulkanRenderState::SetDepthCompareType(EDepthTestOp _eType)
{
    if (!CRenderState::SetDepthCompareType(_eType))
        return false;
    switch (_eType) 
    {
        case eDTO_Never:
            m_kState.m_kDepthStencilState.depthCompareOp = VK_COMPARE_OP_NEVER;
            break;
        case eDTO_Less:
            m_kState.m_kDepthStencilState.depthCompareOp = VK_COMPARE_OP_LESS;
            break;
        case eDTO_Equal:
            m_kState.m_kDepthStencilState.depthCompareOp = VK_COMPARE_OP_EQUAL;
            break;
        case eDTO_LessEqual:
            m_kState.m_kDepthStencilState.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
            break;
        case eDTO_Greater:
            m_kState.m_kDepthStencilState.depthCompareOp = VK_COMPARE_OP_GREATER;
            break;
        case eDTO_NonEqual:
            m_kState.m_kDepthStencilState.depthCompareOp = VK_COMPARE_OP_NOT_EQUAL;
            break;
        case eDTO_GreaterEqual:
            m_kState.m_kDepthStencilState.depthCompareOp = VK_COMPARE_OP_GREATER_OR_EQUAL;
            break;
        case eDTO_Always:
            m_kState.m_kDepthStencilState.depthCompareOp = VK_COMPARE_OP_ALWAYS;
            break;
        default:
            return false;
    }
    return true;
}

void CVulkanRenderState::SetBlendEnable(bool _bEnable)
{
    CRenderState::SetBlendEnable(_bEnable);
    m_kState.m_kBlendState.blendEnable = _bEnable;
}

bool CVulkanRenderState::SetBlendFactor(EBlendFactor _eFactor, bool _bSrc, bool _bAlpha)
{
    if (!CRenderState::SetBlendFactor(_eFactor, _bSrc, _bAlpha))
        return false;
    VkBlendFactor eFector;
    switch (_eFactor)
    {
        case eBF_One:
            eFector = VK_BLEND_FACTOR_ONE;
            break;
        case eBF_Zero:
            eFector = VK_BLEND_FACTOR_ZERO;
            break;
        case eBF_SrcColor:
            eFector = VK_BLEND_FACTOR_SRC_COLOR;
            break;
        case eBF_DstColor:
            eFector = VK_BLEND_FACTOR_DST_COLOR;
            break;
        case eBF_OneMinusSrcColor:
            eFector = VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
            break;
        case eBF_OneMinusDstColor:
            eFector = VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
            break;
        case eBF_SrcAlpha:
            eFector = VK_BLEND_FACTOR_SRC_ALPHA;
            break;
        case eBF_DstAlpha:
            eFector = VK_BLEND_FACTOR_DST_ALPHA;
            break;
        case eBF_OneMinusSrcAlpha:
            eFector = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
            break;
        case eBF_OneMinusDstAlpha:
            eFector = VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
            break;
        case eBF_SrcAlphaSat:
            eFector = VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
            break;
        default:
            return false;
    }
    if (_bAlpha) {
        (_bSrc) ? m_kState.m_kBlendState.srcAlphaBlendFactor = eFector : m_kState.m_kBlendState.dstAlphaBlendFactor = eFector;
    }
    (_bSrc) ? m_kState.m_kBlendState.srcColorBlendFactor = eFector : m_kState.m_kBlendState.dstColorBlendFactor = eFector;
    return true;
}

bool CVulkanRenderState::SetBlendOp(EBlendFuncOp _eOp, bool _bAlpha)
{
    if (!CRenderState::SetBlendOp(_eOp, _bAlpha))
        return false;
    VkBlendOp eOp;
    switch (_eOp)
    {
        case eBFO_Add:
            eOp = VK_BLEND_OP_ADD;
            break;
        case eBFO_Sub:
            eOp = VK_BLEND_OP_SUBTRACT;
            break;
        case eBFO_ReverseSub:
            eOp = VK_BLEND_OP_REVERSE_SUBTRACT;
            break;
        default:
            return false;
    }
    (_bAlpha) ? m_kState.m_kBlendState.alphaBlendOp = eOp : m_kState.m_kBlendState.colorBlendOp = eOp;
    return true;
}

void CVulkanRenderState::SetDepthTestEnable(bool _bEnable)
{
    CRenderState::SetDepthTestEnable(_bEnable);
    m_kState.m_kDepthStencilState.depthTestEnable = _bEnable;
}

void CVulkanRenderState::SetAlphaTest(bool _bEnable)
{
    CRenderState::SetAlphaTest(_bEnable);
}

void CVulkanRenderState::SetCullFaceEnable(bool _bEnable)
{
    CRenderState::SetCullFaceEnable(_bEnable);
}

bool CVulkanRenderState::SetAlphaTestRef(float _fRef)
{
    if (!CRenderState::SetAlphaTestRef(_fRef))
        return false;
    return true;
}

bool CVulkanRenderState::SetAlphaTestMode(EAlphaTestOp _eMode)
{
    if (!CRenderState::SetAlphaTestMode(_eMode))
        return false;
    return true;
}

void CVulkanRenderState::CalculateStateHash()
{
    m_nStateHash = crc32_fast(&m_kState, sizeof(m_kState));
}

bool CVulkanRenderState::IsDataModified() const
{
    m_bDirty |= CRenderState::IsDataModified();
    return m_bDirty;
}

CVulkanDescriptorPool::CVulkanDescriptorPool(size_t _nPoolSize)
    : m_kInstance(VK_NULL_HANDLE)
    , m_nAllsocateSize(0)
    , m_nPoolSize(_nPoolSize)
{
}

CVulkanDescriptorPool::~CVulkanDescriptorPool()
{
}

bool CVulkanDescriptorPool::Allocate(size_t _nSize)
{
    size_t nAllocatedSize = m_nAllsocateSize + _nSize;
    if (nAllocatedSize > m_nPoolSize)
        return false;

    m_nAllsocateSize = nAllocatedSize;
    return true;
}

CVulkanDescriptorSet::CVulkanDescriptorSet(std::shared_ptr <CVulkanDescriptorPool> spkPool)
    : m_spkRefDescriptPool(spkPool)
    , m_nDrawRevision(-1)
{
    memset(m_akDescriptSet, VK_NULL_HANDLE, _ARRAYSIZE(m_akDescriptSet) * sizeof(VkDescriptorSet));
}

CVulkanDescriptorSet::~CVulkanDescriptorSet()
{

}

void CVulkanDescriptorSet::OnRelease(class CVulkanRenderer& _rkRenderer)
{
}

void CVulkanDescriptorSet::Reset(VkDevice kDevice)
{
#if 0       // Free by pool
    const unsigned int nArraySize = _ARRAYSIZE(m_akDescriptSet);
    for (unsigned int nIndex = 0; nIndex < nArraySize; nIndex++)
    {
        if (m_akDescriptSet[nIndex])
        {
            vkFreeDescriptorSets(kDevice, m_pkDescriptPool.m_kInstance, 1, &m_akDescriptSet[nIndex]);
        }
    }
#endif
    memset(m_akDescriptSet, VK_NULL_HANDLE, _ARRAYSIZE(m_akDescriptSet) * sizeof(VkDescriptorSet));
    m_nDrawRevision = -1;
    m_spkRefDescriptPool = NULL;
}

bool CVulkanDescriptorSet::CreateSet(VkDevice kDevice, VkDescriptorSetLayout* pkLayout, unsigned int nLayoutSize)
{
    if (!m_spkRefDescriptPool)
        return false;
    if (!m_spkRefDescriptPool->Allocate(nLayoutSize))
        return false;
    VkDescriptorSetAllocateInfo kDescAllocInfo = {};
    kDescAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    kDescAllocInfo.pNext = NULL;
    kDescAllocInfo.descriptorPool = m_spkRefDescriptPool->GetInstance();
    kDescAllocInfo.pSetLayouts = pkLayout;
    kDescAllocInfo.descriptorSetCount = nLayoutSize;
    VkResult eResult = vkAllocateDescriptorSets(kDevice, &kDescAllocInfo, m_akDescriptSet);
    return (eResult == VK_SUCCESS);
}

void CVulkanMaterial::SVulkanPipelineInstance::OnRelease(class CVulkanRenderer& _rkRenderer)
{
    VkDevice kDevice = _rkRenderer.GetVkDevice();
    if (m_kPipeline != VK_NULL_HANDLE) {
        vkDestroyPipeline(kDevice, m_kPipeline, nullptr);
    }
}

void CVulkanMaterial::SVulkanPipelineLayoutInstance::OnRelease(class CVulkanRenderer& _rkRenderer)
{
    VkDevice kDevice = _rkRenderer.GetVkDevice();
    const size_t nSize = _ARRAYSIZE(m_kDescSetLayout);
    for (size_t nIndex = 0; nIndex < nSize; nIndex++) {
        VkDescriptorSetLayout kLayout = m_kDescSetLayout[nIndex];
        if (kLayout == VK_NULL_HANDLE)
            continue;
        vkDestroyDescriptorSetLayout(kDevice, kLayout, nullptr);
    }
    if (m_kLayout != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(kDevice, m_kLayout, nullptr);
    }
}

CVulkanMaterial::CVulkanMaterial(CVulkanRenderSystem& _rkRenderSystem, CGLSLMaterialSystem& _rkSystem)
    : CGLSLMaterial(_rkSystem)
    , m_rkRenderer(dynamic_cast<CVulkanRenderer&>(*_rkRenderSystem.GetRenderer()))
{
    memset(m_nStageUniformBufferSize, 0, sizeof(m_nStageUniformBufferSize));
}

CVulkanMaterial::~CVulkanMaterial()
{

}

bool CVulkanMaterial::ValidPipeline()
{
    if (m_spkInstance && m_spkInstance->m_nHash == m_kRenderState.GetStateHash())
    {
        if (m_spkInstance->m_kPipeline != VK_NULL_HANDLE)
            return true;
    }
    m_spkInstance = m_kPipelines[m_kRenderState.GetStateHash()];
    return (m_spkInstance->m_kPipeline != VK_NULL_HANDLE);
}

bool CVulkanMaterial::ValidPipelineLayout()
{
    if (m_bRelink == false)
        return false;
    // set / binding / LayoutType
    if (m_spkPipelineLayout != nullptr) {
        m_rkRenderer.ReleaseResource(m_spkPipelineLayout);
    }

    m_spkPipelineLayout = std::make_shared<SVulkanPipelineLayoutInstance>();

    VkDevice kDevice = m_rkRenderer.GetVkDevice();

    std::vector <VkDescriptorSetLayoutBinding> kSetLayoutBinding[eShader_Max];
    std::map <unsigned int, VkDescriptorType> kUniformBlockBinding[eShader_Max];
    const size_t nCount = GetConstantCount();
    for (size_t nIndex = 0; nIndex != nCount; nIndex++) {
        IConstantSemantic* pkSematic = GetConstant(nIndex);
        IVulkanUniformInterface* pkVUI = dynamic_cast <IVulkanUniformInterface*>(pkSematic);
        if (pkVUI == nullptr)
            continue;
        VkDescriptorType eType;
        glslang::TBasicType eBasicType = (glslang::TBasicType)pkVUI->GetBasicType();
        switch (eBasicType)
        {
        case glslang::EbtSampler:
            eType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            break;
        case glslang::EbtStruct:
            eType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            break;
        case glslang::EbtBlock:
        {
            eType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            m_nStageUniformBufferSize[pkVUI->GetDescriptorSetID()] += pkSematic->GetDescSize();
        }
            break;
        default:
            continue;
        }
        kUniformBlockBinding[pkVUI->GetDescriptorSetID()].insert(std::make_pair(pkVUI->GetDescriptorSetBinding(), eType));
    }
    static const VkShaderStageFlagBits aeShaderFlags[eShader_Max] =
    {
        VK_SHADER_STAGE_VERTEX_BIT,
        VK_SHADER_STAGE_GEOMETRY_BIT,
        VK_SHADER_STAGE_FRAGMENT_BIT,
    };
    for (unsigned int eShader = eShader_Vertex; eShader < eShader_Max; eShader++) {
        std::map <unsigned int, VkDescriptorType >& kBindings = kUniformBlockBinding[eShader];
        std::map <unsigned int, VkDescriptorType >::iterator kIt = kBindings.begin();
        std::vector <VkDescriptorSetLayoutBinding> kSetLayout(kBindings.size());
        for (unsigned int nIndex = 0; kIt != kBindings.end(); ++kIt, nIndex++) {
            VkDescriptorSetLayoutBinding& kSetBinding = kSetLayout[nIndex];
            kSetBinding.descriptorType = kIt->second;
            kSetBinding.binding = kIt->first;
            kSetBinding.descriptorCount = 1;
            kSetBinding.stageFlags = aeShaderFlags[eShader];
        }
        VkDescriptorSetLayoutCreateInfo kInfo = vkTools::initializers::descriptorSetLayoutCreateInfo(kSetLayout.data(), kBindings.size());
        VkResult eResult = vkCreateDescriptorSetLayout(kDevice, &kInfo, nullptr, &(m_spkPipelineLayout->m_kDescSetLayout[eShader]));
        if (eResult != VK_SUCCESS)
            return false;
    }

    VkPipelineLayoutCreateInfo kInfo = vkTools::initializers::pipelineLayoutCreateInfo(m_spkPipelineLayout->m_kDescSetLayout, eShader_Max);
    VkResult eResult = vkCreatePipelineLayout(kDevice, &kInfo, nullptr, &m_spkPipelineLayout->m_kLayout);
    if (eResult != VK_SUCCESS)
        return false;

    return true;
}

bool CVulkanMaterial::LinkShader(IBuffer* _pkAttr)
{
    bool bDirty = m_bRelink;
    VkDevice kDevice = m_rkRenderer.GetVkDevice();
    CVulkanCommandBuffer& rkCmdBuffer = m_rkRenderer.GetActiveCmdBuffer(CVulkanRenderer::eCBT_Draw);
    bDirty |= m_kRenderState.IsDataModified();
    if (!bDirty) {
        if (ValidPipeline()) {
            vkCmdBindPipeline(rkCmdBuffer.GetCmdBufferInstance(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_spkInstance->m_kPipeline);
            m_spkInstance->RefCommandBuffer(&rkCmdBuffer);
            return true;
        }
    }
    // Check
    if (m_bRelink) {
        for (int nIndex = eShader_Vertex; nIndex < eShader_Max; nIndex++) {
            std::shared_ptr <IShader>& spkShader = m_apkShaders [nIndex];
            if (!spkShader)
                continue;
            if (!spkShader->IsCompiled()) {
                if (!spkShader->Compile())
                    return false;
            }
            if (spkShader->IsError())
                return false;
            CVulkanShader* pkVKShader = dynamic_cast <CVulkanShader*>(spkShader.get());
            if (!pkVKShader)
                return false;
        }
    }

    m_kRenderState.CalculateStateHash();
    m_kRenderState.ClearDirty();

    if (ValidPipeline()) {
        vkCmdBindPipeline(rkCmdBuffer.GetCmdBufferInstance(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_spkInstance->m_kPipeline);
        m_spkInstance->RefCommandBuffer(&rkCmdBuffer);
        return true;
    }
    // PipelineLayout
    if (!ValidPipelineLayout()) {
        return false;
    }
    m_bRelink = false;
    // PipelineCache
    VkPipelineCache kPipelineCache = m_rkRenderer.GetPipelineCache();
    // Attribute
    CVulkanVertexBuffer* pkVertexBuffer = dynamic_cast <CVulkanVertexBuffer*>(_pkAttr);
    if (pkVertexBuffer == nullptr)
        return false;

    size_t nOffset = 0;
    unsigned int nInputAttrDescCount = 0;
    const size_t nAttrCount = pkVertexBuffer->GetAttrSemanticCount();
    std::shared_ptr <VkVertexInputAttributeDescription> spakInputAttrDescription(new VkVertexInputAttributeDescription [nAttrCount](), SArrayDeleter<VkVertexInputAttributeDescription>());
    for (size_t nIndex = 0; nIndex < nAttrCount; nIndex++) {
        const IVariableSemantic* pkSemantic = pkVertexBuffer->GetAttrSemantic(nIndex);
        if (!pkSemantic)
            continue;
        IVariableAttrDesc* pkAttr = GetVariableDescByName(pkSemantic->GetName());
        if (!pkAttr)
            continue;
        
        VkVertexInputAttributeDescription* pkDesc = spakInputAttrDescription.get();
        pkDesc[nInputAttrDescCount].binding = 0;    // wait for support instancing
        pkDesc[nInputAttrDescCount].location = pkAttr->GetLoc();
        pkDesc[nInputAttrDescCount].format = CovToVulkanType(pkSemantic->GetElementType(), pkSemantic->GetElementCount());
        pkDesc[nInputAttrDescCount].offset = nOffset;
        nOffset += pkSemantic->GetDescSize();
        nInputAttrDescCount++;
    }
    VkVertexInputBindingDescription kInputBindingDescription;
    kInputBindingDescription.binding = 0;
    kInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    kInputBindingDescription.stride = nOffset;
    VkPipelineVertexInputStateCreateInfo kInputState = vkTools::initializers::pipelineVertexInputStateCreateInfo();
    kInputState.pVertexAttributeDescriptions = spakInputAttrDescription.get();
    kInputState.vertexAttributeDescriptionCount = nInputAttrDescCount;
    kInputState.pVertexBindingDescriptions = &kInputBindingDescription;
    kInputState.vertexBindingDescriptionCount = 1;
    //BlendState
    VkPipelineColorBlendStateCreateInfo kBlendCreateInfo = vkTools::initializers::pipelineColorBlendStateCreateInfo(1, &m_kRenderState.GetBlendState());
    //DynamicState
    VkDynamicState kDynamicStateEnables [2];
    kDynamicStateEnables [0] = VK_DYNAMIC_STATE_VIEWPORT;
    kDynamicStateEnables [1] = VK_DYNAMIC_STATE_SCISSOR;
    VkPipelineDynamicStateCreateInfo kDynamicState = vkTools::initializers::pipelineDynamicStateCreateInfo(kDynamicStateEnables, 1, 0);
    // Multisample
    VkPipelineMultisampleStateCreateInfo kMultisampleState = vkTools::initializers::pipelineMultisampleStateCreateInfo(VK_SAMPLE_COUNT_1_BIT, 0);
    // Viewport State
    VkPipelineViewportStateCreateInfo kViewportState = vkTools::initializers::pipelineViewportStateCreateInfo(1, 1, 0);

    VkGraphicsPipelineCreateInfo kPipelineCreatInfo = vkTools::initializers::pipelineCreateInfo(m_spkPipelineLayout->m_kLayout, m_kRenderState.GetRenderPass(), (m_spkBaseInstance == nullptr) ? VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT : VK_PIPELINE_CREATE_DERIVATIVE_BIT);
    kPipelineCreatInfo.basePipelineHandle = (m_spkBaseInstance == nullptr) ? VK_NULL_HANDLE : m_spkBaseInstance->m_kPipeline;
    kPipelineCreatInfo.basePipelineIndex = (m_spkBaseInstance == nullptr) ? 0 : -1;
    kPipelineCreatInfo.pColorBlendState = &kBlendCreateInfo;
    kPipelineCreatInfo.pDepthStencilState = &m_kRenderState.GetDepthStencilState();
    kPipelineCreatInfo.pDynamicState = &kDynamicState;
    kPipelineCreatInfo.pInputAssemblyState = &m_kRenderState.GetInputAssemblyState();
    kPipelineCreatInfo.pVertexInputState = &kInputState;
    kPipelineCreatInfo.pMultisampleState = &kMultisampleState;
    kPipelineCreatInfo.pViewportState = &kViewportState;
    kPipelineCreatInfo.stageCount = eShader_Max;
    kPipelineCreatInfo.subpass = m_kRenderState.GetSubpass();
    std::shared_ptr <SVulkanPipelineInstance> spkInstance = std::make_shared<SVulkanPipelineInstance>();
    spkInstance->m_nHash = m_kRenderState.GetStateHash();
    VkResult eResult = vkCreateGraphicsPipelines(kDevice, kPipelineCache, 1, &kPipelineCreatInfo, nullptr, &spkInstance->m_kPipeline);
    if (eResult != VK_SUCCESS)
        return false;
    m_kPipelines[spkInstance->m_nHash] = spkInstance;
    if (m_spkBaseInstance == nullptr) {
        m_spkBaseInstance = spkInstance;
    }
    m_spkInstance = spkInstance;
    vkCmdBindPipeline(rkCmdBuffer.GetCmdBufferInstance(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_spkInstance->m_kPipeline);
    m_spkInstance->RefCommandBuffer(&rkCmdBuffer);
    return true;
}

bool CVulkanMaterial::SetRenderState(IRenderState* _pkState)
{
    if (!_pkState)
        return false;
    m_kRenderState.SetBlendEnable(_pkState->IsBlendEnable());
    m_kRenderState.SetBlendFactor(_pkState->GetBlendFactor(false, false), false, false);
    m_kRenderState.SetBlendFactor(_pkState->GetBlendFactor(true, false), true, false);
    m_kRenderState.SetBlendFactor(_pkState->GetBlendFactor(false, true), false, true);
    m_kRenderState.SetBlendFactor(_pkState->GetBlendFactor(true, true), true, true);
    m_kRenderState.SetBlendOp(_pkState->GetBlendOp(false), false);
    m_kRenderState.SetBlendOp(_pkState->GetBlendOp(true), true);
    m_kRenderState.SetCullFaceEnable(_pkState->IsCullFaceEnable());
    m_kRenderState.SetCullMode(_pkState->GetCullMode());
    m_kRenderState.SetDepthTestEnable(_pkState->IsDepthTestEnable());
    m_kRenderState.SetDepthCompareType(_pkState->GetDepthCompareType());
    m_kRenderState.SetDepthOpMask(_pkState->GetDepthOpMask());
    m_kRenderState.SetFrontFace(_pkState->GetFrontFace());
    m_kRenderState.SetPolygonMode(_pkState->GetPolygonMode());
    m_kRenderState.SetPrimitiveTopology(_pkState->GetPrimitiveTopology());
    m_kRenderState.SetAlphaTest(_pkState->IsAlphaTestEnable());
    m_kRenderState.SetAlphaTestRef(_pkState->GetAlphaTestRef());
    return true;
}

void CVulkanMaterial::SetSubpass(unsigned char _nSubpass)
{
    m_kRenderState.SetSubpass(_nSubpass);
}