#include "stdafx.h"
#include "Renderer.h"

ID3D11Device *Renderer::device = 0;
IDXGISwapChain *Renderer::swapChain = 0;
ID3D11DeviceContext *Renderer::deviceContext = 0;
D3D11_VIEWPORT Renderer::mainViewPort;
ID3D11RenderTargetView *Renderer::renderTargetView = 0;
ID3D11Texture2D *Renderer::backBufferView = 0;
ID3D11Texture2D *Renderer::depthStencilPointer = 0;
ID3D11DepthStencilView *Renderer::depthStencilViewport = 0;
XMFLOAT4X4 Renderer::viewMatrix;
XMFLOAT4X4 Renderer::projMatrix;
ID3D11Buffer *Renderer::viewProjConBuffer = 0;
ID3D11Buffer *Renderer::worldCOnBuffer = 0;

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
	HRESULT hResult;
	D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL, D3D11_SDK_VERSION, &chainDesc, &swapChain, &device, NULL, &deviceContext);

	ID3D11Resource *temp = nullptr;
	swapChain->GetBuffer(0, __uuidof(ID3D11Resource), (void**)&temp);
	hResult = device->CreateRenderTargetView(temp, NULL, &renderTargetView);
	if (temp)
		temp->Release();
	mainViewPort.TopLeftX = 0;
	mainViewPort.TopLeftY = 0;
	mainViewPort.MaxDepth = 1;
	mainViewPort.MinDepth = 1;
	mainViewPort.Height = (float)windHeight;
	mainViewPort.Width = (float)windWidth;

	D3D11_TEXTURE2D_DESC ztextureDesc;
	SecureZeroMemory(&ztextureDesc, sizeof(ztextureDesc));
	ztextureDesc.Width = windWidth;
	ztextureDesc.Height = windHeight;
	ztextureDesc.MipLevels = 1;
	ztextureDesc.ArraySize = 1;
	ztextureDesc.Format = DXGI_FORMAT_D32_FLOAT;
	ztextureDesc.Usage = D3D11_USAGE_DEFAULT;
	ztextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	ztextureDesc.SampleDesc.Count = 2;
	ztextureDesc.MiscFlags = 0;
	hResult = device->CreateTexture2D(&ztextureDesc, NULL, &backBufferView);

	D3D11_DEPTH_STENCIL_VIEW_DESC depthDesc;
	SecureZeroMemory(&depthDesc, sizeof(depthDesc));
	depthDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthDesc.Texture2D.MipSlice = 0;
	hResult = device->CreateDepthStencilView(backBufferView, NULL, &depthStencilViewport);

	D3D11_BUFFER_DESC VPBuffDesc;
	SecureZeroMemory(&VPBuffDesc, sizeof(VPBuffDesc));
	VPBuffDesc.Usage = D3D11_USAGE_DYNAMIC;
	VPBuffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	VPBuffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	VPBuffDesc.ByteWidth = sizeof(ViewProjectionMatrixs);
	hResult = device->CreateBuffer(&VPBuffDesc, NULL, &viewProjConBuffer);


	XMMATRIX tempMatrix = XMMatrixIdentity();
	XMStoreFloat4x4(&projMatrix, tempMatrix);
	float yscale = 1.0f / tan(((90 / 2) * 3.14159f) / 180);
	float xScale = yscale / ((float)windWidth / ((float)windHeight / 2.0f));
	
	projMatrix._11 = xScale;
	projMatrix._22 = yscale;
	projMatrix._33 = 100 / (100 - 0.1f);
	projMatrix._43 = -(100 * 0.1f) / (100 - 0.1f);
	projMatrix._44 = 0;
	projMatrix._34 = 1;

	XMStoreFloat4x4(&viewMatrix, tempMatrix);

	D3D11_BUFFER_DESC worldConBuffDesc;
	SecureZeroMemory(&worldConBuffDesc, sizeof(worldConBuffDesc));
	worldConBuffDesc.Usage = D3D11_USAGE_DYNAMIC;
	worldConBuffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	worldConBuffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	worldConBuffDesc.ByteWidth = sizeof(XMFLOAT4X4);
	device->CreateBuffer(&worldConBuffDesc, NULL, &worldCOnBuffer);
}


Renderer::~Renderer()
{
}

Renderer::Renderer()
{
}

void Renderer::ClearScreenToColor(float * color)
{
	deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilViewport);
	deviceContext->RSSetViewports(1, &mainViewPort);
	deviceContext->ClearRenderTargetView(renderTargetView, color);
	deviceContext->ClearDepthStencilView(depthStencilViewport, D3D11_CLEAR_DEPTH, 1, 0);
}
void Renderer::Render()
{
	D3D11_MAPPED_SUBRESOURCE VPSubResource;
	deviceContext->Map(viewProjConBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &VPSubResource);
	ViewProjectionMatrixs *VramVP;
	VramVP = (ViewProjectionMatrixs*)VPSubResource.pData;
	VramVP->projectionMatrix = projMatrix;
	VramVP->viewMatrix = viewMatrix;
	deviceContext->Unmap(viewProjConBuffer, NULL);
	deviceContext->VSSetConstantBuffers(1, 1, &viewProjConBuffer);

	swapChain->Present(0, 0);
}	