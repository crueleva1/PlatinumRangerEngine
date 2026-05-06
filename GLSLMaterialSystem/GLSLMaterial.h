#pragma once

#include "IMaterialComponent.h"
#include "GLSLShader.h"

class ECS_GLSL_API CGLSLMaterial : public IMaterial
{
protected:
    class CGLSLMaterialSystem& m_rkSystem;
    std::string m_kProgramLog;
    std::string m_kName;
    std::map <std::string, int> m_kUniformLocation;
    std::vector <ITexture*> m_kTextures;
    std::vector <std::shared_ptr <IConstantSemantic> > m_kConstants;
    std::vector <std::shared_ptr <IStructConstantSematic> > m_kStructs;
    std::vector <std::shared_ptr <IVariableAttrDesc> > m_kAttributes;
    std::shared_ptr <IShader> m_apkShaders [eShader_Max];
    bool m_bRelink;
public:
    CGLSLMaterial(class CGLSLMaterialSystem& _rkSystem);

    virtual ~CGLSLMaterial();

    virtual void SetMaterialName(const char* _pcName);

    virtual const char* GetMaterialName();

    virtual bool SetShader(std::shared_ptr<IShader> _spkShader);

    virtual bool ApplyGeometryTexture(class IGeometryComponent* _pkGeomtry);

    virtual bool ApplyCameraData(class ICameraComponent* _pkCamera);

    virtual bool SetTexture(ITexture* _pkTexture, size_t _nIndex);

    virtual void SetTextureCount(size_t _nSize);

    virtual IShader* GetShader(int _eShaderType);

    virtual ITexture* GetTexture(size_t _nIndex);

    virtual bool SetConstant(std::shared_ptr <IConstantSemantic>& _spkConstant, size_t _nIndex);

    virtual void SetConstantCount(size_t _nCount);

    virtual IConstantSemantic* GetConstant(size_t _nIndex);

    virtual size_t GetConstantCount();

    virtual bool SetConstantStruct(std::shared_ptr <IStructConstantSematic>& _spkStruct, size_t _nIndex);

    virtual void SetConstantStructCount(size_t _nCount);

    virtual IStructConstantSematic* GetConstantStruct(size_t _nIndex);

    virtual size_t GetConstantStructCount();

    virtual bool SetVariable(std::shared_ptr <IVariableAttrDesc>& _spkVariable, size_t _nIndex);

    virtual void SetVariableCount(size_t _nCount);

    virtual IVariableAttrDesc* GetVariableDesc(size_t _nIndex);

    virtual IVariableAttrDesc* GetVariableDescByName(const char* _pcName);

    virtual bool IsVariableActive(const char* _pcVarName);

    virtual const char* GetActiveVariableName(const char* _pcVarSemantic);

    virtual size_t GetVariableDescCount();

    virtual bool LinkShader(IBuffer* _pkAttr) = 0;

    const char* GetProgramLog()
    {
        return m_kProgramLog.c_str();
    }


    int GetUniformLocation(const char* _pcVariableName);
};