#include "ECSVulkanPCH.h"
#include "VulkanRenderSystem.h"
#include "VulkanFrameBuffer.h"

std::shared_ptr <CVulkanSampler> CVulkanRenderer::ms_spkGlobalSampler = nullptr;

CVulkanRenderer::CVulkanRenderer(HWND _hWnd, int _nScreenWidth, int _nScreenHeight, float _fScreenDepth, float _fScreenNear, bool _bVsync, bool _bValidationLayer)
    : IRenderer(1, 0)
    , m_spkLibImporter(std::make_unique <CVulkanImporter>())
    , m_hDeviceContext(0)
    , m_hRenderingContext(0)
    , m_hWND(_hWnd)
    , m_nClearMask(0)
    , m_nScreenWidth(_nScreenWidth)
    , m_nScreenHeight(_nScreenHeight)
    , m_fScreenDepth(_fScreenDepth)
    , m_fScreenNear(_fScreenNear)
    , m_fDegree(70.0f)
    , m_bVsync(_bVsync)
    , m_bValidationLayer(_bValidationLayer)
{
    if (ms_spkGlobalSampler == nullptr)
        ms_spkGlobalSampler = std::make_shared<CVulkanSampler>(*this);
}

CVulkanRenderer::~CVulkanRenderer()
{
    while (m_kReleaseQueue.empty() == false) {
        std::shared_ptr <CVulkanCmdBufferResource> spkResource = m_kReleaseQueue.front();
        if (spkResource->IsCmdbufferAllResolved()) {
            m_kReleaseQueue.pop();
            spkResource->OnRelease(*this);
        }
    }
}

void CVulkanRenderer::SetViewPort(int _nTop, int _nBottom, int _nLeft, int _nRight)
{

}

void CVulkanRenderer::SetClearBufferBit(int _nMask)
{
}

void CVulkanRenderer::ClearBuffer()
{
}

void CVulkanRenderer::SetClearColor(float _fR, float _fG, float _fB, float _fA)
{

}

void CVulkanRenderer::OnSwapBuffers()
{
    if (m_kReleaseQueue.empty() == false) {
        std::shared_ptr <CVulkanCmdBufferResource> spkResource = m_kReleaseQueue.front();
        if (spkResource->IsCmdbufferAllResolved()) {
            m_kReleaseQueue.pop();
            spkResource->OnRelease(*this);
        }
    }
}

void CVulkanRenderer::BuildPerspectiveProjectMatrix(float* _afMatrix [16], float _fNear, float _fFar, int _nWidth, int _nHeight, float _fDegree)
{
}

void CVulkanRenderer::BuildOrthProjectMatrix(float* _afMatrix [16], float _fTop, float _fBottom, float _fRight, float _fLeft, float _fNear, float _fFar)
{
}

void CVulkanRenderer::BuildViewMatrix(float* _afMatrix [16], float _afLoc [3], float _afDir [3], float _afUp [3])
{

}

void CVulkanRenderer::SetFOV(float _fDegree)
{
}

std::shared_ptr <IRenderState> CVulkanRenderer::OnCreateDefaultRenderState()
{
    return nullptr;
}

std::shared_ptr <IFrameBuffer> CVulkanRenderer::OnCreateFrameBuffer(int _nType, int _nWidth, int _nHeight)
{
    return nullptr;
}

std::shared_ptr <ITexture> CVulkanRenderer::OnCreateTexture(int _nTextureType)
{
    return nullptr;
}

std::shared_ptr <ICameraComponent> CVulkanRenderer::OnCreateCamera(ECameraType _eType)
{
    return nullptr;
}

void CVulkanRenderer::receive(const IRenderContextArg&)
{
}

void CVulkanRenderer::ReleaseResource(std::shared_ptr <CVulkanCmdBufferResource> spkResource)
{
    m_kReleaseQueue.push(spkResource);
}

CVulkanCommandBuffer& CVulkanRenderer::GetActiveCmdBuffer(ECommandBufferType _eCmdBufferType)
{
    CVulkanCommandBuffer* pkDummy = nullptr;
    return *pkDummy;
}


CVulkanRenderSystem::CVulkanRenderSystem(HWND _hWnd, int _nScreenWidth, int _nScreenHeight)
    :m_pkEntityMgr(nullptr)
    ,m_pkEventMgr(nullptr)
    ,m_hWND(_hWnd)
{

}

CVulkanRenderSystem::~CVulkanRenderSystem()
{

}

void CVulkanRenderSystem::configure(entityx::EntityManager& _rkEntityMgr, entityx::EventManager& _rkEventMgr)
{
    
}

void CVulkanRenderSystem::OnDraw(entityx::EventManager& _rkEventMgr, IRenderHandler& _rkRenderComp)
{

}

bool CVulkanRenderSystem::PreparePipeline(IRenderHandler& _rkRenderComp, IMaterialHandler& _rkMaterial, IGeometryHandler& _rkGeometry)
{
    return true;
}

void CVulkanRenderSystem::BeginFrame()
{

}

void CVulkanRenderSystem::EndFrame()
{

}

