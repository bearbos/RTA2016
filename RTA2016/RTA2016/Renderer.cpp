#include "stdafx.h"
#include "Renderer.h"
#include "VertexShader.csh"
#include "PixelShader.csh"
#include "SpotLight.csh"
#include "PointLight.csh"
#include "DirLight.csh"
#include "RenderFunctions.h"
#include "Interpolator.h"
#include "Animation.h"
#include "keyFrame.h"
#include "AnimationShader.csh"
#define Release(x) { if(x) {x->Release(); x =0;} }
ID3D11Device *Renderer::device = 0; //Released
IDXGISwapChain *Renderer::swapChain = 0; //Released
ID3D11DeviceContext *Renderer::deviceContext = 0; //Released
D3D11_VIEWPORT Renderer::mainViewPort; //Doesn't need to be Released
ID3D11RenderTargetView *Renderer::renderTargetView = 0; //Released
ID3D11Texture2D *Renderer::depthStencilPointer = 0; //Released
ID3D11DepthStencilView *Renderer::depthStencilViewport = 0; //Released
XMFLOAT4X4 Renderer::viewMatrix;//Doesn't need to be Released
XMFLOAT4X4 Renderer::projMatrix;//Doesn't need to be Released
ID3D11Buffer *Renderer::viewProjConBuffer = 0; //Released
ID3D11Buffer *Renderer::worldCOnBuffer = 0; //Released
ID3D11Buffer *Renderer::spotLightBuffer = 0; //Released
ID3D11InputLayout *Renderer::vertexLayout = 0; //Released
ID3D11PixelShader *Renderer::pixelShader = 0; //Released
ID3D11PixelShader *Renderer::spotLightShader = 0; //Released
ID3D11SamplerState *Renderer::sampleState = 0; //Released
ID3D11RasterizerState *Renderer::rasterState = 0; //Released
std::vector<RenderSet> Renderer::meshes; //Doesn't need to be Released
ID3D11BlendState *Renderer::blendState = 0; //Released
RenderNode *Renderer::head = 0; //Doesn't need to be Released
XMFLOAT4X4 Renderer::camera; //Doesn't need to be Released
POINT Renderer::prevMouseLoc; //Doesn't need to be Released
Timer Renderer::clock; //Doesn't need to be Released
int Renderer::whichLight = 0;//Doesn't need to be Released
float Renderer::delta = 0; //Doesn't need to be Released
ID3D11Buffer *Renderer::pointLightBuffer = 0; //Released
ID3D11PixelShader *Renderer::pointLightShader = 0; //Released
ID3D11PixelShader *Renderer::directionLightShader = 0; //Released
ID3D11Buffer *Renderer::directionLightBuffer = 0; //Released
XMFLOAT4 Renderer::lightDirection; //Doesn't need to be Released
bool Renderer::pressed = false; //Doesn't need to be Released
std::vector<std::vector<Mesh>> Renderer::Objects; //Doesn't need to be Released
Interpolator Renderer::interp; //Doesn't need to be Released
Animation Renderer::animations; //Doesn't need to be Released
bool Renderer::animationBool; //Doesn't need to be Released
float Renderer::animationTimer; //Doesn't need to be Released
ID3D11Buffer *Renderer::animationConBuffer; //Released
unsigned int Renderer::keyFrameIndex;
void Renderer::Initialize(HWND window, unsigned int windHeight, unsigned int windWidth)
{
	keyFrameIndex = 0;
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
	chainDesc.SampleDesc.Count = 1;
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
	mainViewPort.MinDepth = 0;
	mainViewPort.Height = (float)windHeight;
	mainViewPort.Width = (float)windWidth;

	D3D11_BUFFER_DESC VPBuffDesc;
	SecureZeroMemory(&VPBuffDesc, sizeof(VPBuffDesc));
	VPBuffDesc.Usage = D3D11_USAGE_DYNAMIC;
	VPBuffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	VPBuffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	VPBuffDesc.ByteWidth = sizeof(ViewProjectionMatrixs);
	hResult = device->CreateBuffer(&VPBuffDesc, NULL, &viewProjConBuffer);


	XMMATRIX tempMatrix = XMMatrixIdentity();
	XMStoreFloat4x4(&projMatrix, tempMatrix);
	float yscale = 1.0f / tan(XMConvertToRadians(32.5));
	float xScale = yscale / ((float)windWidth / ((float)windHeight));

	projMatrix._11 = xScale;
	projMatrix._22 = yscale;
	projMatrix._33 = 1000 / (1000 - 0.1f);
	projMatrix._43 = -(1000 * 0.1f) / (1000 - 0.1f);
	projMatrix._44 = 0;
	projMatrix._34 = 1;
	tempMatrix = XMMatrixIdentity();
	XMStoreFloat4x4(&camera, tempMatrix);
	camera._43 = 200.0f;
	camera._42 = 100.0f;
	//camera._43 = -20.0f;
	//camera._42 = 10.0f;
	XMStoreFloat4x4(&camera, XMMatrixMultiply(XMMatrixRotationY(XMConvertToRadians(180.0f)), XMLoadFloat4x4(&camera)));
	XMStoreFloat4x4(&viewMatrix, XMMatrixInverse(nullptr, XMLoadFloat4x4(&camera)));

	D3D11_BUFFER_DESC worldConBuffDesc;
	SecureZeroMemory(&worldConBuffDesc, sizeof(worldConBuffDesc));
	worldConBuffDesc.Usage = D3D11_USAGE_DYNAMIC;
	worldConBuffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	worldConBuffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	worldConBuffDesc.ByteWidth = sizeof(XMFLOAT4X4);
	device->CreateBuffer(&worldConBuffDesc, NULL, &worldCOnBuffer);
	device->CreateBuffer(&worldConBuffDesc, NULL, &spotLightBuffer);

	worldConBuffDesc.ByteWidth = sizeof(XMFLOAT4);
	device->CreateBuffer(&worldConBuffDesc, NULL, &pointLightBuffer);
	device->CreateBuffer(&worldConBuffDesc, NULL, &directionLightBuffer);
	worldConBuffDesc.ByteWidth = sizeof(ArrayOfMatrixes);
	device->CreateBuffer(&worldConBuffDesc, NULL, &animationConBuffer);
	D3D11_INPUT_ELEMENT_DESC layoutDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMALS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "INDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	hResult = device->CreateInputLayout(layoutDesc, 5, VertexShader, sizeof(VertexShader), &vertexLayout);
	hResult = device->CreatePixelShader(PixelShader, sizeof(PixelShader), NULL, &pixelShader);
	hResult = device->CreatePixelShader(SpotLight, sizeof(SpotLight), NULL, &spotLightShader);
	hResult = device->CreatePixelShader(PointLight, sizeof(PointLight), NULL, &pointLightShader);
	hResult = device->CreatePixelShader(DirLight, sizeof(DirLight), NULL, &directionLightShader);

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

	D3D11_TEXTURE2D_DESC depthTextureDesc = {
		windWidth,
		windHeight,
		1,
		1,
		DXGI_FORMAT_D32_FLOAT,
		{ 1, 0 },
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_DEPTH_STENCIL,
		0,
		0
	};
	device->CreateTexture2D(&depthTextureDesc, NULL, &depthStencilPointer);
	hResult = device->CreateDepthStencilView(depthStencilPointer, NULL, &depthStencilViewport);

	GetCursorPos(&prevMouseLoc);
	lightDirection.x = 3.0f;
	lightDirection.y = -2.0f;
	lightDirection.z = 3.0f;
	lightDirection.w = 0.0f;
	XMStoreFloat4(&lightDirection, XMVector4Normalize(XMLoadFloat4(&lightDirection)));
	for (size_t i = 0; i < Objects.size(); i++)
	{
		for (size_t j = 0; j < Objects[i].size(); j++)
		{
			vector<uniqueVertex> vertexes;
			for (size_t k = 0; k < Objects[i][j].GetVertices().size(); k++)
			{
				uniqueVertex tempVertex;
				tempVertex.position.x = Objects[i][j].GetVertices()[k].uVPos.x;
				tempVertex.position.y = Objects[i][j].GetVertices()[k].uVPos.y;
				tempVertex.position.z = Objects[i][j].GetVertices()[k].uVPos.z;
				tempVertex.position.w = 1.0f;
				tempVertex.normal.x = Objects[i][j].GetVertices()[k].uVNorm.x;
				tempVertex.normal.y = Objects[i][j].GetVertices()[k].uVNorm.y;
				tempVertex.normal.z = Objects[i][j].GetVertices()[k].uVNorm.z;
				tempVertex.normal.w = 1;
				tempVertex.texture.x = Objects[i][j].GetVertices()[k].textCoord.u;
				tempVertex.texture.y = Objects[i][j].GetVertices()[k].textCoord.v;
				vertexes.push_back(tempVertex);
			}
			for (unsigned int m = 0; m < vertexes.size(); m++)
			{
				std::vector<float> tempWeight;
				std::vector<unsigned int> tempIndexs;
				for (unsigned int k = 0; k < Objects[i][j].GetSkeleton().size(); k++)
				{
					for (unsigned int l = 0; l < Objects[i][j].GetSkeleton()[k].SkinWeight.size(); l++)
					{
						if (m == (unsigned int)Objects[i][j].GetSkeleton()[k].SkinWeight[l].BlendingIndex)
						{
							tempWeight.push_back(Objects[i][j].GetSkeleton()[k].SkinWeight[l].BlendingWeight);
							tempIndexs.push_back(k);
						}
					}
				}
				for (unsigned int k = 0; k < 4; k++)
				{
					if (tempIndexs.size() > k)
					{
						vertexes[m].indexs[k] = tempIndexs[k];
						vertexes[m].weights[k] = tempWeight[k];
					}
					else
					{
						vertexes[m].indexs[k] = 0;
						vertexes[m].weights[k] = 0;
					}
				}
				vertexes[m].weights[3] = 1.0f - vertexes[m].weights[0] - vertexes[m].weights[1] - vertexes[m].weights[2];
			}
			RenderMesh *meshR = new RenderMesh;
			meshR->stride = sizeof(uniqueVertex);
			meshR->SetVertexBuffer(vertexes);
			meshR->SetIndexBuffer(Objects[i][j].GetIndices());
			meshR->func = RenderMeshes;
			meshR->name = Objects[i][j].GetName();
			if ((meshR->name == "Box_BindPose.tribal" || meshR->name == "Teddy_Idle.tribal"))
				device->CreateVertexShader(AnimationShader, sizeof(AnimationShader), NULL, &meshR->vertShader.p);
			else
				device->CreateVertexShader(VertexShader, sizeof(VertexShader), NULL, &meshR->vertShader.p);
			RenderTexture *texterR = new RenderTexture;
			texterR->func = RenderTextures;

			XMFLOAT4X4 objectMatrix;
			XMMATRIX tempMatrix = XMMatrixIdentity();
			//XMMATRIX tempMatrix = XMMatrixRotationY(XMConvertToRadians(180));
			XMStoreFloat4x4(&objectMatrix, tempMatrix);
			//objectMatrix._41 = -150.0f;

			meshR->next = head;
			head = meshR;
			meshR->child = texterR;
			if (meshR->name == "Teddy_Idle.tribal")
			{
				DirectX::CreateDDSTextureFromFile(Renderer::device, L"Teddy_D.dds", NULL, &texterR->texture);
			}
			else if (meshR->name == "Box_BindPose.tribal" || meshR->name == "sphere.tribal")
			{
				DirectX::CreateDDSTextureFromFile(Renderer::device, L"TestCube.dds", NULL, &texterR->texture);
			}
			if (meshR->name == "Teddy_Idle.tribal" || meshR->name == "Box_BindPose.tribal")
			{
				RenderObject *objectR = new RenderObject;
				objectR->func = RenderAndMovement;
				objectR->numIndices = Objects[i][j].GetIndices().size();
				texterR->child = objectR;
				objectR->objectsWorld = objectMatrix;
			}
			else if (meshR->name == "sphere.tribal")
			{
				for (size_t o = 0; o < Objects.size(); o++)
				{
					if (Objects[o][0].GetName() == "Teddy_Idle.tribal" || Objects[o][0].GetName() == "Box_BindPose.tribal")
					{
						for (size_t a = 0; a < Objects[o][0].GetSkeleton().size(); a++)
						{
							RenderObject *objectR = new RenderObject;
							objectR->func = DoNothing;
							objectR->numIndices = Objects[i][j].GetIndices().size();
							objectMatrix = Objects[o][0].GetSkeleton()[a].GlobalBind;
							objectMatrix._42 *= -1;
							objectR->objectsWorld = objectMatrix;
							objectR->next = texterR->child;
							texterR->child = objectR;
						}
						break;
					}
				}
			}
		}
	}
	std::vector<KeyFrame> tempFrames;
	for (unsigned int objectIndex = 0; objectIndex < Objects.size(); objectIndex++)
	{
		for (unsigned int objectMeshIndex = 0; objectMeshIndex < Objects[objectIndex].size(); objectMeshIndex++)
		{
			if (Objects[objectIndex][objectMeshIndex].GetName() == "Teddy_Idle.tribal" || Objects[objectIndex][objectMeshIndex].GetName() == "Box_BindPose.tribal")
			{
				for (unsigned int numKeyFrames = 0; numKeyFrames < Objects[objectIndex][objectMeshIndex].GetSkeleton()[0].frames.size(); numKeyFrames++)
				{
					KeyFrame tempFrame;
					tempFrame.numBones = (int)Objects[objectIndex][objectMeshIndex].GetSkeleton()[0].frames.size();
					for (unsigned int jointIndex = 0; jointIndex < Objects[objectIndex][objectMeshIndex].GetSkeleton().size(); jointIndex++)
					{
						if (Objects[objectIndex][objectMeshIndex].GetSkeleton()[jointIndex].frames.size() > 0)
						{
							tempFrame.world[jointIndex] = Objects[objectIndex][objectMeshIndex].GetSkeleton()[jointIndex].frames[numKeyFrames].local;
							tempFrame.time = Objects[objectIndex][objectMeshIndex].GetSkeleton()[0].frames[numKeyFrames].time;
							tempFrame.numBones = 37;
						}
					}
					tempFrames.push_back(tempFrame);
				}
			}
		}
	}
	animations.Init(tempFrames.size(), tempFrames[tempFrames.size() - 1].time);
	animations.completed = false;
	animations.keyFrames = tempFrames;
	animationBool = true;
	animationTimer = 0.0f;
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
	deviceContext->ClearDepthStencilView(depthStencilViewport, D3D11_CLEAR_DEPTH, 1.0f, 0);
}
void Renderer::Render()
{
	//if (((RenderMesh*)head)->name == "sphere.tribal")
	//{
	//	auto itr = head->child->child;
	//	for (unsigned int i = 0; itr; i++, itr = itr->next)
	//	{
	//		if (!(i == 6 || i == 21 || i == 31 || i == 36))
	//			((RenderObject*)itr)->objectsWorld = animations.keyFrames[keyFrameIndex].world[i];
	//	}
	//}
	//else
	//{
	//	auto itr = head->next->child->child;
	//	for (unsigned int i = 0; itr; i++, itr = itr->next)
	//	{
	//		((RenderObject*)itr)->objectsWorld = animations.keyFrames[keyFrameIndex].world[i];
	//	}
	//}
	switch (whichLight)
	{
	case 0:
		deviceContext->PSSetShader(pixelShader, NULL, 0);
		break;
	case 1:
		D3D11_MAPPED_SUBRESOURCE spotLightSubResource;
		Renderer::deviceContext->Map(spotLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &spotLightSubResource);
		XMFLOAT4X4 *spotLightVram;
		spotLightVram = (XMFLOAT4X4*)spotLightSubResource.pData;
		*spotLightVram = camera;
		Renderer::deviceContext->Unmap(spotLightBuffer, NULL);
		deviceContext->PSSetShader(spotLightShader, NULL, 0);
		deviceContext->PSSetConstantBuffers(0, 1, &spotLightBuffer);
		break;
	case 2:
		deviceContext->PSSetShader(pointLightShader, NULL, 0);
		deviceContext->PSSetConstantBuffers(0, 1, &pointLightBuffer);
		break;
	case 3:
		D3D11_MAPPED_SUBRESOURCE dirLightResource;
		Renderer::deviceContext->Map(directionLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &dirLightResource);
		XMFLOAT4 *dirLightVram;
		dirLightVram = (XMFLOAT4*)dirLightResource.pData;
		*dirLightVram = lightDirection;
		Renderer::deviceContext->Unmap(directionLightBuffer, NULL);
		deviceContext->PSSetShader(directionLightShader, NULL, 0);
		deviceContext->PSSetConstantBuffers(0, 1, &directionLightBuffer);
		break;
	default:
		break;
	}
	deviceContext->PSSetSamplers(0, 1, &sampleState);
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
	deviceContext->VSSetConstantBuffers(1, 1, &viewProjConBuffer);
	deviceContext->Unmap(viewProjConBuffer, NULL);
	RenderNode *itr = head;
	while (itr)
	{
		itr->Process();
		itr = itr->next;
	}
	for (size_t i = 0; i < meshes.size(); i++)
	{
		meshes[i].Process();
	}
	swapChain->Present(0, 0);
}

void Renderer::ShutDown()
{
	for (size_t i = 0; i < meshes.size(); i++)
	{
		meshes[i].Shutdown();
	}
	RenderNode *itr = head;
	while (itr)
	{
		RenderNode *textures = itr->child;
		while (textures)
		{
			RenderNode *shapes = textures->child;
			while (shapes)
			{
				RenderNode *temp = shapes;
				shapes = shapes->next;
				delete temp;
			}
			RenderTexture *tempText = (RenderTexture*)textures;
			textures = textures->next;
			delete tempText;
		}
		RenderMesh *tempMesh = (RenderMesh*)itr;
		itr = itr->next;
		delete tempMesh;
	}
	Release(device);
	Release(swapChain);
	Release(deviceContext);
	Release(renderTargetView);
	Release(depthStencilPointer);
	Release(depthStencilViewport);
	Release(viewProjConBuffer);
	Release(worldCOnBuffer);
	Release(vertexLayout);
	Release(pixelShader);
	Release(sampleState);
	Release(rasterState);
	Release(blendState);
	Release(spotLightBuffer);
	Release(spotLightShader);
	Release(pointLightBuffer);
	Release(pointLightShader);
	Release(directionLightBuffer);
	Release(directionLightShader);
	Release(animationConBuffer);
}

void Renderer::Update()
{

	XMFLOAT4X4 translate, rotationX, rotationY;
	XMMATRIX tempMatrix = XMMatrixIdentity();
	XMStoreFloat4x4(&translate, tempMatrix);
	XMStoreFloat4x4(&rotationX, tempMatrix);
	XMStoreFloat4x4(&rotationY, tempMatrix);



	POINT currMousePos;
	GetCursorPos(&currMousePos);
	if (GetAsyncKeyState('W'))
	{
		translate._43 = MOVEMENTSPEED * delta;
	}
	if (GetAsyncKeyState('S'))
	{
		translate._43 = -MOVEMENTSPEED * delta;
	}
	if (GetAsyncKeyState('A'))
	{
		translate._41 = -MOVEMENTSPEED * delta;
	}
	if (GetAsyncKeyState('D'))
	{
		translate._41 = MOVEMENTSPEED * delta;
	}
	if (GetAsyncKeyState('Q'))
	{
		translate._42 = MOVEMENTSPEED * delta;
	}
	if (GetAsyncKeyState('E'))
	{
		translate._42 = -MOVEMENTSPEED * delta;
	}
	if (GetAsyncKeyState(VK_RBUTTON))
	{
		float diffX = (float)(prevMouseLoc.x - currMousePos.x)* delta;
		float diffY = (float)(prevMouseLoc.y - currMousePos.y)* delta;

		XMStoreFloat4x4(&rotationY, XMMatrixMultiply(XMLoadFloat4x4(&rotationY), XMMatrixRotationY(XMConvertToRadians(20 * diffX))));
		XMStoreFloat4x4(&rotationX, XMMatrixMultiply(XMLoadFloat4x4(&rotationX), XMMatrixRotationX(XMConvertToRadians(20 * diffY))));
	}

	float x, y, z, w;
	x = camera._41;
	y = camera._42;
	z = camera._43;
	w = camera._44;
	camera._41 = 0;
	camera._42 = 0;
	camera._43 = 0;
	camera._44 = 0;
	XMStoreFloat4x4(&camera, XMMatrixMultiply(XMLoadFloat4x4(&camera), XMLoadFloat4x4(&rotationY)));
	XMStoreFloat4x4(&camera, XMMatrixMultiply(XMLoadFloat4x4(&rotationX), XMLoadFloat4x4(&camera)));
	camera._41 = x;
	camera._42 = y;
	camera._43 = z;
	camera._44 = w;
	XMStoreFloat4x4(&camera, XMMatrixMultiply(XMLoadFloat4x4(&translate), XMLoadFloat4x4(&camera)));
	XMStoreFloat4x4(&viewMatrix, XMMatrixInverse(nullptr, XMLoadFloat4x4(&camera)));
	prevMouseLoc = currMousePos;

	if (GetAsyncKeyState('1')) //Switches to no light
	{
		whichLight = 0;
	}
	if (GetAsyncKeyState('2')) //Switches to spotLight
	{
		whichLight = 1;
	}
	if (GetAsyncKeyState('3')) //Switches to pointLight
	{
		whichLight = 2;
	}
	if (GetAsyncKeyState('4')) //Switches to direction Light
	{
		whichLight = 3;
	}
	if (GetAsyncKeyState(VK_TAB) && !pressed) //increments lights
	{
		keyFrameIndex++;
		keyFrameIndex %= 60;
		//whichLight++;
		//whichLight %= 4;
		pressed = true;
	}
	else if (!GetAsyncKeyState(VK_TAB) && pressed)
	{
		pressed = false;
	}
	if (GetAsyncKeyState('N'))
	{
		if (((RenderMesh*)head)->name == "Teddy_Idle.tribal" || ((RenderMesh*)head)->name == "Box_BindPose.tribal")
		{
			head->child->child->func = RenderStuff;
			RenderNode *itr = head->next->child->child;
			while (itr)
			{
				itr->func = DoNothing;
				itr = itr->next;
			}
		}
		else
		{
			head->next->child->child->func = RenderStuff;
			RenderNode *itr = head->child->child;
			while (itr)
			{
				itr->func = DoNothing;
				itr = itr->next;
			}
		}
	}
	if (GetAsyncKeyState('J'))
	{
		if (((RenderMesh*)head)->name == "Teddy_Idle.tribal" || ((RenderMesh*)head)->name == "Box_BindPose.tribal")
		{
			head->child->child->func = DoNothing;
			RenderNode *itr = head->next->child->child;
			while (itr)
			{
				itr->func = RenderStuff;
				itr = itr->next;
			}
		}
		else
		{
			head->next->child->child->func = DoNothing;
			RenderNode *itr = head->child->child;
			while (itr)
			{
				itr->func = RenderStuff;
				itr = itr->next;
			}
		}
	}
	if (animationBool)
	{
		animationTimer += delta;
		if (animationTimer > animations.GetTotalTime())
			animationTimer = 0.05f;
		interp.SetAnimation(&animations);
		interp.Update(animationTimer);
		D3D11_MAPPED_SUBRESOURCE animBufferResource;
		Renderer::deviceContext->Map(animationConBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &animBufferResource);
		ArrayOfMatrixes *animeVram;
		animeVram = (ArrayOfMatrixes*)animBufferResource.pData;
		for (unsigned int i = 0; i < Objects[0][0].GetSkeleton().size(); i++)
		{
			XMFLOAT4X4 temp;
			XMStoreFloat4x4(&temp, XMMatrixMultiply(XMLoadFloat4x4(&Objects[0][0].GetSkeleton()[i].GlobalBind), XMLoadFloat4x4(&interp.world[i])));
			//animeVram->matrix[i] = interp.world[i];
			animeVram->matrix[i] = temp;
		}
		Renderer::deviceContext->Unmap(animationConBuffer, NULL);
		deviceContext->VSSetConstantBuffers(2, 1, &animationConBuffer);
		if (((RenderMesh*)head)->name == "sphere.tribal")
		{
			auto itr = head->child->child;
			for (unsigned int i = 0; itr; i++, itr = itr->next)
			{
				if (!(i == 6 || i == 21 || i == 31 || i == 36))
					((RenderObject*)itr)->objectsWorld = interp.world[i];
			}
		}
		else
		{
			auto itr = head->next->child->child;
			for (unsigned int i = 0; itr; i++, itr = itr->next)
			{
				if (!(i == 6 || i == 21 || i == 31 || i == 36))
					((RenderObject*)itr)->objectsWorld = interp.world[i];
			}
		}
	}
}