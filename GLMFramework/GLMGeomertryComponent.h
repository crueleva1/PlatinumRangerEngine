#pragma once


#include "GeomertryComponent.h"



class ECSGLM_API CGLMGeomertryComponent : public CGeomertryComponent
{
public:
    CGLMGeomertryComponent();

    virtual ~CGLMGeomertryComponent();

    virtual void UpdateWorldBound(class ISceneComponent& _rkComp);
};