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
bool invalid = false;
int frames = 1;

int start_key = clock();
char key_down = '0';


// Draws a pixel at X, Y (from top left corner)
void DrawPixel(int X, int Y, u32 Color) {
    u32* Pixel = (u32*)BitmapMemory;
    Pixel += Y * BitmapWidth + X;
    if (invalid) {
        invalid = false;
        *Pixel = Color;
        frames = 1;
    }
    else {
        if (*Pixel == 0xffffff) {
            *Pixel = Color;
            return;
        }
        vec3 current = u32_colour(*Pixel);
        current *= 7;
        current += u32_colour(Color);
        current /= 8;
        frames += 1;
        *Pixel = current;
    }
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

    VirtualFree(BitmapMemory, 0, MEM_RELEASE);

    BitmapMemory = VirtualAlloc(0,
        BitmapWidth * BitmapHeight * 4,
        MEM_RESERVE | MEM_COMMIT,
        PAGE_READWRITE
    );

    worldCamera.set_size(ClientWidth, ClientHeight);
    invalid = true;
}

LRESULT CALLBACK WindowProc(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam) {
    switch (Message) {
    case WM_KEYDOWN: {
        switch (WParam) { 
        case 'O': { 
            DestroyWindow(Window);
            break;
        }     
        case 'C': {
            worldData.rays_per_pixel += 1; 
            break;
        }
        case 'V': {
            worldData.rays_per_pixel -= 1;
            break;
        }
        case 'Q': {
            key_down = 'Q'; start_key = clock(); break;
        }
        case 'E': {
            key_down = 'E'; start_key = clock(); break;
        }
        case 'W': {
            key_down = 'W'; start_key = clock(); break;
        }
        case 'S': {
            key_down = 'S'; start_key = clock(); break;
        }
        }
        break;
    } 
    case WM_KEYUP:{
        switch (WParam) {
        case 'Q': {
            key_down = '0'; break;
        }
        case 'E': {
            key_down = '0'; break;
        }
        case 'W': {
            key_down = '0'; break;
        }
        case 'S': {
            key_down = '0'; break;
        }
        }
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

            worldCamera.direction.y -= ox * 3.14159265 * 1;
            worldCamera.direction.x += oy * 3.14159265 * 1;
            mouse_start_x = p.x;
            mouse_start_y = p.y;
            invalid = true;
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



    Object sphere1(vec3(0, 1, 12), 1);
    sphere1.mat.colour = vec3(1, 0.2, 0.2);
    sphere1.mat.specularStrength = 0.9;
    Object sphere2(vec3(3, 1, 12), 1);
    sphere2.mat.colour = vec3(0.2, 1, 0.2);
    sphere2.mat.specularStrength = 0.9;

    //Object sphere3(vec3(8, -25, 0), 15);
    //sphere3.mat.colour = vec3(1, 1, 1);
    //sphere3.mat.emissionColour = vec3(1, 1, 1);
    //sphere3.mat.emissionStrength = 1.0;

    Object sphere4(vec3(0, 140, 0), 138);
    sphere4.mat.colour = vec3(0.9, 0.9, 0.9);

    Object sphere5(vec3(1.5, -1, 11.5), 1);
    sphere5.mat.colour = vec3(1, 1, 1);
    sphere5.mat.specularStrength = 0.9;

    worldCamera.position = vec3(0, 0, 0);

    worldObjects.push_back(sphere1);
    worldObjects.push_back(sphere2);
    //worldObjects.push_back(sphere3);
    worldObjects.push_back(sphere4);
    worldObjects.push_back(sphere5);

    lightSource = &worldObjects[2];

    worldData.bounce_limit = 4;
    worldData.scene_colour = vec3(0.5, 0.5, 1);
    worldData.sky_boundary = 100;
    worldData.sun_direction = vec3(0, -1, 0);
    worldData.rays_per_pixel = 1;

    while (Running) {
        MSG Message;
        while (PeekMessage(&Message, nullptr, 0, 0, PM_REMOVE)) {
            if (Message.message == WM_QUIT) Running = false;
            TranslateMessage(&Message);
            DispatchMessage(&Message);
        }

        if (key_down != '0') {

            double dt = double(clock() - start_key + 1) / 1000.0;
            start_key = clock();

            double s = dt * 0.4;

            if (key_down == 'E') {
                worldCamera.position.y += s;
            }
            else if (key_down == 'Q') {
                worldCamera.position.y -= s;
            }

            invalid = true;
        }

        if (invalid) {
            ClearScreen(0xffffff);
        }

        //srand(0);
        for (int x = 0; x < BitmapWidth; x++)
            for (int y = 0; y < BitmapHeight; y++) 
                DrawPixel(x, y, ray_func(worldCamera.coord_ray(x, y)));
            
            
            

        BITMAPINFO BitmapInfo;
        BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
        BitmapInfo.bmiHeader.biWidth = BitmapWidth;
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