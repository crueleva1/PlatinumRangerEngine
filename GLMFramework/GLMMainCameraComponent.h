#pragma once


#include "GLMBaseCameraComponent.h"


class ECSGLM_API CGLMMainCameraComponent : public CGLMBaseCameraComponent
{
    size_t m_nNameHash;
public:
    CGLMMainCameraComponent();

    virtual ~CGLMMainCameraComponent();

    virtual void UpdateViewMatrix(class ISceneComponent& _rkComp);

    virtual void UpdateViewMatrix(class ISceneComponent& _rkComp, glm::vec3& _rkLookAt);

    virtual size_t GetCameraNameHash();

    virtual void UpdateCameraName(class ISceneComponent& _rkComp);
};