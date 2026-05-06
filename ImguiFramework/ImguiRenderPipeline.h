#pragma once

#include "GLMPipeline.h"
#include "ITexture.h"
#include "GLMMeshData.h"

class IMGUI_SYSTEM_API CImguiRenderPipeline : public CGLMDefaultPipeline
{
    entityx::Entity m_kImguiMainEntity;
    std::shared_ptr <ITexture> m_spkFontTxture;
    std::shared_ptr <IGeometryComponent> m_spkGSC;
    IMaterial* m_pkMaterial;
    bool m_bIsValidDraw;
public:
    CImguiRenderPipeline();

    virtual ~CImguiRenderPipeline();

    virtual void Prepared(int _nWidth, int _nHeight, entityx::EntityManager& _rkMgr, entityx::EventManager& _rkEventMgr);

    virtual void Active(entityx::EventManager& _rkEventMgr);

    virtual void Deactive(entityx::EventManager& _rkEventMgr);

    virtual IFrameBuffer* GetFrameBuffer(int _nCurrentPass);

    virtual void SetPassObject(entityx::EntityManager& _rkMgr);

    virtual void OnClear(class IRenderer& _rkRenderer, int _nCurrentPass);

    virtual void PassBegin(class IRenderer& _rkRenderer, int _nCurrentPass);

    virtual void OnDraw(class IRenderer& _rkRenderer, entityx::EventManager& _rkEventMgr, int _nCurrentPass);

    virtual void PassEnd(class IRenderer& _rkRenderer, int _nCurrentPass);

    virtual void OnResize(class IRenderer& _rkRenderer, int _nWidth, int _nHeight);

    bool CreateFontTexture(ImGuiIO& _rkIO);

    bool CreateMeshBuffer(ImDrawData* _pkDrawData);

    void InvalidateResource(ImGuiIO& _rkIO);

    inline void SetValidDraw(bool _bValidDraw)
    {
        m_bIsValidDraw = _bValidDraw;
    }
};