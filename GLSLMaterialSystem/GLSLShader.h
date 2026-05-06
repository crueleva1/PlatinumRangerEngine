#pragma once


#include "IShader.h"

class ECS_GLSL_API CGLSLVariableAttrDesc : public IVariableAttrDesc
{
    const std::string m_kVariableName;
    const int m_nVariableType;
    const int m_nLoc;
public:
    CGLSLVariableAttrDesc(const char* _pcVariable, int _nVariableType, int _nLoc);

    virtual ~CGLSLVariableAttrDesc();

    virtual const char* GetVariableName();

    virtual int GetVariableType();

    virtual size_t GetVariableSize();

    virtual size_t GetVariableCount();

    int GetLoc()
    {
        return m_nLoc;
    }
};

class IOpenGLUniformLocInterface
{
    int m_nUniformLoc;
public:
    IOpenGLUniformLocInterface()
    {
    }

    virtual ~IOpenGLUniformLocInterface()
    {
    }

    void SetLocation(int _nLoc)
    {
        m_nUniformLoc = _nLoc;
    }

    int GetLocation()
    {
        return m_nUniformLoc;
    }
};

template <typename T, typename I = IOpenGLUniformLocInterface>
class TGLSLConstantSemantic
    : public TConstantSemantic <T>
    , public I
{
public:
    TGLSLConstantSemantic(const char* _pcDesc, const char* _pcVariableName, EConstantType _eType)
        : TConstantSemantic(_pcDesc, _pcVariableName, _eType)
    {
    }

    TGLSLConstantSemantic(const char* _pcDesc, const char* _pcVariableName, T& _kValue, EConstantType _eType)
        : TConstantSemantic(_pcDesc, _pcVariableName, _kValue, _eType)
    {
    }

    virtual ~TGLSLConstantSemantic()
    {
    }
};

class ECS_GLSL_API CBackendTShader : public glslang::TShader
{
public:
    CBackendTShader(EShLanguage _eLang)
        :glslang::TShader(_eLang)
    { }
    
    virtual ~CBackendTShader()
    { }

    inline const glslang::TIntermediate& GetTIntermediate()
    {
        return *intermediate;
    }
};

class ECS_GLSL_API CGLSLShader : public IShader
{
protected:
    CBackendTShader m_kShaderParser;
    std::string m_kShaderCode;
    std::string m_kCompileLog;
    EShaderType m_eType;
    unsigned int m_eCompilerFlag;
    bool m_bCompiled;
    bool m_bError;
public:
    CGLSLShader(EShaderType, unsigned int _eComplieFlag);

    virtual ~CGLSLShader();

    virtual bool SetSource(const char* _pcCode);

    virtual bool LoadSource(const char* _pcSourceFileName);

    virtual bool Compile();

    virtual int GetShaderType();

    virtual bool IsError();

    virtual const char* GetCompileInfo();

    virtual bool IsCompiled();

    virtual const char* GetSource();

    glslang::TShader& GetShaderParser();
};

