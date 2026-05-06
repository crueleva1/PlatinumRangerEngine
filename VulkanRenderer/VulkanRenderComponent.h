#pragma once


#include "RenderComponent.h"


VkFormat CovToVulkanType(EVariableSemanticElementType _eType, size_t _nElement);


class CVulkanRenderComponent : public CRenderComponent
{
    class CVulkanRenderSystem& m_rkSystem;
public:
    CVulkanRenderComponent(class CVulkanRenderSystem& _rkSystem);

    virtual ~CVulkanRenderComponent();

    virtual bool SetRenderState(IRenderState* _pkRenderState);

    virtual const IRenderState* GetRenderState() const;

    virtual bool ValidatePipeline(class IRenderSystem&);
};