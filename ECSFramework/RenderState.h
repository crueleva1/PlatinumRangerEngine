#pragma once

#include "DataModifier.h"
#include "IRenderComponent.h"



class ECS_API CRenderState : public IRenderState
{
    TDataModifier <float> m_fAlphaTestRef;
    TDataModifier <EAlphaTestOp> m_eAlphaTestMode;
    TDataModifier <EPolygonMode> m_ePolygonMode;
    TDataModifier <ECullMode> m_eCullMode;
    TDataModifier <EFrontFace> m_eFrontFace;
    TDataModifier <EDepthTestOp> m_eDepthCompareType;
    TDataModifier <EBlendFactor> m_eSrcAlphaBlendFactor;
    TDataModifier <EBlendFactor> m_eSrcBlendFactor;
    TDataModifier <EBlendFactor> m_eDstAlphaBlendFactor;
    TDataModifier <EBlendFactor> m_eDstBlendFactor;
    TDataModifier <EBlendFuncOp> m_eAlphaBlendOp;
    TDataModifier <EBlendFuncOp> m_eBlendOp;
    TDataModifier <bool> m_bBlendEnable;
    TDataModifier <bool> m_bSteniclTest;
    TDataModifier <bool> m_bDepthTest;
    TDataModifier <bool> m_bAlphaTest;
    TDataModifier <bool> m_bCullFaceEnable;
    TDataModifier <bool> m_bDepthWrite;
    EPrimitiveType m_ePrivitiveType;

public:
    CRenderState();

    virtual ~CRenderState();

    virtual EPrimitiveType GetPrimitiveTopology() const;

    virtual bool SetPrimitiveTopology(EPrimitiveType _ePrimitiveType);

    virtual EPolygonMode GetPolygonMode() const;

    virtual bool SetPolygonMode(EPolygonMode _eMode);

    virtual ECullMode GetCullMode() const;

    virtual bool SetCullMode(ECullMode _eMode);

    virtual EFrontFace GetFrontFace() const;

    virtual bool SetFrontFace(EFrontFace _eMode);

    virtual bool GetDepthOpMask() const;

    virtual void SetDepthOpMask(bool _bEnable);

    virtual EDepthTestOp GetDepthCompareType() const;

    virtual bool SetDepthCompareType(EDepthTestOp _eType);

    virtual bool IsBlendEnable() const;

    virtual void SetBlendEnable(bool _bEnable);

    virtual EBlendFactor GetBlendFactor(bool _bSrc, bool _bAlpha) const;

    virtual bool SetBlendFactor(EBlendFactor _eFactor, bool _bSrc, bool _bAlpha);

    virtual EBlendFuncOp GetBlendOp(bool _bAlpha) const;

    virtual bool SetBlendOp(EBlendFuncOp _eOp, bool _bAlpha);

    virtual bool IsDepthTestEnable() const;

    virtual void SetDepthTestEnable(bool _bEnable);

    virtual bool IsAlphaTestEnable() const;

    virtual void SetAlphaTest(bool _bEnable);

    virtual bool IsCullFaceEnable() const;

    virtual void SetCullFaceEnable(bool _bEnable);

    virtual float GetAlphaTestRef() const;

    virtual bool SetAlphaTestRef(float _fRef);

    virtual EAlphaTestOp GetAlphaTestMode() const;

    virtual bool SetAlphaTestMode(EAlphaTestOp _eMode);

    virtual bool IsDataModified() const
    {
#define IF_RETURN(x,y,z) if (x==y) return z;
        IF_RETURN(m_ePolygonMode.isDirty(), true, true)
        IF_RETURN(m_eCullMode.isDirty(), true, true)
        IF_RETURN(m_eFrontFace.isDirty(), true, true)
        IF_RETURN(m_bDepthWrite.isDirty(), true, true)
        IF_RETURN(m_eDepthCompareType.isDirty(), true, true)
        IF_RETURN(m_eSrcAlphaBlendFactor.isDirty(), true, true)
        IF_RETURN(m_eSrcBlendFactor.isDirty(), true, true)
        IF_RETURN(m_eDstAlphaBlendFactor.isDirty(), true, true)
        IF_RETURN(m_eDstBlendFactor.isDirty(), true, true)
        IF_RETURN(m_eAlphaBlendOp.isDirty(), true, true)
        IF_RETURN(m_eBlendOp.isDirty(), true, true)
        IF_RETURN(m_bBlendEnable.isDirty(), true, true)
        IF_RETURN(m_bCullFaceEnable.isDirty(), true, true)
        IF_RETURN(m_bDepthTest.isDirty(), true, true)
        IF_RETURN(m_bSteniclTest.isDirty(), true, true)
        IF_RETURN(m_bAlphaTest.isDirty(), true, true)
        IF_RETURN(m_fAlphaTestRef.isDirty(), true, true)
        IF_RETURN(m_eAlphaTestMode.isDirty(), true, true)
#undef IF_RETURN
        return false;
    }

    inline void ClearDirty()
    {
        m_ePolygonMode.clearDirty();
        m_eCullMode.clearDirty();
        m_eFrontFace.clearDirty();
        m_bDepthWrite.clearDirty();
        m_eDepthCompareType.clearDirty();
        m_eSrcAlphaBlendFactor.clearDirty();
        m_eSrcBlendFactor.clearDirty();
        m_eDstAlphaBlendFactor.clearDirty();
        m_eDstBlendFactor.clearDirty();
        m_eAlphaBlendOp.clearDirty();
        m_eBlendOp.clearDirty();
        m_bBlendEnable.clearDirty();
        m_bCullFaceEnable.clearDirty();
        m_bDepthTest.clearDirty();
        m_bSteniclTest.clearDirty();
        m_bAlphaTest.clearDirty();
        m_fAlphaTestRef.clearDirty();
        m_eAlphaTestMode.clearDirty();
    }
};