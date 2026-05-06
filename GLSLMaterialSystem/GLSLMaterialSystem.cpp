#include "GLSLSystemPCH.h"
#include "GLSLMaterialSystem.h"
#include "IMaterialComponent.h"
#include "GLSLMaterial.h"
#include "RenderSystem.h"
#include "Util.h"
#include <memory>
#include <functional>
#include "ITexture.h"

void GetParserBuildResource(TBuiltInResource& _rkResources)
{
    _rkResources.maxLights = 32;
    _rkResources.maxClipPlanes = 6;
    _rkResources.maxTextureUnits = 32;
    _rkResources.maxTextureCoords = 32;
    _rkResources.maxVertexAttribs = 64;
    _rkResources.maxVertexUniformComponents = 4096;
    _rkResources.maxVaryingFloats = 64;
    _rkResources.maxVertexTextureImageUnits = 32;
    _rkResources.maxCombinedTextureImageUnits = 80;
    _rkResources.maxTextureImageUnits = 32;
    _rkResources.maxFragmentUniformComponents = 4096;
    _rkResources.maxDrawBuffers = 32;
    _rkResources.maxVertexUniformVectors = 128;
    _rkResources.maxVaryingVectors = 8;
    _rkResources.maxFragmentUniformVectors = 16;
    _rkResources.maxVertexOutputVectors = 16;
    _rkResources.maxFragmentInputVectors = 15;
    _rkResources.minProgramTexelOffset = -8;
    _rkResources.maxProgramTexelOffset = 7;
    _rkResources.maxClipDistances = 8;
    _rkResources.maxComputeWorkGroupCountX = 65535;
    _rkResources.maxComputeWorkGroupCountY = 65535;
    _rkResources.maxComputeWorkGroupCountZ = 65535;
    _rkResources.maxComputeWorkGroupSizeX = 1024;
    _rkResources.maxComputeWorkGroupSizeY = 1024;
    _rkResources.maxComputeWorkGroupSizeZ = 64;
    _rkResources.maxComputeUniformComponents = 1024;
    _rkResources.maxComputeTextureImageUnits = 16;
    _rkResources.maxComputeImageUniforms = 8;
    _rkResources.maxComputeAtomicCounters = 8;
    _rkResources.maxComputeAtomicCounterBuffers = 1;
    _rkResources.maxVaryingComponents = 60;
    _rkResources.maxVertexOutputComponents = 64;
    _rkResources.maxGeometryInputComponents = 64;
    _rkResources.maxGeometryOutputComponents = 128;
    _rkResources.maxFragmentInputComponents = 128;
    _rkResources.maxImageUnits = 8;
    _rkResources.maxCombinedImageUnitsAndFragmentOutputs = 8;
    _rkResources.maxCombinedShaderOutputResources = 8;
    _rkResources.maxImageSamples = 0;
    _rkResources.maxVertexImageUniforms = 0;
    _rkResources.maxTessControlImageUniforms = 0;
    _rkResources.maxTessEvaluationImageUniforms = 0;
    _rkResources.maxGeometryImageUniforms = 0;
    _rkResources.maxFragmentImageUniforms = 8;
    _rkResources.maxCombinedImageUniforms = 8;
    _rkResources.maxGeometryTextureImageUnits = 16;
    _rkResources.maxGeometryOutputVertices = 256;
    _rkResources.maxGeometryTotalOutputComponents = 1024;
    _rkResources.maxGeometryUniformComponents = 1024;
    _rkResources.maxGeometryVaryingComponents = 64;
    _rkResources.maxTessControlInputComponents = 128;
    _rkResources.maxTessControlOutputComponents = 128;
    _rkResources.maxTessControlTextureImageUnits = 16;
    _rkResources.maxTessControlUniformComponents = 1024;
    _rkResources.maxTessControlTotalOutputComponents = 4096;
    _rkResources.maxTessEvaluationInputComponents = 128;
    _rkResources.maxTessEvaluationOutputComponents = 128;
    _rkResources.maxTessEvaluationTextureImageUnits = 16;
    _rkResources.maxTessEvaluationUniformComponents = 1024;
    _rkResources.maxTessPatchComponents = 120;
    _rkResources.maxPatchVertices = 32;
    _rkResources.maxTessGenLevel = 64;
    _rkResources.maxViewports = 16;
    _rkResources.maxVertexAtomicCounters = 0;
    _rkResources.maxTessControlAtomicCounters = 0;
    _rkResources.maxTessEvaluationAtomicCounters = 0;
    _rkResources.maxGeometryAtomicCounters = 0;
    _rkResources.maxFragmentAtomicCounters = 8;
    _rkResources.maxCombinedAtomicCounters = 8;
    _rkResources.maxAtomicCounterBindings = 1;
    _rkResources.maxVertexAtomicCounterBuffers = 0;
    _rkResources.maxTessControlAtomicCounterBuffers = 0;
    _rkResources.maxTessEvaluationAtomicCounterBuffers = 0;
    _rkResources.maxGeometryAtomicCounterBuffers = 0;
    _rkResources.maxFragmentAtomicCounterBuffers = 1;
    _rkResources.maxCombinedAtomicCounterBuffers = 1;
    _rkResources.maxAtomicCounterBufferSize = 16384;
    _rkResources.maxTransformFeedbackBuffers = 4;
    _rkResources.maxTransformFeedbackInterleavedComponents = 64;
    _rkResources.maxCullDistances = 8;
    _rkResources.maxCombinedClipAndCullDistances = 8;
    _rkResources.maxSamples = 4;
    _rkResources.limits.nonInductiveForLoops = 1;
    _rkResources.limits.whileLoops = 1;
    _rkResources.limits.doWhileLoops = 1;
    _rkResources.limits.generalUniformIndexing = 1;
    _rkResources.limits.generalAttributeMatrixVectorIndexing = 1;
    _rkResources.limits.generalVaryingIndexing = 1;
    _rkResources.limits.generalSamplerIndexing = 1;
    _rkResources.limits.generalVariableIndexing = 1;
    _rkResources.limits.generalConstantMatrixVectorIndexing = 1;
}

std::string CGLSLMaterialGetInstance::m_kEmptySource;
CGLSLMaterialGetInstance::CGLSLMaterialGetInstance(EMaterialContextType _eType)
    : CGLSLMaterialContextArg(_eType)
    , m_pkInstance(nullptr)
{

}

CGLSLMaterialGetInstance::CGLSLMaterialGetInstance()
    : CGLSLMaterialContextArg(eMCT_GetInstance)
    , m_pkInstance(nullptr)
{

}

CGLSLMaterialGetInstance::~CGLSLMaterialGetInstance()
{

}

void CGLSLMaterialGetInstance::SetInstance(IMaterial* _pkMaterial) const
{
    // Because of EntityX Event arch limit
    const_cast <IMaterial*> (m_pkInstance) = _pkMaterial;
}

void CGLSLMaterialGetInstance::SetShaderName(const char* _pcShaderName, int _eShaderType)
{
    if (!_pcShaderName)
        return;
    if (_eShaderType < eShader_Vertex || _eShaderType >= eShader_Max)
        return;
    m_kShaderName [_eShaderType] = _pcShaderName;
}

const std::string& CGLSLMaterialGetInstance::GetShaderName(int _eShaderType) const
{
    if (_eShaderType < eShader_Vertex || _eShaderType >= eShader_Max)
        return m_kEmptySource;
    return m_kShaderName [_eShaderType];
}

CGLSLMaterialCreateInstanceFromFile::CGLSLMaterialCreateInstanceFromFile(EMaterialContextType _eType)
    : CGLSLMaterialGetInstance(_eType)
{

}

CGLSLMaterialCreateInstanceFromFile::CGLSLMaterialCreateInstanceFromFile()
    : CGLSLMaterialGetInstance(eMCT_CreateInstanceFromFile)
{

}

CGLSLMaterialCreateInstanceFromFile::~CGLSLMaterialCreateInstanceFromFile()
{

}

void CGLSLMaterialCreateInstanceFromFile::SetShaderFile(const char* _pcFileName, int _eShaderType)
{
    if (!_pcFileName)
        return;
    if (_eShaderType < eShader_Vertex || _eShaderType >= eShader_Max)
        return;
    m_kFileName [_eShaderType] = _pcFileName;
}

const std::string& CGLSLMaterialCreateInstanceFromFile::GetShaderFile(int _eShaderType) const
{
    if (_eShaderType < eShader_Vertex || _eShaderType >= eShader_Max)
        return m_kEmptySource;
    return m_kFileName [_eShaderType];
}

CGLSLMaterialCreateComponentFromFile::CGLSLMaterialCreateComponentFromFile(entityx::Entity& _rkEntity, const char* _pcCompType)
    : CGLSLMaterialCreateInstanceFromFile(eMCT_CreateComponentFromFile)
    , m_rkEntity(_rkEntity)
    , m_kCompType(_pcCompType)
{

}

CGLSLMaterialCreateComponentFromFile::~CGLSLMaterialCreateComponentFromFile()
{

}

entityx::Entity& CGLSLMaterialCreateComponentFromFile::GetEntity() const
{
    return m_rkEntity;
}

CGLSLMaterialCreateInstanceFromSrc::CGLSLMaterialCreateInstanceFromSrc(EMaterialContextType _eType)
    : CGLSLMaterialGetInstance(_eType)
{
}

CGLSLMaterialCreateInstanceFromSrc::CGLSLMaterialCreateInstanceFromSrc()
    : CGLSLMaterialGetInstance(eMCT_CreateInstance)
{}

CGLSLMaterialCreateInstanceFromSrc::~CGLSLMaterialCreateInstanceFromSrc()
{}

void CGLSLMaterialCreateInstanceFromSrc::SetShaderSource(const char* _pcShaderSource, int _eShaderType)
{
    if (!_pcShaderSource)
        return;
    if (_eShaderType < eShader_Vertex || _eShaderType >= eShader_Max)
        return;
    m_kShaderSource [_eShaderType] = _pcShaderSource;
}

const std::string& CGLSLMaterialCreateInstanceFromSrc::GetShaderSource (int _eShaderType) const
{
    if (_eShaderType < eShader_Vertex || _eShaderType >= eShader_Max)
        return m_kEmptySource;
    return m_kShaderSource [_eShaderType];
}

CGLSLMaterialCreateComponentFromSrc::CGLSLMaterialCreateComponentFromSrc(entityx::Entity& _rkEntity, const char* _pcCompType)
    : CGLSLMaterialCreateInstanceFromSrc(eMCT_CreateComponent)
    , m_rkEntity (_rkEntity)
    , m_kCompType(_pcCompType)
{
}

CGLSLMaterialCreateComponentFromSrc::~CGLSLMaterialCreateComponentFromSrc ()
{
}

entityx::Entity& CGLSLMaterialCreateComponentFromSrc::GetEntity () const
{
    return m_rkEntity;
}

CGLSLMaterialSystem* CGLSLMaterialSystem::ms_pkThis = nullptr;
CGLSLMaterialSystem::CGLSLMaterialSystem()
{
    ms_pkThis = this;
}

CGLSLMaterialSystem::~CGLSLMaterialSystem()
{
    glslang::FinalizeProcess ();
}

void CGLSLMaterialSystem::configure(entityx::EntityManager& EntityMgr, entityx::EventManager& _rkEvents)
{
    bool bRet = glslang::InitializeProcess();
    assert(bRet);

    _rkEvents.subscribe <IMaterialContextArg, IMatertialSystem>(*this);
}

void CGLSLMaterialSystem::ValidateMaterials(entityx::EntityManager& _rkEntityMgr, entityx::EventManager& _rkEventMgr)
{
}

int CGLSLMaterialSystem::GetDescHash(IGeometryHandler& _rkGeometry, ILightHandler& _rkLight)
{
    return 0;
}

IMaterial* CGLSLMaterialSystem::CreateMaterialInstance(size_t _nHash)
{
    IRenderSystem* pkRenderer = IRenderSystem::GetSystem();
    if (!pkRenderer)
        return nullptr;
    IGLSLMaterialFactory* pkFactory = m_kMaterialFactory.GetFactoryByType<IGLSLMaterialFactory>(pkRenderer->GetRendererName());
    if (!pkFactory)
        return nullptr;
    std::shared_ptr <CGLSLMaterial> spkMaterial = (*pkFactory)(*this);
    if (!spkMaterial)
        return nullptr;
    m_kMaterialPool.insert(std::make_pair(_nHash, spkMaterial));
    return m_kMaterialPool [_nHash].get();
}

IMaterial* CGLSLMaterialSystem::GetMaterialInstance(size_t _nHash)
{
    std::map <size_t, std::shared_ptr <IMaterial> >::iterator kIt = m_kMaterialPool.find (_nHash);
    if (kIt != m_kMaterialPool.end ())
        return kIt->second.get ();
    return nullptr;
}

std::shared_ptr <ITexture> CGLSLMaterialSystem::CreateRenderData(ITextureDesc* _pkDesc, bool _bEmpty)
{
    std::shared_ptr <ITexture> spkTexture;
    if (!_pkDesc)
        return nullptr;

    if (_bEmpty) {
        spkTexture = IRenderSystem::CreateTexture(_pkDesc->GetTextureDim());
        m_kTexturePool.insert(std::make_pair(_pkDesc->GetFileName(), spkTexture));
        return spkTexture;
    }

    gli::texture kTexture = gli::load (_pkDesc->GetFileName ());
    if (kTexture.empty ())
        return nullptr;
    const size_t nLevel = kTexture.levels ();
    spkTexture = IRenderSystem::CreateTexture(_pkDesc->GetTextureDim());
    spkTexture->SetLevel (nLevel);
    for (int nIndex = 0; nIndex < nLevel; nIndex++) {
        gli::texture::dim_type kDim = kTexture.dimensions (nIndex);
        if (!spkTexture->SetTextureData ((const char*)kTexture.data (0, 0, nIndex), kDim.x, kDim.y, kDim.z, kTexture.format (), nIndex))
            continue;
        spkTexture->SetFilterMode(eFT_Min, (nLevel > 1) ? eFM_Linear_Mipmap_Linear : eFM_Linear);
        spkTexture->SetFilterMode(eFT_Mag, eFM_Linear);
        spkTexture->SetClampMode(eCT_U, eCM_Repeat);
        spkTexture->SetClampMode(eCT_V, eCM_Repeat);
    }

    m_kTexturePool.insert(std::make_pair(_pkDesc->GetFileName(), spkTexture));

    return spkTexture;
}

std::shared_ptr <ITexture> CGLSLMaterialSystem::GetRenderData (ITextureDesc* _pkDesc)
{
    if (!_pkDesc)
        return nullptr;
    std::map <std::string, std::shared_ptr <ITexture> >::iterator kIt = m_kTexturePool.find(_pkDesc->GetFileName());
    if (kIt != m_kTexturePool.end())
        return kIt->second;

    return nullptr;
}

void CGLSLMaterialSystem::RemoveRenderData (ITextureDesc* _pkDesc)
{
    if (_pkDesc)
        return;
    std::map <std::string, std::shared_ptr <ITexture> >::iterator kIt = m_kTexturePool.find (_pkDesc->GetName ());
    if (kIt == m_kTexturePool.end ())
        return;

    _pkDesc->SetRenderData (nullptr);

    if (kIt->second.use_count () == 1) {
        m_kTexturePool.erase (kIt);
    }
}

void CGLSLMaterialSystem::receive(const IMaterialContextArg& _rkEventArg)
{
    const CGLSLMaterialContextArg* pkEvent = dynamic_cast <const CGLSLMaterialContextArg*>(&_rkEventArg);
    if (!pkEvent)
        return;

    EMaterialContextType eType = (EMaterialContextType)pkEvent->GetEventType ();
    switch (eType)
    {
        default:
        case eMCT_None:
            return;
        case eMCT_CreateInstance:
        {
            const CGLSLMaterialCreateInstanceFromSrc& rkArgs = static_cast <const CGLSLMaterialCreateInstanceFromSrc&>(_rkEventArg);
            if (!CreateMaterialInstance(rkArgs)) {
                break;
            }
            break;
        }
        case eMCT_CreateInstanceFromFile:
        {
            const CGLSLMaterialCreateInstanceFromFile& rkArgs = static_cast <const CGLSLMaterialCreateInstanceFromFile&>(_rkEventArg);
            if (!CreateMaterialInstance(rkArgs)) {
                break;
            }
            break;
        }
        case eMCT_GetInstance:
        {
            const CGLSLMaterialGetInstance& rkArgs = static_cast <const CGLSLMaterialGetInstance&>(_rkEventArg);
            if (!GetMaterialInstance(rkArgs))
                break;
            break;
        }
        case eMCT_CreateComponent:
        {
            const CGLSLMaterialCreateComponentFromSrc& rkArgs = static_cast <const CGLSLMaterialCreateComponentFromSrc&>(_rkEventArg);
            if (!CreateMaterialComponent(rkArgs)) {
                // Do assert?
                break;
            }
            break;
        }
        case eMCT_CreateComponentFromFile:
        {
            const CGLSLMaterialCreateComponentFromFile& rkArgs = static_cast <const CGLSLMaterialCreateComponentFromFile&>(_rkEventArg);
            if (!CreateMaterialComponent(rkArgs)) {
                break;
            }
            break;
        }
    }
}

bool CGLSLMaterialSystem::CreateMaterialInstance(const CGLSLMaterialCreateInstanceFromSrc& _rkArg)
{
    if (GetMaterialInstance(_rkArg)) {
        return true;
    }
    std::string kNameHashContain;
    for (int eShaderType = eShader_Vertex; eShaderType < eShader_Max; eShaderType++) {
        kNameHashContain += _rkArg.GetShaderName(eShaderType);
    }
    std::hash <std::string> kHasher;
    size_t nMaterialHash = kHasher(kNameHashContain);
    IMaterial* pkMaterial = CreateMaterialInstance (nMaterialHash);
    // Check Shader assgined to Material
    for (int eShaderType = eShader_Vertex; eShaderType < eShader_Max; eShaderType++) {
        const std::string& kName = _rkArg.GetShaderName (eShaderType);
        if (kName.empty ())
            continue;
        IShader* pkShader = pkMaterial->GetShader (eShaderType);
        if (pkShader)
            continue;
        std::shared_ptr <IShader> spkShader = GetShaderInstance (kName);
        if (!spkShader) {
            spkShader = CreateShaderInstance (kName, _rkArg.GetShaderSource (eShaderType), (EShaderType)eShaderType);
        }
        pkMaterial->SetShader (spkShader);
        if (!spkShader->Compile ()) {
            ILogger::Log(eLL_Error, "Shader Compiler Error : \nFile[%s] \nCompileLog : %s", _rkArg.GetShaderName(eShaderType).c_str(), spkShader->GetCompileInfo());
            assert (0);
            return false;
        }
    }
    UpdateConstantReflection(pkMaterial);
    _rkArg.SetInstance(pkMaterial);
    return true;
}

bool CGLSLMaterialSystem::CreateMaterialComponent(const CGLSLMaterialCreateComponentFromSrc& _rkArg)
{
    entityx::Entity& rkEntity = _rkArg.GetEntity ();
    if (!rkEntity.valid ())
        return false;

    if (!CreateMaterialInstance(_rkArg))
        return false;

    IMaterialComponent* pkComp = nullptr;
    bool bHasComp = rkEntity.has_component <IMaterialHandler>();
    if (bHasComp) {
        entityx::ComponentHandle <IMaterialHandler> kCompH = rkEntity.component <IMaterialHandler> ();
        IMaterialHandler& rkIHandler = (*kCompH.get ());
        pkComp = rkIHandler.get ();
    }

    if (!pkComp) {
        IMaterialCompFactory* pkFactory = m_kMaterialCompFactory.GetFactoryByType<IMaterialCompFactory>(_rkArg.GetCompType());
        if (!pkFactory)
            return false;
        std::shared_ptr <IMaterialComponent> spkMaterial = (*pkFactory)(_rkArg.GetMaterialInstance());
        pkComp = spkMaterial.get();
        if (!bHasComp)
            rkEntity.assign <IMaterialHandler>(spkMaterial);
    }
    if (!pkComp)
        return false;

    return true;
}

bool CGLSLMaterialSystem::CreateMaterialInstance(const CGLSLMaterialCreateInstanceFromFile& _rkArg)
{
    if (GetMaterialInstance(_rkArg)) {
        return true;
    }
    std::string kNameHashContain;
    for (int eShaderType = eShader_Vertex; eShaderType < eShader_Max; eShaderType++) {
        kNameHashContain += _rkArg.GetShaderName(eShaderType);
    }
    std::hash <std::string> kHasher;
    size_t nMaterialHash = kHasher(kNameHashContain);
    IMaterial* pkMaterial = CreateMaterialInstance(nMaterialHash);
    if (!pkMaterial)
        return false;

    auto fnLoadShaderSource = [this] (std::shared_ptr <IShader>& rspkShader, const std::string& _rkName, const std::string& _rkShaderFile, int _eShaderType)
    {
        FILE* pkFile = fopen(_rkShaderFile.c_str(), "rb");
        if (pkFile) {
            std::string kSrc;
            {
                fseek(pkFile, 0, SEEK_END);
                size_t nSize = ftell(pkFile);
                fseek(pkFile, 0, SEEK_SET);
                std::shared_ptr <char> spcBuff(new char [nSize + 1](), SArrayDeleter<char>());
                char* pcBuff = spcBuff.get();
                fread(pcBuff, nSize, 1, pkFile);
                pcBuff [nSize] = '\0';
                fclose(pkFile);
                kSrc = pcBuff;
            }
            
            rspkShader = CreateShaderInstance(_rkName, kSrc, (EShaderType)_eShaderType);
        }
    };
    for (int eShaderType = eShader_Vertex; eShaderType < eShader_Max; eShaderType++) {
        const std::string& kName = _rkArg.GetShaderName(eShaderType);
        if (kName.empty())
            continue;
        IShader* pkShader = pkMaterial->GetShader(eShaderType);
        if (pkShader)
            continue;
        std::shared_ptr <IShader> spkShader = GetShaderInstance(kName);
        if (!spkShader) {
            fnLoadShaderSource(spkShader, kName, _rkArg.GetShaderFile(eShaderType), eShaderType);
        }
        pkMaterial->SetShader(spkShader);
        if (!spkShader->Compile()) {
            ILogger::Log(eLL_Error, "Shader Compiler Error : \nFile[%s] \nCompileLog : %s", _rkArg.GetShaderName(eShaderType).c_str(), spkShader->GetCompileInfo());
            assert(0);
            return false;
        }
    }

    UpdateConstantReflection(pkMaterial);
    _rkArg.SetInstance(pkMaterial);
    return true;
}

bool CGLSLMaterialSystem::CreateMaterialComponent(const CGLSLMaterialCreateComponentFromFile& _rkArg)
{
    entityx::Entity& rkEntity = _rkArg.GetEntity();
    if (!rkEntity.valid())
        return false;

    if (!CreateMaterialInstance(_rkArg))
        return false;

    IMaterialComponent* pkComp = nullptr;
    bool bHasComp = rkEntity.has_component <IMaterialHandler>();
    if (bHasComp) {
        entityx::ComponentHandle <IMaterialHandler> kCompH = rkEntity.component <IMaterialHandler>();
        IMaterialHandler& rkIHandler = (*kCompH.get());
        pkComp = rkIHandler.get();
    }

    if (!pkComp) {
        IMaterialCompFactory* pkFactory = m_kMaterialCompFactory.GetFactoryByType<IMaterialCompFactory>(_rkArg.GetCompType());
        if (!pkFactory)
            return false;
        std::shared_ptr <IMaterialComponent> spkMaterial = (*pkFactory)(_rkArg.GetMaterialInstance());
        pkComp = spkMaterial.get();
        if (!bHasComp)
            rkEntity.assign <IMaterialHandler>(spkMaterial);
    }
    if (!pkComp)
        return false;

    return true;
}

bool CGLSLMaterialSystem::GetMaterialInstance(const CGLSLMaterialGetInstance& _rkArg)
{
    std::string kNameHashContain;
    for (int eShaderType = eShader_Vertex; eShaderType < eShader_Max; eShaderType++) {
        kNameHashContain += _rkArg.GetShaderName(eShaderType);
    }
    std::hash <std::string> kHasher;
    size_t nMaterialHash = kHasher(kNameHashContain);
    IMaterial* pkMaterial = GetMaterialInstance(nMaterialHash);
    _rkArg.SetInstance(pkMaterial);
    return (pkMaterial) ? true : false;
}

std::shared_ptr <IConstantSemantic> CGLSLMaterialSystem::CreateConstantSematic(glslang::TProgram* _pkProgram, int _nConstantIndex)
{
    std::shared_ptr <IConstantSemantic> spkConstant;
    int nType = _pkProgram->getUniformType(_nConstantIndex);
    const char* pcName = _pkProgram->getUniformName(_nConstantIndex);
    switch (nType)
    {
    case GL_FLOAT_VEC2:
        spkConstant = std::make_shared <TGLSLConstantSemantic <glm::vec2> >(pcName, pcName, eConstant_floatVec2);
        break;
    case GL_FLOAT_VEC3:
        spkConstant = std::make_shared <TGLSLConstantSemantic <glm::vec3> >(pcName, pcName, eConstant_floatVec3);
        break;
    case GL_FLOAT_VEC4:
        spkConstant = std::make_shared <TGLSLConstantSemantic <glm::vec4> >(pcName, pcName, eConstant_floatVec4);
        break;
    case GL_INT_VEC2:
        spkConstant = std::make_shared <TGLSLConstantSemantic <glm::ivec2> >(pcName, pcName, eConstant_intVec2);
        break;
    case GL_INT_VEC3:
        spkConstant = std::make_shared <TGLSLConstantSemantic <glm::ivec2> >(pcName, pcName, eConstant_intVec3);
        break;
    case GL_INT_VEC4:
        spkConstant = std::make_shared <TGLSLConstantSemantic <glm::ivec2> >(pcName, pcName, eConstant_intVec4);
        break;
    case GL_FLOAT_MAT2:
        spkConstant = std::make_shared <TGLSLConstantSemantic <glm::mat2> >(pcName, pcName, eConstant_Matrix2x2);
        break;
    case GL_FLOAT_MAT3:
        spkConstant = std::make_shared <TGLSLConstantSemantic <glm::mat3> >(pcName, pcName, eConstant_Matrix3x3);
        break;
    case GL_FLOAT_MAT4:
        spkConstant = std::make_shared <TGLSLConstantSemantic <glm::mat4> >(pcName, pcName, eConstant_Matrix4x4);
        break;
    case GL_FLOAT:
        spkConstant = std::make_shared <TGLSLConstantSemantic <float> >(pcName, pcName, eConstant_float);
        break;
    case GL_INT:
        spkConstant = std::make_shared <TGLSLConstantSemantic <int> >(pcName, pcName, eConstant_int);
        break;
    case GL_UNSIGNED_INT:
        spkConstant = std::make_shared <TGLSLConstantSemantic <unsigned int> >(pcName, pcName, eConstant_uint);
        break;
    case GL_SHORT:
        spkConstant = std::make_shared <TGLSLConstantSemantic <short> >(pcName, pcName, eConstant_short);
        break;
    case GL_UNSIGNED_SHORT:
        spkConstant = std::make_shared <TGLSLConstantSemantic <unsigned short> >(pcName, pcName, eConstant_ushort);
        break;
    case GL_BOOL:
        spkConstant = std::make_shared <TGLSLConstantSemantic <bool> >(pcName, pcName, eConstant_bool);
        break;
    case GL_SAMPLER_2D:
    case GL_SAMPLER_3D:
    case GL_SAMPLER_CUBE:
        spkConstant = std::make_shared <TGLSLConstantSemantic <ITexture*> >(pcName, pcName, eConstant_Texture);
        break;
    default:
        assert(0);
    }
    const int nUniformIndex = _pkProgram->getUniformBlockIndex(_nConstantIndex);
    if (nUniformIndex != -1) {
        spkConstant->SetMemberIndex(nUniformIndex);
        spkConstant->SetMemberOffset(_pkProgram->getUniformBufferOffset(_nConstantIndex));
    }
    return spkConstant;
}

std::shared_ptr <IStructConstantSematic> CGLSLMaterialSystem::CreateConstantStructSematic(glslang::TProgram* pkProgream, int _nConstantIndex)
{
    const glslang::TType* pkType = pkProgream->getUniformBlockTType(_nConstantIndex);
    const char* pcName = pkProgream->getUniformBlockName(_nConstantIndex);
    std::shared_ptr <CGenericStructSematic> spkSematic = std::make_shared<CGenericStructSematic>(pcName, pcName, pkProgream->getUniformBlockSize(_nConstantIndex));
    return spkSematic;
}

void CGLSLMaterialSystem::UpdateConstantReflection(IMaterial* _pkMaterial)
{
    if (!_pkMaterial)
        return;

    _pkMaterial->SetConstantCount(0);
    // Type Mapping
    EShLanguage aeShaderLangType[eShader_Max] =
    {
        EShLangVertex,
        EShLangFragment,
        EShLangGeometry,
    };

    std::shared_ptr <glslang::TProgram> spkProgram = std::make_shared<glslang::TProgram>();
    for (int eShaderType = eShader_Vertex; eShaderType < eShader_Max; eShaderType++) {
        IShader* pkShader = _pkMaterial->GetShader(eShaderType);
        if (!pkShader)
            continue;
        CGLSLShader* pkGLSLShader = dynamic_cast<CGLSLShader*>(pkShader);
        spkProgram->addShader(&pkGLSLShader->GetShaderParser());
    }

    if (!spkProgram->link (EShMsgDefault)) {
        assert(0);
        return;
    }
    if (!spkProgram->buildReflection()) {
        assert(0);
        return;
    }
    const int nBlockCount = spkProgram->getNumLiveUniformBlocks();
    if (nBlockCount > 0) {
        CGLSLMaterial* pkMat = dynamic_cast <CGLSLMaterial*>(_pkMaterial);
        if (pkMat) {
            pkMat->SetConstantStructCount(nBlockCount);
            for (unsigned int nIndex = 0; nIndex < nBlockCount; nIndex++) {
                std::shared_ptr <IStructConstantSematic> spkStructSematic = CreateConstantStructSematic(spkProgram.get(), nIndex);
                pkMat->SetConstantStruct(spkStructSematic, nIndex);
            }
        }
    }
    const int nUniformCount = spkProgram->getNumLiveUniformVariables();
    _pkMaterial->SetConstantCount(nUniformCount);
    for (int nCount = 0; nCount < nUniformCount; nCount++) {
        std::shared_ptr <IConstantSemantic> spkConstant = CreateConstantSematic(spkProgram.get(), nCount);
        _pkMaterial->SetConstant(spkConstant, nCount);
    }
    // Active Attritube
    const size_t nActiveAttribue = spkProgram->getNumLiveAttributes();
    _pkMaterial->SetVariableCount(nActiveAttribue);
    for (size_t nIndex = 0; nIndex < nActiveAttribue; nIndex++) {
        const char* pcVariableName = spkProgram->getAttributeName(nIndex);
        const glslang::TType* pkType = spkProgram->getAttributeTType(nIndex);
        std::shared_ptr <IVariableAttrDesc> spkDesc = std::make_shared<CGLSLVariableAttrDesc>(pcVariableName, spkProgram->getAttributeType(nIndex), pkType->getQualifier().layoutLocation);
        _pkMaterial->SetVariable(spkDesc, nIndex);
    }
    // N.B.: Destruct a linked program *before* destructing the shaders linked into it.
    spkProgram = nullptr;
}

std::shared_ptr <IShader> CGLSLMaterialSystem::GetShaderInstance(const std::string& _rkName)
{
    auto spkShaderIt = m_kShaderPool.find(_rkName);
    if (spkShaderIt == m_kShaderPool.end())
        return nullptr;
    return spkShaderIt->second;
}

std::shared_ptr <IShader> CGLSLMaterialSystem::CreateShaderInstance(const std::string& _rkName, const std::string& _rkSource, EShaderType _eType)
{
    IGLSLShaderFactory* pkFactory = m_kShaderFactory.GetFactoryByType <IGLSLShaderFactory>(_eType);
    if (!pkFactory)
        return nullptr;

    std::shared_ptr <IShader> spkShader = (*pkFactory)();
    m_kShaderPool.insert(std::make_pair(_rkName, spkShader));
    spkShader->SetSource(_rkSource.c_str());
    return spkShader;
}