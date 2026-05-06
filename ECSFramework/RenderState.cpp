#include "ECSFrameworkPCH.h"
#include "RenderState.h"


CRenderState::CRenderState()
{
    m_fAlphaTestRef = 0.0f;
    m_eAlphaTestMode = eATO_Less;
    m_ePrivitiveType = ePrimitive_Triangles;
    m_ePolygonMode = ePM_Fill;
    m_eCullMode = eCM_Back;
    m_eFrontFace = eFF_CounterClockWise;
    m_eDepthCompareType = eDTO_Less;
    m_eSrcAlphaBlendFactor = eBF_One;
    m_eSrcBlendFactor = eBF_SrcAlpha;
    m_eDstAlphaBlendFactor = eBF_OneMinusSrcAlpha;
    m_eDstBlendFactor = eBF_OneMinusSrcAlpha;
    m_eAlphaBlendOp = eBFO_Add;
    m_eBlendOp = eBFO_Add;
    m_bBlendEnable = false;
    m_bSteniclTest = true;
    m_bDepthTest = true;
    m_bAlphaTest = true;
    m_bCullFaceEnable = true;
    m_bDepthWrite = true;
}

CRenderState::~CRenderState()
{
}

EPrimitiveType CRenderState::GetPrimitiveTopology() const
{
    return m_ePrivitiveType;
}

bool CRenderState::SetPrimitiveTopology(EPrimitiveType _ePrimitiveType)
{
    switch (_ePrimitiveType)
    {
        default:
        case ePrimitive_Max:
        case ePrimitive_None:
            return false;
        case ePrimitive_Points:
        case ePrimitive_Lines:
        case ePrimitive_LineStrip:
        case ePrimitive_Triangles:
        case ePrimitive_TriangleStrip:
        case ePrimitive_TriangleFan:
            break;
    }
    m_ePrivitiveType = _ePrimitiveType;
    return true;
}

EPolygonMode CRenderState::GetPolygonMode() const
{
    return m_ePolygonMode;
}

bool CRenderState::SetPolygonMode(EPolygonMode _eMode)
{
    switch (_eMode)
    {
        case ePM_Point:
        case ePM_Line:
        case ePM_Fill:
            break;
        default:
            return false;
    }
    m_ePolygonMode = _eMode;
    return true;
}

ECullMode CRenderState::GetCullMode() const
{
    return m_eCullMode;
}

bool CRenderState::SetCullMode(ECullMode _eMode)
{
    switch (_eMode)
    {
        case eCM_Front:
        case eCM_Back:
        case eCM_FrontAndBack:
            break;
        default:
            return false;
    }
    m_eCullMode = _eMode;
    return true;
}

EFrontFace CRenderState::GetFrontFace() const
{
    return m_eFrontFace;
}

bool CRenderState::SetFrontFace(EFrontFace _eMode)
{
    switch (_eMode)
    {
        case eFF_ClockWise:
        case eFF_CounterClockWise:
        default:
            return false;
    }
    m_eFrontFace = _eMode;
    return true;
}

bool CRenderState::GetDepthOpMask() const
{
    return m_bDepthWrite;
}

void CRenderState::SetDepthOpMask(bool _bEnable)
{
    m_bDepthWrite = _bEnable;
}

EDepthTestOp CRenderState::GetDepthCompareType() const
{
    return m_eDepthCompareType;
}

bool CRenderState::SetDepthCompareType(EDepthTestOp _eType)
{
    switch (_eType) 
    {
        case eDTO_Never:
        case eDTO_Less:
        case eDTO_Equal:
        case eDTO_LessEqual:
        case eDTO_Greater:
        case eDTO_NonEqual:
        case eDTO_GreaterEqual:
        case eDTO_Always:
            break;
        default:
            return false;
    }

    m_eDepthCompareType = _eType;
    return true;
}

bool CRenderState::IsBlendEnable() const
{
    return m_bBlendEnable;
}


void CRenderState::SetBlendEnable(bool _bEnable)
{
    m_bBlendEnable = _bEnable;
}

EBlendFactor CRenderState::GetBlendFactor(bool _bSrc, bool _bAlpha) const
{
    if (_bSrc) {
        if (_bAlpha) {
            return m_eSrcAlphaBlendFactor;
        }
        return m_eSrcBlendFactor;
    }
    
    if (_bAlpha) {
        return m_eDstAlphaBlendFactor;
    }
    return m_eDstBlendFactor;
}

bool CRenderState::SetBlendFactor(EBlendFactor _eFactor, bool _bSrc, bool _bAlpha)
{
    if (_bSrc) {
        switch (_eFactor)
        {
            case eBF_One:
            case eBF_Zero:
            case eBF_SrcColor:
            case eBF_DstColor:
            case eBF_OneMinusSrcColor:
            case eBF_OneMinusDstColor:
            case eBF_SrcAlpha:
            case eBF_DstAlpha:
            case eBF_OneMinusSrcAlpha:
            case eBF_OneMinusDstAlpha:
            case eBF_SrcAlphaSat:
                break;
            default:
                return false;
        }
        if (_bAlpha) {
            m_eSrcAlphaBlendFactor = _eFactor;
        }
        else {
            m_eSrcBlendFactor = _eFactor;
        }
    }
    else {
        switch (_eFactor)
        {
            case eBF_One:
            case eBF_Zero:
            case eBF_SrcColor:
            case eBF_DstColor:
            case eBF_OneMinusSrcColor:
            case eBF_OneMinusDstColor:
            case eBF_SrcAlpha:
            case eBF_DstAlpha:
            case eBF_OneMinusSrcAlpha:
            case eBF_OneMinusDstAlpha:
            case eBF_SrcAlphaSat:
                break;
            default:
                return false;
        }
        if (_bAlpha) {
            m_eDstAlphaBlendFactor = _eFactor;
        }
        else {
            m_eDstBlendFactor = _eFactor;
        }
    }
    return true;
}

EBlendFuncOp CRenderState::GetBlendOp(bool _bAlpha) const
{
    if (_bAlpha) {
        return m_eAlphaBlendOp;
    }
    return m_eBlendOp;
}

bool CRenderState::SetBlendOp(EBlendFuncOp _eOp, bool _bAlpha)
{
    switch (_eOp)
    {
        case eBFO_Add:
        case eBFO_Sub:
        case eBFO_ReverseSub:
            break;
        default:
            return false;
    }

    if (_bAlpha) {
        m_eAlphaBlendOp = _eOp;
        return true;
    }
    m_eBlendOp = _eOp;
    return true;
}

bool CRenderState::IsDepthTestEnable() const
{
    return m_bDepthTest;
}

void CRenderState::SetDepthTestEnable(bool _bEnable)
{
    m_bDepthTest = _bEnable;
}

bool CRenderState::IsAlphaTestEnable() const
{
    return m_bAlphaTest;
}

void CRenderState::SetAlphaTest(bool _bEnable)
{
    m_bAlphaTest = _bEnable;
}

bool CRenderState::IsCullFaceEnable() const
{
    return m_bCullFaceEnable;
}

void CRenderState::SetCullFaceEnable(bool _bEnable)
{
    m_bCullFaceEnable = _bEnable;
}

float CRenderState::GetAlphaTestRef() const
{
    return m_fAlphaTestRef;
}

bool CRenderState::SetAlphaTestRef(float _fRef)
{
    if (_fRef < 0.0f || _fRef > 1.0f)
        return false;
    m_fAlphaTestRef = _fRef;
    return true;
}

EAlphaTestOp CRenderState::GetAlphaTestMode() const
{
    return m_eAlphaTestMode;
}

bool CRenderState::SetAlphaTestMode(EAlphaTestOp _eMode)
{
    switch (_eMode)
    {
        case eATO_Never:
        case eATO_Less:
        case eATO_Equal:
        case eATO_LessEqual:
        case eATO_Greater:
        case eATO_NonEqual:
        case eATO_GreaterEqual:
        case eATO_Always:
            break;
        default:
            return false;
    }

    m_eAlphaTestMode = _eMode;
    return true;
}