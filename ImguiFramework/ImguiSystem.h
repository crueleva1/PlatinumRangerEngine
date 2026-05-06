#pragma once


class IMGUI_SYSTEM_API IImgui
{
public:
    virtual ~IImgui()
    {
    }

    virtual short GetSubPipelinePriority() = 0;

    virtual void ConfigIO(ImGuiIO& _rkIO) = 0;

    virtual void ConfigWndHandle(void* _hWnd) = 0;

    virtual void ConfigRenderer(ImGuiIO& _rkIO, class IRenderSystem& _rkRenderSystem) = 0;

    virtual void UpdateNewFrame(ImGuiIO& _rkIO, float _nDelta) = 0;

    virtual void UpdateMouseCursor(ImGuiIO& _rkIO) = 0;

    virtual void UpdateMousePos(ImGuiIO& _rkIO) = 0;

    virtual void ShutDown() = 0;

    virtual void RenderNewFrame() = 0;

    virtual void RenderDrawData(ImGuiIO& _rkIO, ImDrawData* _pkDrawData) = 0;

    virtual void InvalidateDeviceObjects() = 0;

    virtual bool CreateDeviceObjects(ImGuiIO& _rkIO) = 0;

    virtual void DrawImgui() = 0;
};

class IMGUI_SYSTEM_API CImgui : public IImgui
{
    std::shared_ptr <class CImguiRenderPipeline> m_spkPipeline;
protected:
    inline CImguiRenderPipeline* GetRenderPipeline()
    {
        return m_spkPipeline.get();
    }
public:
    CImgui();

    virtual ~CImgui();
    // Default Priority
    virtual short GetSubPipelinePriority()
    {
        return 20000;
    }

    virtual void ConfigRenderer(ImGuiIO& _rkIO, class IRenderSystem& _rkRenderSystem);

    virtual void ConfigIO(ImGuiIO& _rkIO);

    virtual void RenderNewFrame();

    virtual void UpdateNewFrame(ImGuiIO& _rkIO, float _nDelta);
};

class IMGUI_SYSTEM_API CImguiSystem
    :public entityx::System <CImguiSystem>
{
    ImGuiMouseCursor m_eMouseCursorType;
    IImgui* m_pkImgui;
    void* m_hWnd;
public:
    CImguiSystem(IImgui* _pkImgui, void* _hWnd);
    
    virtual ~CImguiSystem();

    virtual void configure(entityx::EntityManager& _rkEntities, entityx::EventManager& _rkEvents);

    virtual void update(entityx::EntityManager& _rkEntityMgr, entityx::EventManager& _rkEventMgr, entityx::TimeDelta _nDelta);

    void ConfigIO(ImGuiIO& _rkIO);

    void ConfigWndHandle(void* _hWnd);

    void ConfigRenderer(class IRenderSystem& _rkRenderSystem);
};

typedef void (*fnCreateImguiSystem)(entityx::SystemManager& _rkSystemMgr, IImgui* _pkImgui, class IRenderSystem& _rkRenderSystem, void* _hWnd);
