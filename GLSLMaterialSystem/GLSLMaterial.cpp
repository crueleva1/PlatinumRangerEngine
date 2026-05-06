#include "GLSLSystemPCH.h"
#include "GLSLMaterial.h"
#include "IGeometryComponent.h"
#include "GLMBaseCameraComponent.h"
#include "ITexture.h"
#include "GLSLMaterialSystem.h"

CGLSLMaterial::CGLSLMaterial(CGLSLMaterialSystem& _rkSystem)
    : m_rkSystem(_rkSystem)
    , m_bRelink(true)
{
}

CGLSLMaterial::~CGLSLMaterial()
{
}

void CGLSLMaterial::SetMaterialName(const char* _pcName)
{
    m_kName = _pcName;
}

const char* CGLSLMaterial::GetMaterialName()
{
    return m_kName.c_str();
}

bool CGLSLMaterial::SetShader(std::shared_ptr<IShader> _spkShader)
{
    if (!_spkShader)
        return false;
    EShaderType eType = (EShaderType)_spkShader->GetShaderType();
    if (eType < eShader_Vertex || eType >= eShader_Max)
        return false;

    m_apkShaders [eType] = _spkShader;
    m_bRelink = true;
    return true;
}

bool CGLSLMaterial::ApplyGeometryTexture(IGeometryComponent* _pkGeomtry)
{
    // Update Constant
    SetTextureCount(_pkGeomtry->GetTextureDescCount());
    size_t nTextureCount = 0;
    const size_t nConstantCount = GetConstantCount();
    if (!_pkGeomtry->GetTextureDescCount()) {
        for (size_t nIndex = 0; nIndex < nConstantCount; nIndex++) {
            IConstantSemantic* pkSemantic = GetConstant(nIndex);
            if (!pkSemantic)
                continue;
            if (pkSemantic->GetType() == eConstant_bool) {
                if (strcmp(pkSemantic->GetVaribleName(), "uHasTexture"))
                    continue;
                TConstantSemantic <bool>* pkBoolSemantic = dynamic_cast <TConstantSemantic <bool>*>(pkSemantic);
                if (!pkBoolSemantic)
                    continue;
                pkBoolSemantic->SetValue(false);
                continue;
            }
            if (pkSemantic->GetType() != eConstant_Texture)
                continue;
            TConstantSemantic <ITexture*>* pkTextureSemantic = dynamic_cast <TConstantSemantic <ITexture*>*>(pkSemantic);
            if (!pkTextureSemantic)
                continue;
            pkTextureSemantic->SetValue(nullptr);
        }
        return true;
    }

    for (size_t nIndex = 0; nIndex < nConstantCount; nIndex++) {
        IConstantSemantic* pkSemantic = GetConstant(nIndex);
        if (!pkSemantic)
            continue;
        if (pkSemantic->GetType() == eConstant_bool) {
            if (strcmp(pkSemantic->GetVaribleName(), "uHasTexture"))
                continue;
            TConstantSemantic <bool>* pkBoolSemantic = dynamic_cast <TConstantSemantic <bool>*>(pkSemantic);
            if (!pkBoolSemantic)
                continue;
            pkBoolSemantic->SetValue(true);
            continue;
        }
        if (pkSemantic->GetType() != eConstant_Texture)
            continue;
        TConstantSemantic <ITexture*>* pkTextureSemantic = dynamic_cast <TConstantSemantic <ITexture*>*>(pkSemantic);
        if (!pkTextureSemantic)
            continue;
        if (!_pkGeomtry->ValidateRenderDataByName(m_rkSystem, pkTextureSemantic->GetVaribleName())) {
            pkTextureSemantic->SetValue(nullptr);
            continue;
        }
        ITextureDesc* pkDesc = _pkGeomtry->GetTextureDescByName(pkTextureSemantic->GetVaribleName());
        if (!pkDesc) {
            pkTextureSemantic->SetValue(nullptr);
            continue;
        }
        ITexture* pkRenderData = pkDesc->GetRenderData();
#if 0
        if (pkRenderData == nullptr) {
            std::shared_ptr <ITexture> spkRenderData = m_rkSystem.GetRenderData(pkDesc);
            if (spkRenderData == nullptr) {
                spkRenderData = m_rkSystem.CreateRenderData(pkDesc);
                if (spkRenderData == nullptr) {
                    ILogger::Log(eLL_Error, "Load Texture Failed! [File]:%s", pkDesc->GetFileName());
                    assert(0);
                    pkTextureSemantic->SetValue(nullptr);
                    return false;
                }
            }
            pkDesc->SetRenderData(spkRenderData);
            pkRenderData = pkDesc->GetRenderData();
        }
#endif
        SetTexture(pkRenderData, nTextureCount++);
        pkTextureSemantic->SetValue(pkRenderData);
    }
    return true;
}

bool CGLSLMaterial::ApplyCameraData(ICameraComponent* _pkCamera)
{
    CGLMBaseCameraComponent* pkCameraComp = dynamic_cast <CGLMBaseCameraComponent*>(_pkCamera);
    if (!pkCameraComp)
        return false;

    const size_t nSize = GetConstantCount();
    for (size_t nIndex = 0; nIndex < nSize; nIndex++) {
        IConstantSemantic* pkSemantic = GetConstant(nIndex);
        if (!pkSemantic)
            continue;

        if (pkSemantic->GetType() != eConstant_Matrix4x4)
            continue;

        if (strstr(pkSemantic->GetVaribleName(), "ViewMatrix")) {
            TConstantSemantic <glm::mat4>* pkView = static_cast <TConstantSemantic <glm::mat4>*>(pkSemantic);
            if (pkView) {
                pkView->SetValue(pkCameraComp->GetViewMatrix());
            }
        }
        else if (strstr(pkSemantic->GetVaribleName(), "ProjectionMatrix")) {
            TConstantSemantic <glm::mat4>* pkProj = static_cast <TConstantSemantic <glm::mat4>*>(pkSemantic);
            if (pkProj) {
                pkProj->SetValue(pkCameraComp->GetProjectMatrix());
            }
        }
        else if (strstr(pkSemantic->GetVaribleName(), "ViewProjMatrix")) {
            glm::mat4 kViewPorj = pkCameraComp->GetProjectMatrix() * pkCameraComp->GetViewMatrix();
            TConstantSemantic <glm::mat4>* pkViewProj = static_cast <TConstantSemantic <glm::mat4>*>(pkSemantic);
            if (pkViewProj) {
                pkViewProj->SetValue(kViewPorj);
            }
        }
    }
    return true;
}

bool CGLSLMaterial::SetTexture(ITexture* _pkTexture, size_t _nIndex)
{
    if (!_pkTexture)
        return false;
    if (m_kTextures.size() <= _nIndex)
        return false;

    m_kTextures [_nIndex] = _pkTexture;
    return true;
}

void CGLSLMaterial::SetTextureCount(size_t _nSize)
{
    m_kTextures.resize(_nSize);
}

IShader* CGLSLMaterial::GetShader(int _eShaderType)
{
    if (_eShaderType < eShader_Vertex || _eShaderType >= eShader_Max)
        return nullptr;

    return m_apkShaders [_eShaderType].get();
}

ITexture* CGLSLMaterial::GetTexture(size_t _nIndex)
{
    if (m_kTextures.size() <= _nIndex)
        return nullptr;

    return m_kTextures [_nIndex];
}

bool CGLSLMaterial::SetConstant(std::shared_ptr <IConstantSemantic>& _spkConstant, size_t _nIndex)
{
    if (m_kConstants.size() <= _nIndex)
        return false;

    m_kConstants [_nIndex] = _spkConstant;

    // Is owner struct member
    const size_t nMemberIndex = _spkConstant->MemberIndex();
    if (nMemberIndex < m_kStructs.size()) {
        return m_kStructs[nMemberIndex]->addMember(_spkConstant);
    }

    return true;
}

void CGLSLMaterial::SetConstantCount(size_t _nCount)
{
    m_kConstants.resize(_nCount);
}

IConstantSemantic* CGLSLMaterial::GetConstant(size_t _nIndex)
{
    return m_kConstants [_nIndex].get();
}

size_t CGLSLMaterial::GetConstantCount()
{
    return m_kConstants.size();
}

bool CGLSLMaterial::SetConstantStruct(std::shared_ptr <IStructConstantSematic>& _spkStruct, size_t _nIndex)
{
    if (m_kStructs.size() <= _nIndex)
        return false;
    
    m_kStructs[_nIndex] = _spkStruct;
    return true;
}

void CGLSLMaterial::SetConstantStructCount(size_t _nCount)
{
    m_kStructs.resize(_nCount);
}

IStructConstantSematic* CGLSLMaterial::GetConstantStruct(size_t _nIndex)
{
    return m_kStructs[_nIndex].get();
}

size_t CGLSLMaterial::GetConstantStructCount()
{
    return m_kStructs.size();
}

bool CGLSLMaterial::SetVariable(std::shared_ptr <IVariableAttrDesc>& _spkVariable, size_t _nIndex)
{
    if (m_kAttributes.size() <= _nIndex)
        return false;

    m_kAttributes [_nIndex] = _spkVariable;
    return true;
}

void CGLSLMaterial::SetVariableCount(size_t _nCount)
{
    m_kAttributes.resize(_nCount);
}

IVariableAttrDesc* CGLSLMaterial::GetVariableDesc(size_t _nIndex)
{
    if (m_kAttributes.size() <= _nIndex)
        return nullptr;

    return m_kAttributes [_nIndex].get();
}

IVariableAttrDesc* CGLSLMaterial::GetVariableDescByName(const char* _pcName)
{
    for (auto pkVariableAttrDesc : m_kAttributes) {
        if (!pkVariableAttrDesc)
            continue;
        if (strstr(pkVariableAttrDesc->GetVariableName(), _pcName))
            return pkVariableAttrDesc.get();
    }
    return nullptr;
}

bool CGLSLMaterial::IsVariableActive(const char* _pcVarName)
{
    for (auto pkVariableAttrDesc : m_kAttributes) {
        if (!pkVariableAttrDesc)
            continue;
        if (strstr(pkVariableAttrDesc->GetVariableName(), _pcVarName))
            return true;
    }
    return false;
}

const char* CGLSLMaterial::GetActiveVariableName(const char* _pcVarSemantic)
{
    for (auto pkVariableAttrDesc : m_kAttributes) {
        if (!pkVariableAttrDesc)
            continue;
        if (strstr(pkVariableAttrDesc->GetVariableName(), _pcVarSemantic))
            return pkVariableAttrDesc->GetVariableName();
    }
    return nullptr;
}

size_t CGLSLMaterial::GetVariableDescCount()
{
    return m_kAttributes.size();
}

int CGLSLMaterial::GetUniformLocation(const char* _pcVariableName)
{
    if (!_pcVariableName)
        return -1;
    auto kUniformLocIt = m_kUniformLocation.find(_pcVariableName);
    if (kUniformLocIt != m_kUniformLocation.end())
        return kUniformLocIt->second;
    return -1;
}