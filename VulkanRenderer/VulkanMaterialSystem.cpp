#include "ECSVulkanPCH.h"
#include "VulkanMaterialSystem.h"
#include "VulkanMaterial.h"
#include "DefaultMaterialComponent.h"
#include "VulkanRenderSystem.h"
#include "DataWalker.h"

#define GL_SHORT                          0x1402
#define GL_FLOAT                          0x1406
#define GL_FLOAT_VEC2                     0x8B50
#define GL_FLOAT_VEC3                     0x8B51
#define GL_FLOAT_VEC4                     0x8B52
#define GL_INT                            0x1404
#define GL_INT_VEC2                       0x8B53
#define GL_INT_VEC3                       0x8B54
#define GL_INT_VEC4                       0x8B55
#define GL_FLOAT_MAT2                     0x8B5A
#define GL_FLOAT_MAT3                     0x8B5B
#define GL_FLOAT_MAT4                     0x8B5C
#define GL_FLOAT_MAT2x3                   0x8B65
#define GL_FLOAT_MAT2x4                   0x8B66
#define GL_FLOAT_MAT3x2                   0x8B67
#define GL_FLOAT_MAT3x4                   0x8B68
#define GL_FLOAT_MAT4x2                   0x8B69
#define GL_FLOAT_MAT4x3                   0x8B6A
#define GL_UNSIGNED_INT                   0x1405
#define GL_UNSIGNED_SHORT                 0x1403
#define GL_BOOL                           0x8B56
#define GL_SAMPLER_2D                     0x8B5E
#define GL_SAMPLER_3D                     0x8B5F
#define GL_SAMPLER_CUBE                   0x8B60

class CVulkanShaderFactory : public IGLSLShaderFactory
{
public:
    CVulkanShaderFactory(EShaderType _eType)
        : IGLSLShaderFactory(_eType)
    {
    }

    virtual std::shared_ptr <CGLSLShader> operator()()
    {
        CVulkanRenderSystem* pkRenderer = dynamic_cast <CVulkanRenderSystem*>(IRenderSystem::GetRenderer());
        return std::make_shared <CVulkanShader>(*pkRenderer, GetType());
    }
};

class CVulkanMaterialFactory : public IGLSLMaterialFactory
{
public:
    CVulkanMaterialFactory(std::string _kType)
        : IGLSLMaterialFactory(_kType)
    {
    }

    virtual std::shared_ptr <CGLSLMaterial> operator()(class CGLSLMaterialSystem& _rkSystem)
    {
        CVulkanRenderSystem* pkRenderer = dynamic_cast <CVulkanRenderSystem*>(IRenderSystem::GetRenderer());
        return std::make_shared <CVulkanMaterial>(*pkRenderer, _rkSystem);
    }
};

class CVulkanMaterialCompFactory : public IMaterialCompFactory
{
public:
    CVulkanMaterialCompFactory(std::string _kType)
        : IMaterialCompFactory(_kType)
    {
    }

    virtual std::shared_ptr <IMaterialComponent> operator()(IMaterial* _plMaterialIns)
    {
        std::shared_ptr <IMaterialComponent> spkComp = std::make_shared <CDefaultMaterialComponent>();
        spkComp->SetMaterialInstance(_plMaterialIns);
        return spkComp;
    }
};

CVulkanMaterialSystem::CVulkanMaterialSystem()
{
}

CVulkanMaterialSystem::~CVulkanMaterialSystem()
{
}

void CVulkanMaterialSystem::configure(entityx::EntityManager& EntityMgr, entityx::EventManager& _rkEvents)
{
    CGLSLMaterialSystem::configure(EntityMgr, _rkEvents);

    RegisterComponentFactory<CVulkanMaterialCompFactory>("Default");
    RegisterMaterialFactory<CVulkanMaterialFactory>(IRenderSystem::GetSystem()->GetRendererName());
    RegisterShaderFactory<CVulkanShaderFactory>(eShader_Vertex);
    RegisterShaderFactory<CVulkanShaderFactory>(eShader_Pixel);
    RegisterShaderFactory<CVulkanShaderFactory>(eShader_Geomtery);
}

std::shared_ptr <IConstantSemantic> CVulkanMaterialSystem::CreateConstantSematic(glslang::TProgram* _pkProgram, int _nConstantIndex)
{
    std::shared_ptr <IConstantSemantic> spkConstant;
    int nType = _pkProgram->getUniformType(_nConstantIndex);
    const char* pcName = _pkProgram->getUniformName(_nConstantIndex);
    switch (nType)
    {
    case GL_FLOAT_VEC2:
        spkConstant = std::make_shared <TGLSLConstantSemantic <glm::vec2, IVulkanUniformInterface > >(pcName, pcName, eConstant_floatVec2);
        break;
    case GL_FLOAT_VEC3:
        spkConstant = std::make_shared <TGLSLConstantSemantic <glm::vec3, IVulkanUniformInterface > >(pcName, pcName, eConstant_floatVec3);
        break;
    case GL_FLOAT_VEC4:
        spkConstant = std::make_shared <TGLSLConstantSemantic <glm::vec4, IVulkanUniformInterface > >(pcName, pcName, eConstant_floatVec4);
        break;
    case GL_INT_VEC2:
        spkConstant = std::make_shared <TGLSLConstantSemantic <glm::ivec2, IVulkanUniformInterface > >(pcName, pcName, eConstant_intVec2);
        break;
    case GL_INT_VEC3:
        spkConstant = std::make_shared <TGLSLConstantSemantic <glm::ivec2, IVulkanUniformInterface > >(pcName, pcName, eConstant_intVec3);
        break;
    case GL_INT_VEC4:
        spkConstant = std::make_shared <TGLSLConstantSemantic <glm::ivec2, IVulkanUniformInterface > >(pcName, pcName, eConstant_intVec4);
        break;
    case GL_FLOAT_MAT2:
        spkConstant = std::make_shared <TGLSLConstantSemantic <glm::mat2, IVulkanUniformInterface > >(pcName, pcName, eConstant_Matrix2x2);
        break;
    case GL_FLOAT_MAT3:
        spkConstant = std::make_shared <TGLSLConstantSemantic <glm::mat3, IVulkanUniformInterface > >(pcName, pcName, eConstant_Matrix3x3);
        break;
    case GL_FLOAT_MAT4:
        spkConstant = std::make_shared <TGLSLConstantSemantic <glm::mat4, IVulkanUniformInterface > >(pcName, pcName, eConstant_Matrix4x4);
        break;
    case GL_FLOAT:
        spkConstant = std::make_shared <TGLSLConstantSemantic <float, IVulkanUniformInterface > >(pcName, pcName, eConstant_float);
        break;
    case GL_INT:
        spkConstant = std::make_shared <TGLSLConstantSemantic <int, IVulkanUniformInterface > >(pcName, pcName, eConstant_int);
        break;
    case GL_UNSIGNED_INT:
        spkConstant = std::make_shared <TGLSLConstantSemantic <unsigned int, IVulkanUniformInterface > >(pcName, pcName, eConstant_uint);
        break;
    case GL_SHORT:
        spkConstant = std::make_shared <TGLSLConstantSemantic <short, IVulkanUniformInterface > >(pcName, pcName, eConstant_short);
        break;
    case GL_UNSIGNED_SHORT:
        spkConstant = std::make_shared <TGLSLConstantSemantic <unsigned short, IVulkanUniformInterface > >(pcName, pcName, eConstant_ushort);
        break;
    case GL_BOOL:
        spkConstant = std::make_shared <TGLSLConstantSemantic <bool, IVulkanUniformInterface > >(pcName, pcName, eConstant_bool);
        break;
    case GL_SAMPLER_2D:
    case GL_SAMPLER_3D:
    case GL_SAMPLER_CUBE:
        spkConstant = std::make_shared <TGLSLConstantSemantic <ITexture*, IVulkanUniformInterface > >(pcName, pcName, eConstant_Texture);
        break;
    default:
        assert(0);
    }
    const glslang::TType* pkType = _pkProgram->getUniformTType(_nConstantIndex);
    if (pkType) {
        IVulkanUniformInterface* pkVulkanConstant = dynamic_cast<IVulkanUniformInterface*>(spkConstant.get());
        pkVulkanConstant->SetDescriptorSetID(pkType->getQualifier().layoutSet);
        pkVulkanConstant->SetDescriptorSetBinding(pkType->getQualifier().layoutBinding);
        pkVulkanConstant->SetBasicType(pkType->getBasicType());
    }
    return spkConstant;
}

std::shared_ptr <IStructConstantSematic> CVulkanMaterialSystem::CreateConstantStructSematic(glslang::TProgram* pkProgream, int _nConstantIndex)
{
    const glslang::TType* pkType = pkProgream->getUniformBlockTType(_nConstantIndex);
    const char* pcName = pkProgream->getUniformBlockName(_nConstantIndex);
    std::shared_ptr <CVulkanGenericStructSematic> spkSematic = std::make_shared<CVulkanGenericStructSematic>(pcName, pcName, pkProgream->getUniformBlockSize(_nConstantIndex));
    spkSematic->SetBasicType(pkType->getBasicType());
    spkSematic->SetDescriptorSetID(pkType->getQualifier().layoutSet);
    spkSematic->SetDescriptorSetBinding(pkType->getQualifier().layoutBinding);
    return spkSematic;
}

bool ParseConsiderQualifier(const char* _pcVar, TDataWalker<const char>& kSrc, TDataWalker<char>& kDst, int _nSet)
{
    while (kSrc == ' ') {
        kSrc.Skip(" ");
    }
    std::string kVar = _pcVar;
    if (kSrc.Compare(kVar.c_str(), kVar.length())) {
        kSrc.Skip(kVar.c_str(), kVar.length());
        std::string kWrite = _pcVar;
        kWrite += " = ";
        kDst.Write(kWrite.c_str(), kWrite.length());
        while (kSrc == ' ' || kSrc == '=') {
            kSrc.Skip(" ");
        }
        int nCurrSet = -1;
        verify(sscanf_s(kSrc.GetCurr(), "%d", &nCurrSet) > 0);
        if (nCurrSet != _nSet) {
            ILogger::Log(eLL_Warning, "Warning! shader Type %d is not match with uniform qualifier %s!\n", _nSet, _pcVar);
        }
        std::string kValue = std::to_string(_nSet);
        kDst.Write(kValue.c_str(), kValue.size());
        if (kSrc != ',' && kSrc != ')') {
            do {
                kSrc++;
            } while (kSrc != ',' && kSrc != ')');
        }
        return true;
    }
    return false;
}

std::shared_ptr <IShader> CVulkanMaterialSystem::CreateShaderInstance(const std::string& _rkName, const std::string& _rkSource, EShaderType _eType)
{
    IGLSLShaderFactory* pkFactory = m_kShaderFactory.GetFactoryByType <IGLSLShaderFactory>(_eType);
    if (!pkFactory)
        return nullptr;

    const char* apcGlobalUniformName[] =
    {
        "VertexGlobalUniform",
        "PixelGlobalUniform",
        "GeometryGlobalUniform",
    };

    const char* apcVector[] =
    {
        "vec2",
        "vec3",
        "vec4",
    };
    const char* apcMatrix[] =
    {
        "mat2x2",
        "mat2x3",
        "mat2x4",
        "mat3x2",
        "mat3x3",
        "mat3x4",
        "mat4x2",
        "mat4x3",
        "mat4x4",
    };
    const char* apcSampler[] =
    {
        "sampler2D",
        "sampler3D",
        "samplerCube",
    };

    std::shared_ptr <IShader> spkShader = (*pkFactory)();
    m_kShaderPool.insert(std::make_pair(_rkName, spkShader));
    std::string kVulkanSource;
    kVulkanSource.resize(65535);
    TDataWalker <char> kDst((char*)kVulkanSource.data(), kVulkanSource.size());
    TDataWalker <const char> kSrc(_rkSource.data(), _rkSource.size());
    std::string kGlobalUniforms;
    bool bLayout = false;
    bool bUniform = false;
    bool bSet = false;
    bool bInLocation = false;
    bool bOutLocation = false;
    bool bBinding = false;
    bool bMain = false;
    bool bFunction = false;
    short nCurrSet = -1;
    short nCurrBinding = -1;
    short nCurrInLoc = -1;
    short nCurrOutLoc = -1;
    while (kSrc.isEnd()) {
        switch (kSrc)
        {
        case '#':
        {
            if (!kSrc.Compare("version")) {
                break;
            }
            kSrc.Skip("version ");
            kDst.Write("version 450\n");
            while (kSrc != ' ') {
                kSrc++;
            }
            break;
        }
        case '{':
            break;
        case '}':
            bFunction = false;
            break;
        case ';':
            bLayout = false;
            break;
        case '(':
        {
            kSrc.Skip(" ");
            kDst.Write('(');
            if (bLayout) {
                TDataWalker <const char> kSubSrc(kSrc.GetCurr(), 512);
                while (kSubSrc.isEnd() == false) {
                    bool bEnd = false;
                    switch (kSubSrc)
                    {
                    case 'i':
                    {
                        if (!kSubSrc.Compare("in"))
                            continue;
                        bInLocation = true;
                        bEnd = true;
                        break;
                    }
                    case 'o':
                    {
                        if (!kSubSrc.Compare("out"))
                            continue;
                        bOutLocation = true;
                        bEnd = true;
                        break;
                    }
                    case 'u':
                    {
                        if (!kSubSrc.Compare("uniform"))
                            break;
                        bUniform = true;
                        bEnd = true;
                        break;
                    }
                    case ';':
                    case '\0':
                        bEnd = true;
                        break;
                    default:
                        break;
                    }
                    if (bEnd)
                        break;
                    kSubSrc++;
                }
                kDst.Write('(');
                if (bInLocation) {
                    if (!ParseConsiderQualifier("location", kSrc, kDst, ++nCurrInLoc)) {
                        char cBuffer[256] = {};
                        sprintf_s(cBuffer, sizeof(cBuffer), "location = %d)", nCurrInLoc);
                        kDst.Write(cBuffer, strlen(cBuffer));
                    }
                    bInLocation = false;
                }
                else if (bOutLocation) {
                    if (!ParseConsiderQualifier("location", kSrc, kDst, ++nCurrOutLoc)) {
                        char cBuffer[256] = {};
                        sprintf_s(cBuffer, sizeof(cBuffer), "location = %d)", nCurrOutLoc);
                        kDst.Write(cBuffer, strlen(cBuffer));
                    }
                    bOutLocation = false;
                }
                else if (bUniform) {
                    if (ParseConsiderQualifier("set", kSrc, kDst, spkShader->GetShaderType())) {
                        kDst.Write(", ");
                    }
                    else {
                        char cBuffer [256] = {};
                        sprintf_s(cBuffer, sizeof(cBuffer), "set = %d,", spkShader->GetShaderType());
                        kDst.Write(cBuffer, strlen(cBuffer));
                    }
                    if (ParseConsiderQualifier("binding", kSrc, kDst, ++nCurrBinding)) {
                        kDst.Write(')');
                    }
                    else {
                        char cBuffer [256] = {};
                        sprintf_s(cBuffer, sizeof(cBuffer), "binding = %d)", nCurrBinding);
                        kDst.Write(cBuffer, strlen(cBuffer));
                    }
                }
            }
            break;
        }
        case 'i':
        {
            
        }
        case 'o':
        {

        }
        case 'u':
        {
            if (!kSrc.Compare("uniform")) {
                break;
            }
            kSrc.Skip("uniform");
            TDataWalker <const char> kSubSrc(kSrc.GetCurr(), 512);
            bool bSampler = false;
            while (kSubSrc.isEnd() == false) {
                bool bEnd = false;
                bool bFound = false;
                switch (kSubSrc)
                {
                case 'v':
                {
                    bool bFound = false;
                    const size_t nSize = _ARRAYSIZE(apcVector);
                    for (size_t nIndex = 0; nIndex != nSize; nIndex++) {
                        if (kSubSrc.Compare(apcVector[nIndex], strlen(apcVector[nIndex]))) {
                            bFound = true;
                            break;
                        }
                    }
                    if (!bFound) {
                        break;
                    }
                }
                case 'm':
                {
                    if (!bFound) {
                        const size_t nSize = _ARRAYSIZE(apcMatrix);
                        for (size_t nIndex = 0; nIndex != nSize; nIndex++) {
                            if (kSubSrc.Compare(apcMatrix[nIndex], strlen(apcMatrix[nIndex]))) {
                                bFound = true;
                                break;
                            }
                        }
                        if (!bFound) {
                            break;
                        }
                    }
                }
                case 'f':
                {
                    if (!bFound) {
                        if (!kSubSrc.Compare("float"))
                            continue;
                        bFound = true;
                    }
                }
                case 'b':
                {
                    if (!bFound) {
                        if (!kSubSrc.Compare("bool"))
                            continue;
                        bFound = true;
                    }
                }
                case 'i':
                {
                    if (!bFound) {
                        if (!kSubSrc.Compare("int"))
                            continue;
                        bFound = true;
                    }
                    bEnd = bFound;
                    break;
                }
                case 's':
                {
                    const size_t nSize = _ARRAYSIZE(apcSampler);
                    for (size_t nIndex = 0; nIndex != nSize; nIndex++) {
                        if (kSubSrc.Compare(apcSampler[nIndex], strlen(apcSampler[nIndex]))) {
                            bSampler = true;
                            break;
                        }
                    }
                    bEnd = bSampler;
                    break;
                }
                case ';':
                case '\0':
                    bEnd = true;
                    break;
                default:
                    break;
                }
                if (bEnd)
                    break;
                kSubSrc++;
            }
            if (bSampler) {
                kDst.Write("layout ");
                char cBuffer[256] = {};
                sprintf_s(cBuffer, sizeof(cBuffer), "(set = %d, binding = %d) uniform", spkShader->GetShaderType(), ++nCurrBinding);
                kDst.Write(cBuffer, strlen(cBuffer));
            }
            else {
                do {
                    kGlobalUniforms += kSrc;
                    kSrc++;
                } while ((kSrc != ';') && (kSrc.isEnd() == false));
            }
            continue;
        }
        case 'l':
        {
            if (!kSrc.Compare("layout")) {
                break;
            }
            kSrc.Skip("layout");
            kDst.Write("layout");
            bLayout = true;
            break;
        }
        case 'v':
        {
            if (kSrc.Compare("void")) {
                kSrc.Skip("void");
                bFunction = true;
                TDataWalker <const char> kSubSrc(kSrc.GetCurr(), 512);
                while (kSubSrc.isEnd() == false) {
                    bool bEnd = false;
                    switch (kSubSrc)
                    {
                    case 'm':
                    {
                        if (!kSubSrc.Compare("main")) {
                            break;
                        }
                        if (kGlobalUniforms.empty() == false) {
                            kDst.Write("layout ");
                            char cBuffer [256] = {};
                            sprintf_s(cBuffer, sizeof(cBuffer), "(set = %d, binding = %d) uniform %s \n{\n", spkShader->GetShaderType(), ++nCurrBinding, apcGlobalUniformName[spkShader->GetShaderType()]);
                            kDst.Write(cBuffer, strlen(cBuffer));
                            kDst.Write(kGlobalUniforms.c_str(), kGlobalUniforms.length());
                            kDst.Write("};\n");
                        }
                    }
                    case '\0':
                    case ';':
                    case '{':
                        bEnd = true;
                    default:
                        break;
                    }
                    if (bEnd)
                        break;
                    kSubSrc++;
                }
                kDst.Write("void");
                continue;
            }
            break;
        }
        case 'm':
        {
            if (bFunction && kSrc.Compare("main")) {
                kSrc.Skip("main");
                kDst.Write("main");
                bMain = true;
                continue;
            }
            break;
        }
        default:
            break;
        }
        kDst.Write(kSrc);
        kSrc++;
    }

    spkShader->SetSource(_rkSource.c_str());
    return spkShader;
}