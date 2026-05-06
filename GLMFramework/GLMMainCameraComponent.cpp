#include "GLMMainPCH.h"
#include "GLMMainCameraComponent.h"
#include "GLMSceneComponent.h"
#include "RenderSystem.h"

CGLMMainCameraComponent::CGLMMainCameraComponent ()
{

}

CGLMMainCameraComponent::~CGLMMainCameraComponent ()
{

}

void CGLMMainCameraComponent::UpdateViewMatrix (ISceneComponent& _rkComp)
{
    CGLMSceneComponent* pkComp = dynamic_cast <CGLMSceneComponent*>(&_rkComp);
    if (!pkComp)
        return;
    IRenderer* pkRenderer = IRenderSystem::GetRenderer();
    if (!pkRenderer)
        return;
#if 1
    float* pkViewMat = (float*)&m_kViewMat;
    const glm::mat4& kWorld = pkComp->GetTransform();
    glm::vec3 kDir = -kWorld [2];
    glm::vec3 kEye = kWorld [3];
    glm::vec3 kUp = kWorld [1];
    float* pkDir = (float*)&kDir;
    float* pkEye = (float*)&kEye;
    float* pkUp = (float*)&kUp;

    pkRenderer->BuildViewMatrix(&pkViewMat, pkEye, pkDir, pkUp);
#else
    const glm::mat4& kWorld = pkComp->GetTransform();
    glm::vec3 kDir = -kWorld[2];
    glm::vec3 kEye = kWorld[3];
    glm::vec3 kUp = kWorld[1];
    m_kViewMat = glm::lookAtRH(kEye, kEye + kDir, kUp);
#endif
    UpdateFrustum();
}

void CGLMMainCameraComponent::UpdateViewMatrix (class ISceneComponent& _rkComp, glm::vec3& _rkLookAt)
{
    CGLMSceneComponent* pkComp = dynamic_cast <CGLMSceneComponent*>(&_rkComp);
    if (!pkComp)
        return;
    IRenderer* pkRenderer = IRenderSystem::GetRenderer ();
    if (!pkRenderer)
        return;
#if 1
    float* pkViewMat = (float*)&m_kViewMat;
    glm::vec3 kEye = pkComp->GetPosition ();
    glm::vec3 kDir = _rkLookAt - kEye;
    kDir = glm::normalize(kDir);
    glm::vec3 kUp(0.0f, 1.0f, 0.0f);
    float* pkDir = (float*)&kDir;
    float* pkEye = (float*)&kEye;
    float* pkUp = (float*)&kUp;

    pkRenderer->BuildViewMatrix (&pkViewMat, pkEye, pkDir, pkUp);
#else
    m_kViewMat = glm::lookAtRH(pkComp->GetPosition(), _rkLookAt, glm::vec3(0.0f, 1.0f, 0.0f));
#endif
    glm::mat3 kRot = m_kViewMat;
    pkComp->SetRotate(kRot);

    UpdateFrustum();
}

size_t CGLMMainCameraComponent::GetCameraNameHash()
{
    return m_nNameHash;
}

void CGLMMainCameraComponent::UpdateCameraName(class ISceneComponent& _rkComp)
{
    std::string kName = _rkComp.GetName();
    std::hash <std::string> kHash;
    m_nNameHash = kHash(kName);
}