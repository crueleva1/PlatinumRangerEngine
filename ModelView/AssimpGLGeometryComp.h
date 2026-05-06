#pragma once

#include "GLMGeomertryComponent.h"
#include "DefaultResourceComponent.h"
#include "MeshData.h"
#include <assimp/scene.h>


class ECS_MODELVIEW_API CAssimpGLGeometryComp
    : public CGLMGeomertryComponent
{
    std::shared_ptr <CMeshData> m_spkMesh;
public:

    CAssimpGLGeometryComp(const aiScene* _pkScene, size_t _nMeshIndex, bool _bBPR, const char* _pcBasePath = nullptr);

    virtual ~CAssimpGLGeometryComp();

    virtual size_t GetGeometryCount();

    virtual IBuffer* GetVertexBuffer();

    virtual IBuffer* GetIndiceBuffer();

    virtual struct ITextureDesc* GetTextureDesc(size_t _nIndex);

    virtual bool ValidateRenderDataByName(class IMatertialSystem& _rkSystem, const char* _pcName);

    virtual size_t GetTextureDescCount();

    void CreateVertexBuffer();

    void CreateIndexBuffer();

    virtual int GetPrimitiveType();

    virtual IBound* GetBound();
};