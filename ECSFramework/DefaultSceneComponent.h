#pragma once

#include "ISceneComponent.h"

class ISceneSystem;
class ECS_API CDefaultSceneComponent : public ISceneComponent
{
    ISceneSystem& m_rkSceneSystem;
    std::set <ISceneComponent*> m_kChilds;
    std::string m_kName;
    ISceneComponent* m_pkParent;
    bool m_bIsStatic;
    bool m_bScreenSpace;
protected:
    virtual void SetParent(ISceneComponent*);
public:
    CDefaultSceneComponent(entityx::Entity& _rkEntity, ISceneSystem& _rkSceneSys);

    virtual ~CDefaultSceneComponent();

    virtual void SetName(const char* _pcName);

    virtual size_t AttachGroup(ISceneComponent*);

    virtual size_t DeatchGroup(ISceneComponent*);

    virtual ISceneComponent* GetObjectByName(const char* _pcName);

    virtual ISceneComponent* GetParent();

    virtual void Update(float _fDelta);

    virtual bool IsScreenSpace();

    void SetScreenSpace(bool _bEnable);

    virtual const size_t GetChildCount();

    virtual ISceneComponent* GetChildByIndex(size_t _nIndex);

    bool IsStatic()
    {
        return m_bIsStatic;
    }

    void SetStatic(bool _bEnable)
    {
        m_bIsStatic = _bEnable;
    }
};