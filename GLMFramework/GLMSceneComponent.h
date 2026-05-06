#pragma once

#include "DefaultSceneComponent.h"
#include "DataModifier.h"

struct IMaterial;
class ECSGLM_API CGLMSceneComponent : public CDefaultSceneComponent
{
    glm::mat4 m_kTransform;
    TDataModifier<glm::mat3> m_kRotate;
    TDataModifier<glm::vec3> m_kScale;
    bool m_bIsDirty;

    bool IsWorldDataDirty();
    void ClearWorldDirty();
public:
    CGLMSceneComponent(entityx::Entity& _rkEntity, ISceneSystem& _rkSceneSys);

    virtual ~CGLMSceneComponent();

    inline const glm::mat4& GetTransform()
    {
        return m_kTransform;
    }

    float GetDistance(CGLMSceneComponent* pkOther);

    void SetPosition(const glm::vec3& _rkPos);

    void SetScale(const glm::vec3& _rkScale);

    void SetRotate(const glm::mat3& _rkRotate);

    void SetRotate(const glm::mat4& _rkRotate);

    void OnRotate(const glm::mat4& _rkRotate);

    glm::vec3 GetPosition();

    glm::vec3 GetScale();

    glm::mat3 GetRotate();

    virtual void Update (float _fDelta);

    virtual void UpdateMaterialConstant(IMaterial* _pkMaterial) = 0;
};