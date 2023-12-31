#include "DxWinHelper.h"

UINT Resolution::g_ResizeWidth = 0;
UINT Resolution::g_ResizeHeight = 0;

bool CreateDeviceD3D(HWND hWnd, d3dStuff& d3d)
{
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &d3d.g_pSwapChain, &d3d.g_pd3dDevice, &featureLevel, &d3d.g_pd3dDeviceContext);
    if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
        res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &d3d.g_pSwapChain, &d3d.g_pd3dDevice, &featureLevel, &d3d.g_pd3dDeviceContext);
    if (res != S_OK)
        return false;

    CreateRenderTarget(d3d);
    return true;
}

void CleanupDeviceD3D(d3dStuff& d3d)
{
    CleanupRenderTarget(d3d);
    if (d3d.g_pSwapChain) { d3d.g_pSwapChain->Release(); d3d.g_pSwapChain = nullptr; }
    if (d3d.g_pd3dDeviceContext) { d3d.g_pd3dDeviceContext->Release(); d3d.g_pd3dDeviceContext = nullptr; }
    if (d3d.g_pd3dDevice) { d3d.g_pd3dDevice->Release(); d3d.g_pd3dDevice = nullptr; }
}

void CreateRenderTarget(d3dStuff& d3d)
{
    ID3D11Texture2D* pBackBuffer;
    d3d.g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    d3d.g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &d3d.g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget(d3dStuff& d3d)
{
    if (d3d.g_mainRenderTargetView) { d3d.g_mainRenderTargetView->Release(); d3d.g_mainRenderTargetView = nullptr; }
}

#ifndef WM_DPICHANGED
#define WM_DPICHANGED 0x02E0 // From Windows SDK 8.1+ headers
#endif

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;
        Resolution::g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
        Resolution::g_ResizeHeight = (UINT)HIWORD(lParam);
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    case WM_DPICHANGED:
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)
        {
            //const int dpi = HIWORD(wParam);
            //printf("WM_DPICHANGED to %d (%.0f%%)\n", dpi, (float)dpi / 96.0f * 100.0f);
            const RECT* suggested_rect = (RECT*)lParam;
            ::SetWindowPos(hWnd, nullptr, suggested_rect->left, suggested_rect->top, suggested_rect->right - suggested_rect->left, suggested_rect->bottom - suggested_rect->top, SWP_NOZORDER | SWP_NOACTIVATE);
        }
        break;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}