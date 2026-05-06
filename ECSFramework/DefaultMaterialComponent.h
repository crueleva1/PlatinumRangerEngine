#pragma once

#include "IMaterialComponent.h"

struct IMaterial;
class ECS_API CDefaultMaterialComponent : public IMaterialComponent
{
    IMaterial* m_pkMaterial;
public:
    CDefaultMaterialComponent();

    virtual ~CDefaultMaterialComponent();

    virtual void SetMaterialInstance(IMaterial* _pkMaterial);

    virtual IMaterial* GetMaterialInstance();

};