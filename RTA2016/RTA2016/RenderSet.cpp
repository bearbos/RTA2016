#include "stdafx.h"
#include "RenderSet.h"
#include "RenderMaterial.h"

RenderSet::RenderSet()
{
	stride = sizeof(Vertex);
	offset = 0;
}


RenderSet::~RenderSet()
{

}

void RenderSet::Process()
{
	Renderer::deviceContext->IASetVertexBuffers(0, 1, &meshVertexBuffer, &stride, &offset);
	Renderer::deviceContext->IASetIndexBuffer(meshIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	for (size_t i = 0; i < texturesMaterials.size(); i++)
	{
		texturesMaterials[i].Process();
	}
}

void RenderSet::SetVertexBuffer(std::vector<Vertex> _vertexes)
{
	D3D11_BUFFER_DESC buffDesc;
	SecureZeroMemory(&buffDesc, sizeof(buffDesc));
	buffDesc.Usage = D3D11_USAGE_IMMUTABLE;
	buffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffDesc.CPUAccessFlags = NULL;
	buffDesc.ByteWidth = sizeof(Vertex) * (int)_vertexes.size();

	D3D11_SUBRESOURCE_DATA buffData;
	buffData.pSysMem = &_vertexes[0];
	buffData.SysMemPitch = 0;
	buffData.SysMemSlicePitch = 0;
	Renderer::device->CreateBuffer(&buffDesc, &buffData, &meshVertexBuffer);
}
void RenderSet::SetIndexBuffer(std::vector<unsigned int> _indices)
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
void RenderSet::Shutdown()
{
	for (size_t i = 0; i < texturesMaterials.size(); i++)
	{
		texturesMaterials[i].Shutdown();
	}
	if (meshIndexBuffer)
		meshIndexBuffer->Release();
	if (meshVertexBuffer)
		meshVertexBuffer->Release();
}