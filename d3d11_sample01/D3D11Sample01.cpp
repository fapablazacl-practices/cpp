
#include <iostream>
#include <Windows.h>
#include <d3d11.h>

LRESULT CALLBACK WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
    PAINTSTRUCT ps;
    HDC hDC;

    switch (Msg) {
    case WM_PAINT:
        hDC = ::BeginPaint(hWnd, &ps);
        ::EndPaint(hWnd, &ps);
        break;

    case WM_DESTROY:
        ::PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, Msg, wParam, lParam);
    }

    return 0;
}

int main() {

    // create window
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
    wc.lpszClassName = "D3D10Sample01";

    ::RegisterClassEx(&wc);

    const DWORD style = WS_OVERLAPPEDWINDOW;

    RECT rc = {0, 0, 800, 600};
    ::AdjustWindowRect(&rc, style, FALSE);

    const int ScreenWidth = rc.right - rc.left;
    const int ScreenHeight = rc.bottom - rc.top;

    HWND hWnd = ::CreateWindow (
        "D3D10Sample01", 
        "Window", 
        style, 
        CW_USEDEFAULT, CW_USEDEFAULT, 
        ScreenWidth, ScreenHeight,
        NULL, NULL, 
        wc.hInstance, NULL
    );

    ::ShowWindow(hWnd, SW_SHOWDEFAULT);

    // initialize direct3d 11
    D3D_DRIVER_TYPE driverTypes[] = {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_SOFTWARE,
    };

    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_1, 
        D3D_FEATURE_LEVEL_11_0, 
        D3D_FEATURE_LEVEL_10_1, 
        D3D_FEATURE_LEVEL_10_0
    };

    D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_UNKNOWN;

    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    swapChainDesc.BufferCount = 1;
    swapChainDesc.BufferDesc.Width = ScreenWidth;
    swapChainDesc.BufferDesc.Height = ScreenHeight;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = hWnd;
    swapChainDesc.Windowed = TRUE;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;

    D3D_FEATURE_LEVEL featureLevel;
    ID3D11Device *device;
    ID3D11DeviceContext *context;
    IDXGISwapChain *swapChain;

    unsigned creationFlags = 0;

#ifdef _DEBUG
    creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    HRESULT hResult;
    for (D3D_DRIVER_TYPE driverType_ : driverTypes) {
        hResult = ::D3D11CreateDeviceAndSwapChain (
            nullptr, 
            driverType_, 
            NULL, 
            creationFlags, 
            featureLevels, 4, 
            D3D11_SDK_VERSION, 
            &swapChainDesc, &swapChain, 
            &device, 
            &featureLevel, 
            &context
        );

        if (SUCCEEDED(hResult)) {
            driverType = driverType_;
            break;
        }
    }

    if (FAILED(hResult)) {
        std::cout << "Direct3D Device Creation Failed." << std::endl;
        return 1;
    }

    // 
    ID3D11RenderTargetView *backBufferTarget;
    ID3D11Texture2D *backBufferTexture;

    hResult = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferTexture);

    device->CreateRenderTargetView(backBufferTexture, nullptr, &backBufferTarget);

    if (backBufferTexture) {
        backBufferTexture->Release();
    }
    
    context->OMSetRenderTargets(1, &backBufferTarget, nullptr);
    
    D3D11_VIEWPORT viewport;
    viewport.Width = ScreenWidth;
    viewport.Height = ScreenHeight;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    context->RSSetViewports(1, &viewport);

    MSG msg = {};

    while (msg.message != WM_QUIT) {
        if (::PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        } else {
            const float clearColor[] = {0.0f, 0.0f, 0.0f, 1.0f};
            context->ClearRenderTargetView(backBufferTarget, clearColor);

            swapChain->Present(0, 0);
        }
    }

    if (backBufferTarget) {
        backBufferTarget->Release();
    }

    if (swapChain) {
        swapChain->Release();
    }

    if (context) {
        context->Release();
    }

    if (device) {
        device->Release();
    }

    return static_cast<int>(msg.wParam);
}
