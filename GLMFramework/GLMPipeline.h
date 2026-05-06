#pragma once

#include "RenderSystem.h"

class ECSGLM_API CGLMDefaultPipeline : public IRenderPipeline
{
protected:
    std::shared_ptr<IFrameBuffer> m_spkDefaultBuffer;
    std::vector <entityx::Entity> m_kDrawEntity;
    std::vector <entityx::Entity> m_kTranspanecyEntity;
    std::vector <entityx::Entity> m_kScreenEntity;
    entityx::Entity m_kCameraEntity;
    glm::mat4 m_kOrthoMatrix;

    void UpdateOrthoMatrix(int _nWidth, int nHeight, int _nXOffset = 0, int _nYOffset = 0, bool _bInv = false);
public:
    CGLMDefaultPipeline();

    CGLMDefaultPipeline(int _nWidth, int nHeight);

    virtual ~CGLMDefaultPipeline();

    virtual void Prepared(int _nWidth, int _nHeight, entityx::EntityManager& _rkEntityMgr, entityx::EventManager& _rkEventMgr);

    virtual void Active(entityx::EventManager& _rkEventMgr);

    virtual void Deactive(entityx::EventManager& _rkEventMgr);

    virtual IFrameBuffer* GetFrameBuffer(int _nCurrentPass);

    virtual void SetPassObject(entityx::EntityManager& _rkMgr);

    virtual void OnClear(class IRenderer& _rkRenderer, int _nCurrentPass);

    virtual void PassBegin(class IRenderer& _rkRenderer, int _nCurrent);

    virtual void OnDraw(class IRenderer& _rkRenderer, entityx::EventManager& rkEventMgr, int _nCurrent);

    virtual void PassEnd(class IRenderer& _rkRenderer, int _nCurrent);

    virtual void RenderScreen(class IRenderer& _rkRenderer, entityx::EventManager& _rkEventMgr, class ECSGLM_API ICameraComponent* _pkCamera, const glm::mat4& _rkOrtha);

    virtual void OnResize(class IRenderer& _rkRenderer, int _nWidth, int _nHeight);
};