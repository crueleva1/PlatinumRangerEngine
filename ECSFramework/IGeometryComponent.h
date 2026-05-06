#pragma once

#include "IBound.h"

struct IBuffer;
struct IRenderState;
struct ITextureDesc;

class IGeometryComponent
{
public:
    virtual ~IGeometryComponent()
    {
    }

    virtual void ActiveGeometry(size_t _nIndex) = 0;

    virtual size_t GetGeometryCount() = 0;

    virtual size_t CurrentActiveGeometry() = 0;

    virtual IBuffer* GetVertexBuffer() = 0;

    virtual IBuffer* GetIndiceBuffer() = 0;

    virtual IRenderState* GetRenderState() = 0;

    virtual ITextureDesc* GetTextureDesc(size_t _nIndex) = 0;

    virtual ITextureDesc* GetTextureDescByName(const char* _pcName) = 0;

    virtual bool ValidateRenderDataByName (class IMatertialSystem& _rkSystem, const char* _pcName) = 0;

    virtual size_t GetTextureDescCount() = 0;

    virtual int GetPrimitiveType() = 0;

    virtual void SetCastLighting(bool _bEnable) = 0;

    virtual bool IsCastLighting() = 0;

    virtual void UpdateWorldBound(class ISceneComponent& _rkComp) = 0;

    virtual IBound* GetBound() = 0;

    virtual void PurgeBuffer(size_t _nIndex) = 0;
};

typedef TComponent<IGeometryComponent> IGeometryHandler;