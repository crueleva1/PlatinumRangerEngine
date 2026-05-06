#pragma once

#include "IRenderComponent.h"

class ECS_API CRenderComponent : public IRenderComponent
{
protected:
    IBuffer* m_pkVertexBuffer;
    IBuffer* m_pkIndexBuffer;
    IMaterial* m_pkMaterial;
    bool m_bRenderable;
public:
    CRenderComponent();

    virtual ~CRenderComponent();

    virtual bool SetMaterial(IMaterial* _pkMaterial);

    virtual bool SetAttribute(IBuffer* _pkVertexBuffer, IBuffer* _pkIndicesBuffer);

    virtual bool SetRenderState(IRenderState* _pkRenderState);

    virtual IShader* GetShader(int _eShaderType) const;

    virtual IBuffer* GetVertexBuffer() const;

    virtual IBuffer* GetIndiceBuffer() const;

    virtual const IRenderState* GetRenderState() const;

    virtual ITexture* GetTexture(size_t _nIndex) const;

    virtual IConstantSemantic* GetConstant(size_t _nIndex);

    virtual IConstantSemantic* GetConstantByName(const char* _pcName);

    virtual size_t GetConstantCount();

    virtual bool ValidatePipeline(class IRenderer&);

    virtual void SetRenderable(bool _bEnable)
    {
        m_bRenderable = _bEnable;
    }

    virtual bool IsRenderable()
    {
        return m_bRenderable;
    }
};