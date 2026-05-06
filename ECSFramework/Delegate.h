#pragma once

#include <map>
#include <memory>

#define RegisterFunctionPtr(FunctionPtr) &FunctionPtr, #FunctionPtr
#define RegisterObjectFunctionPtr(Object, FunctionPtr) Object, &FunctionPtr, #FunctionPtr
#define FunctionStr(Function) #Function

enum EDelegateFunctionType
{
    eDFT_Function,
    eDFT_MemberFunction,
    eDFT_Max,
};

class ECS_API IFunctionInterface
{
    EDelegateFunctionType m_eType;
public:
    IFunctionInterface(EDelegateFunctionType _eType)
        :m_eType(_eType)
    {
    }

    virtual ~IFunctionInterface()
    {
    }

    EDelegateFunctionType GetType()
    {
        return m_eType;
    }

    virtual const char* GetFunctionName() = 0;

    virtual void* GetInsPtr()
    {
        return nullptr;
    }
};

template <typename Return, typename... Args>
class TFunctionInterface : public IFunctionInterface
{
public:
    TFunctionInterface(EDelegateFunctionType _eType)
        :IFunctionInterface(_eType)
    {}

    virtual ~TFunctionInterface()
    {}

    virtual typename Return Invoke(Args&& ...args) = 0;
};

template <typename Object, typename Return, typename... Args>
class TObjectFunctionInterface : public TFunctionInterface<Return, Args...>
{
public:
    TObjectFunctionInterface(EDelegateFunctionType _eType)
        :TFunctionInterface(_eType)
    {}

    virtual ~TObjectFunctionInterface()
    {}

    virtual typename Object* GetInstance() = 0;
};

template <typename Return, typename... Args>
class TFunctionCallback : public TFunctionInterface<Return, Args...>
{
    typedef Return(*Callback)(Args ...);
    Callback m_pfCallBack;
    std::string m_kFunctionName;
public:
    TFunctionCallback(Callback _fpCallback, const char* _pcFunctionName)
        :TFunctionInterface<Return, Args...>(eDFT_Function)
        ,m_pfCallBack(_fpCallback)
        ,m_kFunctionName(_pcFunctionName)
    {
    }

    virtual ~TFunctionCallback()
    {
    }

    typename Return Invoke(Args&& ...args)
    {
        return m_pfCallBack(std::forward<Args>(args)...);
    }

    const char* GetFunctionName()
    {
        return m_kFunctionName.c_str();
    }
};

template <typename Object, typename Return, typename... Args>
class TObjectFunctionCallback : public TObjectFunctionInterface<Object, Return, Args...>
{
    typedef Return(Object::*Callback)(Args ...);
    Object& m_rkObject;
    Callback m_pfCallBack;
    std::string m_kFunctionName;
public:
    template <typename FunctionPtr>
    TObjectFunctionCallback(Object& _rkObject, FunctionPtr _pfCallback, const char* _pcFunctionName)
        :TObjectFunctionInterface<Object, Return, Args...>(eDFT_MemberFunction)
        ,m_rkObject(_rkObject)
        ,m_pfCallBack(_pfCallback)
    {
        char cBuff [512] = {};
        sprintf_s(cBuff, sizeof(cBuff), "%p_%s", &m_rkObject, _pcFunctionName);
        m_kFunctionName = cBuff;
    }
    
    template <typename FunctionPtr>
    TObjectFunctionCallback(Object* _pkObject, FunctionPtr _pfCallback, const char* _pcFunctionName)
        :TObjectFunctionInterface<Object, Return, Args...>(eDFT_MemberFunction)
        ,m_rkObject(*_pkObject)
        ,m_pfCallBack(_pfCallback)
    {
        char cBuff [512] = {};
        sprintf_s(cBuff, sizeof(cBuff), "%p_%s", &m_rkObject, _pcFunctionName);
        m_kFunctionName = cBuff;
    }

    virtual ~TObjectFunctionCallback()
    {
    }

    void* GetInsPtr()
    {
        return &m_rkObject;
    }

    typename Object* GetInstance()
    {
        return &m_rkObject;
    }

    typename Return Invoke(Args&& ...args)
    {
        return (m_rkObject.*m_pfCallBack)(std::forward<Args>(args)...);
    }

    const char* GetFunctionName()
    {
        return m_kFunctionName.c_str();
    }

    Object* GetObject()
    {
        return &m_rkObject;
    }
};

template <typename Return>
class TFunctionDelegateReturn : public std::vector <Return>
{
public:
    TFunctionDelegateReturn()
    {
    }

    virtual ~TFunctionDelegateReturn()
    {
    }
};

template <typename Object, typename Return>
class TObjectDelegateReturn : public std::multimap <Object*, Return>
{
public:
    TObjectDelegateReturn()
    {}

    virtual ~TObjectDelegateReturn()
    {}
};

class ECS_API IDelegate
{
public:
    virtual ~IDelegate()
    {
    }

    virtual bool RegisterDelegate(std::shared_ptr<IFunctionInterface> _spkFunction) = 0;

    virtual void UnregisterDelegate(std::shared_ptr<IFunctionInterface> _spkFunction) = 0;

    virtual void UnregisterDelegate(const char* _pcFunctionName) = 0;

    virtual void UnregisterDelegate(void* _pInstnace, const char* _pcFunctionName) = 0;

    virtual bool IsExist(const char* _pcFunctionName) = 0;
};

class ECS_API CDelegate : public IDelegate
{
    typedef std::multimap <std::string, std::shared_ptr<IFunctionInterface> > CFunctionMap;
    typedef CFunctionMap::iterator CFunctionMapItr;
    typedef std::pair<CFunctionMap::iterator, CFunctionMap::iterator> CFunctionMapItrPair;
    CFunctionMap m_kFunctionMap;
public:

    CDelegate()
    {
    }

    virtual ~CDelegate()
    {
    }

    template <typename Return, typename ... Args>
    typename TFunctionDelegateReturn<Return> Invoke(const char* _pcFunctionName, Args&& ...args)
    {
        TFunctionDelegateReturn<Return> kReturn;
        CFunctionMapItrPair kItrPair = m_kFunctionMap.equal_range(_pcFunctionName);
        if (kItrPair.first == kItrPair.second)
            return kReturn;
        for (CFunctionMapItr kIt = kItrPair.first; kIt != kItrPair.second; ++kIt) {
            std::shared_ptr <IFunctionInterface> spkCallback = kIt->second;
            assert(spkCallback->GetType() == eDFT_Function);
            TFunctionCallback<Return, Args...>* pkFunction = static_cast<TFunctionCallback<Return, Args...>*>(spkCallback.get());
            kReturn.emplace_back(pkFunction->Invoke(std::forward<Args>(args)...));
        }
        return kReturn;
    }

    template <typename Object, typename Return, typename ... Args>
    typename TObjectDelegateReturn<Object, Return> ObjectInvoke(const char* _pcFunctionName, Args&& ...args)
    {
        TObjectDelegateReturn<Object, Return> kReturn;
        CFunctionMapItrPair kItrPair = m_kFunctionMap.equal_range(_pcFunctionName);
        if (kItrPair.first == kItrPair.second)
            return kReturn;
        for (CFunctionMapItr kIt = kItrPair.first; kIt != kItrPair.second; ++kIt) {
            std::shared_ptr <IFunctionInterface> spkCallback = kIt->second;
            assert(spkCallback->GetType() == eDFT_MemberFunction);
            TObjectFunctionCallback<Object, Return, Args...>* pkFunction = static_cast<TObjectFunctionCallback<Object, Return, Args...>* >(spkCallback.get());
            kReturn.insert(std::make_pair(pkFunction->GetObject(), pkFunction->Invoke(std::forward<Args>(args)...)));
        }
        return kReturn;
    }

    virtual bool RegisterDelegate(std::shared_ptr<IFunctionInterface> _spkFunction)
    {
        if (m_kFunctionMap.find(_spkFunction->GetFunctionName()) != m_kFunctionMap.end())
            return false;

        m_kFunctionMap.insert(std::make_pair(_spkFunction->GetFunctionName(), _spkFunction));
        return true;
    }

    virtual void UnregisterDelegate(std::shared_ptr<IFunctionInterface> _spkFunction)
    {
        UnregisterDelegate(_spkFunction->GetFunctionName());
    }

    virtual void UnregisterDelegate(const char* _pcFunctionName)
    {
        m_kFunctionMap.erase(_pcFunctionName);
    }

    virtual void UnregisterDelegate(void* _pInstnace, const char* _pcFunctionName)
    {
        char cBuff[512] = {};
        sprintf_s(cBuff, sizeof(cBuff), "%p_%s", _pInstnace, _pcFunctionName);
        m_kFunctionMap.erase(_pcFunctionName);
    }

    bool IsExist(const char* _pcFunctionName)
    {
        return (m_kFunctionMap.find(_pcFunctionName) != m_kFunctionMap.end());
    }
};