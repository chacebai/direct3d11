#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <Windows.h>
#include <d3d11.h>

//DirectX从这里开始
class Graphics
{
public:
    Graphics(HWND hWnd)
    {
        DXGI_SWAP_CHAIN_DESC sd = {};
        sd.BufferDesc.Width = 0;
        sd.BufferDesc.Height = 0;
        sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator = 0;
        sd.BufferDesc.RefreshRate.Denominator = 0;
        sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
        sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.BufferCount = 1;
        sd.OutputWindow = hWnd;
        sd.Windowed = TRUE;
        sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        sd.Flags = 0;

        D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, NULL, 0, D3D11_SDK_VERSION, &sd, &pSwap, &pDevice, NULL, &pContext);
    }
    Graphics(const Graphics&) = delete;
    Graphics& operator=(const Graphics &) = delete;
    ~Graphics()
    {
        if (pDevice != NULL)
        {
            pDevice->Release();
        }
        if (pSwap != NULL)
        {
            pSwap->Release();
        }
        if (pContext)
        {
            pContext->Release();
        }
    }
private:
    ID3D11Device *pDevice = NULL;
    IDXGISwapChain *pSwap = NULL;
    ID3D11DeviceContext *pContext = NULL;
};

#endif