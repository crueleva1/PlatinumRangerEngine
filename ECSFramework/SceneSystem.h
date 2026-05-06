#pragma once


#include "TSystemInterface.h"
#include "ISceneComponent.h"


class ECS_API ISceneContext
{
public:
    virtual ~ISceneContext()
    {

    }

    virtual int GetContextType() const = 0;
};

class ECS_API ISceneSystem 
    : public entityx::System <ISceneSystem>
    , public entityx::Receiver <ISceneSystem>
{
public:
    virtual ~ISceneSystem()
    {
    }

    virtual void receive(const ISceneContext&) = 0;

    virtual void Attached(ISceneComponent* _pkChild) = 0;

    virtual void Detached(ISceneComponent* _pkChild) = 0;

    virtual void Renamed(ISceneComponent* _pkChild, const char* _pcNewName) = 0;
};