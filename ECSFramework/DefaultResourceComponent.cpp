#include "ECSFrameworkPCH.h"
#include "DefaultResourceComponent.h"


CDefaultResourceComponent::CDefaultResourceComponent()
    :m_pkCallBack(nullptr)
    ,m_eStatus(eStatus_Idle)
{
}

CDefaultResourceComponent::~CDefaultResourceComponent()
{

}

ISerializeStreamCallBack* CDefaultResourceComponent::GetSerializerCallBack()
{
    return m_pkCallBack;
}

bool CDefaultResourceComponent::SetSerializerCallBack(ISerializeStreamCallBack* _pkCallBack)
{
    if (!IsValid())
        return false;

    m_pkCallBack = _pkCallBack;
    return true;
}

bool CDefaultResourceComponent::IsValid() const
{
    return ((m_eStatus == eStatus_Idle) && (m_pkCallBack != nullptr));
}

bool CDefaultResourceComponent::SetSave(bool _bAsync) const
{
    if (!IsValid())
        return false;

    m_eStatus = (_bAsync) ? eStatus_AsyncSavingStart : eStatus_Saving;
    return true;
}

bool CDefaultResourceComponent::SetLoad(bool _bAsync) const
{
    if (!IsValid())
        return false;

    m_eStatus = (_bAsync) ? eStatus_AsyncLoadingStart : eStatus_Loading;
    return true;
}

bool CDefaultResourceComponent::ReadBinrary(char*& _rpcData, size_t _nSize)
{
    if (!m_spkStream)
        return false;
    return m_spkStream->Read(_rpcData, _nSize);
}

bool CDefaultResourceComponent::WriteBinrary(char*& _rpcData, size_t _nSize)
{
    if (!m_spkStream)
        return false;
    return m_spkStream->Write(_rpcData, _nSize);
}

bool CDefaultResourceComponent::ReadBinrary(char*& _rpcData, size_t _nOffset, size_t _nSize)
{
    if (!m_spkStream)
        return false;
    return m_spkStream->ReadSection(_rpcData, _nOffset, _nSize);
}

bool CDefaultResourceComponent::WriteBinrary(char*& _rpcData, size_t _nOffset, size_t _nSize)
{
    if (!m_spkStream)
        return false;
    return m_spkStream->WriteSection(_rpcData, _nOffset, _nSize);
}

void CDefaultResourceComponent::ResetSeek()
{
    if (!m_spkStream)
        return;
    m_spkStream->ResetSeek();
}

size_t CDefaultResourceComponent::GetSeekPos()
{
    if (!m_spkStream)
        return -1;
    return m_spkStream->GetSeek();
}

CDefaultResourceFileComponent::CDefaultResourceFileComponent ()
    : m_pkCallBack(NULL)
    , m_eStatus(eStatus_Idle)
{

}

CDefaultResourceFileComponent::~CDefaultResourceFileComponent ()
{

}

IFileCallBack* CDefaultResourceFileComponent::GetFileCallBack ()
{
    return m_pkCallBack;
}

bool CDefaultResourceFileComponent::SetFileCallBack (IFileCallBack* _pkCallback)
{
    if (!_pkCallback)
        return false;

    m_pkCallBack = _pkCallback;
    return true;
}

void CDefaultResourceFileComponent::SetFile (std::shared_ptr <IFile>& _spkFile)
{
    m_pkFile = _spkFile;
}

IFile* CDefaultResourceFileComponent::GetFile ()
{
    return m_pkFile.get ();
}

bool CDefaultResourceFileComponent::IsValid () const
{
    return ((m_eStatus == eStatus_Idle) && (m_pkCallBack != nullptr));
}

bool CDefaultResourceFileComponent::SetSave (bool _bAsync) const
{
    if (!IsValid ())
        return false;

    m_eStatus = _bAsync ? eStatus_AsyncSavingStart : eStatus_Saving;
    return true;
}

bool CDefaultResourceFileComponent::SetLoad (bool _bAsync) const
{
    if (!IsValid ())
        return false;

    m_eStatus = _bAsync ? eStatus_AsyncLoadingStart : eStatus_Loading;
    return true;
}