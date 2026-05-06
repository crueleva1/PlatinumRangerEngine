#include "ImguiSystemPCH.h"
#include "ImguiSystem.h"
#include "RenderSystem.h"
#include "ImguiRenderPipeline.h"

extern "C"
{
    IMGUI_SYSTEM_API void CreateImguiSystem(entityx::SystemManager& _rkSystemMgr, IImgui* _pkImgui, void* _hWnd)
    {
        _rkSystemMgr.add<CImguiSystem>(_pkImgui, _hWnd);
    }
}

CImgui::CImgui()
{

}

CImgui::~CImgui()
{

}

void CImgui::ConfigRenderer(ImGuiIO& _rkIO, IRenderSystem& _rkRenderSystem)
{
    if (m_spkPipeline)
        return;

    m_spkPipeline = std::make_shared<class CImguiRenderPipeline>();
    short nPriority = GetSubPipelinePriority();
    _rkRenderSystem.SetSubRenderPipeline(nPriority, m_spkPipeline.get());
}

void CImgui::ConfigIO(ImGuiIO& _rkIO)
{
    if (m_spkPipeline == nullptr)
        return;

    m_spkPipeline->CreateFontTexture(_rkIO);
}

void CImgui::RenderNewFrame()
{
    
}

void CImgui::UpdateNewFrame(ImGuiIO& _rkIO, float _nDelta)
{
    _rkIO.DeltaTime = _nDelta;
}

CImguiSystem::CImguiSystem(IImgui* _pkImgui, void* _hWnd)
    :m_pkImgui(_pkImgui)
    ,m_eMouseCursorType(ImGuiMouseCursor_COUNT)
    ,m_hWnd(_hWnd)
{

}

CImguiSystem::~CImguiSystem()
{

}

void CImguiSystem::configure(entityx::EntityManager& _rkEntities, entityx::EventManager& _rkEvents)
{
    entityx::System<CImguiSystem>::configure(_rkEntities, _rkEvents);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& rkIO = ImGui::GetIO();
    ConfigWndHandle(m_hWnd);
    ConfigIO(rkIO);
}

void CImguiSystem::update(entityx::EntityManager& _rkEntityMgr, entityx::EventManager& _rkEventMgr, entityx::TimeDelta _nDelta)
{
    ImGuiIO& rkIO = ImGui::GetIO();
    rkIO.DeltaTime = _nDelta;

    if (!m_pkImgui)
        return;

    m_pkImgui->RenderNewFrame();
    m_pkImgui->UpdateNewFrame(rkIO, _nDelta);
    ImGui::NewFrame();

    m_pkImgui->UpdateMousePos(rkIO);
    // Update OS mouse cursor with the cursor requested by imgui
    ImGuiMouseCursor eMouseCursor = rkIO.MouseDrawCursor ? ImGuiMouseCursor_None : ImGui::GetMouseCursor();
    if (m_eMouseCursorType != eMouseCursor) {
        m_eMouseCursorType = eMouseCursor;
        m_pkImgui->UpdateMouseCursor(rkIO);
    }

    m_pkImgui->DrawImgui();

    ImGui::EndFrame();
    ImGui::Render();
    m_pkImgui->RenderDrawData(rkIO, ImGui::GetDrawData());
}

void CImguiSystem::ConfigIO(ImGuiIO& _rkIO)
{
    if (!m_pkImgui)
        return;
    m_pkImgui->ConfigIO(_rkIO);
}

void CImguiSystem::ConfigWndHandle(void* _hWnd)
{
    if (!_hWnd)
        return;

    if (!m_pkImgui)
        return;

    m_pkImgui->ConfigWndHandle(_hWnd);
}

void CImguiSystem::ConfigRenderer(class IRenderSystem& _rkRenderSystem)
{
    if (!m_pkImgui)
        return;
    
	ImGuiIO& rkIO = ImGui::GetIO();
    m_pkImgui->ConfigRenderer(rkIO, _rkRenderSystem);
}