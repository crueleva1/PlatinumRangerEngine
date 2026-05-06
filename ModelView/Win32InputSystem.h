#pragma once

#include "InputSystem.h"


enum EInputEventType
{
    eIET_App,
    eIET_Mouse,
    eIET_Keyboard,
};

class ECS_MODELVIEW_API CAppInputEvent : public IInputEvent
{
public:
    enum
    {
        eAIE_Quit,
        eAIE_ResizeWindow,
        eAIE_Max,
    };
protected:
    LPARAM m_lParam;
    WPARAM m_wParam;
    int m_eEventType;
public:
    CAppInputEvent(int _eEventType)
        : m_eEventType(_eEventType)
        , m_lParam(0)
        , m_wParam(0)
    {

    }

    CAppInputEvent(int _eEventType, WPARAM _wParam, LPARAM _lParam)
        : m_eEventType(_eEventType)
        , m_lParam(_lParam)
        , m_wParam(_wParam)
    {

    }

    virtual ~CAppInputEvent()
    {

    }

    virtual int GetInputType() const
    {
        return eIET_App;
    }

    int GetEventType() const
    {
        return m_eEventType;
    }

    LPARAM GetLParam() const
    {
        return m_lParam;
    }

    WPARAM GetWParam() const
    {
        return m_wParam;
    }
};

class ECS_MODELVIEW_API CKeyboardEvent : public IInputEvent
{
public:
    enum EKeyBoardCode
    {
        eKBC_Unknown         = 0x00,
        eKBC_Escape          = 0x01,
        eKBC_One             = 0x02,
        eKBC_Two             = 0x03,
        eKBC_Three           = 0x04,
        eKBC_Four            = 0x05,
        eKBC_Five            = 0x06,
        eKBC_Six             = 0x07,
        eKBC_Seven           = 0x08,
        eKBC_Eight           = 0x09,
        eKBC_Nine            = 0x0A,
        eKBC_Zero            = 0x0B,
        eKBC_Minus           = 0x0C,    /* - on main keyboard */
        eKBC_Equals          = 0x0D,
        eKBC_Backspace       = 0x0E,    /* backspace */
        eKBC_Tab             = 0x0F,
        eKBC_Q               = 0x10,
        eKBC_W               = 0x11,
        eKBC_E               = 0x12,
        eKBC_R               = 0x13,
        eKBC_T               = 0x14,
        eKBC_Y               = 0x15,
        eKBC_U               = 0x16,
        eKBC_I               = 0x17,
        eKBC_O               = 0x18,
        eKBC_P               = 0x19,
        eKBC_LeftBracket     = 0x1A,
        eKBC_RightBracket    = 0x1B,
        eKBC_Return          = 0x1C,    /* Enter on main keyboard */
        eKBC_LeftControl     = 0x1D,
        eKBC_A               = 0x1E,
        eKBC_S               = 0x1F,
        eKBC_D               = 0x20,
        eKBC_F               = 0x21,
        eKBC_G               = 0x22,
        eKBC_H               = 0x23,
        eKBC_J               = 0x24,
        eKBC_K               = 0x25,
        eKBC_L               = 0x26,
        eKBC_Semicolon       = 0x27,
        eKBC_Apostrophe      = 0x28,
        eKBC_Grave           = 0x29,    /* accent grave */
        eKBC_LeftShift       = 0x2A,
        eKBC_Backslash       = 0x2B,
        eKBC_Z               = 0x2C,
        eKBC_X               = 0x2D,
        eKBC_C               = 0x2E,
        eKBC_V               = 0x2F,
        eKBC_B               = 0x30,
        eKBC_N               = 0x31,
        eKBC_M               = 0x32,
        eKBC_Comma           = 0x33,
        eKBC_Period          = 0x34,    /* . on main keyboard */
        eKBC_Slash           = 0x35,    /* '/' on main keyboard */
        eKBC_RightShift      = 0x36,
        eKBC_Multiply        = 0x37,    /* * on numeric keypad */
        eKBC_LeftAlt         = 0x38,    /* left Alt */
        eKBC_Space           = 0x39,
        eKBC_Capital         = 0x3A,
        eKBC_F1              = 0x3B,
        eKBC_F2              = 0x3C,
        eKBC_F3              = 0x3D,
        eKBC_F4              = 0x3E,
        eKBC_F5              = 0x3F,
        eKBC_F6              = 0x40,
        eKBC_F7              = 0x41,
        eKBC_F8              = 0x42,
        eKBC_F9              = 0x43,
        eKBC_F10             = 0x44,
        eKBC_NumLock         = 0x45,
        eKBC_ScrollLock      = 0x46,    /* Scroll Lock */
        eKBC_Numpad7         = 0x47,
        eKBC_Numpad8         = 0x48,
        eKBC_Numpad9         = 0x49,
        eKBC_Subtract        = 0x4A,    /* - on numeric keypad */
        eKBC_Numpad4         = 0x4B,
        eKBC_Numpad5         = 0x4C,
        eKBC_Numpad6         = 0x4D,
        eKBC_Add             = 0x4E,    /* + on numeric keypad */
        eKBC_Numpad1         = 0x4F,
        eKBC_Numpad2         = 0x50,
        eKBC_Numpad3         = 0x51,
        eKBC_Numpad0         = 0x52,
        eKBC_Decimal         = 0x53,    /* . on numeric keypad */
        eKBC_OEM_102         = 0x56,    /* < > | on UK/Germany keyboards */
        eKBC_F11             = 0x57,
        eKBC_F12             = 0x58,
        eKBC_F13             = 0x64,    /*                     (NEC PC98) */
        eKBC_F14             = 0x65,    /*                     (NEC PC98) */
        eKBC_F15             = 0x66,    /*                     (NEC PC98) */
        eKBC_Kana            = 0x70,    /* (Japanese keyboard)            */
        eKBC_ABNT_C1         = 0x73,    /* / ? on Portugese (Brazilian) keyboards */
        eKBC_Convert         = 0x79,    /* (Japanese keyboard)            */
        eKBC_NoConvert       = 0x7B,    /* (Japanese keyboard)            */
        eKBC_Yen             = 0x7D,    /* (Japanese keyboard)            */
        eKBC_ABNT_C2         = 0x7E,    /* Numpad . on Portugese (Brazilian) keyboards */
        eKBC_NumpadEquals    = 0x8D,    /* = on numeric keypad (NEC PC98) */
        eKBC_PrevTrack       = 0x90,    /* Previous Track (KC_CIRCUMFLEX on Japanese keyboard) */
        eKBC_At              = 0x91,    /*                     (NEC PC98) */
        eKBC_Colon           = 0x92,    /*                     (NEC PC98) */
        eKBC_Underline       = 0x93,    /*                     (NEC PC98) */
        eKBC_Kanji           = 0x94,    /* (Japanese keyboard)            */
        eKBC_Stop            = 0x95,    /*                     (NEC PC98) */
        eKBC_AX              = 0x96,    /*                     (Japan AX) */
        eKBC_Unlabeled       = 0x97,    /*                        (J3100) */
        eKBC_NextTrack       = 0x99,    /* Next Track */
        eKBC_NumpadEnter     = 0x9C,    /* Enter on numeric keypad */
        eKBC_RightControl    = 0x9D,
        eKBC_Mute            = 0xA0,    /* Mute */
        eKBC_Calculator      = 0xA1,    /* Calculator */
        eKBC_PlayPause       = 0xA2,    /* Play / Pause */
        eKBC_MediaStop       = 0xA4,    /* Media Stop */
        eKBC_VolumeDown      = 0xAE,    /* Volume - */
        eKBC_VolumeUp        = 0xB0,    /* Volume + */
        eKBC_WebHome         = 0xB2,    /* Web home */
        eKBC_NumpadComma     = 0xB3,    /* , on numeric keypad (NEC PC98) */
        eKBC_Divide          = 0xB5,    /* / on numeric keypad */
        eKBC_SysRq           = 0xB7,
        eKBC_RightAlt        = 0xB8,    /* right Alt */
        eKBC_Pause           = 0xC5,    /* Pause */
        eKBC_Home            = 0xC7,    /* Home on arrow keypad */
        eKBC_ArrowUp         = 0xC8,    /* UpArrow on arrow keypad */
        eKBC_PageUp          = 0xC9,    /* PgUp on arrow keypad */
        eKBC_ArrowLeft       = 0xCB,    /* LeftArrow on arrow keypad */
        eKBC_ArrowRight      = 0xCD,    /* RightArrow on arrow keypad */
        eKBC_End             = 0xCF,    /* End on arrow keypad */
        eKBC_ArrowDown       = 0xD0,    /* DownArrow on arrow keypad */
        eKBC_PageDown        = 0xD1,    /* PgDn on arrow keypad */
        eKBC_Insert          = 0xD2,    /* Insert on arrow keypad */
        eKBC_Delete          = 0xD3,    /* Delete on arrow keypad */
        eKBC_LeftWindows     = 0xDB,    /* Left Windows key */
        eKBC_RightWindows    = 0xDC,    /* Right Windows key - Correct spelling :) */
        eKBC_AppMenu         = 0xDD,    /* AppMenu key */
        eKBC_Power           = 0xDE,    /* System Power */
        eKBC_Sleep           = 0xDF,    /* System Sleep */
        eKBC_Wake            = 0xE3,    /* System Wake */
        eKBC_WebSearch       = 0xE5,    /* Web Search */
        eKBC_WebFavorites    = 0xE6,    /* Web Favorites */
        eKBC_WebRefresh      = 0xE7,    /* Web Refresh */
        eKBC_WebStop         = 0xE8,    /* Web Stop */
        eKBC_WebForward      = 0xE9,    /* Web Forward */
        eKBC_WebBack         = 0xEA,    /* Web Back */
        eKBC_MyComputer      = 0xEB,    /* My Computer */
        eKBC_Mail            = 0xEC,    /* Mail */
        eKBC_MediaSelect     = 0xED     /* Media Select */
    };
protected:
    int m_eKeyCode;
    bool m_bDown;
public:
    CKeyboardEvent(int _eKeyBoardCode, bool _bDown)
        : m_eKeyCode(_eKeyBoardCode)
        , m_bDown(_bDown)
    {

    }

    virtual ~CKeyboardEvent()
    {

    }

    int GetInputType() const
    {
        return eIET_Keyboard;
    }

    inline int GetKey() const
    {
        return m_eKeyCode;
    }

    inline bool IsDown() const
    {
        return m_bDown;
    }
};

class ECS_MODELVIEW_API CMouseEvent : public IInputEvent
{
    float m_nX;
    float m_nY;
    bool m_bDown;
    bool m_bMoving;
    bool m_bLeft;
public:
    CMouseEvent(float _nX, float _nY, bool _bLeft, bool _bDown, bool _bMoving)
        : m_nX(_nX)
        , m_nY(_nY)
        , m_bLeft(_bLeft)
        , m_bDown(_bDown)
        , m_bMoving(_bMoving)
    {}

    virtual ~CMouseEvent()
    {}

    int GetInputType() const
    {
        return eIET_Mouse;
    }

    inline float GetX() const
    {
        return m_nX;
    }

    inline float GetY() const
    {
        return m_nY;
    }

    inline bool IsDown() const
    {
        return m_bDown;
    }

    inline bool IsLeft() const
    {
        return m_bLeft;
    }

    inline bool IsMoving() const
    {
        return m_bMoving;
    }
};

enum EInputSystemContextType
{
    eISCT_None,
    eISCT_CreateKeyboard,
    eISCT_CreateMouse,
    eISCT_CreateKeyboardMouse,
};

class ECS_MODELVIEW_API CWin32InputSystemContext : public IInputSystemContext
{
    entityx::Entity& m_rkEntity;
    EInputSystemContextType m_eType;
public:
    CWin32InputSystemContext(entityx::Entity& _rkEntity, EInputSystemContextType _eType)
        : m_rkEntity(_rkEntity)
        , m_eType(_eType)
    {}

    virtual ~CWin32InputSystemContext()
    {}

    int GetContextType() const
    {
        return m_eType;
    }

    inline entityx::Entity GetEntity() const
    {
        return m_rkEntity;
    }
};

class ECS_MODELVIEW_API CWin32InputSystem : public IInputSystem
{
    entityx::EventManager* m_pkEventMgr;
public:
    CWin32InputSystem();

    virtual ~CWin32InputSystem();

    virtual void configure(entityx::EventManager& _rkEvnetMgr);

    LRESULT WndProc(HWND _hWnd, UINT _nMessage, WPARAM _wParam, LPARAM _lParam);

    virtual void ResolveInput(entityx::EventManager& _rkEventMgr, const IInputEvent& _rkEvent);

    virtual void receive (const IInputSystemContext&);
};