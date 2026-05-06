#include "GLMMainPCH.h"
#include "GLMGeomertryComponent.h"
#include "GLMSceneComponent.h"
#include "GLMBoxBound.h"

CGLMGeomertryComponent::CGLMGeomertryComponent()
{

}

CGLMGeomertryComponent::~CGLMGeomertryComponent()
{

}

void CGLMGeomertryComponent::UpdateWorldBound(class ISceneComponent& _rkComp)
{
    CGLMSceneComponent* pkSceneComp = dynamic_cast <CGLMSceneComponent*>(&_rkComp);
    if (!pkSceneComp)
        return;
    if (m_kGeometryDatas.empty())
        return;

    for (auto& rkBuffer : m_kGeometryDatas) {
        CGLMBoundBox* pkBoundBox = dynamic_cast <CGLMBoundBox*>(rkBuffer.m_kBound.get());
        if (!pkBoundBox)
            continue;
        pkBoundBox->SetWorldTransform(pkSceneComp->GetTransform());
    }
}