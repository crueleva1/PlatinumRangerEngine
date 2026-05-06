#include "MainPCH.h"
#include "ModelViewMovementSystem.h"
#include "ModelViewMovementComponent.h"
#include "ModelViewSceneComponent.h"

CModelViewMovementSystem::CModelViewMovementSystem()
{

}

CModelViewMovementSystem::~CModelViewMovementSystem()
{

}

void CModelViewMovementSystem::update(entityx::EntityManager& _rkEntities, entityx::EventManager& _rkEvents, entityx::TimeDelta _fDelta)
{
    CDefaultMovementSystem::update (_rkEntities, _rkEvents, _fDelta);

    for (auto kEntity : _rkEntities.entities_with_components <IMovementHandler, ISceneHandler>()) {
        IMovementComponent* pkMovement = kEntity.component <IMovementHandler>()->get ();
        ISceneComponent* pkScene = kEntity.component <ISceneHandler>()->get ();

        CModelViewMovementComponent* pkModelMovement = dynamic_cast <CModelViewMovementComponent*>(pkMovement);
        if (!pkModelMovement)
            continue;
        CModelViewSceneComponent* pkModelScene = dynamic_cast <CModelViewSceneComponent*>(pkScene);
        if (!pkModelScene)
            continue;

        glm::vec4 kVelocity = glm::vec4(pkModelMovement->GetVelocity (), 0.0f);
        kVelocity = pkModelMovement->GetRotate () * kVelocity;
        glm::vec3 kPos = pkModelScene->GetPosition ();
        kPos += (glm::vec3(kVelocity) * (float)_fDelta);
        pkModelScene->SetPosition(kPos);
        pkModelScene->SetRotate(pkModelMovement->GetRotate());
    }
}

void CModelViewMovementSystem::receive(const IMovementSystemContext& _rkContext)
{
    EMovementSystemContextType eType = (EMovementSystemContextType)_rkContext.GetContextType();
    switch (eType)
    {
        case eMSCT_CreateKBComp:
        {
            const CCreateKBMovementCompContext& rkContext = static_cast <const CCreateKBMovementCompContext&>(_rkContext);
            entityx::Entity kEntity = rkContext.GetEntity();
            if (!kEntity)
                return;

            if (kEntity.has_component <IMovementHandler> ()) {
                kEntity.remove <IMovementHandler>();
            }

            std::shared_ptr <CModelViewKBMovementComponent> spkComp = std::make_shared <CModelViewKBMovementComponent>();
            kEntity.assign <IMovementHandler> (spkComp);
            if (kEntity.has_component <ISceneHandler> ()) {
                CModelViewSceneComponent* pkSceneComp = dynamic_cast <CModelViewSceneComponent*>(kEntity.component <ISceneHandler>()->get());
                if (pkSceneComp)
                    spkComp->SetCurrentRotation (pkSceneComp->GetRotate ());
            }
            break;
        }
        case eMSCT_CreateSeqComp:
        {
            const CCreateSequenceMovementCompContext& rkContext = static_cast <const CCreateSequenceMovementCompContext&>(_rkContext);
            entityx::Entity kEntity = rkContext.GetEntity();
            if (!kEntity)
                return;

            if (kEntity.has_component <IMovementHandler>()) {
                kEntity.remove <IMovementHandler>();
            }

            std::shared_ptr <CModelViewSequenceMovementComponent> spkComp = std::make_shared <CModelViewSequenceMovementComponent>();
            kEntity.assign <IMovementHandler>(spkComp);
            const std::string& rkRotate = rkContext.GetSequenceString();
            if (!rkRotate.empty()) {
                std::vector <std::string> kSequenceSet;
                SplitToken(rkRotate, '|', kSequenceSet);
                glm::mat3 kRotateMat;
                int nColumn = 0;
                
                for (auto kPosString : kSequenceSet) {
                    std::vector <std::string> kPos;
                    SplitToken(kPosString, ',', kPos);
                    if (kPos.size() != 3) {
                        continue;
                    }
                    int nDim = 0;
                    CModelViewSequenceMovementComponent::SPosSequence kSeq;
                    for (auto kRow : kPos) {
                        kSeq.m_kPos[nDim++] = std::stof(kRow);
                    }
                    spkComp->AddSequence(kSeq);
                }
            }
            if (rkContext.IsAutoStart()) {
                spkComp->StartSequence();
            }
            if (kEntity.has_component <ISceneHandler>()) {
                CModelViewSceneComponent* pkSceneComp = dynamic_cast <CModelViewSceneComponent*>(kEntity.component <ISceneHandler>()->get());
                if (pkSceneComp)
                    spkComp->SetCurrentRotation(pkSceneComp->GetRotate());
            }
        }
        default:
            break;
    }
}