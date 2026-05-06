#include "MainPCH.h"
#include "ModelViewSceneComponent.h"
#include "GLSLMaterial.h"
#include "IRenderComponent.h"


CModelViewSceneComponent::CModelViewSceneComponent(entityx::Entity& _rkEntity, ISceneSystem& _rkSceneSys)
    : CGLMSceneComponent(_rkEntity, _rkSceneSys)
{}

CModelViewSceneComponent::~CModelViewSceneComponent()
{}

void CModelViewSceneComponent::Update (float _fDelta)
{
    CGLMSceneComponent::Update (_fDelta);
}

void CModelViewSceneComponent::UpdateMaterialConstant(IMaterial* _pkMaterial)
{
    CGLSLMaterial* pkMaterial = dynamic_cast <CGLSLMaterial*>(_pkMaterial);
    if (!pkMaterial)
        return;

    const size_t nSize = pkMaterial->GetConstantCount();
    for (size_t nIndex = 0; nIndex < nSize; nIndex++) {
        IConstantSemantic* pkSemantic = pkMaterial->GetConstant(nIndex);
        if (!pkSemantic)
            continue;

        if (pkSemantic->GetType() != eConstant_Matrix4x4)
            continue;

        if (strstr(pkSemantic->GetVaribleName(), "WorldMatrix")) {
            TConstantSemantic <glm::mat4>* pkView = static_cast <TConstantSemantic <glm::mat4>*>(pkSemantic);
            if (pkView) {
                pkView->SetValue(GetTransform());
            }
        }
    }

}