#pragma once
class RenderSet;
class RenderNode;
class XTime;
class Timer;
class Renderer
{
	static XMFLOAT4X4 camera;
	static POINT prevMouseLoc;
	static int whichLight;
public:
	static Timer clock;
	static float delta;
	static ID3D11Device *device;
	static IDXGISwapChain *swapChain;
	static ID3D11DeviceContext *deviceContext;
	static ID3D11RenderTargetView *renderTargetView;
	static ID3D11Texture2D *depthStencilPointer;
	static ID3D11DepthStencilView *depthStencilViewport;
	static ID3D11Buffer *viewProjConBuffer;
	static ID3D11Buffer *worldCOnBuffer;
	static ID3D11Buffer *spotLightBuffer;
	static ID3D11Buffer *pointLightBuffer;
	static ID3D11InputLayout *vertexLayout;
	static ID3D11VertexShader *vertexShader;
	static ID3D11PixelShader *pixelShader;
	static ID3D11PixelShader *spotLightShader;
	static ID3D11PixelShader *pointLightShader;
	static ID3D11SamplerState *sampleState;
	static ID3D11RasterizerState *rasterState;
	static D3D11_VIEWPORT mainViewPort;
	static ID3D11BlendState *blendState;
	static std::vector<RenderSet> meshes;
	static RenderNode *head;
	static XMFLOAT4X4 viewMatrix;
	static XMFLOAT4X4 projMatrix;
	Renderer();
	~Renderer();
	static void Initialize(HWND window, unsigned int windHeight, unsigned int windWidth);
	static void ClearScreenToColor(float * color);
	static void Render();
	static void ShutDown();
	static void Update();
};

