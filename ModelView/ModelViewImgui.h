#pragma once

#include "ImguiSystem.h"
#include "IInputControlComponent.h"

class ECS_MODELVIEW_API CModelViewImgui
    : public CImgui
    , public IInputEventListener
{
    IRenderSystem* m_pkRenderSystem;
    void* m_hWnd;
    float TitleBarOffsetX;
	float TitleBarOffsetY;
    float m_nX;
    float m_nY;
    bool m_bDown;
    bool m_bLeft;
	bool m_bRight;
    bool m_bMoving;
public:
    CModelViewImgui();

    virtual ~CModelViewImgui();

    virtual void ConfigIO(ImGuiIO& _rkIO);

    virtual void ConfigWndHandle(void* _hWnd);

    virtual void ConfigRenderer(ImGuiIO& _rkIO, class IRenderSystem& _rkRenderSystem);

    virtual void UpdateNewFrame(ImGuiIO& _rkIO, float _nDelta);

    virtual void UpdateMouseCursor(ImGuiIO& _rkIO);

    virtual void UpdateMousePos(ImGuiIO& _rkIO);

    virtual void ShutDown();

    virtual void RenderNewFrame();

    virtual void RenderDrawData(ImGuiIO& _rkIO, ImDrawData* _pkDrawData);

    virtual void InvalidateDeviceObjects();

    virtual bool CreateDeviceObjects(ImGuiIO& _rkIO);

    virtual void DrawImgui();

    virtual void receive(const IInputEvent& _rkEvent);
};