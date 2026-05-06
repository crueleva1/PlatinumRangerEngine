#pragma once


#include "IGeometryComponent.h"
#include "ITexture.h"


class ECS_API CGeomertryComponent : public IGeometryComponent
{
protected:
    std::vector <std::vector <ITextureDescPtr> > m_kTextureDescripts;
    struct SGeomertryBuffer
    {
        std::shared_ptr <IBuffer> m_kVertexBuffer;
        std::shared_ptr <IBuffer> m_kIndexBuffer;
        std::shared_ptr <IBound> m_kBound;
        int m_ePrimitiveType;
        int m_nTextureIndex;

        SGeomertryBuffer()
            : m_ePrimitiveType(-1)
            , m_nTextureIndex(-1)
        {
        }
    };
    std::vector <SGeomertryBuffer> m_kGeometryDatas;
    std::shared_ptr <struct IRenderState> m_spkRenderState;
    size_t m_nActiveIndex;
    bool m_bIsCastLight;
public:
    CGeomertryComponent();

    virtual ~CGeomertryComponent();

    virtual void ActiveGeometry (size_t _nIndex);

    virtual size_t GetGeometryCount ();

    virtual size_t CurrentActiveGeometry ();

    virtual IBuffer* GetVertexBuffer();

    virtual IBuffer* GetIndiceBuffer();

    virtual IRenderState* GetRenderState();

    virtual struct ITextureDesc* GetTextureDesc(size_t _nIndex);

    virtual ITextureDesc* GetTextureDescByName(const char* _pcName);

    virtual bool ValidateRenderDataByName(class IMatertialSystem& _rkSystem, const char* _pcName);

    virtual size_t GetTextureDescCount();

    virtual int GetPrimitiveType();

    virtual void SetCastLighting(bool _bEnable);

    virtual bool IsCastLighting();

    virtual void UpdateWorldBound(class ISceneComponent& _rkComp);

    virtual IBound* GetBound();

    virtual void PurgeBuffer(size_t _nIndex);
};