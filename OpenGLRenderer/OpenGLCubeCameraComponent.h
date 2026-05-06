#pragma once


#include "GLMCubeCameraComponent.h"
#include "OpenGLRenderSystem.h"


class ECS_OGL_API COpenGLCubeCameraComponent 
    : public CGLMCubeCameraComponent
    //, public IOpenGLRenderListener
{
#if 0
    virtual void BeginFrame(IRenderer& _rkRenderer, int _nPass);

    virtual void PrepareState(IRenderer& _rkRenderer, int _nPass);

    virtual void EndFrame(IRenderer& _rkRenderer, int _nPass);

    virtual void PreDraw(IRenderer& _rkRenderer, int _nPass);

    virtual void PostDraw(IRenderer& _rkRenderer, int _nPass);
#endif
public:
    COpenGLCubeCameraComponent();

    COpenGLCubeCameraComponent(int _nRadius, int _nResolution);

    virtual ~COpenGLCubeCameraComponent();

    virtual bool Initialize(std::shared_ptr <IFrameBuffer> spkFrameBuffer, int _nRadius);

    virtual bool ActiveCurrentFace();
};