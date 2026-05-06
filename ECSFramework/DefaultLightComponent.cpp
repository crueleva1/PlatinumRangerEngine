#include "ECSFrameworkPCH.h"
#include "DefaultLightComponent.h"

CPointLightComponent::CPointLightComponent()
    :ILightComponent()
    ,m_nRadius(0.0f)
    ,m_nLightBodyRadius(0.0f)
{

}

CPointLightComponent::~CPointLightComponent()
{

}

int CPointLightComponent::GetLightType()
{
    return eLT_Point;
}

float CPointLightComponent::GetRadius()
{
    return m_nRadius;
}

float CPointLightComponent::GetLightBodyRadius()
{
    return m_nLightBodyRadius;
}

void CPointLightComponent::SetRadius(float _nRadius)
{
    m_nRadius = _nRadius;
}

void CPointLightComponent::SetLightBodyRadius(float _nRadius)
{
    m_nLightBodyRadius = _nRadius;
}