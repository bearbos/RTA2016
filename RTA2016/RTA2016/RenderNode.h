#pragma once
#include <atlbase.h>
class RenderNode;
typedef void(*RenderFunc)(RenderNode &);
class RenderNode
{
public:
	RenderNode *next = nullptr;
	RenderFunc func;
	RenderNode *child = nullptr;
	inline void Process(){ func(*this); }
};

class RenderObject : public RenderNode
{
public:
	XMFLOAT4X4 objectsWorld;
	unsigned int numIndices, startIndexLocation = 0, startVertLocation = 0;
};

class RenderTexture : public RenderNode
{
public:
	CComPtr<ID3D11ShaderResourceView> texture;
};

class RenderMesh : public RenderNode
{
public:
	CComPtr<ID3D11Buffer> meshVertexBuffer;
	CComPtr<ID3D11Buffer> meshIndexBuffer;
	unsigned int stride = 0, offset = 0;

	void SetVertexBuffer(std::vector<uniqueVertex> _vertexes)
	{
		D3D11_BUFFER_DESC buffDesc;
		SecureZeroMemory(&buffDesc, sizeof(buffDesc));
		buffDesc.Usage = D3D11_USAGE_IMMUTABLE;
		buffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		buffDesc.CPUAccessFlags = NULL;
		buffDesc.ByteWidth = sizeof(uniqueVertex) * (int)_vertexes.size();

		D3D11_SUBRESOURCE_DATA buffData;
		buffData.pSysMem = &_vertexes[0];
		buffData.SysMemPitch = 0;
		buffData.SysMemSlicePitch = 0;
		Renderer::device->CreateBuffer(&buffDesc, &buffData, &meshVertexBuffer);
	}
	void SetIndexBuffer(std::vector<unsigned int> _indices)
	{
		D3D11_BUFFER_DESC indexBuffDesc;
		SecureZeroMemory(&indexBuffDesc, sizeof(indexBuffDesc));
		indexBuffDesc.Usage = D3D11_USAGE_IMMUTABLE;
		indexBuffDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBuffDesc.CPUAccessFlags = NULL;
		indexBuffDesc.ByteWidth = sizeof(unsigned int) * (int)_indices.size();

		D3D11_SUBRESOURCE_DATA indexData;
		indexData.pSysMem = &_indices[0];
		indexData.SysMemPitch = 0;
		indexData.SysMemSlicePitch = 0;
		Renderer::device->CreateBuffer(&indexBuffDesc, &indexData, &meshIndexBuffer);
	}
};