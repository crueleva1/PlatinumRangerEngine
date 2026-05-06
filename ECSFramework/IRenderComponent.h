#pragma once

#include "DataModifier.h"
#include "IShader.h"


enum EPrimitiveType
{
    ePrimitive_None,
    ePrimitive_Points,
    ePrimitive_Lines,
    ePrimitive_LineStrip,
    ePrimitive_Triangles,
    ePrimitive_TriangleStrip,
    ePrimitive_TriangleFan,
    ePrimitive_Max,
};

enum EPolygonMode
{
    ePM_None,
    ePM_Point,
    ePM_Line,
    ePM_Fill,
    ePM_Max,
};

enum ECullMode
{
    eCM_None,
    eCM_Front,
    eCM_Back,
    eCM_FrontAndBack,
    eCM_Max,
};

enum EFrontFace
{
    eFF_None,
    eFF_ClockWise,
    eFF_CounterClockWise,
    eFF_Max,
};

enum EDepthTestOp
{
    eDTO_None,
    eDTO_Never,
    eDTO_Less,
    eDTO_Equal,
    eDTO_LessEqual,
    eDTO_Greater,
    eDTO_NonEqual,
    eDTO_GreaterEqual,
    eDTO_Always,
    eDTO_Max,
};

enum EBlendFactor
{
    eBF_None,
    eBF_One,
    eBF_Zero,
    eBF_SrcColor,
    eBF_DstColor,
    eBF_OneMinusSrcColor,
    eBF_OneMinusDstColor,
    eBF_SrcAlpha,
    eBF_DstAlpha,
    eBF_OneMinusSrcAlpha,
    eBF_OneMinusDstAlpha,
    eBF_SrcAlphaSat,
    eBF_Max,
};

enum EBlendFuncOp
{
    eBFO_None,
    eBFO_Add,
    eBFO_Sub,
    eBFO_ReverseSub,
    eBFO_Max,
};

enum EAlphaTestOp
{
    eATO_None,
    eATO_Never,
    eATO_Less,
    eATO_Equal,
    eATO_LessEqual,
    eATO_Greater,
    eATO_NonEqual,
    eATO_GreaterEqual,
    eATO_Always,
    eATO_Max,
};

struct ECS_API IRenderState
{
    virtual ~IRenderState()
    {
    }

    virtual EPrimitiveType GetPrimitiveTopology() const = 0;

    virtual bool SetPrimitiveTopology(EPrimitiveType _ePrimitiveType) = 0;

    virtual EPolygonMode GetPolygonMode() const = 0;

    virtual bool SetPolygonMode(EPolygonMode _eMode) = 0;

    virtual ECullMode GetCullMode() const = 0;

    virtual bool SetCullMode(ECullMode _eMode) = 0;

    virtual EFrontFace GetFrontFace() const = 0;

    virtual bool SetFrontFace(EFrontFace _eMode) = 0;

    virtual bool GetDepthOpMask() const = 0;

    virtual void SetDepthOpMask(bool _bEnable) = 0;

    virtual EDepthTestOp GetDepthCompareType() const = 0;

    virtual bool SetDepthCompareType(EDepthTestOp _eType) = 0;

    virtual bool IsBlendEnable() const = 0;

    virtual void SetBlendEnable(bool _bEnable) = 0;

    virtual EBlendFactor GetBlendFactor(bool _bSrc, bool _bAlpha) const = 0;

    virtual bool SetBlendFactor(EBlendFactor _eFector, bool _bSrc, bool _bAlpha) = 0;

    virtual EBlendFuncOp GetBlendOp(bool _bAlpha) const = 0;

    virtual bool SetBlendOp(EBlendFuncOp _eOp, bool _bAlpha) = 0;

    virtual bool IsDepthTestEnable() const = 0;

    virtual void SetDepthTestEnable(bool _bEnable) = 0;

    virtual bool IsAlphaTestEnable() const = 0;

    virtual void SetAlphaTest(bool _bEnable) = 0;

    virtual float GetAlphaTestRef() const = 0;

    virtual bool SetAlphaTestRef(float _fRef) = 0;

    virtual EAlphaTestOp GetAlphaTestMode() const = 0;

    virtual bool SetAlphaTestMode(EAlphaTestOp _eMode) = 0;

    virtual bool IsCullFaceEnable() const = 0;

    virtual void SetCullFaceEnable(bool _bEnable) = 0;
}; 

struct ECS_API IBuffer
{
    virtual ~IBuffer()
    {
    }

    virtual int GetBufferType() = 0;

    virtual void* Map() = 0;

    virtual void UnMap() = 0;

    virtual bool Allocate(size_t _nSize) = 0;

    virtual bool Bind() = 0;

    virtual size_t GetSize() = 0;

};

template <class T = IBuffer>
struct TVertexBuffer : public T
{
    template <class ...Args>
    TVertexBuffer(Args&& ..._Args)
        :T(static_cast <Args&&> (_Args)...)
    {
    }
    virtual ~TVertexBuffer()
    {
    }

    virtual void SetAttrSemanticCount(short _nSize) = 0;

    virtual bool OnSetAttrSemantic(std::shared_ptr <class IVariableSemantic> _spkSemantic, size_t _nIndex) = 0;

    template <class T, class S = TVariableSemantic <T> >
    bool SetAttrSemantic(const char* _pcSemantic, size_t _nIndex, int _nElementCount, EVariableSemanticElementType _eType)
    {
        std::shared_ptr <class IVariableSemantic> spkSemantic = std::make_shared <S>(_pcSemantic, _nIndex, _nElementCount, _eType);
        return OnSetAttrSemantic(spkSemantic, _nIndex);
    }

    virtual size_t GetAttrSemanticCount() = 0;

    virtual const class IVariableSemantic* GetAttrSemantic(size_t _nIndex) = 0;

    virtual size_t GetOffset() = 0;

    virtual void SetOffset(size_t _nOffset) = 0;

    virtual bool SetVertexBuffer(struct IMaterial*) = 0;

    virtual size_t GetAttrSemanticSize() = 0;
};
typedef TVertexBuffer<> IVertexBuffer;

template <class T = IBuffer>
struct TIndiceBuffer : public T
{
    template <class ...Args>
    TIndiceBuffer(Args&& ..._Args)
        :T(static_cast <Args&&> (_Args)...)
    {
    }

    virtual ~TIndiceBuffer()
    {
    }

    virtual int GetIndicesType() = 0;

    virtual size_t GetElementSize() = 0;
};
typedef TIndiceBuffer<> IIndiceBuffer;


struct IShader;
struct ITexture;
struct IMaterial;
class ECS_API IRenderComponent
{
public:
    virtual ~IRenderComponent()
    {
    }

    virtual bool SetMaterial(IMaterial* _pkMaterial) = 0;

    virtual bool SetAttribute(IBuffer* _pkVertexBuffer, IBuffer* _pkIndicesBuffer) = 0;

    virtual bool SetRenderState(IRenderState* _pkRenderState) = 0;

    virtual IShader* GetShader(int _eShaderType) const = 0;

    virtual IBuffer* GetVertexBuffer() const = 0;

    virtual IBuffer* GetIndiceBuffer() const = 0;

    virtual const IRenderState* GetRenderState() const = 0;

    virtual ITexture* GetTexture(size_t _nIndex) const = 0;

    virtual class IConstantSemantic* GetConstant(size_t _nIndex) = 0;

    virtual class IConstantSemantic* GetConstantByName(const char* _pcName) = 0;

    virtual size_t GetConstantCount() = 0;

    virtual bool ValidatePipeline(class IRenderer&) = 0;

    virtual void SetRenderable(bool _bEnable) = 0;

    virtual bool IsRenderable() = 0;
};

typedef TComponent <IRenderComponent> IRenderHandler;

template <class Constant>
bool AssignConstant(IRenderHandler& _rkRenderHandler, const char* _pcConstantName, const Constant& _rkData, EConstantType _eType)
{
    IConstantSemantic* pkConstant = _rkRenderHandler->GetConstantByName(_pcConstantName);
    if (!pkConstant)
        return false;

    if (pkConstant->GetType() != _eType)
        return false;

    TConstantSemantic<Constant>* pkConstantIns = static_cast<TConstantSemantic<Constant>*>(pkConstant);
    pkConstantIns->SetValue(_rkData);
    return true;
}
