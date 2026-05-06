#include "GLMMainPCH.h"
#include "GLMBaseCameraComponent.h"
#include "RenderSystem.h"
#include "GLMBoxBound.h"


CGLMBaseCameraComponent::CGLMBaseCameraComponent()
    : m_bActive(false)
    , m_bOrth(false)
{
}

CGLMBaseCameraComponent::~CGLMBaseCameraComponent()
{

}

void CGLMBaseCameraComponent::ActiveCamera ()
{
    m_bActive = true;
}

void CGLMBaseCameraComponent::DeactiveCamera ()
{
    m_bActive = false;
}

bool CGLMBaseCameraComponent::IsCameraActive ()
{
    return m_bActive;
}

bool CGLMBaseCameraComponent::IsOrth()
{
    return m_bOrth;
}

void CGLMBaseCameraComponent::UpdateOrthProjectMatrix(float _fTop, float _fBottom, float _fRight, float _fLeft, float _fNear, float _fFar)
{
    IRenderer* pkRenderer = IRenderSystem::GetRenderer();
    if (!pkRenderer)
        return;
    float* pkProjectMat = (float*)&m_kProjectMat;
    pkRenderer->BuildOrthProjectMatrix(&pkProjectMat, _fTop, _fBottom, _fRight, _fLeft, _fNear, _fFar);
    m_bOrth = true;
    UpdateFrustum();
    m_kNearFar.x = _fNear;
    m_kNearFar.y = _fFar;
}

void CGLMBaseCameraComponent::UpdatePerspectiveProjectMatrix(float _fNear, float _fFar, int _nWidth, int _nHeight)
{
    IRenderer* pkRenderer = IRenderSystem::GetRenderer();
    if (!pkRenderer)
        return;
    float* pkProjectMat = (float*)&m_kProjectMat;
    pkRenderer->BuildPerspectiveProjectMatrix(&pkProjectMat, _fNear, _fFar, _nWidth, _nHeight);
    m_bOrth = false;
    UpdateFrustum();
    m_kNearFar.x = _fNear;
    m_kNearFar.y = _fFar;
}

bool CGLMBaseCameraComponent::IsInView(IBound* _pkBound, size_t _nHash)
{
    CGLMBoundBox* pkBox = static_cast <CGLMBoundBox*>(_pkBound);
    if (!pkBox)
        return false;

    const glm::vec3& rkBoxVec = pkBox->GetBoxVector();
    const glm::vec3& rkBoundPos = pkBox->GetWorldCenter();
    bool bRet = m_kFrustum.PointInFrustum(rkBoundPos, rkBoxVec);
    if (_nHash != -1)
        pkBox->SetTested(_nHash, bRet);
    return bRet;
}

bool CGLMBaseCameraComponent::IsInView(const glm::vec3& _rkPoint)
{
    return m_kFrustum.PointInFrustum(_rkPoint);
}

bool CGLMBaseCameraComponent::IsInView(const glm::vec3& _rkPoint, const glm::vec3& _rkHalf)
{
    return m_kFrustum.PointInFrustum(_rkPoint, _rkHalf);
}

void CGLMBaseCameraComponent::UpdateFrustum()
{
    m_kFrustum.CalculateFrustum(m_kViewMat, m_kProjectMat);
}