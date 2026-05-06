#pragma once

#include "DefaultMaterialComponent.h"


class ECS_MODELVIEW_API CPBRMaterialComponent : public CDefaultMaterialComponent
{
    float m_nRoughness;
    float m_nMetallic;
public:
    CPBRMaterialComponent();

    virtual ~CPBRMaterialComponent();

    inline void SetRoughness(float _nRoughness)
    {
        m_nRoughness = _nRoughness;
    }

    inline float GetRoughness()
    {
        return m_nRoughness;
    }

    inline void SetMatallic(float _nMetallic)
    {
        m_nMetallic = _nMetallic;
    }

    inline float GetMetallic()
    {
        return m_nMetallic;
    }

};