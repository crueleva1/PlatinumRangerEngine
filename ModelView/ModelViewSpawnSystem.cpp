#include "MainPCH.h"
#include "ModelViewSpawnSystem.h"
#include "AssimpGLGeometryComp.h"
#include "CustomGeometryComp.h"
#include "GLSLMaterialSystem.h"
#include "OpenGLRenderSystem.h"
#include "ModelViewSceneSystem.h"
#include "GLMMainCameraComponent.h"
#include "GLMCubeCameraComponent.h"
#include "ISceneComponent.h"
#include "Win32InputSystem.h"
#include "ModelViewMovementSystem.h"
#include "DefaultLightComponent.h"
#include "ModelViewTextSystem.h"
#include <assimp/Importer.hpp>
#include <assimp/material.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "PBRMaterialComponent.h"


class ECS_MODELVIEW_API CPBRMaterialCompFactory : public IMaterialCompFactory
{
public:
    CPBRMaterialCompFactory(std::string _kType)
        : IMaterialCompFactory(_kType)
    {
    }

    virtual std::shared_ptr <IMaterialComponent> operator()(IMaterial* _plMaterialIns)
    {
        std::shared_ptr <IMaterialComponent> spkComp = std::make_shared <CPBRMaterialComponent>();
        spkComp->SetMaterialInstance(_plMaterialIns);
        return spkComp;
    }
};

int GetTextureFormatByText(std::string& _rkText)
{
    if (_rkText == "R8_UNORM") {
        return gli::FORMAT_R8_UNORM;
    }
    else if (_rkText == "RG8_UNORM") {
        return gli::FORMAT_RG8_UNORM;
    }
    else if (_rkText == "RGB8_UNORM") {
        return gli::FORMAT_RGB8_UNORM;
    }
    else if (_rkText == "RGBA8_UNORM") {
        return gli::FORMAT_RGBA8_UNORM;
    }
    else if (_rkText == "R8_SNORM") {
        return gli::FORMAT_R8_SNORM;
    }
    else if (_rkText == "RG8_SNORM") {
        return gli::FORMAT_RG8_SNORM;
    }
    else if (_rkText == "RGB8_SNORM") {
        return gli::FORMAT_RGB8_SNORM;
    }
    else if (_rkText == "RGBA8_SNORM") {
        return gli::FORMAT_RGBA8_SNORM;
    }
    else if (_rkText == "RGBA4_UNORM") {
        return gli::FORMAT_RGBA4_UNORM;
    }
    else if (_rkText == "DEPTH16") {
        return gli::FORMAT_D16_UNORM;
    }
    else if (_rkText == "DEPTH24") {
        return gli::FORMAT_D24_UNORM;
    }
    else if (_rkText == "DEPTH32") {
        return gli::FORMAT_D32_UFLOAT;
    }
    else if (_rkText == "DEPTH24_STENCIL8") {
        return gli::FORMAT_D24S8_UNORM;
    }
    else if (_rkText == "RGB_DXT1_UNORM") {
        return gli::FORMAT_RGB_DXT1_UNORM;
    }
    else if (_rkText == "FORMAT_RGBA_DXT1_UNORM") {
        return gli::FORMAT_RGBA_DXT1_UNORM;
    }
    else if (_rkText == "RGBA_DXT3_UNORM") {
        return gli::FORMAT_RGBA_DXT3_UNORM;
    }
    else if (_rkText == "RGBA_DXT5_UNORM") {
        return gli::FORMAT_RGBA_DXT5_UNORM;
    }
    else if (_rkText == "RGBA32_SFLOAT") {
        return gli::FORMAT_RGBA32_SFLOAT;
    }
    else if (_rkText == "RGBA16_SFLOAT") {
        return gli::FORMAT_RGBA16_SFLOAT;
    }
    else if (_rkText == "R32_SFLOAT") {
        return gli::FORMAT_R32_SFLOAT;
    }
    return -1;
}

struct SAssimpComponentDesc : public SComponentDesc
{
    std::shared_ptr<aiScene> m_spkScene;
};

CModelViewSpawnSystem::CModelViewSpawnSystem(const char* _pcSparkXmlFile, const char* _pcSparkXmlPath, const char* _pcVerison)
    : CDefaultSpawnSystem(_pcSparkXmlFile, _pcSparkXmlPath, _pcVerison)
{
}

CModelViewSpawnSystem::~CModelViewSpawnSystem()
{

}

void CModelViewSpawnSystem::OnElementStart (const std::string& _rkElement, const rapidxml::xml_attribute<>& _rkAttr)
{
    if (_rkElement == "Assimp") {
        AssimpElementStart(_rkAttr);
        return;
    }
    else if (_rkElement == "Text") {
        TextElementStart(_rkAttr);
        return;
    }
    else if (_rkElement == "config") {
        ConfigElementStart(_rkAttr);
        return;
    }
    CDefaultSpawnSystem::OnElementStart (_rkElement, _rkAttr);
}

void CModelViewSpawnSystem::OnElementEnd (const std::string& _rkElement)
{
    if (_rkElement == "Assimp") {
        AssimpElementEnd();
        return;
    }
    else if (_rkElement == "Text") {
        AssimpElementEnd();
        return;
    }
    else if (_rkElement == "config") {
        ConfigElementEnd();
        return;
    }
    CDefaultSpawnSystem::OnElementEnd (_rkElement);
}

void CModelViewSpawnSystem::AssimpElementStart(const rapidxml::xml_attribute<>& _rkAttr)
{
    std::map <std::string, std::string> kMap;
    CModelViewSpawnSystem::InsertAttr(_rkAttr, kMap);

    std::string kFileName;
    auto kPathDir = kMap.find("PathDir");
    if (kPathDir != kMap.end()) {
        const std::string& kPath = kPathDir->second;
        if (!kPath.empty ()) {
            kFileName = kPath;
            if ((*kPath.rbegin()) != '/' && (*kPath.rbegin ()) != '\\') {
                kFileName += "/";
            }
        }
    }
    auto kFileNameIt = kMap.find ("FileName");
    if (kFileNameIt == kMap.end ()) {
        ILogger::Log(eLL_Warning, "Assimp: FileName is not defined!");
        return;
    }
    kFileName += kFileNameIt->second;
    std::shared_ptr <aiScene> spkScene;
    {
        Assimp::Importer kImporter;
        if (!kImporter.ReadFile(kFileName.c_str(), aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals)) {
            ILogger::Log(eLL_Warning, "Assimp: File:%s is not found!", kFileName.c_str());
            assert(0);
            return;
        }
        // this pointer is reference from Importer so we make orphaned
        spkScene = std::shared_ptr <aiScene>(kImporter.GetOrphanedScene());
    }
    // We make entity per mesh
    if (!spkScene->HasMeshes ()) {
        ILogger::Log(eLL_Warning, "Assimp: There are no any Mesh exist!");
        return;
    }
    
    const size_t nMeshCount = spkScene->mNumMeshes;
    for (size_t nIndex = 0; nIndex < nMeshCount; nIndex++) {
        CDefaultSpawnDescPtr spkDesc = std::make_shared <CDefaultSpawnDesc> ();
        // PBR
        auto kPBRProperty = kMap.find("PBR");
        {
            SAssimpComponentDesc& rkCompDesc = static_cast <SAssimpComponentDesc&>(spkDesc->NewComponentDesc<SAssimpComponentDesc>());
            rkCompDesc.SetAttrTag("Geometry");
            rkCompDesc.SetProperty("MeshIndex", std::to_string(nIndex));
            rkCompDesc.SetProperty("GeomType", "Assimp");
            rkCompDesc.SetProperty("SubPath", kPathDir->second);
            rkCompDesc.m_spkScene = spkScene;
            if (kPBRProperty != kMap.end()) {
                rkCompDesc.SetProperty(kPBRProperty->first.c_str(), kPBRProperty->second.c_str());
            }
        }
        {
            IComponentDesc& rkCompDesc = spkDesc->NewComponentDesc<SAssimpComponentDesc>();
            rkCompDesc.SetAttrTag("Material");
            rkCompDesc.SetProperty("LoadMode", "File");
            {
                auto kVSFileName = kMap.find ("VertexShaderFile");
                if (kVSFileName != kMap.end()) {
                    rkCompDesc.SetProperty(kVSFileName->first.c_str(), kVSFileName->second.c_str());
                }
            }
            {
                auto kVSName = kMap.find ("VertexShaderName");
                if (kVSName != kMap.end ()) {
                    rkCompDesc.SetProperty(kVSName->first.c_str(), kVSName->second.c_str());
                }
            }
            {
                auto kPSFileName = kMap.find ("FragmentShaderFile");
                if (kPSFileName != kMap.end ()) {
                    rkCompDesc.SetProperty(kPSFileName->first.c_str(), kPSFileName->second.c_str());
                }
            }
            {
                auto kPSName = kMap.find ("FragmentShaderName");
                if (kPSName != kMap.end ()) {
                    rkCompDesc.SetProperty(kPSName->first.c_str(), kPSName->second.c_str());
                }
            }
            if (kPBRProperty != kMap.end()) {
                rkCompDesc.SetProperty("CompType", "PBR");
            }
            else {
                rkCompDesc.SetProperty("CompType", "Default");
            }
        }
        {
            IComponentDesc& rkCompDesc = spkDesc->NewComponentDesc<SAssimpComponentDesc>();
            rkCompDesc.SetAttrTag("Renderable");
            {
                auto kVisible = kMap.find ("Visible");
                if (kVisible != kMap.end ()) {
                    rkCompDesc.SetProperty(kVisible->first.c_str(), kVisible->second.c_str());
                }
            }
        }
        {
            IComponentDesc& rkCompDesc = spkDesc->NewComponentDesc<SAssimpComponentDesc>();
            rkCompDesc.SetAttrTag("Scene");
            {
                auto kPosition = kMap.find ("Position");
                if (kPosition != kMap.end ()) {
                    rkCompDesc.SetProperty(kPosition->first.c_str(), kPosition->second.c_str());
                }
            }
            {
                auto kRotate = kMap.find ("Rotate");
                if (kRotate != kMap.end ()) {
                    rkCompDesc.SetProperty(kRotate->first.c_str(), kRotate->second.c_str());
                }
            }
            {
                auto kScale = kMap.find ("Scale");
                if (kScale != kMap.end ()) {
                    rkCompDesc.SetProperty(kScale->first.c_str(), kScale->second.c_str());
                }
            }
            {
                auto kStatic = kMap.find("IsStatic");
                if (kStatic != kMap.end()) {
                    rkCompDesc.SetProperty(kStatic->first.c_str(), kStatic->second.c_str());
                }
            }
            aiMesh* pkMesh = spkScene->mMeshes[nIndex];
            if (pkMesh)
                rkCompDesc.SetProperty("Name", pkMesh->mName.C_Str());
        }
        PushSpawnDesc(spkDesc);
    }
}

void CModelViewSpawnSystem::AssimpElementEnd ()
{

}

void CModelViewSpawnSystem::ConfigElementStart(const rapidxml::xml_attribute<>& _rkAttr)
{
    std::map <std::string, std::string> kMap;
    CModelViewSpawnSystem::InsertAttr(_rkAttr, kMap);
    auto kIt = kMap.begin();
    for (; kIt != kMap.end(); kIt++) {
        if (kIt->first == "RegisterMaterialComp") {
            if (kIt->second == "PBR") {
                CGLSLMaterialSystem::RegisterComponentFactory <CPBRMaterialCompFactory>("PBR");
            }
        }
    }
}

void CModelViewSpawnSystem::ConfigElementEnd()
{

}

void CModelViewSpawnSystem::TextElementStart(const rapidxml::xml_attribute<>& _rkAttr)
{
    std::map <std::string, std::string> kMap;
    CModelViewSpawnSystem::InsertAttr(_rkAttr, kMap);
    CDefaultSpawnDescPtr spkDesc = std::make_shared <CDefaultSpawnDesc>();
    {
        IComponentDesc& rkDesc = spkDesc->NewComponentDesc<SComponentDesc>();
        rkDesc.SetAttrTag("Text");
        auto kIt = kMap.find("Font");
        if (kIt != kMap.end()) {
            rkDesc.SetProperty(kIt->first.c_str(), kIt->second.c_str());
        }
        kIt = kMap.find("String");
        if (kIt != kMap.end()) {
            rkDesc.SetProperty(kIt->first.c_str(), kIt->second.c_str());
        }
    }
    {
        IComponentDesc& rkCompDesc = spkDesc->NewComponentDesc<SComponentDesc>();
        rkCompDesc.SetAttrTag("Renderable");
        {
            auto kVisible = kMap.find("Visible");
            if (kVisible != kMap.end()) {
                rkCompDesc.SetProperty(kVisible->first.c_str(), kVisible->second.c_str());
            }
        }
    }
    {
        IComponentDesc& rkCompDesc = spkDesc->NewComponentDesc<SComponentDesc>();
        rkCompDesc.SetAttrTag("Scene");
        {
            auto kPosition = kMap.find("ScreenPos");
            if (kPosition != kMap.end()) {
                char cBuffer [256] = {};
                sprintf_s(cBuffer, sizeof(cBuffer), "%s,0.0", kPosition->second.c_str());
                rkCompDesc.SetProperty("Position", cBuffer);
                rkCompDesc.SetProperty("ScreenSpace", "true");
            }
        }
    }
    PushSpawnDesc(spkDesc);
}

void CModelViewSpawnSystem::TextElementEnd()
{
}

bool CModelViewSpawnSystem::AssignComp(entityx::EventManager& _rkEventMgr, entityx::Entity& _rkEntity, IComponentDesc& _rkDesc)
{
    if (!_rkEntity.valid())
        return false;

    if (_rkDesc.IsAttrTagMatch("Geometry")) {
        if (!_rkDesc.IsPropertyExist("GeomType"))
            return false;

        std::string kGeomType = _rkDesc.GetProperty("GeomType");
        if (kGeomType == "Assimp") {
            SAssimpComponentDesc& rkAssimp = static_cast<SAssimpComponentDesc&>(_rkDesc);
            if (!rkAssimp.m_spkScene)
                return false;
            if (!_rkDesc.IsPropertyExist("MeshIndex")) {
                return false;
            }
            size_t nIndex = std::stoi(_rkDesc.GetProperty("MeshIndex"));
            if (nIndex >= rkAssimp.m_spkScene->mNumMeshes)
                return false;
            std::string kSubPath;
            const char* pcPath = _rkDesc.GetProperty("SubPath");
            if (pcPath) {
                 kSubPath = pcPath;
                 if ((*kSubPath.rbegin ()) != '/' && (*kSubPath.rbegin ()) != '\\') {
                     kSubPath += "/";
                 }
            }
            bool bBPR = false;
            const char* pcValue = _rkDesc.GetProperty("PBR");
            if (pcValue) {
                bBPR = (!strcmp("true", pcValue));
            }
            std::shared_ptr <CAssimpGLGeometryComp> spkGeometryComp = std::make_shared<CAssimpGLGeometryComp>(rkAssimp.m_spkScene.get(), nIndex, bBPR, kSubPath.c_str());
            _rkEntity.assign <IGeometryHandler> (spkGeometryComp);
            if (_rkEntity.has_component <ISceneHandler>()) {
                ISceneComponent* pkSceneComp = _rkEntity.component <ISceneHandler>()->get();
                spkGeometryComp->UpdateWorldBound(*pkSceneComp);
            }
            return true;
        }

        std::shared_ptr <CGLMMeshBuffer> kMesh;
        ECustomModelType eType = eCMT_None;
        if (kGeomType == "Rect") {
            const char* pcRect = _rkDesc.GetProperty("Rect");
            if (pcRect) {
                std::string kRectStr = pcRect;
                std::vector <std::string> kDatas;
                SplitToken(kRectStr, ',', kDatas);
                if (kDatas.size() != 4) {
                    return false;
                }
                eType = eCMT_Rect;
                kMesh = CreateRectData(std::stof(kDatas [0]), std::stof(kDatas [1]), std::stof(kDatas [2]), std::stof(kDatas [3]));
            }
        }
        else if (kGeomType == "Sphere") {
            const char* pcSphere = _rkDesc.GetProperty("Sphere");
            if (pcSphere) {
                std::string kSphereStr = pcSphere;
                std::vector <std::string> kDatas;
                SplitToken(pcSphere, ',', kDatas);
                if (kDatas.size() != 3) {
                    return false;
                }
                eType = eCMT_Sphere;
                kMesh = CreateSphereData(std::stof(kDatas [0]), std::stoi(kDatas [1]), std::stoi(kDatas [2]));
            }
        }

        if (_rkEntity.has_component <IGeometryHandler>()) {
            _rkEntity.remove <IGeometryHandler>();
        }
        
        if (kMesh) {
            std::shared_ptr <CCustomGeometryComp> spkGeometryComp = std::make_shared <CCustomGeometryComp>(kMesh, eType);
            _rkEntity.assign <IGeometryHandler>(spkGeometryComp);
            if (_rkEntity.has_component <ISceneHandler>()) {
                ISceneComponent* pkSceneComp = _rkEntity.component <ISceneHandler>()->get();
                spkGeometryComp->UpdateWorldBound(*pkSceneComp);
            }
        }
    }
    else if (_rkDesc.IsAttrTagMatch("Scene")) {
        SComponentDesc* pkDesc = dynamic_cast <SComponentDesc*>(&_rkDesc);
        if (!pkDesc)
            return false;
        CSceneCreateContext kContext(_rkEntity, *pkDesc);
        const ISceneContext& rkIContext = kContext;
        _rkEventMgr.emit <ISceneContext>(rkIContext);
    }
    else if (_rkDesc.IsAttrTagMatch("Material")) {
        std::string kLoadMode = _rkDesc.GetProperty("LoadMode");
        if (kLoadMode == "File") {
            const char* pcCompType = _rkDesc.GetProperty("CompType");
            CGLSLMaterialCreateComponentFromFile kArgs(_rkEntity, pcCompType);
            const char* pcFileName = _rkDesc.GetProperty("VertexShaderFile");
            if (pcFileName) {
                std::string kFileName = pcFileName;
                const char* pcShaderName = _rkDesc.GetProperty("VertexShaderName");
                assert(pcShaderName);
                kArgs.SetShaderName(pcShaderName, eShader_Vertex);
                kArgs.SetShaderFile(pcFileName, eShader_Vertex);
            }
            pcFileName = _rkDesc.GetProperty("FragmentShaderFile");
            if (pcFileName) {
                std::string kFileName = pcFileName;
                const char* pcShaderName = _rkDesc.GetProperty("FragmentShaderName");
                assert(pcShaderName);
                kArgs.SetShaderName(pcShaderName, eShader_Pixel);
                kArgs.SetShaderFile(pcFileName, eShader_Pixel);
            }
            const IMaterialContextArg& rkIArg = kArgs;
            _rkEventMgr.emit <IMaterialContextArg>(rkIArg);
        }
        else if (kLoadMode == "Src") {
            const char* pcCompType = _rkDesc.GetProperty("CompType");
            CGLSLMaterialCreateComponentFromSrc kArgs(_rkEntity, pcCompType);
            const char* pcShaderSource = _rkDesc.GetProperty("VertexShaderSource");
            if (pcShaderSource) {
                const char* pcShaderName = _rkDesc.GetProperty("VertexShaderName");
                assert(pcShaderName);
                kArgs.SetShaderName(pcShaderName, eShader_Vertex);
                kArgs.SetShaderSource(pcShaderSource, eShader_Vertex);
            }
            pcShaderSource = _rkDesc.GetProperty("FragmentShaderSource");
            if (pcShaderSource) {
                const char* pcShaderName = _rkDesc.GetProperty("FragmentShaderName");
                assert(pcShaderName);
                kArgs.SetShaderName(pcShaderName, eShader_Pixel);
                kArgs.SetShaderSource(pcShaderSource, eShader_Pixel);
            }
            const IMaterialContextArg& rkIArg = kArgs;
            _rkEventMgr.emit <IMaterialContextArg>(rkIArg);
        }
    }
    else if (_rkDesc.IsAttrTagMatch("EnvMap")) {
        CGLMCubeCameraComponent* pkEnvCubeMap = nullptr;
        if (_rkEntity.has_component <ICameraHandler>()) {
            entityx::ComponentHandle <ICameraHandler> kHandler = _rkEntity.component<ICameraHandler>();
            ICameraComponent* pkIComp = kHandler->get();
            pkEnvCubeMap = dynamic_cast <CGLMCubeCameraComponent*>(pkIComp);
            if (!pkEnvCubeMap) {
                _rkEntity.remove <ICameraHandler>();
            }
        }
        const char* pcValue = _rkDesc.GetProperty("Radius");
        if (!pcValue)
            return false;
        std::string kValue = pcValue;
        int nRadius = std::stoi(kValue);
        pcValue = _rkDesc.GetProperty("Resolution");
        if (!pcValue)
            return false;
        kValue = pcValue;
        int nRes = std::stoi(pcValue);
        if (!pkEnvCubeMap) {
            std::shared_ptr <ICameraComponent> spkCam = IRenderSystem::CreateCamera(IRenderer::eCT_Cube);
            pkEnvCubeMap = dynamic_cast <CGLMCubeCameraComponent*>(spkCam.get());
            if (!pkEnvCubeMap)
                return false;
            std::shared_ptr <IFrameBuffer> spkFrameBuffer = IRenderSystem::CreateFrameBuffer(IRenderer::eFBT_Cube, nRes, nRes);
            pkEnvCubeMap->Initialize(spkFrameBuffer, nRadius);
            _rkEntity.assign<ICameraHandler>(spkCam);
        }
        pcValue = _rkDesc.GetProperty("Active");
        if (pcValue) {
            if (!strcmp(pcValue, "true")) {
                pkEnvCubeMap->ActiveCamera();
            }
            else {
                pkEnvCubeMap->DeactiveCamera();
            }
        }
        pcValue = _rkDesc.GetProperty("CheckByDynamic");
        if (pcValue) {
            pkEnvCubeMap->SetCheckByDyanmic(!strcmp(pcValue, "true"));
        }
        // Support Attachment 6
        std::map <std::string, std::pair <int, std::string>> kTextures;
        int nAttachment = 0;
        for (int nIndex = 0; nIndex < 6; nIndex++) {
            char cBuffer [64] = {};
            sprintf_s(cBuffer, sizeof(cBuffer), "Texture%d", nIndex);
            if (_rkDesc.IsPropertyExist(cBuffer)) {
                kTextures.insert(std::make_pair(cBuffer, std::make_pair(nIndex, _rkDesc.GetProperty(cBuffer))));
                nAttachment = nIndex + 1;
            }
        }
        if (_rkDesc.IsPropertyExist("DepthTexture")) {
            kTextures.insert(std::make_pair("DepthTexture", std::make_pair(0, _rkDesc.GetProperty("DepthTexture"))));
        }
        IFrameBuffer* pkFrameBuffer = pkEnvCubeMap->GetTargetFrameBuffer();
        pkFrameBuffer->SetRenderBufferColorAttactmentCount(nAttachment);
        for (auto kIt : kTextures) {
            int nFormat = GetTextureFormatByText(kIt.second.second);
            if (nFormat == -1)
                continue;
            std::shared_ptr<ITexture> spkTexture = IRenderSystem::CreateTexture(IRenderer::eTT_Cube);
            spkTexture->SetTextureDimension(nRes, nRes, 1);
            spkTexture->SetTextureFormat(nFormat);
            spkTexture->SetFilterMode(eFT_Min, eFM_Nearest);
            spkTexture->SetFilterMode(eFT_Mag, eFM_Nearest);
            spkTexture->SetClampMode(eCT_U, eCM_Clamp);
            spkTexture->SetClampMode(eCT_V, eCM_Clamp);
            if (kIt.first == "DepthTexture") {
                pkFrameBuffer->SetDepthTexture(spkTexture);
                continue;
            }
            pkFrameBuffer->SetTexture(spkTexture, kIt.second.first);
        }
        if (_rkEntity.has_component <ISceneHandler>()) {
            ISceneComponent* pkSceneIComp = _rkEntity.component <ISceneHandler>()->get();
            pkEnvCubeMap->UpdateCameraName(*pkSceneIComp);
        }
    }
    else if (_rkDesc.IsAttrTagMatch("Camera")) {
        ICameraComponent* pkComp = nullptr;
        if (_rkEntity.has_component <ICameraHandler>()) {
            entityx::ComponentHandle <ICameraHandler> kHandler = _rkEntity.component<ICameraHandler>();
            ICameraComponent* pkIComp = kHandler->get();
            pkComp = dynamic_cast <CGLMMainCameraComponent*>(pkIComp);
            if (!pkComp) {
                _rkEntity.remove <ICameraHandler>();
            }
        }
        if (!pkComp) {
            std::shared_ptr <ICameraComponent> spkComp = IRenderSystem::CreateCamera(IRenderer::eCT_Main);
            pkComp = spkComp.get();
            _rkEntity.assign<ICameraHandler>(spkComp);
        }
        const char* pcValue = _rkDesc.GetProperty("Active");
        if (pcValue) {
            if (!strcmp(pcValue, "true")) {
                pkComp->ActiveCamera();
            }
            else {
                pkComp->DeactiveCamera();
            }
        }
        pcValue = _rkDesc.GetProperty("Perspective");
        if (pcValue) {
            std::string kValue = pcValue;
            std::vector <std::string> kDatas;
            SplitToken(kValue, ',', kDatas);
            if ((kDatas.size() != 2) && (kDatas.size() != 4)) {
                return false;
            }
            float fNear = std::stof(kDatas [0]);
            float fFar = std::stof(kDatas [1]);
            int nWidth = (kDatas.size() == 4) ? std::stoi(kDatas [2]) : 0;
            int nHeight = (kDatas.size() == 4) ? std::stoi(kDatas [3]) : 0;
            pkComp->UpdatePerspectiveProjectMatrix(fNear, fFar, nWidth, nHeight);
        }
        if (_rkEntity.has_component <ISceneHandler> ()) {
            ISceneComponent* pkSceneIComp = _rkEntity.component <ISceneHandler>()->get();
            pcValue = _rkDesc.GetProperty("LookAt");
            if (pcValue) {
                std::string kValue = pcValue;
                std::vector <std::string> kDatas;
                SplitToken (kValue, ',', kDatas);
                if (kDatas.size () != 3) {
                    return false;
                }
                glm::vec3 kLookAtPos = glm::vec3 (std::stof (kDatas [0]), std::stof (kDatas [1]), std::stof (kDatas [2]));
                CGLMMainCameraComponent* pkMainCam = static_cast <CGLMMainCameraComponent*>(pkComp);
                pkMainCam->UpdateViewMatrix(*pkSceneIComp, kLookAtPos);
                
            }
            pkComp->UpdateCameraName(*pkSceneIComp);
        }
    }
    else if (_rkDesc.IsAttrTagMatch("Renderable")) {
        COpenGLRenderCreateContextArg kArgs(_rkEntity);
        const char* pcValue = _rkDesc.GetProperty("Visible");
        if (pcValue) {
            kArgs.SetProperty("Visible", pcValue);
        }
        const IRenderContextArg& rkIArg = kArgs;
        _rkEventMgr.emit <IRenderContextArg> (rkIArg);
    }
    else if (_rkDesc.IsAttrTagMatch("Control")) {
        EInputSystemContextType eType = eISCT_None;
        const char* pcValue = _rkDesc.GetProperty("Input");
        if (pcValue) {
            if (!strcmp(pcValue, "Keyboard")) {
                eType = eISCT_CreateKeyboard;
            }
            else if (!strcmp(pcValue, "Mouse")) {
                eType = eISCT_CreateMouse;
            }
            else if (!strcmp(pcValue, "KeyboardMouse")) {
                eType = eISCT_CreateKeyboardMouse;
            }
        }
        CWin32InputSystemContext kContext(_rkEntity, eType);
        const IInputSystemContext& rkIContext = kContext;
        _rkEventMgr.emit <IInputSystemContext>(rkIContext);
    }
    else if (_rkDesc.IsAttrTagMatch("Movement")) {
        const char* pcValue = _rkDesc.GetProperty("Control");
        if (!pcValue)
            return false;
        std::string kValue = pcValue;
        if (kValue == "KeyBorad") {
            CCreateKBMovementCompContext kContext(_rkEntity, 10.0f);
            const IMovementSystemContext& rkIContext = kContext;
            _rkEventMgr.emit <IMovementSystemContext>(rkIContext);
        }
        else if (kValue == "PosSequence") {
            pcValue = _rkDesc.GetProperty("AutoStart");
            bool bAutoStart = false;
            if (pcValue) {
                bAutoStart |= (!strcmp(pcValue, "true"));
            }
            pcValue = _rkDesc.GetProperty("Sequence");
            if (!pcValue)
                return false;
            
            CCreateSequenceMovementCompContext kContext(_rkEntity, pcValue, bAutoStart);
            const IMovementSystemContext& rkIContext = kContext;
            _rkEventMgr.emit <IMovementSystemContext>(rkIContext);
        }
        else {
            return false;
        }
        return true;
    }
    else if (_rkDesc.IsAttrTagMatch("Light")) {
        const char* pcLightType = _rkDesc.GetProperty("LightType");
        if (!pcLightType)
            return false;
        float nDimmer = 1.0;
        SColor kDiffuse(1.0f, 1.0f, 1.0f);
        SColor kAmbient(1.0f, 1.0f, 1.0f);
        SColor kSpecular(1.0f, 1.0f, 1.0f);
        const char* pcValue = _rkDesc.GetProperty("Dimmer");
        if (pcValue) {
            nDimmer = std::stof(pcValue);
        }
        pcValue = _rkDesc.GetProperty("Diffuse");
        if (pcValue) {
            std::string kValue = pcValue;
            std::vector <std::string> kColorText;
            SplitToken(kValue, ',', kColorText);
            if (kColorText.size() != 3)
                return false;
            kDiffuse.m_fR = std::stof(kColorText [0]);
            kDiffuse.m_fG = std::stof(kColorText [1]);
            kDiffuse.m_fB = std::stof(kColorText [2]);
        }
        pcValue = _rkDesc.GetProperty("Ambient");
        if (pcValue) {
            std::string kValue = pcValue;
            std::vector <std::string> kColorText;
            SplitToken(kValue, ',', kColorText);
            if (kColorText.size() != 3)
                return false;
            kAmbient.m_fR = std::stof(kColorText [0]);
            kAmbient.m_fG = std::stof(kColorText [1]);
            kAmbient.m_fB = std::stof(kColorText [2]);
        }
        pcValue = _rkDesc.GetProperty("Specular");
        if (pcValue) {
            std::string kValue = pcValue;
            std::vector <std::string> kColorText;
            SplitToken(kValue, ',', kColorText);
            if (kColorText.size() != 3)
                return false;
            kSpecular.m_fR = std::stof(kColorText [0]);
            kSpecular.m_fG = std::stof(kColorText [1]);
            kSpecular.m_fB = std::stof(kColorText [2]);
        }

        std::shared_ptr <ILightComponent> spkIComp;
        if (!strcmp(pcLightType, "Point")) {
            if (_rkEntity.has_component <ILightHandler>()) {
                _rkEntity.remove <ILightHandler>();
            }

            CPointLightComponent* pkLight = new CPointLightComponent();
            pcValue = _rkDesc.GetProperty("Radius");
            if (pcValue) {
                float nRadius = std::stof(pcValue);
                pkLight->SetRadius(nRadius);
            }
            pcValue = _rkDesc.GetProperty("Body");
            if (pcValue) {
                float nBodyRadius = std::stof(pcValue);
                pkLight->SetLightBody((nBodyRadius > 0));
                pkLight->SetLightBodyRadius(nBodyRadius);
            }
            spkIComp = std::shared_ptr <ILightComponent>(pkLight);
        }

        spkIComp->SetLightDimmer(nDimmer);
        spkIComp->SetDiffuseColor(kDiffuse);
        spkIComp->SetAmbientColor(kAmbient);
        spkIComp->SetSpecularColor(kSpecular);
        _rkEntity.assign<ILightHandler>(spkIComp);
    }
    else if (_rkDesc.IsAttrTagMatch("Text")) {
        const char* pcValue = _rkDesc.GetProperty("Font");
        if (!pcValue)
            return false;
        const char* pcString = _rkDesc.GetProperty("String");
        pcString = (pcString) ? pcString : "";
        CCreateTextCompContext kArg(_rkEntity, pcValue, pcString);
        const ITextContext& kIArg = kArg;
        _rkEventMgr.emit <ITextContext>(kIArg);
    }
    return true;
}