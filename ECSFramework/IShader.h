#pragma once

#include "DataModifier.h"

enum EShaderType
{
    eShader_Vertex,
    eShader_Pixel,
    eShader_Geomtery,
    eShader_Max,
};

struct ECS_API IVariableAttrDesc
{
    virtual ~IVariableAttrDesc()
    {
    }

    virtual const char* GetVariableName() = 0;

    virtual int GetVariableType() = 0;

    virtual int GetLoc() = 0;

    virtual size_t GetVariableSize() = 0;

    virtual size_t GetVariableCount() = 0;
};

enum EVariableSemanticElementType
{
    eVSET_None,
    eVSET_bool,
    eVSET_short,
    eVSET_ushort,
    eVSET_float,
    eVSET_int,
};

class ECS_API IVariableSemantic
{
    EVariableSemanticElementType m_eType;
public:
    IVariableSemantic(EVariableSemanticElementType _eType)
        : m_eType(_eType)
    {
    }

    virtual ~IVariableSemantic()
    {
    }

    virtual const size_t& GetDescSize() const = 0;

    virtual const char* GetName() const = 0;

    virtual const size_t& GetSemanticIndexType() const = 0;

    virtual int GetElementCount() const = 0;

    inline EVariableSemanticElementType GetElementType() const
    {
        return m_eType;
    }
};

template <class T>
class TVariableSemantic : public IVariableSemantic
{
    const std::string m_kSemanticName;
    const size_t m_nSemanticSize;
    const size_t m_nSemanticIndex;
    const int m_nElementCount;
public:

    TVariableSemantic(const char* _pcName, size_t _nIndex, int _nElementCount, EVariableSemanticElementType _eType)
        : IVariableSemantic(_eType)
        , m_kSemanticName(_pcName)
        , m_nSemanticIndex(_nIndex)
        , m_nSemanticSize(sizeof(T))
        , m_nElementCount(_nElementCount)
    {
    }

    virtual ~TVariableSemantic()
    {
    }

    const size_t& GetDescSize() const
    {
        return m_nSemanticSize;
    }

    const size_t& GetSemanticIndexType() const
    {
        return m_nSemanticIndex;
    }

    const char* GetName() const
    {
        return m_kSemanticName.c_str();
    }

    int GetElementCount() const
    {
        return m_nElementCount;
    }
};

enum EConstantType
{
    eConstant_None,
    eConstant_bool,
    eConstant_short,
    eConstant_ushort,
    eConstant_int,
    eConstant_uint,
    eConstant_float,
    eConstant_floatVec2,
    eConstant_floatVec3,
    eConstant_floatVec4,
    eConstant_intVec2,
    eConstant_intVec3,
    eConstant_intVec4,
    eConstant_Matrix2x2,
    eConstant_Matrix3x3,
    eConstant_Matrix4x4,
    eConstant_Texture,
    eConstant_Struct,
    eConstant_Max,
};

class ECS_API IConstantSemantic
{
    const EConstantType m_eType;
    size_t m_nMemberIndex;
    size_t m_nMemberOffset;
public:
    IConstantSemantic(EConstantType _eType)
        : m_eType(_eType)
        , m_nMemberIndex(-1)
        , m_nMemberOffset(0)
    {
    }

    virtual ~IConstantSemantic()
    {
    }

    virtual const size_t& GetDescSize() = 0;

    virtual const char* GetVaribleName() = 0;

    virtual void* GetRawData() = 0;

    virtual bool isDirty() = 0;

    size_t MemberIndex()
    {
        return m_nMemberIndex;
    }

    void SetMemberIndex(size_t _nIndex)
    {
        m_nMemberIndex = _nIndex;
    }

    size_t MemberOffset()
    {
        return m_nMemberOffset;
    }

    void SetMemberOffset(size_t _nOffset)
    {
        m_nMemberOffset = _nOffset;
    }

    inline EConstantType GetType()
    {
        return m_eType;
    }
};

template <typename T>
class TConstantSemantic : public IConstantSemantic
{
    std::string m_kDesc;
    std::string m_kVariableName;
    TDataModifier <T> m_kData;
    const size_t m_nDescSize;
public:
    TConstantSemantic(const char* _pcDesc, const char* _pcVariableName, EConstantType _eType)
        : IConstantSemantic(_eType)
        , m_kDesc(_pcDesc)
        , m_kVariableName(_pcVariableName)
        , m_nDescSize(sizeof(T))
    {
    }

    TConstantSemantic(const char* _pcDesc, const char* _pcVariableName, T& _kValue, EConstantType _eType)
        : IConstantSemantic(_eType)
        , m_kDesc(_pcDesc)
        , m_kVariableName(_pcVariableName)
        , m_nDescSize(sizeof(T))
        , m_kData(_kValue)
    {
    }

    virtual ~TConstantSemantic()
    {
    }

    const size_t& GetDescSize()
    {
        return m_nDescSize;
    }

    const char* GetVariableDesc()
    {
        return m_kDesc.c_str();
    }

    const char* GetVaribleName()
    {
        return m_kVariableName.c_str();
    }

    typename const T& GetValue()
    {
        return m_kData;
    }

    void SetValue(const T& _kData)
    {
        m_kData = _kData;
    }

    void* GetRawData()
    {
        m_kData.clearDirty();
        T& rData = m_kData.get();
        return &rData;
    }

    bool isDirty()
    {
        return m_kData.isDirty();
    }
};

class ECS_API IStructConstantSematic : public IConstantSemantic
{
public:
    IStructConstantSematic()
        : IConstantSemantic(eConstant_Struct)
    {
    }

    virtual ~IStructConstantSematic()
    {
    }

    virtual bool addMember(std::shared_ptr <IConstantSemantic> _spkMemeber) = 0;

    virtual size_t getMemberDescSize() = 0;
};

class ECS_API CGenericStructSematic : public IStructConstantSematic
{
    std::vector <std::shared_ptr<IConstantSemantic> > m_kMembers;
    std::string m_kDesc;
    std::string m_kVariableName;
    size_t m_nActiveMember;
    const size_t m_nDescSize;
public:
    CGenericStructSematic(const char* _pcDesc, const char* _pcVariableName, size_t _nStructSize)
        :IStructConstantSematic()
        ,m_kDesc(_pcDesc)
        ,m_kVariableName(_pcVariableName)
        ,m_nDescSize(_nStructSize)
        ,m_nActiveMember(-1)
    {
    }

    virtual ~CGenericStructSematic()
    {
    }

    inline bool isBlock()
    {
        return GetType() == eConstant_Struct;
    }

    virtual bool addMember(std::shared_ptr <IConstantSemantic> _spkMemeber)
    {
        if (isBlock() == false)
            return false;

        size_t nTargetSize = _spkMemeber->GetDescSize() + getMemberDescSize();
        if (nTargetSize > m_nDescSize)
            return false;

        m_kMembers.push_back(_spkMemeber);
        return true;
    }

    virtual size_t getMemberDescSize()
    {
        size_t nSize = 0;
        for (auto& spkMemeber : m_kMembers) {
            nSize += spkMemeber->GetDescSize();
        }
        return nSize;
    }

    bool setActiveMember(size_t _nIndex)
    {
        if (_nIndex >= m_kMembers.size())
            return false;
        m_nActiveMember = _nIndex;
        return true;
    }

    const size_t& GetDescSize()
    {
        return m_nDescSize;
    }

    const char* GetVariableDesc()
    {
        return m_kDesc.c_str();
    }

    const char* GetVaribleName()
    {
        return m_kVariableName.c_str();
    }

    void* GetRawData()
    {
        if (m_nActiveMember >= m_kMembers.size())
            return nullptr;

        return m_kMembers[m_nActiveMember]->GetRawData();
    }

    bool isDirty()
    {
        for (auto& spkMemeber : m_kMembers) {
            if (spkMemeber->isDirty())
                return true;
        }
        return false;
    }
};

struct ECS_API IShader
{
    virtual ~IShader()
    {

    }

    virtual bool SetSource(const char* _pcCode) = 0;

    virtual bool LoadSource(const char* _pcSourceFileName) = 0;

    virtual bool Compile() = 0;

    virtual int GetShaderType() = 0;

    virtual bool IsError() = 0;

    virtual const char* GetCompileInfo() = 0;

    virtual bool IsCompiled() = 0;

    virtual const char* GetSource() = 0;
};