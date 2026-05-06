#pragma once


#include "DefaultSceneSystem.h"
#include "kdTree.h"
#include "GLMBinTree.h"
#include "PropertySet.h"

enum ESceneContextType
{
    eSCT_None,
    eSCT_Create,
};

class ECS_MODELVIEW_API CSceneCreateContext 
    : public ISceneContext
    , public CPropertySet
{
    entityx::Entity& m_rkEntity;
public:
    CSceneCreateContext(entityx::Entity& _rkEntity, CPropertySet& _rkMap);

    CSceneCreateContext(entityx::Entity& _rkEntity);

    virtual ~CSceneCreateContext();

    virtual int GetContextType() const;

    entityx::Entity& GetEntity() const;    
};

class ECS_MODELVIEW_API ITreeContext
{
public:
    virtual ~ITreeContext()
    {}

    virtual float getDimension(int _nDimension) const = 0;

    virtual TPoint <3> getCenter() const = 0;

    virtual TPoint <3> getHalf() const = 0;
};

class ECS_MODELVIEW_API CSceneContext : public ITreeContext
{
    std::shared_ptr <ISceneComponent> m_spkSceneObj;
    unsigned int m_nQueriedRevision;
public:
    static const int dimension = 3;

    CSceneContext(std::shared_ptr <ISceneComponent> _spkSceneObj);

    virtual ~CSceneContext();

    float getDimension(int _nDimension) const;

    TPoint <3> getCenter() const;

    TPoint <3> getHalf() const;

    const CSceneContext& operator=(const CSceneContext& _rkRight);

    ISceneComponent* GetSceneComponent() const;

    inline void SetDataQueryed(unsigned int _nRevsiosn) const
    {
        unsigned int& rnQueriedRevision = const_cast <unsigned int&>(m_nQueriedRevision);
        rnQueriedRevision = _nRevsiosn;
    }

    inline bool IsDataQueried(unsigned int _nRevision) const
    {
        return m_nQueriedRevision == _nRevision;
    }
};

class ECS_MODELVIEW_API CModelViewSceneSystem : public CDefaultSceneSystem
{
    TBinTree <std::shared_ptr <ITreeContext>, 3> m_kSceneCubeTree;
    std::vector <std::shared_ptr <ITreeContext> > m_kStaticList;
    static CModelViewSceneSystem* m_pkThis;
    bool m_bRebuildTree;

    template <typename Functor>
    void TestDynamicObjects(Functor& _fnFunctor, ISceneComponent* pkComp)
    {
        if (!_fnFunctor(pkComp))
            return;

        const size_t nCount = pkComp->GetChildCount();
        for (size_t nIndex = 0; nIndex < nCount; nIndex++) {
            ISceneComponent* pkChild = pkComp->GetChildByIndex(nIndex);
            if (!pkChild)
                continue;
            TestDynamicObjects(_fnFunctor, pkChild);
        }
    }
public: 
    CModelViewSceneSystem();

    virtual ~CModelViewSceneSystem();

    virtual void configure(entityx::EntityManager& _rkEntities, entityx::EventManager& _rkEvents);

    virtual void update(entityx::EntityManager& _rkEntities, entityx::EventManager& _rkEvents, entityx::TimeDelta _fDelta);

    virtual void receive(const ISceneContext&);

    bool AssignSceneComponent(const CSceneCreateContext& pkContext);

    template <typename Functor>
    void PreOrderTestOctree(Functor& _fnFunctor, bool _bNonRecusive = false)
    {
        m_kSceneCubeTree.PreOrderTraveral<Functor>(_fnFunctor, _bNonRecusive);
    }

    template <typename Functor>
    void TestDynamicObjects(Functor& _fnFunctor)
    {
        for (auto& kIt : m_kRootedComp) {
            TestDynamicObjects(_fnFunctor, kIt.second);
        }
    }

    void ResetQueryFlag();

    static CModelViewSceneSystem& GetSingleton()
    {
        return *m_pkThis;
    }

};