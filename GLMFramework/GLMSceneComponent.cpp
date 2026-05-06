#include "GLMMainPCH.h"
#include "GLMSceneComponent.h"
#include "IGeometryComponent.h"


CGLMSceneComponent::CGLMSceneComponent (entityx::Entity& _rkEntity, ISceneSystem& _rkSceneSys)
    : CDefaultSceneComponent(_rkEntity, _rkSceneSys)
    , m_bIsDirty(false)
{
    m_kScale = glm::vec3(1.0f, 1.0f, 1.0f);
    m_kRotate = glm::mat3(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
}

CGLMSceneComponent::~CGLMSceneComponent ()
{
}

float CGLMSceneComponent::GetDistance(CGLMSceneComponent* pkOther)
{
    if (!pkOther)
        return 0.0f;
    return glm::distance(GetPosition(), pkOther->GetPosition());
}

void CGLMSceneComponent::SetPosition (const glm::vec3& _rkPos)
{
    glm::vec4& rkPos = m_kTransform [3];
    glm::vec3 kCurrPos = rkPos.xyz;
    if (kCurrPos == _rkPos)
        return;
    rkPos.xyz = _rkPos;
    m_bIsDirty |= true;
}

void CGLMSceneComponent::SetRotate (const glm::mat3& _rkRotate)
{
    glm::mat4 kRotate = glm::mat4(m_kRotate.get());
    kRotate = glm::inverse(kRotate);
    m_kTransform *= kRotate;
    kRotate = glm::mat4(_rkRotate);
    m_kTransform *= kRotate;
    m_kRotate = _rkRotate;
}

void CGLMSceneComponent::SetRotate(const glm::mat4& _rkRotate)
{
    glm::mat4 kRotate = glm::mat4(m_kRotate.get());
    kRotate = glm::inverse(kRotate);
    m_kTransform *= kRotate;
    kRotate = _rkRotate;
    m_kTransform *= kRotate;
    m_kRotate = kRotate;
}

void CGLMSceneComponent::OnRotate(const glm::mat4& _rkRotate)
{
    glm::mat4 kRotate = m_kRotate;
    kRotate *= _rkRotate;
    m_kTransform *= kRotate;
    m_kRotate = kRotate;
}

void CGLMSceneComponent::SetScale(const glm::vec3& _rkScale)
{
    glm::mat4 kInvScale;
    for (int nIndex = 0; nIndex < 3; nIndex++) {
        kInvScale[nIndex][nIndex] = m_kScale.get()[nIndex];
    }
    kInvScale = glm::inverse (kInvScale);
    m_kTransform *= kInvScale;
    for (int nIndex = 0; nIndex < 3; nIndex++) {
        kInvScale [nIndex] [nIndex] = _rkScale [nIndex];
    }
    m_kTransform *= kInvScale;
    m_kScale = _rkScale;
}

glm::vec3 CGLMSceneComponent::GetPosition ()
{
    return m_kTransform [3].xyz;
}

glm::vec3 CGLMSceneComponent::GetScale()
{
    return m_kScale.get();
}

glm::mat3 CGLMSceneComponent::GetRotate()
{
    return m_kRotate.get();
}

void CGLMSceneComponent::Update(float _fDelta)
{
    CDefaultSceneComponent::Update(_fDelta);

    if (IsStatic() && !IsWorldDataDirty())
        return;

    ISceneComponent* pkParent = GetParent();
    if (pkParent) {
        CGLMSceneComponent* pkParentTranform = dynamic_cast <CGLMSceneComponent*>(pkParent);
        if (pkParentTranform) {
            m_kTransform *= pkParentTranform->GetTransform();
        }
    }

    if (!m_kEntity.has_component <IGeometryHandler>())
        return;
    IGeometryHandler& rkGeomHandler = *(m_kEntity.component <IGeometryHandler>().get());
    rkGeomHandler->UpdateWorldBound(*this);
    ClearWorldDirty();
}

bool CGLMSceneComponent::IsWorldDataDirty()
{
    bool bDirty = m_kRotate.isDirty();
    bDirty |= m_kScale.isDirty();
    bDirty |= m_bIsDirty;
    return bDirty;
}

void CGLMSceneComponent::ClearWorldDirty()
{
    m_kRotate.clearDirty();
    m_kScale.clearDirty();
    m_bIsDirty = false;
}