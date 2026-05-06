#pragma once

class ECS_API IMovementSystemContext
{
public:
    virtual ~IMovementSystemContext ()
    {
    }

    virtual int GetContextType() const = 0;
};

class ECS_API IMovementSystem 
    : public entityx::System <IMovementSystem>
    , public entityx::Receiver <IMovementSystem>
{
public:
    virtual ~IMovementSystem()
    {}

    virtual void receive(const IMovementSystemContext&) = 0;
};