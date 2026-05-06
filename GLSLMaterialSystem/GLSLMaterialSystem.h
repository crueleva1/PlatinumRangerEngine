#pragma once

#include "MaterialSystem.h"
#include "GLSLShader.h"
#include "GLSLMaterial.h"
#include <map>

extern void GetParserBuildResource(struct TBuiltInResource& _rkResources);

class ECS_GLSL_API CGLSLMaterialContextArg : public IMaterialContextArg
{
    EMaterialContextType m_eType;
public:
    CGLSLMaterialContextArg(EMaterialContextType _eType)
        : m_eType(_eType)
    {}
    virtual ~CGLSLMaterialContextArg()
    {}

    int GetEventType () const override
    {
        return m_eType;
    }
};

class ECS_GLSL_API CGLSLMaterialGetInstance : public CGLSLMaterialContextArg
{
protected:
    friend class CGLSLMaterialSystem;
    IMaterial* m_pkInstance;
    std::string m_kShaderName [eShader_Max];
    static std::string m_kEmptySource;

    CGLSLMaterialGetInstance(EMaterialContextType _eType);
    void SetInstance(IMaterial* _pkMaterial) const;
public:
    CGLSLMaterialGetInstance();

    virtual ~CGLSLMaterialGetInstance();

    void SetShaderName(const char* _pcShaderName, int _eShaderType);

    const std::string& GetShaderName(int _eShaderType) const;

    IMaterial* GetMaterialInstance () const
    {
        return m_pkInstance;
    }
};

class ECS_GLSL_API CGLSLMaterialCreateInstanceFromFile : public CGLSLMaterialGetInstance
{
    std::string m_kFileName[eShader_Max];
    friend class CGLSLMaterialSystem;
    friend class CGLSLMaterialCreateComponentFromFile;
    CGLSLMaterialCreateInstanceFromFile(EMaterialContextType _eType);
public:
    CGLSLMaterialCreateInstanceFromFile();

    virtual ~CGLSLMaterialCreateInstanceFromFile();

    void SetShaderFile(const char* _pcShaderSource, int _eShaderType);

    const std::string& GetShaderFile(int _eShaderType) const;
};

class ECS_GLSL_API CGLSLMaterialCreateComponentFromFile : public CGLSLMaterialCreateInstanceFromFile
{
    std::string m_kCompType;
    entityx::Entity& m_rkEntity;
public:
    CGLSLMaterialCreateComponentFromFile(entityx::Entity& _rkEntity, const char* _pcCompType = "Default");

    virtual ~CGLSLMaterialCreateComponentFromFile();

    entityx::Entity& GetEntity() const;

    const std::string& GetCompType() const
    {
        return m_kCompType;
    }
};

class ECS_GLSL_API CGLSLMaterialCreateInstanceFromSrc : public CGLSLMaterialGetInstance
{
    std::string m_kShaderSource [eShader_Max];
    friend class CGLSLMaterialSystem;
    friend class CGLSLMaterialCreateComponentFromSrc;
    CGLSLMaterialCreateInstanceFromSrc (EMaterialContextType _eType);
public:
    CGLSLMaterialCreateInstanceFromSrc ();

    virtual ~CGLSLMaterialCreateInstanceFromSrc();

    void SetShaderSource(const char* _pcShaderSource, int _eShaderType);

    const std::string& GetShaderSource(int _eShaderType) const;
};

class ECS_GLSL_API CGLSLMaterialCreateComponentFromSrc : public CGLSLMaterialCreateInstanceFromSrc
{
    std::string m_kCompType;
    entityx::Entity& m_rkEntity;
public:
    CGLSLMaterialCreateComponentFromSrc(entityx::Entity& _rkEntity, const char* _pcCompType = "Default");

    virtual ~CGLSLMaterialCreateComponentFromSrc();

    entityx::Entity& GetEntity() const;

    const std::string& GetCompType() const
    {
        return m_kCompType;
    }
};

struct ECS_GLSL_API IGLSLShaderFactory : public IFactoryObj <CGLSLShader, EShaderType>
{
    IGLSLShaderFactory(EShaderType _eType)
        : IFactoryObj<CGLSLShader, EShaderType>(_eType)
    {
    }

    virtual std::shared_ptr <CGLSLShader> operator()() = 0;
};

struct ECS_GLSL_API IGLSLMaterialFactory : public IFactoryObj <CGLSLMaterial, std::string>
{
    IGLSLMaterialFactory(std::string _kType)
        : IFactoryObj <CGLSLMaterial, std::string>(_kType)
    {
    }

    virtual std::shared_ptr <CGLSLMaterial> operator()(class CGLSLMaterialSystem& _rkSystem) = 0;
};

struct ECS_GLSL_API IMaterialCompFactory : public IFactoryObj <IMaterialComponent, std::string>
{
    IMaterialCompFactory(std::string _kType)
        : IFactoryObj <IMaterialComponent, std::string>(_kType)
    {
    }

    virtual std::shared_ptr <IMaterialComponent> operator()(IMaterial* _plMaterialIns) = 0;
};

class ECS_GLSL_API CGLSLMaterialSystem : public IMatertialSystem
{
protected:
    IFactoryMgr <CGLSLShader, EShaderType> m_kShaderFactory;
    IFactoryMgr <CGLSLMaterial, std::string> m_kMaterialFactory;
    IFactoryMgr <IMaterialComponent, std::string> m_kMaterialCompFactory;

    std::map <std::string, std::shared_ptr <IShader> > m_kShaderPool;
    std::map <size_t, std::shared_ptr <IMaterial> > m_kMaterialPool;
    std::map <std::string, std::shared_ptr <ITexture> > m_kTexturePool;
    static CGLSLMaterialSystem* ms_pkThis;

    virtual std::shared_ptr <IConstantSemantic> CreateConstantSematic(glslang::TProgram* _pkProgream, int _nConstantIndex);
    virtual std::shared_ptr <IStructConstantSematic> CreateConstantStructSematic(glslang::TProgram* pkProgream, int _nConstantIndex);
public:
    CGLSLMaterialSystem();

    virtual ~CGLSLMaterialSystem();

    virtual void configure(entityx::EntityManager& EntityMgr, entityx::EventManager& _rkEvents);

    virtual void ValidateMaterials(entityx::EntityManager& _rkEntityMgr, entityx::EventManager& _rkEventMgr);

    virtual int GetDescHash(IGeometryHandler& _rkGeometry, ILightHandler& _rkLight);

    virtual IMaterial* CreateMaterialInstance(size_t _nHash);

    virtual IMaterial* GetMaterialInstance (size_t _nHash);

    virtual std::shared_ptr <ITexture> CreateRenderData(ITextureDesc* _pkDesc, bool _bEmpty = false);

    virtual std::shared_ptr <ITexture> GetRenderData (ITextureDesc* _pkDesc);

    virtual void RemoveRenderData(ITextureDesc* _pkDesc);

    virtual void receive (const IMaterialContextArg& _rkEventArg);

    virtual bool CreateMaterialInstance(const CGLSLMaterialCreateInstanceFromSrc& _rkArg);

    virtual bool CreateMaterialComponent(const CGLSLMaterialCreateComponentFromSrc& _rkArg);

    virtual bool CreateMaterialInstance(const CGLSLMaterialCreateInstanceFromFile& _rkArg);

    virtual bool CreateMaterialComponent(const CGLSLMaterialCreateComponentFromFile& _rkArg);

    virtual bool GetMaterialInstance(const CGLSLMaterialGetInstance& _rkArg);

    virtual void UpdateConstantReflection(IMaterial* _pkMaterial);

    virtual std::shared_ptr <IShader> GetShaderInstance(const std::string& _rkName);

    virtual std::shared_ptr <IShader> CreateShaderInstance(const std::string& _rkName, const std::string& _rkSource, EShaderType _eType);

    template <typename T>
    static bool RegisterComponentFactory(const char* _pcName)
    {
        return ms_pkThis->m_kMaterialCompFactory.RegisterFactory(std::make_shared<T>(_pcName));
    }

    template <typename T>
    static bool RegisterMaterialFactory(const char* _pcName)
    {
        return ms_pkThis->m_kMaterialFactory.RegisterFactory(std::make_shared<T>(_pcName));
    }

    template <typename T>
    static bool RegisterShaderFactory(EShaderType _eType)
    {
        return ms_pkThis->m_kShaderFactory.RegisterFactory(std::make_shared<T>(_eType));
    }
};