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
            real32 MoveVertical;
            real32 MoveHorizontal;
            
            real32 CameraVertical;
            real32 CameraHorizontal;
            
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

void Win32GetInput(input *Input, input *OldInput, win32_windowdim Dim, game_state *GameState)
{
    // NOTE(Barret5Ocal): Pay attention to what you type these 
    real32  MoveUp = Win32IsDown(0x57);
    real32  MoveDown = -Win32IsDown(0x53);
    real32  MoveLeft = -Win32IsDown(0x41);
    real32  MoveRight = Win32IsDown(0x44);
    
    Input->MoveVertical = MoveUp + MoveDown;
    Input->MoveHorizontal = MoveLeft + MoveRight;
    
    real32 CameraUp = Win32IsDown(VK_UP);
    real32 CameraDown = -Win32IsDown(VK_DOWN);
    real32 CameraLeft = -Win32IsDown(VK_LEFT);
    real32 CameraRight = Win32IsDown(VK_RIGHT);
    
    Input->CameraVertical = CameraUp + CameraDown;
    Input->CameraHorizontal = CameraLeft + CameraRight;
    
    POINT Point = {}; 
    GetCursorPos(&Point);
    
    v2 Pos = {(real32)Point.x - Dim.x, ((real32)Point.y - Dim.y)};
    
    Input->MouseMove = Pos; 
    
    SetCursorPos(Dim.x, Dim.y);
    
#if 0
    char Buffer[100] = {};
    stbsp_sprintf(Buffer, "Pos:%f %f\n", Pos.x, Pos.y);
    OutputDebugStringA(Buffer);
#endif 
    
    XINPUT_STATE State;
    if(XInputGetState(0, &State) != ERROR_DEVICE_NOT_CONNECTED)
    {
        XINPUT_GAMEPAD Pad = State.Gamepad;
        if(Pad.wButtons & XINPUT_GAMEPAD_DPAD_UP) MoveUp = 1.0f;
        if(Pad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) MoveDown = -1.0f;
        if(Pad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)MoveLeft = -1.0f;
        if(Pad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)MoveRight = 1.0f;
        
        Input->MoveVertical = MoveUp + MoveDown;
        Input->MoveHorizontal = MoveLeft + MoveRight;
        
        if(Pad.sThumbLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
            MoveRight = (real32)Pad.sThumbLX/32767.0f;  
        if(Pad.sThumbLX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
            MoveLeft = (real32)Pad.sThumbLX/32768.0f; 
        if(Pad.sThumbLY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) 
            MoveUp = (real32)Pad.sThumbLY/32767.0f; 
        if(Pad.sThumbLY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) 
            MoveDown = (real32)Pad.sThumbLY/32768.0f;
        
        Input->MoveVertical = MoveUp + MoveDown;
        Input->MoveHorizontal = MoveLeft + MoveRight;
        
        if(Pad.sThumbRX > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) CameraRight = (real32)Pad.sThumbRX/32767.0f; 
        if(Pad.sThumbRX < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) CameraLeft = (real32)Pad.sThumbRX/32768.0f; 
        if(Pad.sThumbRY > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) CameraUp = (real32)Pad.sThumbRY/32767.0f; 
        if(Pad.sThumbRY < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) CameraDown = (real32)Pad.sThumbRY/32768.0f;
        
        Input->CameraVertical = CameraUp + CameraDown;
        Input->CameraHorizontal = CameraLeft + CameraRight;
        
    }
}

void InitRawInput()
{
    UINT nDevices = 0;
    PRAWINPUTDEVICELIST pRawInputDeviceList;
    if (GetRawInputDeviceList(NULL, &nDevices, sizeof(RAWINPUTDEVICELIST)) != 0) 
    { 
        InvalidCodePath;
    }
    pRawInputDeviceList = (RAWINPUTDEVICELIST *)malloc(sizeof(RAWINPUTDEVICELIST) * nDevices); 
    
    GetRawInputDeviceList(pRawInputDeviceList, &nDevices,sizeof(RAWINPUTDEVICELIST));
    int i =0; 
    
    uint32 NumDevices = 0;
    for (int32 Index = 0; 
         Index < nDevices;
         ++Index)
    {
        if(pRawInputDeviceList[Index].dwType == 0)
            ++NumDevices;
    }
    
    
    RAWINPUTDEVICE *RawInputDevices = (RAWINPUTDEVICE *)malloc(sizeof(RAWINPUTDEVICE) * NumDevices);
    for (int32 Index = 0; 
         Index < NumDevices;
         ++Index)
    {
        RAWINPUTDEVICE *Rid = RawInputDevices + Index;
        Rid->usUsagePage = 0x01; 
        Rid->usUsage = 0x02; 
        Rid->dwFlags = RIDEV_NOLEGACY;   // adds HID mouse and also ignores legacy mouse messages
        Rid->hwndTarget = 0;
    }
    
    if(RegisterRawInputDevices(RawInputDevices, NumDevices, sizeof(RawInputDevices[0])) == FALSE)
    {
        InvalidCodePath;
    }
    
    free(RawInputDevices);
    free(pRawInputDeviceList);
}
