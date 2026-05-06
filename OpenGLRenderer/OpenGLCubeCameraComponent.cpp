#include "ECSOpenGLPCH.h"
#include "OpenGLCubeCameraComponent.h"
#include "GLMSceneComponent.h"
#include "OpenGLMaterial.h"
#include "OpenGLFrameBuffer.h"
#include "OpenGLTexture.h"
#include "OpenGLRenderSystem.h"
#if 0
void COpenGLCubeCameraComponent::BeginFrame(IRenderer& _rkRenderer, int _nPass)
{

}

void COpenGLCubeCameraComponent::PrepareState(IRenderer& _rkRenderer, int _nPass)
{

}

void COpenGLCubeCameraComponent::EndFrame(IRenderer& _rkRenderer, int _nPass)
{
    //ClearRedrawDirty();
}

void COpenGLCubeCameraComponent::PreDraw(IRenderer& _rkRenderer, int _nPass)
{

}

void COpenGLCubeCameraComponent::PostDraw(IRenderer& _rkRenderer, int _nPass)
{
}
#endif
COpenGLCubeCameraComponent::COpenGLCubeCameraComponent()
    : CGLMCubeCameraComponent()
{
}

COpenGLCubeCameraComponent::COpenGLCubeCameraComponent(int _nRadius, int _nResolution)
    : CGLMCubeCameraComponent(_nRadius, _nResolution)
{
}

COpenGLCubeCameraComponent::~COpenGLCubeCameraComponent()
{
   
}

bool COpenGLCubeCameraComponent::Initialize(std::shared_ptr <IFrameBuffer> spkFrameBuffer, int _nRadius)
{
    COpenGLCubeFrameBuffer* pkFrameBuffer = dynamic_cast<COpenGLCubeFrameBuffer*>(spkFrameBuffer.get());
    if (!pkFrameBuffer)
        return false;

    return CGLMCubeCameraComponent::Initialize(spkFrameBuffer, _nRadius);
}

bool COpenGLCubeCameraComponent::ActiveCurrentFace()
{
#if 1 
    // There was Optimize issue about what scene object get in or get out Cube Map View
    // m_bIsDynamic let static Env map management by it self render flow
    if (!m_bIsTransformModified && m_bCheckByDynamic)
        return false; // No Need to redraw
#endif
    COpenGLCubeFrameBuffer* pkFrameBuffer = static_cast<COpenGLCubeFrameBuffer*>(m_spkFrameBuffer.get());
    if (!pkFrameBuffer)
        return false;
    IRenderer* pkRenderer = IRenderSystem::GetRenderer();
    if (!pkRenderer)
        return false;

    int nClearBit = (pkFrameBuffer->GetRenderBufferColorAttachmentCount() > 0) ? IRenderer::eBCB_Color : 0;
    COpenGLTexture* pkGLDepthTexture = static_cast<COpenGLTexture*>(pkFrameBuffer->GetDepthTexture());
    if (pkGLDepthTexture) {
        switch (pkGLDepthTexture->GetTextureFormat()) {
        case gli::FORMAT_D24S8_UNORM:
        case gli::FORMAT_D32_UFLOAT_S8_UNORM:
            nClearBit |= IRenderer::eBCB_Depth;
        case gli::FORMAT_D16_UNORM:
        case gli::FORMAT_D24_UNORM:
        case gli::FORMAT_D32_UFLOAT:
            nClearBit |= IRenderer::eBCB_Depth;
            break;
        default:
            assert(0);
            return false;
        }
    }

    pkRenderer->SetClearBufferBit(nClearBit);
    pkRenderer->SetClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    pkFrameBuffer->ActiveTextureFace(m_nCurrentFace.get());
    pkFrameBuffer->ActiveDepthTextureFace(m_nCurrentFace.get());
    pkRenderer->ClearBuffer();

    if (m_nCurrentFace.get() == 5)
        ClearRedrawDirty();

    return CGLMCubeCameraComponent::ActiveCurrentFace();
}