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
    GetClientRect(Window, &Rect);
    Dim.x = Rect.left;
    Dim.y = Rect.top;
    Dim.Width = Rect.right - Rect.left;
    Dim.Height = Rect.bottom - Rect.top;
    return Dim; 
}

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
        
        
        Win32InitOpenGL(Window);
        LoadAssets();
        
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
            v2 ScreenDim = {(float)Dim.Width, (float)Dim.Height};
            RunRenderBuffer(ScreenDim, dt);
            
            Win32RenderFrame(Window, Dim.Width, Dim.Height);
        }
        
    }
    
    return 0;
}

