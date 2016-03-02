#include "stdafx.h"
#include "Renderer.h"


void Renderer::Initialize(HWND window, unsigned int windHeight, unsigned int windWidth)
{
	DXGI_SWAP_CHAIN_DESC chainDesc;
	SecureZeroMemory(&chainDesc, sizeof(chainDesc));
	chainDesc.BufferDesc.Height = windHeight;
	chainDesc.BufferDesc.Width = windWidth;
	chainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	chainDesc.BufferDesc.RefreshRate.Numerator = 60;
	chainDesc.BufferDesc.RefreshRate.Denominator = 1;
	chainDesc.BufferCount = 1;
	chainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	chainDesc.Windowed = true;
	chainDesc.OutputWindow = window;
	chainDesc.SampleDesc.Count = 2;
	chainDesc.SampleDesc.Quality = 0;

	D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG, NULL, NULL, D3D11_SDK_VERSION, &chainDesc, &swapChain, &device, NULL, &deviceContext);

	ID3D11Resource *temp = nullptr;
	swapChain->GetBuffer(0, __uuidof(ID3D11Resource), (void**)temp);
	device->CreateRenderTargetView(temp, NULL, &renderTargetView);
	if (temp)
		temp->Release();
	mainViewPort.TopLeftX = 0;
	mainViewPort.TopLeftY = 0;
	mainViewPort.MaxDepth = 1;
	mainViewPort.MinDepth = 1;
	mainViewPort.Height = windHeight;
	mainViewPort.Width = windWidth;
}


Renderer::~Renderer()
{
}

Renderer::Renderer()
{
}
