#pragma once

#include "IResourceComponent.h"

class ECS_API CStream : public IStream
{
    std::vector <char> m_kData;
    size_t m_nSeek;
public:
    CStream()
        : m_nSeek(0)
    {

    }

    CStream (size_t _nSize)
        : m_kData(_nSize)
        , m_nSeek(0)
    {

    }

    virtual ~CStream()
    {

    }

    void ResetSeek()
    {
        m_nSeek = 0;
    }

    size_t GetSeek()
    {
        return m_nSeek;
    }

    void SetData(const char* _pcData, size_t _nSize)
    {
        if (!_pcData)
            return;

        m_kData.resize(_nSize);
        m_nSeek = 0;
        memcpy(m_kData.data(), _pcData, _nSize);
    }

    void ClearData()
    {
        m_nSeek = 0;
        m_kData.clear();
    }

    bool Read(char*& _rpcOut, size_t _nSize)
    {
        const size_t nRemainSize = m_kData.size() - m_nSeek;
        if (nRemainSize == 0)
            return false;
        if (nRemainSize < _nSize)
            return false;
        memcpy(_rpcOut, (m_kData.data() + m_nSeek), _nSize);
        m_nSeek += _nSize;
        return true;
    }

    bool ReadSection(char*& _rpcOut, size_t _nOffset, size_t _nSize)
    {
        const size_t nMaxSize = m_kData.size();
        if (_nOffset >= nMaxSize)
            return false;

        size_t nRemainSize = nMaxSize - _nOffset;
        if (nRemainSize < _nSize)
            return false;

        memcpy(_rpcOut, (m_kData.data() + _nOffset), _nSize);
        return true;
    }

    bool Write(char*& _rpcIn, size_t _nSize)
    {
        size_t nOldSize = m_kData.size();
        m_kData.resize(nOldSize + _nSize);
        memcpy((m_kData.data() + nOldSize), _rpcIn, _nSize);
        return true;
    }

    bool WriteSection(char*& _rpcIn, size_t _nOffset, size_t _nSize)
    {
        size_t nTargetTotalSize = _nOffset + _nSize;
        if (nTargetTotalSize > m_kData.size())
            m_kData.resize(nTargetTotalSize);

        memcpy((m_kData.data() + _nOffset), _rpcIn, _nSize);
        return true;
    }

    void SetSize(size_t _nSize)
    {
        m_kData.resize(_nSize);
    }

    char* GetData()
    {
        if (m_kData.empty())
            return NULL;
        return m_kData.data();
    }

    size_t GetDataSize()
    {
        return m_kData.size();
    }
};

class ECS_API CDefaultResourceComponent : public IResourceStreamComponent
{
public:
    enum EStatus
    {
        eStatus_Error = -1,
        eStatus_Idle,
        eStatus_Loading,
        eStatus_Saving,
        eStatus_AsyncLoadingStart,
        eStatus_AsyncLoading,
        eStatus_AsyncLoadingEnd,
        eStatus_AsyncSavingStart,
        eStatus_AsyncSaving,
        eStatus_AsyncSavingEnd,
    };

    CDefaultResourceComponent();

    virtual ~CDefaultResourceComponent();

    virtual ISerializeStreamCallBack* GetSerializerCallBack();

    virtual bool SetSerializerCallBack(ISerializeStreamCallBack*);

    virtual bool IsValid() const;

    virtual bool SetSave(bool _bAsync = false) const;

    virtual bool SetLoad(bool _bAsync = false) const;

    virtual int GetStatus() const
    {
        return m_eStatus;
    }

    virtual bool ReadBinrary(char*& _rpcData, size_t _nSize);

    virtual bool WriteBinrary(char*& _rpcData, size_t _nSize);

    virtual bool ReadBinrary(char*& _rpcData, size_t _nOffset, size_t _nSize);

    virtual bool WriteBinrary(char*& _rpcData, size_t _nOffset, size_t _nSize);

    virtual void ResetSeek();

    virtual size_t GetSeekPos();

    virtual void SetStream(std::shared_ptr <IStream>& _rspkStream)
    {
        m_spkStream = _rspkStream;
    }
protected:

    virtual void SetStatus(int _eStatus)
    {
        m_eStatus = (CDefaultResourceComponent::EStatus)_eStatus;
    }

    friend class CDefaultResourceSystem;
    std::shared_ptr <IStream> m_spkStream;
    ISerializeStreamCallBack* m_pkCallBack;
    mutable EStatus m_eStatus;
};

class ECS_API CDefaultResourceFileComponent : public IResourceFileComponent
{
public:
    enum EStatus
    {
        eStatus_Error = -1,
        eStatus_Idle,
        eStatus_Loading,
        eStatus_Saving,
        eStatus_AsyncLoadingStart,
        eStatus_AsyncLoading,
        eStatus_AsyncLoadingEnd,
        eStatus_AsyncSavingStart,
        eStatus_AsyncSaving,
        eStatus_AsyncSavingEnd,
    };
protected:
    std::shared_ptr <IFile> m_pkFile;
    IFileCallBack* m_pkCallBack;
    mutable EStatus m_eStatus;

    virtual void SetStatus (int _eStatus)
    {
        m_eStatus = (CDefaultResourceFileComponent::EStatus)_eStatus;
    }
public:


    CDefaultResourceFileComponent();

    virtual ~CDefaultResourceFileComponent ();

    virtual IFileCallBack* GetFileCallBack ();

    virtual bool SetFileCallBack (IFileCallBack*);

    virtual void SetFile (std::shared_ptr <IFile>&);

    virtual IFile* GetFile ();

    virtual bool IsValid () const;

    virtual bool SetSave (bool _bAsync = false) const;

    virtual bool SetLoad (bool _bAsync = false) const;

    virtual int GetStatus () const
    {
        return m_eStatus;
    }
};