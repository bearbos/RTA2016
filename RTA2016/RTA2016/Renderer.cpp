#include "stdafx.h"
#include "Renderer.h"
#include "VertexShader.csh"
#include "PixelShader.csh"

#define Release(x) { if(x) {x->Release(); x =0;} }
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
ID3D11InputLayout *Renderer::vertexLayout = 0;
ID3D11VertexShader *Renderer::vertexShader = 0;
ID3D11PixelShader *Renderer::pixelShader = 0;
ID3D11SamplerState *Renderer::sampleState = 0;
ID3D11RasterizerState *Renderer::rasterState = 0;
std::vector<RenderSet> Renderer::meshes;
ID3D11BlendState *Renderer::blendState = 0;
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
	Release(temp);
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
	float yscale = 1.0f / tan(0.785398f);
	float xScale = yscale / ((float)windWidth / ((float)windHeight));

	projMatrix._11 = xScale;
	projMatrix._22 = yscale;
	projMatrix._33 = 100 / (100 - 0.1f);
	projMatrix._43 = -(100 * 0.1f) / (100 - 0.1f);
	projMatrix._44 = 0;
	projMatrix._34 = 1;

	XMStoreFloat4x4(&viewMatrix, tempMatrix);
	viewMatrix._43 = -1;
	XMStoreFloat4x4(&viewMatrix, XMMatrixInverse(nullptr, XMLoadFloat4x4(&viewMatrix)));

	D3D11_BUFFER_DESC worldConBuffDesc;
	SecureZeroMemory(&worldConBuffDesc, sizeof(worldConBuffDesc));
	worldConBuffDesc.Usage = D3D11_USAGE_DYNAMIC;
	worldConBuffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	worldConBuffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	worldConBuffDesc.ByteWidth = sizeof(XMFLOAT4X4);
	device->CreateBuffer(&worldConBuffDesc, NULL, &worldCOnBuffer);

	D3D11_INPUT_ELEMENT_DESC layoutDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMALS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	hResult = device->CreateInputLayout(layoutDesc, 3, VertexShader, sizeof(VertexShader), &vertexLayout);
	hResult = device->CreatePixelShader(PixelShader, sizeof(PixelShader), NULL, &pixelShader);
	hResult = device->CreateVertexShader(VertexShader, sizeof(VertexShader), NULL, &vertexShader);

	D3D11_SAMPLER_DESC sampleDesc;
	SecureZeroMemory(&sampleDesc, sizeof(sampleDesc));
	sampleDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.MinLOD = -FLT_MAX;
	sampleDesc.MaxLOD = FLT_MAX;
	sampleDesc.MaxAnisotropy = 1;
	sampleDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	hResult = device->CreateSamplerState(&sampleDesc, &sampleState);

	D3D11_RASTERIZER_DESC rasterDesc;
	SecureZeroMemory(&rasterDesc, sizeof(rasterDesc));
	rasterDesc.AntialiasedLineEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.DepthBias = 0;
	rasterDesc.SlopeScaledDepthBias = 0.0f;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.ScissorEnable = false;
	rasterDesc.MultisampleEnable = false;
	hResult = device->CreateRasterizerState(&rasterDesc, &rasterState);

	D3D11_BLEND_DESC blendDesc;
	SecureZeroMemory(&blendDesc, sizeof(blendDesc));
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	device->CreateBlendState(&blendDesc, &blendState);
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
	deviceContext->VSSetShader(vertexShader, NULL, 0);
	deviceContext->PSSetShader(pixelShader, NULL, 0);
	//deviceContext->PSSetSamplers(0, 1, &sampleState);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deviceContext->RSSetState(rasterState);
	deviceContext->IASetInputLayout(vertexLayout);
	deviceContext->OMSetBlendState(blendState, nullptr, 0xFFFFFFFF);

	D3D11_MAPPED_SUBRESOURCE VPSubResource;
	deviceContext->Map(viewProjConBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &VPSubResource);
	ViewProjectionMatrixs *VramVP;
	VramVP = (ViewProjectionMatrixs*)VPSubResource.pData;
	VramVP->projectionMatrix = projMatrix;
	VramVP->viewMatrix = viewMatrix;
	deviceContext->Unmap(viewProjConBuffer, NULL);
	deviceContext->VSSetConstantBuffers(1, 1, &viewProjConBuffer);
	for (size_t i = 0; i < meshes.size(); i++)
	{
		meshes[i].Process();
	}
	swapChain->Present(0, 0);
}

void Renderer::ShutDown()
{
	Release(device);
	Release(swapChain);
	Release(deviceContext);
	Release(renderTargetView);
	Release(backBufferView);
	Release(depthStencilPointer);
	Release(depthStencilViewport);
	Release(viewProjConBuffer);
	Release(worldCOnBuffer);
	Release(vertexLayout);
	Release(vertexShader);
	Release(pixelShader);
	Release(sampleState);
	Release(rasterState);
	for (size_t i = 0; i < meshes.size(); i++)
	{
		meshes[i].Shutdown();
	}
}