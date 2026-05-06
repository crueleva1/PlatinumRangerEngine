#pragma once

#include "ModelViewPipeline.h"
#include "GLMBinTree.h"

class ECS_MODELVIEW_API CForwardRenderPipeline : public CModelViewPipeline
{
    std::shared_ptr <struct IRenderState> m_spkDebugZTestState;
    bool m_bDrawDebugNormal;

    IMaterial* m_pkNormalDebugMaterial;

    virtual void Prepared(int _nWidth, int _nHeight, entityx::EntityManager& _rkEntityMgr, entityx::EventManager& _rkEventMgr);

    void RenderDebugNormal(class IRenderer& _rkRenderer, entityx::EventManager& _rkEventMgr);

    void RenderDiffuse(class IRenderer& _rkRenderer, entityx::EventManager& _rkEventMgr, int _nCurrent);

    void RenderBound();

    virtual void SetScreenText();
public:
    CForwardRenderPipeline(entityx::EventManager& _rkEventMgr, int _nWidth, int _nHeight);

    virtual ~CForwardRenderPipeline();

    virtual void SetPassObject(entityx::EntityManager& _rkMgr);

    bool operator()(const TBinTreeNode<3>& _rkSceneNode);

    bool operator()(ISceneComponent* _pkDynamicObject);

    virtual bool IsDeferred()
    {
        return false;
    }

    virtual void OnDraw(class IRenderer& _rkRenderer, entityx::EventManager& rkEventMgr, int _nCurrent);

    inline void SetDrawDebugNormal(bool _bEnable)
    {
        m_bDrawDebugNormal = _bEnable;
    }
};
