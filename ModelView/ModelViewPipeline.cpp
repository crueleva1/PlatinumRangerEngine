#include "MainPCH.h"
#include "ModelViewPipeline.h"
#include "MeshData.h"
#include "GLSLMaterialSystem.h"
#include "ModelViewTextSystem.h"
#include "ModelViewSceneSystem.h"
#include "OpenGLRenderSystem.h"

bool CModelViewPipeline::ms_bDrawBinTreeBox = false;
bool CModelViewPipeline::ms_bDrawObjectBound = false;
std::shared_ptr <IMeshBuffer> CModelViewPipeline::ms_spkBoxMesh;
std::shared_ptr <IBuffer> CModelViewPipeline::ms_spkBoxVB;
std::shared_ptr <IBuffer> CModelViewPipeline::ms_spkBoxIB;
std::shared_ptr <IRenderState> CModelViewPipeline::ms_spkBoxRenderState;

CModelViewPipeline::CModelViewPipeline(entityx::EventManager& _rkEventMgr, int _nWidth, int _nHeight)
    :CGLMDefaultPipeline(_nWidth, _nHeight)
    ,m_rkEventMgr(_rkEventMgr)
    ,m_pkBasicMaterial(nullptr)
    ,m_kTemplateRenderComp(IRenderSystem::CreateDefaultRenderCompoenet())
{

}

CModelViewPipeline::~CModelViewPipeline()
{

}

void CModelViewPipeline::Prepared(int _nWidth, int _nHeight, entityx::EntityManager& _rkEntityMgr, entityx::EventManager& _rkEventMgr)
{
    CGLMDefaultPipeline::Prepared(_nWidth, _nHeight, _rkEntityMgr, _rkEventMgr);
    if (!ms_spkBoxRenderState) {
        ms_spkBoxRenderState = IRenderSystem::CreateDefaultRenderState();
    }

    if (!ms_spkBoxMesh) {
        glm::vec3 kHalf(1.0f, 1.0f, 1.0f);
        std::shared_ptr <IMeshBuffer> spkBoxMesh = CreateLineBoxdata(kHalf);
        ms_spkBoxMesh = spkBoxMesh;
        ms_spkBoxVB = spkBoxMesh->CreateVertexBuffer();
        ms_spkBoxIB = spkBoxMesh->CreateIndexBuffer();
    }

    CGLSLMaterialCreateInstanceFromFile kArgs;
    std::string kFileName = "data/BasicShader.vs";
    kArgs.SetShaderName("BasicVertexShader", eShader_Vertex);
    kArgs.SetShaderFile(kFileName.c_str(), eShader_Vertex);
    kFileName = "data/BasicShader.fs";
    kArgs.SetShaderName("BasicPixelShader", eShader_Pixel);
    kArgs.SetShaderFile(kFileName.c_str(), eShader_Pixel);
    SetCreateMaterialEvent(kArgs);
    m_pkBasicMaterial = kArgs.GetMaterialInstance();

    ms_spkBoxRenderState->SetPolygonMode(ePM_Line);
    ms_spkBoxRenderState->SetDepthOpMask(false);
    ms_spkBoxRenderState->SetDepthTestEnable(true);

    m_kTextEntity = _rkEntityMgr.create();
    {
        CCreateTextCompContext kTextContext (m_kTextEntity, "monaco-20", "FPS:");
        const ITextContext& rkIContext = kTextContext;
        _rkEventMgr.emit <ITextContext> (rkIContext);
    }
    {
        CSceneCreateContext kContext (m_kTextEntity);
        kContext.SetProperty("Position", "0.0,0.0,0.0");
        kContext.SetProperty("ScreenSpace", "true");
        const ISceneContext& rkIContext = kContext;
        _rkEventMgr.emit <ISceneContext> (rkIContext);
    }
    {
        COpenGLRenderCreateContextArg kArgs (m_kTextEntity);
        kArgs.SetProperty ("Visible", "true");
        const IRenderContextArg& rkIArg = kArgs;
        _rkEventMgr.emit <IRenderContextArg> (rkIArg);
    }
}

void CModelViewPipeline::Active(entityx::EventManager& _rkEventMgr)
{
    CGLMDefaultPipeline::Active(_rkEventMgr);
    if (!m_kTextEntity)
        return;
    if (!m_kTextEntity.has_component <IRenderHandler>())
        return;

    IRenderHandler& rkRenderHandler = (*m_kTextEntity.component <IRenderHandler>().get());
    rkRenderHandler->SetRenderable(true);
}

void CModelViewPipeline::Deactive(entityx::EventManager& _rkEventMgr)
{
    CGLMDefaultPipeline::Deactive(_rkEventMgr);
    if (!m_kTextEntity)
        return;
    if (!m_kTextEntity.has_component <IRenderHandler>())
        return;

    IRenderHandler& rkRenderHandler = (*m_kTextEntity.component <IRenderHandler>().get());
    rkRenderHandler->SetRenderable(false);
}

void CModelViewPipeline::SetScreenText()
{
    AppendSceenText("DrawBinTreeBound: %s\n", (ms_bDrawBinTreeBox) ? "ON" : "OFF");
    AppendSceenText("DrawObjectBound: %s\n", (ms_bDrawObjectBound) ? "ON" : "OFF");
}

void CModelViewPipeline::SetCreateMaterialEvent(const IMaterialContextArg& _rkArg)
{
    m_rkEventMgr.emit <IMaterialContextArg>(_rkArg);
}

void CModelViewPipeline::AddRenderBoundBoxList(const glm::vec3& _rkCenter, const glm::vec3& _rkHalf, const glm::vec4& _rkColor)
{
    m_kRenderBoxIndex.push_back(m_kBoundBoxPool.Allocate<CModelViewPipeline::SBoundBox>(_rkCenter, _rkHalf, _rkColor));
}

void CModelViewPipeline::AppendSceenText(const std::string& _rkText)
{
    m_kScreenText += _rkText;
}

void CModelViewPipeline::AppendSceenText(const char* _pcText, ...)
{
    char cBuffer [2048] = {};
    va_list kArgPtr;
    va_start(kArgPtr, _pcText);
    vsprintf_s(cBuffer, _pcText, kArgPtr);
    va_end(kArgPtr);
    m_kScreenText += cBuffer;
}

void CModelViewPipeline::RenderBox(IRenderer& _rkRenderer, const glm::vec3& _rkCenter, const glm::vec3& _rkHalf, const glm::vec4& _rkColor, ICameraComponent* _pkCamera)
{
    if (!m_kTemplateRenderComp->SetMaterial(m_pkBasicMaterial))
        return;
    if (!m_kTemplateRenderComp->SetAttribute(ms_spkBoxVB.get(), ms_spkBoxIB.get()))
        return;
    if (!m_pkBasicMaterial->ApplyCameraData(_pkCamera))
        return;

    glm::mat4 kWorld;
    for (char nAxis = 0; nAxis < 3; nAxis++) {
        kWorld [nAxis] [nAxis] = _rkHalf [nAxis];
    }
    kWorld [3] = glm::vec4(_rkCenter, 1.0f);
    AssignConstant <glm::mat4>(m_kTemplateRenderComp, "uWorldMatrix", kWorld, eConstant_Matrix4x4);
    AssignConstant <glm::vec4>(m_kTemplateRenderComp, "uColor", _rkColor, eConstant_floatVec4);
    AssignConstant <bool>(m_kTemplateRenderComp, "uCustomColor", true, eConstant_bool);
    
    ms_spkBoxRenderState->SetPrimitiveTopology(ePrimitive_Lines);
    m_kTemplateRenderComp->SetRenderState(ms_spkBoxRenderState.get());
    if (!m_kTemplateRenderComp->ValidatePipeline(_rkRenderer))
        return;
    _rkRenderer.OnDraw(m_rkEventMgr, m_kTemplateRenderComp);
}

void CModelViewPipeline::SetPassObject(entityx::EntityManager& _rkMgr)
{
    SetScreenText();
    m_nFrameRevision++;
    if (!ms_bDrawBinTreeBox && !ms_bDrawObjectBound)
        return;
    m_kRenderBoxIndex.clear();
    m_kBoundBoxPool.RecycleAll();
}

void CModelViewPipeline::RenderScreen(IRenderer& _rkRenderer, entityx::EventManager& _rkEventMgr, ICameraComponent* _pkCamera, const glm::mat4& _rkOrtha)
{
    if (ms_bDrawBinTreeBox || ms_bDrawObjectBound) {
        if (!m_kRenderBoxIndex.empty()) {
            for (auto nIndex : m_kRenderBoxIndex) {
                if (!m_kBoundBoxPool.IsAllocated(nIndex))
                    continue;
                std::shared_ptr <SBoundBox> spkData = m_kBoundBoxPool.GetData(nIndex);
                if (!spkData)
                    continue;
                RenderBox(_rkRenderer, spkData->m_kCenter, spkData->m_kHalf, spkData->m_kColor, _pkCamera);
            }
        }
    }
    if (m_kTextEntity) {
        if (m_kTextEntity.has_component <ITextHandler>()) {
            ITextHandler& rkTextHandler = (*m_kTextEntity.component <ITextHandler>().get());
            rkTextHandler->SetText(m_kScreenText.c_str());
        }
    }
    CGLMDefaultPipeline::RenderScreen(_rkRenderer, _rkEventMgr, _pkCamera, _rkOrtha);
    m_kScreenText.clear();
}