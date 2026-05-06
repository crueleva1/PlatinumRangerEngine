#pragma once


class ILanucherConfig;
class IApplication
{
public:
    virtual ~IApplication()
    {
    }

    virtual void Run() = 0;

    virtual void Initialize(const ILanucherConfig& _rkConfig) = 0;

    virtual int Return() = 0;
};