#pragma once


struct SColor
{
    float m_fR, m_fG, m_fB;
    SColor()
        : m_fR(0.0f)
        , m_fG(0.0f)
        , m_fB(0.0f)
    {}
    SColor(float _fR, float _fG, float _fB)
        : m_fR(_fR)
        , m_fG(_fG)
        , m_fB(_fB)
    {}
};

class ECS_API ILightComponent
{
    SColor m_kDiffuse;
    SColor m_kAmbient;
    SColor m_kSpecular;
    float m_nDimmer;
    bool m_bHasBody;
public:
    ILightComponent()
        : m_nDimmer(1.0f)
        , m_bHasBody(false)
    {}

    virtual ~ILightComponent()
    {
    }

    virtual int GetLightType() = 0;

    inline float GetLightDimmer()
    {
        return m_nDimmer;
    }

    inline void SetLightDimmer(float _fDimmer)
    {
        m_nDimmer = _fDimmer;
    }

    inline const SColor& GetDiffuseColor()
    {
        return m_kDiffuse;
    }

    inline const SColor& GetAmbientColor()
    {
        return m_kAmbient;
    }

    inline const SColor& GetSpecularColor()
    {
        return m_kSpecular;
    }

    inline bool IsHasLightBody()
    {
        return m_bHasBody;
    }

    inline void SetDiffuseColor(const SColor& _rkColor)
    {
        m_kDiffuse = _rkColor;
    }

    inline void SetAmbientColor(const SColor& _rkColor)
    {
        m_kAmbient = _rkColor;
    }

    inline void SetSpecularColor(const SColor& _rkColor)
    {
        m_kSpecular = _rkColor;
    }

    inline void SetLightBody(bool _bEnable)
    {
        m_bHasBody = _bEnable;
    }
};

typedef TComponent<ILightComponent> ILightHandler;