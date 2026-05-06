#include "GLMMainPCH.h"
#include "GLMCubeCameraComponent.h"
#include "GLMSceneComponent.h"
#include "RenderSystem.h"

CGLMCubeCameraComponent::SFaceVector CGLMCubeCameraComponent::ms_akFaceVector[6] =
{
#if 1
    { glm::vec3(-1.0f, 0.0f, 0.0f),  glm::vec3(0.0f, -1.0f, 0.0f) },
    { glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f) },
    { glm::vec3(0.0f, -1.0f, 0.0f),  glm::vec3(0.0f, 0.0f, 1.0f) },
    { glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)  },
    { glm::vec3(0.0f, 0.0f, -1.0f),  glm::vec3(0.0f, -1.0f, 0.0f) },
    { glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f) }
#else
    { glm::vec3(-1.0f, 0.0f, 0.0f),  glm::vec3(0.0f, 1.0f, 0.0f) },
    { glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f) },
    { glm::vec3(0.0f, -1.0f, 0.0f),  glm::vec3(0.0f, 0.0f, 1.0f) },
    { glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f) },
    { glm::vec3(0.0f, 0.0f, 1.0f),  glm::vec3(0.0f, 1.0f, 0.0f) },
    { glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f) }
#endif
};

CGLMCubeCameraComponent::CGLMCubeCameraComponent()
    : m_nRadius(0)
    , m_bIsTransformModified(true)
    , m_bCheckByDynamic(true)
{
    memset(m_anCameraNameHashs, 0, sizeof(m_anCameraNameHashs));
}

CGLMCubeCameraComponent::CGLMCubeCameraComponent(int _nRadius, int _nResolution)
    : m_nRadius(_nRadius)
    , m_bIsTransformModified(true)
{
    m_nCurrentFace = 0;
    m_spkFrameBuffer = IRenderSystem::CreateFrameBuffer(IRenderer::eFBT_Cube, _nResolution, _nResolution);
    UpdatePerspectiveProjectMatrix(0.1f, _nRadius, _nResolution, _nResolution);
    memset(m_anCameraNameHashs, 0, sizeof(m_anCameraNameHashs));
}

CGLMCubeCameraComponent::~CGLMCubeCameraComponent()
{
    m_spkFrameBuffer = nullptr;
}

bool CGLMCubeCameraComponent::Initialize(std::shared_ptr <IFrameBuffer> spkFrameBuffer, int _nRadius)
{
    if (!spkFrameBuffer)
        return false;

    if (_nRadius < 1)
        return false;

    m_spkFrameBuffer = spkFrameBuffer;
    m_nRadius = _nRadius;
    UpdatePerspectiveProjectMatrix(0.1f, _nRadius, spkFrameBuffer->GetViewPortWidth(), spkFrameBuffer->GetViewPortHeight());
    return true;
}

void CGLMCubeCameraComponent::SetCurrentFace(char _nFace)
{
    m_nCurrentFace = _nFace;
}

bool CGLMCubeCameraComponent::ActiveCurrentFace()
{
#if 1 
    // There was Optimize issue about what scene object get in or get out Cube Map View
    // m_bIsDynamic let static Env map management by it self render flow
    if (!m_bIsTransformModified && m_bCheckByDynamic)
        return false; // No Need to redraw
#endif
    IRenderer* pkRenderer = IRenderSystem::GetRenderer();
    if (!pkRenderer)
        return false;
    // Set Face Transform
    //#define GL_TEXTURE_CUBE_MAP_POSITIVE_X 0x8515
    //#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X 0x8516
    //#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y 0x8517
    //#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y 0x8518
    //#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z 0x8519
    //#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z 0x851A
    if (!IsDirty())
        return true;

    float* pkViewMat = (float*)&m_kViewMat;
    const glm::mat4& kWorld = m_kTransform;
    glm::vec3& kDir = ms_akFaceVector [m_nCurrentFace].m_kDir;
    glm::vec3 kEye = kWorld [3];
    glm::vec3& kUp = ms_akFaceVector [m_nCurrentFace].m_kUp;
    float* pkDir = (float*)&kDir;
    float* pkEye = (float*)&kEye;
    float* pkUp = (float*)&kUp;
    pkRenderer->BuildViewMatrix(&pkViewMat, pkEye, pkDir, pkUp);
    ClearDirty();
    return true;
}

char CGLMCubeCameraComponent::GetCurrentFace()
{
    return m_nCurrentFace;
}

void CGLMCubeCameraComponent::UpdateViewMatrix(class ISceneComponent& _rkComp)
{
    CGLMSceneComponent* pkComp = dynamic_cast <CGLMSceneComponent*>(&_rkComp);
    if (!pkComp)
        return;
    m_kTransform = pkComp->GetTransform();
    m_bIsTransformModified |= m_kTransform.isDirty();
}

void CGLMCubeCameraComponent::UpdatePerspectiveProjectMatrix(float _fNear, float _fFar, int _nWidth, int _nHeight)
{
    IRenderer* pkRenderer = IRenderSystem::GetRenderer();
    if (!pkRenderer)
        return;
    float* pkProjectMat = (float*)&m_kProjectMat;
    pkRenderer->BuildPerspectiveProjectMatrix(&pkProjectMat, _fNear, _fFar, _nWidth, _nHeight, 90.0f);
    m_bOrth = false;
}

IFrameBuffer* CGLMCubeCameraComponent::GetTargetFrameBuffer()
{
    return m_spkFrameBuffer.get();
}

bool CGLMCubeCameraComponent::IsInView(struct IBound* _pkBound, size_t _nHash)
{
    IRenderer* pkRenderer = IRenderSystem::GetRenderer();
    if (!pkRenderer)
        return false;

    if (IsDirty()) {
        float* pkViewMat = (float*)&m_kViewMat;
        const glm::mat4& kWorld = m_kTransform;
        glm::vec3& kDir = ms_akFaceVector [m_nCurrentFace].m_kDir;
        glm::vec3 kEye = kWorld [3];
        glm::vec3& kUp = ms_akFaceVector [m_nCurrentFace].m_kUp;
        float* pkDir = (float*)&kDir;
        float* pkEye = (float*)&kEye;
        float* pkUp = (float*)&kUp;
        pkRenderer->BuildViewMatrix(&pkViewMat, pkEye, pkDir, pkUp);
        UpdateFrustum();
        ClearDirty();
    }
    return CGLMBaseCameraComponent::IsInView(_pkBound, _nHash);
}

bool CGLMCubeCameraComponent::IsInView(const glm::vec3& _rkPoint)
{
    IRenderer* pkRenderer = IRenderSystem::GetRenderer();
    if (!pkRenderer)
        return false;

    if (IsDirty()) {
        float* pkViewMat = (float*)&m_kViewMat;
        const glm::mat4& kWorld = m_kTransform;
        glm::vec3& kDir = ms_akFaceVector [m_nCurrentFace].m_kDir;
        glm::vec3 kEye = kWorld [3];
        glm::vec3& kUp = ms_akFaceVector [m_nCurrentFace].m_kUp;
        float* pkDir = (float*)&kDir;
        float* pkEye = (float*)&kEye;
        float* pkUp = (float*)&kUp;
        pkRenderer->BuildViewMatrix(&pkViewMat, pkEye, pkDir, pkUp);
        UpdateFrustum();
        ClearDirty();
    }
    return CGLMBaseCameraComponent::IsInView(_rkPoint);
}

bool CGLMCubeCameraComponent::IsInView(const glm::vec3& _rkPoint, const glm::vec3& _rkHalf)
{
    IRenderer* pkRenderer = IRenderSystem::GetRenderer();
    if (!pkRenderer)
        return false;

    if (IsDirty()) {
        float* pkViewMat = (float*)&m_kViewMat;
        const glm::mat4& kWorld = m_kTransform;
        glm::vec3& kDir = ms_akFaceVector [m_nCurrentFace].m_kDir;
        glm::vec3 kEye = kWorld [3];
        glm::vec3& kUp = ms_akFaceVector [m_nCurrentFace].m_kUp;
        float* pkDir = (float*)&kDir;
        float* pkEye = (float*)&kEye;
        float* pkUp = (float*)&kUp;
        pkRenderer->BuildViewMatrix(&pkViewMat, pkEye, pkDir, pkUp);
        UpdateFrustum();
        ClearDirty();
    }
    return CGLMBaseCameraComponent::IsInView(_rkPoint, _rkHalf);
}

size_t CGLMCubeCameraComponent::GetCameraNameHash()
{
    return m_anCameraNameHashs [m_nCurrentFace];
}

void CGLMCubeCameraComponent::UpdateCameraName(class ISceneComponent& _rkComp)
{
    for (char nFace = 0; nFace < 6; nFace++) {
        char cGroupName [64] = {};
        sprintf_s(cGroupName, sizeof(cGroupName), "%s%d", _rkComp.GetName(), (int)nFace);
        std::string kGroupName = cGroupName;
        std::hash <std::string> kHash;
        m_anCameraNameHashs [nFace] = kHash(kGroupName);
    }
}

void CGLMCubeCameraComponent::RenderComplate()
{
    ClearRedrawDirty();
}