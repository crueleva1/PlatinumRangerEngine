#include "MainPCH.h"
#include "ModelViewMovementComponent.h"
#include "Win32KeyboardMouseInputComponent.h"
#include "Win32InputSystem.h"
#include "GLMSceneComponent.h"

CModelViewMovementComponent::CModelViewMovementComponent()
    : m_nMaxVelocity(1000.0f)
{

}

CModelViewMovementComponent::~CModelViewMovementComponent()
{

}

const glm::vec3& CModelViewMovementComponent::GetVelocity()
{
    return m_kVelocity;
}

const glm::mat4& CModelViewMovementComponent::GetRotate()
{
    return m_kRotate;
}

void CModelViewMovementComponent::SetVelocity(const glm::vec3& _rkVelocity)
{
    m_kVelocity = _rkVelocity;
}

void CModelViewMovementComponent::SetCurrentRotation(const glm::mat3& _rkRotate)
{
    m_kRotate = _rkRotate;
}

CModelViewKBMovementComponent::CModelViewKBMovementComponent()
    : CModelViewMovementComponent()
    , m_nX(0.0f)
    , m_nY(0.0f)
{
}

CModelViewKBMovementComponent::~CModelViewKBMovementComponent()
{
}

void CModelViewKBMovementComponent::onHandleControl(entityx::Entity _kEntity)
{
    if (!_kEntity)
        return;

    if (!_kEntity.has_component <IInputControlHandler> ())
        return;

    entityx::ComponentHandle <IInputControlHandler> kHandler = _kEntity.component <IInputControlHandler> ();
    IInputControlComponent* pkComp = kHandler->get();
    if (pkComp->isHandled())
        return;
    CWin32KeyboardMouseInputComponent* pkKeyboradMouseComp = dynamic_cast <CWin32KeyboardMouseInputComponent*>(pkComp);
    if (!pkKeyboradMouseComp)
        return;
    // Key Board
    if (!pkKeyboradMouseComp->isKeyHandled()) {
        int nKey = pkKeyboradMouseComp->getKey();
        bool bDown = pkKeyboradMouseComp->isKeyDown();
        switch (nKey)
        {
            case CKeyboardEvent::eKBC_W:
                m_kVelocity.z = ((bDown) ? 1.0f : 0.0f) * m_nMaxVelocity;
                break;
            case CKeyboardEvent::eKBC_S:
                m_kVelocity.z = ((bDown) ? -1.0f : 0.0f) * m_nMaxVelocity;
                break;
            case CKeyboardEvent::eKBC_A:
                m_kVelocity.x = ((bDown) ? 1.0f : 0.0f) * m_nMaxVelocity;
                break;
            case CKeyboardEvent::eKBC_D:
                m_kVelocity.x = ((bDown) ? -1.0f : 0.0f) * m_nMaxVelocity;
                break;
            default:
                break;
        }
    }
    // Mouse
#if 1
    if (!pkKeyboradMouseComp->isMouseHandled()) {
        if (pkKeyboradMouseComp->isLeft()) {
            if (!pkKeyboradMouseComp->isDown()) {
                m_nX = 0.0f;
                m_nY = 0.0f;
                if (_kEntity.has_component <ISceneHandler> ()) {
                    CGLMSceneComponent* pkSceneComp = dynamic_cast <CGLMSceneComponent*>(_kEntity.component<ISceneHandler> ()->get());
                    if (pkSceneComp)
                        m_kRotate = pkSceneComp->GetRotate ();
                }
                if (!pkKeyboradMouseComp->isMoving ())
                    ILogger::Log(eLL_Verbose, "MouseUp X:%f, Y:%f\n", pkKeyboradMouseComp->getX (), pkKeyboradMouseComp->getY ());
            }
            else {
                if (!pkKeyboradMouseComp->isMoving()) {
                    m_nX = pkKeyboradMouseComp->getX();
                    m_nY = pkKeyboradMouseComp->getY();
                    
                    ILogger::Log(eLL_Verbose, "MouseDown X:%f, Y:%f\n", m_nX, m_nY);
                }
                else {
                    float nDeltaX = pkKeyboradMouseComp->getX() - m_nX;
                    float nDeltaY = pkKeyboradMouseComp->getY() - m_nY;
                    ILogger::Log(eLL_Verbose, "MouseDownMove X:%f, Y:%f | DeltaX:%f, DeltaY:%f\n", pkKeyboradMouseComp->getX(), pkKeyboradMouseComp->getY(), nDeltaX, nDeltaY);
                    glm::mat4 kYaw; 
                    kYaw = glm::rotate(kYaw, -nDeltaX / 360.f, glm::vec3(0.0f, 1.0f, 0.0f));
                    m_kRotate = glm::rotate(m_kRotate, nDeltaY / 360.f, glm::vec3(1.0f, 0.0f, 0.0f));

                    m_kRotate = kYaw * m_kRotate;

                    m_nX += nDeltaX;
                    m_nY += nDeltaY;
                }
            }
        }
    }
#endif
    pkKeyboradMouseComp->setHandled();
}

CModelViewSequenceMovementComponent::CModelViewSequenceMovementComponent()
    :m_nCurrentSeq(-1)
{

}

CModelViewSequenceMovementComponent::~CModelViewSequenceMovementComponent()
{

}

void CModelViewSequenceMovementComponent::ClearSequence()
{
    m_kSequence.clear();
}

void CModelViewSequenceMovementComponent::AddSequence(CModelViewSequenceMovementComponent::SPosSequence& _rkSeq)
{
    m_kSequence.push_back(_rkSeq);
}

void CModelViewSequenceMovementComponent::StartSequence()
{
    m_nCurrentSeq = 0;
}

void CModelViewSequenceMovementComponent::StopSequence()
{
    m_nCurrentSeq = -1;
}

void CModelViewSequenceMovementComponent::onHandleControl(entityx::Entity _kEntity)
{
    if (m_nCurrentSeq == -1)
        return;

    if (m_kSequence.empty())
        return;

    if (!_kEntity.has_component <ISceneHandler>())
        return;

    ISceneComponent* pkSceneComp = _kEntity.component<ISceneHandler>()->get();
    CGLMSceneComponent* pkGLMSceneComp = dynamic_cast <CGLMSceneComponent*>(pkSceneComp);
    if (!pkGLMSceneComp)
        return;
    SPosSequence& rkSeqPos = m_kSequence [m_nCurrentSeq];
    glm::vec3 kPos = pkGLMSceneComp->GetPosition();
    glm::vec3 kDef = rkSeqPos.m_kPos - kPos;
    glm::vec3 kCompare = glm::abs(kDef);
    bool bArrived = true;
    for (char nAxis = 0; nAxis < 3; nAxis++) {
        bArrived &= ((kCompare [nAxis] <= 0.01f) || (kCompare[nAxis] < 100.0f));
    }
    if (bArrived) {
        size_t nNext = ++m_nCurrentSeq;
        m_nCurrentSeq = (nNext < m_kSequence.size()) ? nNext : 0;
        return;
    }

    kDef = glm::normalize(kDef);
    kDef *= 100.0f;

    m_kVelocity = kDef;
}