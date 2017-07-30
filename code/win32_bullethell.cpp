#include <windows.h>
#define STB_SPRINTF_IMPLEMENTATION
#include "stb_sprintf.h"
#include "b50_timing.h"

#define GB_MATH_IMPLEMENTATION
#include "gb_math.h"
typedef gbVec2 v2;
typedef gbVec3 v3;
typedef gbVec4 v4;
typedef gbMat4 m4;

#include <stdint.h>
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef float real32;
typedef double real64;

#define global_variable static 
#define local_persist static 
#define internal static 

#define Assert(Expression) if(!(Expression)) {*(int *)0 = 0;}
#define InvalidCodePath Assert(false)
#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

#define Kilobyte(Value) 1024 * Value
#define Megabyte(Value) 1024 * Kilobyte(Value)
#define Gigabyte(Value) 1024 * Megabyte(Value)
#define Terabyte(Value) 1024 * Gigabyte(Value)

#include "win32_memory.h"

#include "bullethell.h"
#include "win32_opengl.h"
#include "opengl.cpp"

int Running = 1; 
int ActiveApp = 1;
LRESULT CALLBACK WindowProc(HWND   Window,
                            UINT   Message,
                            WPARAM WParam,
                            LPARAM LParam)
{
    LRESULT Result = {};
    switch (Message)
    {
        case WM_DESTROY:
        {
            Running = 0; 
        }break;
        case WM_ACTIVATEAPP:
        {
            ActiveApp = WParam;
        }break;
        case WM_SIZE:
        {
            if(WParam == SIZE_RESTORED)
            {
                RECT Rect;
                GetClientRect(Window, &Rect);
                int Width = Rect.right - Rect.left;
                int Height = Rect.bottom - Rect.top; 
                float CurrentRatio = (float)Width/(float)Height; 
                float ProperRatio = 16.0f/9.0f; 
                if(CurrentRatio != ProperRatio)
                {
                    int NewHeight = (9 * Width) / 16;
                    GetWindowRect(Window, &Rect);
                    Rect.right = Rect.left + Width;
                    Rect.bottom = Rect.top + NewHeight; 
                    AdjustWindowRect(&Rect, WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_VISIBLE, 0);
                    SetWindowPos(Window, HWND_TOP, Rect.left, Rect.top, Rect.right - Rect.left, Rect.bottom - Rect.top, SWP_DRAWFRAME | SWP_NOMOVE | SWP_SHOWWINDOW);
                    int i = 0;
                }
            }
        }break;
        default:
        {
            Result = DefWindowProc(Window, Message, WParam, LParam);
        }break;
    }
    
    return(Result);
}

struct win32_windowdim 
{
    int Width, Height; 
    int x, y;
    //int DisplayWidth, DisplayHeight; 
};

win32_windowdim Win32GetWindowDim(HWND Window)
{
    win32_windowdim Dim = {};
    
    RECT Rect = {};
    //GetClientRect(Window, &Rect);
    GetWindowRect(Window, &Rect);
    Dim.x = Rect.left;
    Dim.y = Rect.top;
    Dim.Width = Rect.right - Rect.left;
    Dim.Height = Rect.bottom - Rect.top;
    return Dim; 
}

#include <xinput.h>

typedef DWORD WINAPI B50XInputGetState(DWORD dwUserIndex, XINPUT_STATE *pState);
B50XInputGetState *XInputGetState_;
#define XInputGetState XInputGetState_

void LoadXInput()
{
    HMODULE XInput = LoadLibraryA("xinput1_3.dll");
    if(XInput)
    {
        XInputGetState = (B50XInputGetState *)GetProcAddress(XInput, "XInputGetState");
    }
    else
    {
        XInput = LoadLibraryA("Xinput1_4.dll");
        if(XInput)
        {
            XInputGetState = (B50XInputGetState *)GetProcAddress(XInput, "XInputGetState");
        }
        else 
        {
            XInput = LoadLibraryA("Xinput9_1_0.dll");
            if(XInput)
            {
                XInputGetState = (B50XInputGetState *)GetProcAddress(XInput, "XInputGetState");
            }
            else
            {
                InvalidCodePath;
            }
        }
    }
}

struct input
{
    union 
    {
        struct 
        {
            int32 MoveVertical;
            int32 MoveHorizontal;
            
            int32 CameraVertical;
            int32 CameraHorizontal;
            
        };
        int E[4];
    };
    
    gbVec2 MousePos; 
    gbVec2 MouseMove;
    
    int LeftMouseClick;
    int RightMouseClick;
};

int Win32IsDown(int Code)
{
    short Button = GetKeyState(Code);
    int Toggle = (Button & 0xFF);
    int Down = (Button >> 15) & 0x1; 
    
    return Down;
}

gbVec2 Win32GetMousePosFromCenter(win32_windowdim Dim)
{
    POINT Point; 
    GetCursorPos(&Point);
    v2 Center = {(float)Dim.Width/2, (float)Dim.Height/2};
    
    return {(float)(Point.x - Dim.x) - Center.x, (float)(Point.y - Dim.y) - Center.y};
}

gbVec2 Win32GetMousePos(win32_windowdim Dim)
{
    POINT Point; 
    GetCursorPos(&Point);
    return {(float)(Point.x - Dim.x), (float)(Point.y - Dim.y)};
}

void Win32GetInput(input *Input, input *OldInput, win32_windowdim Dim, game_state *GameState)
{
    int32 MoveUp = Win32IsDown(0x57);
    int32 MoveDown = -Win32IsDown(0x53);
    int32 MoveLeft = -Win32IsDown(0x41);
    int32 MoveRight = Win32IsDown(0x44);
    
    Input->MoveVertical = MoveUp + MoveDown;
    Input->MoveHorizontal = MoveLeft + MoveRight;
    
    int32 CameraUp = -Win32IsDown(VK_UP);
    int32 CameraDown = Win32IsDown(VK_DOWN);
    int32 CameraLeft = -Win32IsDown(VK_LEFT);
    int32 CameraRight = Win32IsDown(VK_RIGHT);
    
    Input->CameraVertical = CameraUp + CameraDown;
    Input->CameraHorizontal = CameraLeft + CameraRight;
    
    Input->MousePos = Win32GetMousePosFromCenter(Dim);
    
    Input->MouseMove = Input->MousePos - OldInput->MousePos;
    
    char Buffer[100] = {};
    stbsp_sprintf(Buffer, "%f %f\n", Input->MouseMove.x, Input->MouseMove.y);
    OutputDebugStringA(Buffer);
    
    XINPUT_STATE State;
    if(XInputGetState(0, &State) != ERROR_DEVICE_NOT_CONNECTED)
    {
        XINPUT_GAMEPAD Pad = State.Gamepad;
        if(Pad.wButtons & XINPUT_GAMEPAD_DPAD_UP) MoveUp = -1;
        if(Pad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) MoveDown = 1;
        if(Pad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)MoveLeft = -1;
        if(Pad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)MoveRight = 1;
        
        Input->MoveVertical = MoveUp + MoveDown;
        Input->MoveHorizontal = MoveLeft + MoveRight;
        
        if(Pad.sThumbLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) MoveRight = 1; 
        if(Pad.sThumbLX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) MoveLeft = -1; 
        if(Pad.sThumbLY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) MoveUp = -1; 
        if(Pad.sThumbLY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) MoveDown = 1;
        
        Input->MoveVertical = MoveUp + MoveDown;
        Input->MoveHorizontal = MoveLeft + MoveRight;
    }
}

#include "bullethell.cpp"

int CALLBACK 
WinMain(HINSTANCE Instance,
        HINSTANCE PrevInstance, 
        LPSTR CmdLine,
        int ShowCode)
{
    WNDCLASSEX WindowClass = {};
    WindowClass.cbSize = sizeof(WNDCLASSEX);
    WindowClass.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
    WindowClass.lpfnWndProc = WindowProc;
    WindowClass.hInstance = Instance;
    WindowClass.lpszClassName = "Bullet Hell Class";
    
    if(RegisterClassEx(&WindowClass))
    {
        HWND Window = CreateWindowEx(0, 
                                     WindowClass.lpszClassName,
                                     "Bullet Hell",
                                     WS_OVERLAPPEDWINDOW|WS_VISIBLE,
                                     CW_USEDEFAULT,
                                     CW_USEDEFAULT,
                                     1440,
                                     900,
                                     0,
                                     0,
                                     Instance, 
                                     0);
        
        LoadXInput();
        Win32InitOpenGL(Window);
        LoadAssets();
        
        game_state GameState = {};
        memory_arena RenderBuffer = {};
        AllocateArena(&RenderBuffer, Megabyte(4));
        
        Setup(&GameState);
        
        input OldInput = {};
        input NewInput = {};
        
        time_info TimeInfo = {};
        float FrameRate = 60;
        float dt = 1 / FrameRate; 
        while(RunLoop(&TimeInfo, Running, FrameRate))
        {
            MSG Message; 
            while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&Message);
                DispatchMessage(&Message);
            }
            
            win32_windowdim Dim = Win32GetWindowDim(Window);
            
            OldInput = NewInput;
            NewInput = {};
            if(ActiveApp)
                Win32GetInput(&NewInput, &OldInput, Dim, &GameState);
            
            Update(&GameState, &NewInput, dt, &RenderBuffer);
            
            v2 ScreenDim = {(float)Dim.Width, (float)Dim.Height};
            RunRenderBuffer(ScreenDim, dt, &RenderBuffer);
            
            Win32RenderFrame(Window, Dim.Width, Dim.Height);
        }
        
    }
    
    return 0;
}

