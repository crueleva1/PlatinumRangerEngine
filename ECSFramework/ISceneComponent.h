#pragma once

class ISceneComponent;
typedef TComponent <ISceneComponent> ISceneHandler;

class ECS_API ISceneComponent
{
protected:
    std::string m_kName;
    entityx::Entity m_kEntity;

    virtual void SetParent(ISceneComponent*) = 0;
public:
    ISceneComponent(entityx::Entity& _rkEntity)
        : m_kEntity(_rkEntity)
    {
        bool bHas = m_kEntity.has_component <ISceneHandler>();
        assert(!bHas);
    }

    virtual ~ISceneComponent()
    {}

    entityx::Entity& GetEntity()
    {
        return m_kEntity;
    }

    inline const char* GetName()
    {
        return m_kName.c_str();
    }

    virtual void SetName(const char* _pcName)
    {
        m_kName = _pcName;
    }

    virtual size_t AttachGroup(ISceneComponent*) = 0;

    virtual size_t DeatchGroup(ISceneComponent*) = 0;

    virtual ISceneComponent* GetObjectByName(const char* _pcName) = 0;

    virtual ISceneComponent* GetParent() = 0;

    virtual void Update(float _fDelta) = 0;

    virtual bool IsScreenSpace() = 0;

    virtual const size_t GetChildCount() = 0;

    virtual ISceneComponent* GetChildByIndex(size_t _nIndex) = 0;

    virtual bool IsStatic() = 0;

    virtual void SetStatic(bool _bEnable) = 0;
};