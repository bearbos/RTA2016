#pragma once
#include "stdafx.h"
class Renderer
{
	ID3D11Device *device;
	IDXGISwapChain *swapChain;
	ID3D11DeviceContext *deviceContext;
	D3D11_VIEWPORT mainViewPort;
	ID3D11RenderTargetView *renderTargetView;
	ID3D11Texture2D *backBufferView;
	ID3D11Texture2D *depthStencilPointer;
	ID3D11DepthStencilView *depthStencilViewport;
public:
	Renderer();
	~Renderer();
	void Initialize(HWND window, unsigned int windHeight, unsigned int windWidth);
};

