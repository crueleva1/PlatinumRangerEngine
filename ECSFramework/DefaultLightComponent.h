#pragma once

#include "ILightComponent.h"

enum ELightType
{
    eLT_Point,
    eLT_Direct,
    eLT_Spot,
};

class ECS_API CPointLightComponent : public ILightComponent
{
    float m_nLightBodyRadius;
    float m_nRadius;
public:
    CPointLightComponent();

    virtual ~CPointLightComponent();

    virtual int GetLightType();

    float GetRadius();

    float GetLightBodyRadius();

    void SetRadius(float _nRadius);

    void SetLightBodyRadius(float _nRadius);
};