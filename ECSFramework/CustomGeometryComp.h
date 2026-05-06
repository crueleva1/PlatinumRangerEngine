#pragma once

#include "GeomertryComponent.h"

class IMeshBuffer;

enum ECustomModelType
{
    eCMT_None,
    eCMT_Rect,
    eCMT_Sphere,
    eCMT_Polygun,
};

class ECS_API CCustomGeometryComp : public CGeomertryComponent
{
protected:
    std::shared_ptr <IMeshBuffer> m_spkMesh;
public:
    CCustomGeometryComp(std::shared_ptr <IMeshBuffer> _spkMesh, ECustomModelType);

    virtual ~CCustomGeometryComp();

    virtual void ActiveGeometry(size_t _nIndex);

    virtual size_t GetGeometryCount();

    virtual size_t CurrentActiveGeometry();

    virtual IBuffer* GetVertexBuffer();

    virtual IBuffer* GetIndiceBuffer();

    virtual IRenderState* GetRenderState();

    virtual struct ITextureDesc* GetTextureDesc(size_t _nIndex);

    virtual size_t GetTextureDescCount();

    virtual int GetPrimitiveType();

    virtual IBound* GetBound();

    virtual void PurgeBuffer(size_t _nIndex);

    inline IMeshBuffer* GetMeshBuffer()
    {
        return m_spkMesh.get();
    }
};