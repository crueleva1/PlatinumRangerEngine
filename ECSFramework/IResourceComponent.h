#pragma once


class ECS_API IStream
{
public:
    virtual ~IStream()
    {
    }

    virtual void ResetSeek() = 0;

    virtual size_t GetSeek() = 0;

    virtual void SetData(const char* _pcData, size_t _nSize) = 0;

    virtual bool Read(char*& _rpcOut, size_t _nSize) = 0;

    virtual bool ReadSection(char*& _rpcOut, size_t _nOffset, size_t _nSize) = 0;

    virtual bool Write(char*& _rpcIn, size_t _nSize) = 0;

    virtual bool WriteSection(char*& _rpcIn, size_t _nOffset, size_t _nSize) = 0;

    virtual void SetSize(size_t _nSize) = 0;

    virtual size_t GetDataSize() = 0;

    virtual char* GetData() = 0;
};

class ECS_API IFile
{
public:
    virtual ~IFile()
    {
    }

    virtual bool Load() = 0;

    virtual bool Save() = 0;

    virtual const char* GetFileName() = 0;
};

enum EResourceIOType
{
    eRIO_None,
    eRIO_Stream,
    eRIO_File,
    eRIO_Max,
};

class ECS_API IResourceComponent
{
protected:
    friend class IResourceSystem;
    virtual void SetStatus(int _eStatus) = 0;
public:
    virtual ~IResourceComponent()
    {
    }

    virtual bool IsValid() const = 0;

    virtual bool SetSave(bool _bAsync = false) const = 0;

    virtual bool SetLoad(bool _bAsync = false) const = 0;

    virtual int GetStatus() const = 0;

    virtual EResourceIOType GetType() = 0;
};

class ECS_API IBaseResourceCallback
{
public:
    virtual ~IBaseResourceCallback()
    {
    }

    virtual void SetResourcePath(const char* _pcPath) = 0;

    virtual void SetResourceName(const char* _pcName) = 0;

    virtual const char* GetResourcePath() = 0;

    virtual const char* GetResourceName() = 0;

    virtual const char* GetResourcePathName() = 0;

    virtual bool IsHandled() = 0;
};

typedef TComponent <IResourceComponent> IResourceHandler;

class ISerializeStreamCallBack;
class ECS_API IResourceStreamComponent : public IResourceComponent
{
public:

    virtual ~IResourceStreamComponent()
    {
    }

    virtual ISerializeStreamCallBack* GetSerializerCallBack() = 0;

    virtual bool SetSerializerCallBack(ISerializeStreamCallBack*) = 0;

    template <typename T>
    bool operator >> (T& _rkOut)
    {
        if (!IsValid())
            return false;
        size_t nSize = sizeof(T);
        return ReadBinrary((char*&)&_rkOut, nSize);
    }

    template <typename T>
    bool operator << (T& _rkIn)
    {
        if (!IsValid())
            return false;
        size_t nSize = sizeof(T);
        return WriteBinrary((char*&)&_rkIn, nSize);
    }

    virtual bool ReadBinrary(char*& _rpcData, size_t _nSize) = 0;

    virtual bool WriteBinrary(char*& _rpcData, size_t _nSize) = 0;

    virtual bool ReadBinrary(char*& _rpcData, size_t _nOffset, size_t _nSize) = 0;

    virtual bool WriteBinrary(char*& _rpcData, size_t _nOffset, size_t _nSize) = 0;

    virtual void ResetSeek() = 0;

    virtual size_t GetSeekPos() = 0;

    virtual void SetStream(std::shared_ptr <IStream>&) = 0;

    EResourceIOType GetType()
    {
        return eRIO_Stream;
    }
};

typedef TComponent<IResourceStreamComponent> IResourceStreamHandler;

class ECS_API ISerializeStreamCallBack : public IBaseResourceCallback
{
public:
    virtual ~ISerializeStreamCallBack()
    {
    }

    virtual std::shared_ptr <IStream> CreateStream(const char* _pcPathName) = 0;

    virtual bool Serialize(IResourceStreamComponent&) = 0;

    virtual bool Deserialize(const IResourceStreamComponent&) = 0;
};

class IFileCallBack;
class ECS_API IResourceFileComponent : public IResourceComponent
{
public:

    virtual ~IResourceFileComponent()
    {
    }

    virtual IFileCallBack* GetFileCallBack() = 0;

    virtual bool SetFileCallBack(IFileCallBack*) = 0;

    virtual void SetFile(std::shared_ptr <IFile>&) = 0;

    virtual IFile* GetFile() = 0;

    EResourceIOType GetType()
    {
        return eRIO_File;
    }
};

typedef TComponent<IResourceFileComponent> IResourceFileHandler;

class ECS_API IFileCallBack : public IBaseResourceCallback
{
public:
    virtual ~IFileCallBack()
    {
    }

    virtual std::shared_ptr <IFile> OnCreateFile(const char* _pcPathName) = 0;

    virtual bool OnLoadFile(IResourceFileComponent&) = 0;

    virtual bool OnSaveFile(IResourceFileComponent&) = 0;
};