#define WIN32_LEAN_AND_MEAN
#define UNICODE

#include "func.h"

void* BitmapMemory;

int BitmapWidth;
int BitmapHeight;

int ClientWidth = 1;
int ClientHeight = 1;

int mouse_start_x, mouse_start_y;
bool mouse_down = false;


Camera worldCamera;

// Draws a pixel at X, Y (from top left corner)
void DrawPixel(int X, int Y, u32 Color) {
    u32* Pixel = (u32*)BitmapMemory;
    Pixel += Y * BitmapWidth + X;
    *Pixel = Color;
}

void ClearScreen(u32 Color) {
    u32* Pixel = (u32*)BitmapMemory;
    for (int Index = 0; Index < BitmapWidth * BitmapHeight; ++Index) 
        *Pixel++ = Color;
}
void Resize(HWND Window) {
    RECT ClientRect;
    GetClientRect(Window, &ClientRect);
    ClientWidth = ClientRect.right - ClientRect.left;
    ClientHeight = ClientRect.bottom - ClientRect.top;

    BitmapWidth = ClientWidth;
    BitmapHeight = ClientHeight;

    // Allocate memory for the bitmap

    BitmapMemory = 
    BitmapMemory = VirtualAlloc(0,
        BitmapWidth * BitmapHeight * 4,
        MEM_RESERVE | MEM_COMMIT,
        PAGE_READWRITE
    );

    worldCamera.set_size(ClientWidth, ClientHeight);
}

LRESULT CALLBACK WindowProc(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam) {
    switch (Message) {
    case WM_KEYDOWN: {
        switch (WParam) { 
        case 'O': { DestroyWindow(Window); }; }
        break;
    } 
    case WM_DESTROY: { 
        PostQuitMessage(0); 
        break;
    } 
    case WM_EXITSIZEMOVE: {
        Resize(Window);
        break;
    }
    case WM_LBUTTONDOWN: {
        POINT p;
        mouse_down = true;
        if (GetCursorPos(&p)) {
            mouse_start_x = p.x;
            mouse_start_y = p.y;
        } break;
    }
    case WM_MOUSEMOVE: {
        POINT p;
        if (GetCursorPos(&p) && mouse_down) {
            int dx = p.x - mouse_start_x;
            int dy = p.y - mouse_start_y;

            double ox = double(dx) / double(ClientWidth);
            double oy = double(dy) / double(ClientWidth);

            worldCamera.direction.y -= ox * 3.14159265 * 0.5;
            worldCamera.direction.x += oy * 3.14159265 * 0.5;
            mouse_start_x = p.x;
            mouse_start_y = p.y;
        } break;
    }
    case WM_LBUTTONUP: {
        POINT p;
        mouse_down = false;
        break;
    }
    case WM_ENTERSIZEMOVE: {
        Resize(Window);
        break;
    }
    default: { 
    return DefWindowProc(Window, Message, WParam, LParam); }
    }
    return 0;
}

int WINAPI wWinMain(HINSTANCE Instance, HINSTANCE PrevInstance, PWSTR CmdLine, int CmdShow) {

    WNDCLASS WindowClass = {};
    const wchar_t ClassName[] = L"MyWindowClass";
    WindowClass.lpfnWndProc = WindowProc;
    WindowClass.hInstance = Instance;
    WindowClass.lpszClassName = ClassName;
    WindowClass.hCursor = LoadCursor(0, IDC_CROSS);

    if (!RegisterClass(&WindowClass)) {
        MessageBox(0, L"RegisterClass failed", 0, 0);
        return GetLastError();
    }

    HWND Window = CreateWindowEx(0, ClassName, L"Program",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT,
        0, 0, Instance, 0);

    if (!Window) {
        MessageBox(0, L"CreateWindowEx failed", 0, 0);
        return GetLastError();
    }


    // Get client area dimensions 

    Resize(Window);


    HDC DeviceContext = GetDC(Window);

    bool Running = true;

    Object sphere1(vec3(0, 0, 12), 1);
    sphere1.colour = vec3(1, 0.1, 0.1);
    Object sphere2(vec3(3, 0, 12), 0.5);
    sphere2.colour = vec3(0.1, 1, 0.1);
    Object sphere3(vec3(4, 12, 12), 1.5);
    sphere3.colour = vec3(1, 1, 1);

    worldObjects.push_back(sphere1);
    worldObjects.push_back(sphere2);
    worldObjects.push_back(sphere3);

    worldData.bounce_limit = 1;
    worldData.scene_colour = vec3(0.4, 0.4, 1);
    worldData.sky_boundary = 100;


    while (Running) {
        MSG Message;
        while (PeekMessage(&Message, nullptr, 0, 0, PM_REMOVE)) {
            if (Message.message == WM_QUIT) Running = false;
            TranslateMessage(&Message);
            DispatchMessage(&Message);
        }

        ClearScreen(0xffffff);

        for (int x = 0; x < BitmapWidth; x++)
            for (int y = 0; y < BitmapHeight; y++) 
                DrawPixel(x, ClientHeight - y, raycast(worldCamera.coord_ray(x, y)));
            
            
            

        BITMAPINFO BitmapInfo;
        BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
        BitmapInfo.bmiHeader.biWidth = BitmapWidth;
        // Negative height makes top left as the coordinate system origin for the DrawPixel function, otherwise its bottom left
        BitmapInfo.bmiHeader.biHeight = -BitmapHeight;
        BitmapInfo.bmiHeader.biPlanes = 1;
        BitmapInfo.bmiHeader.biBitCount = 32;
        BitmapInfo.bmiHeader.biCompression = BI_RGB;

        StretchDIBits(DeviceContext,
            0, 0,
            BitmapWidth, BitmapHeight,
            0, 0,
            ClientWidth, ClientHeight,
            BitmapMemory, &BitmapInfo,
            DIB_RGB_COLORS, SRCCOPY);
    }

    return 0;
}