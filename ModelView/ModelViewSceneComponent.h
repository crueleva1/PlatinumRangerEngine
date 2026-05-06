#pragma once


#include "GLMSceneComponent.h"


class ECS_MODELVIEW_API CModelViewSceneComponent : public CGLMSceneComponent
{
public:
    CModelViewSceneComponent(entityx::Entity& _rkEntity, ISceneSystem& _rkSceneSys);

    virtual ~CModelViewSceneComponent();

    virtual void Update(float _fDelta);

    virtual void UpdateMaterialConstant(IMaterial* _pkMaterial);
};