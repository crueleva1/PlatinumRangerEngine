#pragma once


template <typename T>
class TScopeHelper
{
    T& m_rkHandler;
public:

    TScopeHelper(T& _rkHandler)
        :m_rkHandler(_rkHandler)
    {
        m_rkHandler.EnterScope();
    }


    ~TScopeHelper()
    {
        m_rkHandler.LeaveScope();
    }
};

template <typename T, typename ... Args>
class TScopeArgHelper
{
    std::tuple<Args ...> m_kArgs;
    T& m_rkHandler;
public:
    template <typename ... Args>
    TScopeArgHelper(T& _rkHandler, Args&& ... args)
        :m_rkHandler(_rkHandler)
        ,m_kArgs(std::forward<Args>(args)...)
    {
        m_rkHandler.EnterScope(std::forward<Args>(args)...);
    }


    ~TScopeArgHelper()
    {
        m_rkHandler.LeaveScope(std::get<Args>(m_kArgs)...);
    }
};

template <typename T, typename F1, typename F2>
class TScopeHelperByFunctor
{
    F1 m_kEnterFunctor;
    F2 m_kLeaveFunctor;
    T& m_rkHandler;
public:
    TScopeHelperByFunctor(T& _rkHandler, F1 _kEnterFunctor, F2 _kLeaveFunctor)
        :m_rkHandler(_rkHandler)
        ,m_kEnterFunctor(_kEnterFunctor)
        ,m_kLeaveFunctor(_kLeaveFunctor)
    {
        m_kEnterFunctor(m_rkHandler);
    }

    ~TScopeHelperByFunctor()
    {
        m_kLeaveFunctor(m_rkHandler);
    }
};