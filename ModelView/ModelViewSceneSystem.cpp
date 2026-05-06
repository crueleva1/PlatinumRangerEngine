#include "MainPCH.h"
#include "ModelViewSceneSystem.h"
#include "ModelViewSceneComponent.h"
#include "GLMBaseCameraComponent.h"
#include "Win32KeyboardMouseInputComponent.h"
#include "AssimpGLGeometryComp.h"
#include "GLMBoxBound.h"
#include <numeric>


class ECS_MODELVIEW_API CKdTestCubeContext : public ITreeContext
{
    glm::vec3 m_kOrgin;
public:
    CKdTestCubeContext()
    {

    }

    virtual ~CKdTestCubeContext()
    {

    }

    float getDimension(int _nDimension) const
    {
        float fValue = std::numeric_limits<float>::quiet_NaN();
        switch (_nDimension) {
        case 0:
        {
            fValue = m_kOrgin.x;
            break;
        }
        case 1:
        {
            fValue = m_kOrgin.y;
            break;
        }
        case 2:
        {
            fValue = m_kOrgin.z;
            break;
        }
        default:
            break;
        }
        return fValue;
    }
};

CSceneCreateContext::CSceneCreateContext(entityx::Entity& _rkEntity, CPropertySet& _rkSet)
    : CPropertySet(_rkSet)
    , m_rkEntity(_rkEntity)
{
}

CSceneCreateContext::CSceneCreateContext(entityx::Entity& _rkEntity)
    : CPropertySet()
    , m_rkEntity(_rkEntity)
{
}

CSceneCreateContext::~CSceneCreateContext()
{

}

int CSceneCreateContext::GetContextType() const
{
    return eSCT_Create;
}

entityx::Entity& CSceneCreateContext::GetEntity() const
{
    return m_rkEntity;
}

CSceneContext::CSceneContext(std::shared_ptr<ISceneComponent> _spkSceneObj)
    : m_spkSceneObj(_spkSceneObj)
{}

CSceneContext::~CSceneContext()
{}

float CSceneContext::getDimension(int _nDimension) const
{
    CGLMSceneComponent* pkSceneComp = dynamic_cast <CGLMSceneComponent*>(m_spkSceneObj.get());
    if (!pkSceneComp)
        return std::numeric_limits<float>::quiet_NaN();
    glm::vec3 kPos = pkSceneComp->GetPosition();
    float fValue = std::numeric_limits<float>::quiet_NaN();
    switch (_nDimension)
    {
        case 0:
        {
            fValue = kPos.x;
            break;
        }
        case 1:
        {
            fValue = kPos.y;
            break;
        }
        case 2:
        {
            fValue = kPos.z;
            break;
        }
        default:
            break;
    }
    return fValue;
}

TPoint <3> CSceneContext::getCenter() const
{
    TPoint <3> kCenter;
    if (!m_spkSceneObj)
        return kCenter;

    CGLMSceneComponent* pkGLMSceneComp = dynamic_cast <CGLMSceneComponent*> (m_spkSceneObj.get());
    if (!pkGLMSceneComp)
        return kCenter;

    entityx::Entity kEntity = m_spkSceneObj->GetEntity();
    if (!kEntity.has_component<IGeometryHandler>())
        return kCenter;

    IGeometryHandler& rkGeomHandler = (*kEntity.component <IGeometryHandler>().get());
    CGLMBoundBox* pkBound = static_cast <CGLMBoundBox*>(rkGeomHandler->GetBound());
    const glm::vec3& kWorldCenter = pkBound->GetWorldCenter();
    for (char nDim = 0; nDim < 3; nDim++) {
        kCenter [nDim] = kWorldCenter[nDim];
    }
    return kCenter;
}

TPoint <3> CSceneContext::getHalf() const
{
    TPoint <3> kHalf;
    if (!m_spkSceneObj)
        return kHalf;

    entityx::Entity kEntity = m_spkSceneObj->GetEntity();
    if (!kEntity.has_component<IGeometryHandler>())
        return kHalf;

    IGeometryHandler& rkGeomHandler = (*kEntity.component <IGeometryHandler>().get());
    CGLMBoundBox* pkBound = static_cast <CGLMBoundBox*>(rkGeomHandler->GetBound());
    const glm::vec3& rkHalf = pkBound->GetBoxVector();
    for (char nDim = 0; nDim < 3; nDim++) {
        kHalf[nDim] = rkHalf[nDim];
    }
    return kHalf;
}

const CSceneContext& CSceneContext::operator=(const CSceneContext& _rkContext)
{
    m_spkSceneObj = _rkContext.m_spkSceneObj;
    return *this;
}

ISceneComponent* CSceneContext::GetSceneComponent() const
{
    return m_spkSceneObj.get();
}

CModelViewSceneSystem* CModelViewSceneSystem::m_pkThis = nullptr;

CModelViewSceneSystem::CModelViewSceneSystem()
    :m_bRebuildTree(true)
    ,m_kSceneCubeTree(TPoint <3>(), 3500.0f, 4)
{
    m_pkThis = this;
}

CModelViewSceneSystem::~CModelViewSceneSystem()
{

}

void CModelViewSceneSystem::configure(entityx::EntityManager& _rkEntities, entityx::EventManager& _rkEvents)
{
    _rkEvents.subscribe <ISceneContext, ISceneSystem>(*this);
}

void CModelViewSceneSystem::receive(const ISceneContext& _rkContext)
{
    ESceneContextType eType = (ESceneContextType)_rkContext.GetContextType();
    switch (eType)
    {
    case eSCT_Create:
    {
        const CSceneCreateContext& rkCreateContext = static_cast <const CSceneCreateContext&>(_rkContext);
        AssignSceneComponent(rkCreateContext);
    }
        break;
    default:
        break;
    }
}

void CModelViewSceneSystem::update(entityx::EntityManager& _rkEntities, entityx::EventManager& _rkEvents, entityx::TimeDelta _fDelta)
{
    if (m_bRebuildTree) {
        m_bRebuildTree = false;
        //m_kSceneKDTree.buildTree(m_kStaticList);
    }
    CDefaultSceneSystem::update(_rkEntities, _rkEvents, _fDelta);
    // update camera pos
    for (auto kEntity : _rkEntities.entities_with_components <ISceneHandler, ICameraHandler>()) {
        ISceneComponent* pkSceneComp = kEntity.component <ISceneHandler>()->get();
        ICameraComponent* pkCameraComp = kEntity.component <ICameraHandler>()->get();
        if (!pkCameraComp->IsCameraActive())
            continue;
        CGLMBaseCameraComponent* pkModelCameraComp = dynamic_cast <CGLMBaseCameraComponent*>(pkCameraComp);
        if (!pkModelCameraComp)
            continue;
        pkModelCameraComp->UpdateViewMatrix(*pkSceneComp);
    }
}

bool CModelViewSceneSystem::AssignSceneComponent(const CSceneCreateContext& rkContext)
{
    entityx::Entity& rkEntity = rkContext.GetEntity();
    CModelViewSceneComponent* pkComp = nullptr;
    if (rkEntity.has_component <ISceneHandler>()) {
        entityx::ComponentHandle <ISceneHandler> kHandler = rkEntity.component<ISceneHandler>();
        ISceneComponent* pkIComp = kHandler->get();
        if (rkEntity != pkIComp->GetEntity()) {
            assert(0);
            return false;
        }
        pkComp = dynamic_cast <CModelViewSceneComponent*>(pkIComp);
        if (!pkComp) {
            rkEntity.remove <ISceneHandler>();
        }
    }
    std::shared_ptr <CModelViewSceneComponent> spkComp;
    if (!pkComp) {
        spkComp = std::make_shared<CModelViewSceneComponent>(rkEntity, *this);
        pkComp = spkComp.get();
        rkEntity.assign<ISceneHandler>(spkComp);
    }
    {
        const std::string& rkName = rkContext.GetPropertyStr("Name");
        if (!rkName.empty()) {
            pkComp->SetName(rkName.c_str());
        }
    }
    {
        const std::string& rkScreenSpace = rkContext.GetPropertyStr("ScreenSpace");
        pkComp->SetScreenSpace(rkScreenSpace == "true");
    }
    {
        const std::string& rkPosition = rkContext.GetPropertyStr("Position");
        if (!rkPosition.empty()) {
            std::vector <std::string> kDatas;
            SplitToken(rkPosition, ',', kDatas);
            if (kDatas.size() != 3) {
                return false;
            }
            glm::vec3 kPos = glm::vec3(std::stof(kDatas [0]), std::stof(kDatas [1]), std::stof(kDatas [2]));
            if (pkComp->IsScreenSpace()) {
                IRenderer* pkRenderer = IRenderSystem::GetRenderer();
                float nWidth = (float)pkRenderer->GetScreenWidth();
                float nHeight = (float)pkRenderer->GetScreenHeight();
                kPos[0] -= nWidth / 2.0f;
                kPos[1] += nHeight / 2.0f;
            }
            pkComp->SetPosition(kPos);
        }
    }
    {
        const std::string& rkScale = rkContext.GetPropertyStr("Scale");
        if (!rkScale.empty()) {
            std::vector <std::string> kDatas;
            SplitToken(rkScale, ',', kDatas);
            if (kDatas.size() != 3) {
                return false;
            }
            glm::vec3 kScale = glm::vec3(std::stof(kDatas [0]), std::stof(kDatas [1]), std::stof(kDatas [2]));
            pkComp->SetScale(kScale);
        }
    }
    {
        const std::string& rkRotate = rkContext.GetPropertyStr("Rotate");
        if (!rkRotate.empty()) {
            std::vector <std::string> kColumns;
            SplitToken(rkRotate, '|', kColumns);
            if (kColumns.size() != 3) {
                return false;
            }
            glm::mat3 kRotateMat;
            int nColumn = 0;
            int nRow = 0;
            for (auto kColumn : kColumns) {
                std::vector <std::string> kPos;
                SplitToken(kColumn, ',', kPos);
                if (kPos.size() != 3) {
                    return false;
                }
                for (auto kRow : kPos) {
                    kRotateMat [nColumn] [nRow] = std::stof(kRow);
                    nRow++;
                }
                nRow = 0;
                nColumn++;
            }
            pkComp->SetRotate(kRotateMat);
        }
    }
    std::shared_ptr <CSceneContext> spkScenContext;
    {
        const std::string& rkStatic = rkContext.GetPropertyStr("IsStatic");
        bool bIsStatic = !rkStatic.empty();
        spkComp->SetStatic(bIsStatic);
        if (bIsStatic && !pkComp->IsScreenSpace()) {
            spkScenContext = std::make_shared <CSceneContext>(spkComp);
            m_kStaticList.emplace_back(spkScenContext);
            m_bRebuildTree |= true;
        }
    }
    // Need to do XForm for correct position
    if (rkEntity.has_component <IGeometryHandler>()) {
        IGeometryComponent* pkGeom = rkEntity.component<IGeometryHandler>()->get();
        CAssimpGLGeometryComp* pkAssGeom = dynamic_cast <CAssimpGLGeometryComp*>(pkGeom);
        if (pkAssGeom) {
            pkAssGeom->ActiveGeometry(0);
            CGLMBoundBox* pkBoundBox = static_cast<CGLMBoundBox*>(pkAssGeom->GetBound());
            if (pkBoundBox) {
                pkComp->SetPosition(pkBoundBox->GetCenter());
            }
            pkBoundBox->SetCenter(glm::vec3());
            pkBoundBox->SetWorldTransform(pkComp->GetTransform());
        }

        pkGeom->UpdateWorldBound(*pkComp);
    }
    if (rkEntity.has_component <ICameraHandler>()) {
        ICameraHandler& rkCamera = (*rkEntity.component <ICameraHandler>().get());
        rkCamera->UpdateCameraName(*pkComp);
    }

    if (spkScenContext) {   // Static Object
        m_kSceneCubeTree.InsertBoundBox(spkScenContext);
    }
    else if (!pkComp->IsScreenSpace()) {    // Dynamic Object
        if (!pkComp->GetParent())
            Attached(pkComp);
    }
    return true;
}