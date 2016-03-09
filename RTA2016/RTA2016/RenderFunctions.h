#pragma once
#include "stdafx.h"

void RenderStuff(RenderNode &_node)
{
	RenderObject &objectNode = (RenderObject&)_node;
	D3D11_MAPPED_SUBRESOURCE currWorldSubResource;
	Renderer::deviceContext->Map(Renderer::worldCOnBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &currWorldSubResource);
	XMFLOAT4X4 *currWorldVram;
	currWorldVram = (XMFLOAT4X4*)currWorldSubResource.pData;
	*currWorldVram = objectNode.objectsWorld;
	Renderer::deviceContext->Unmap(Renderer::worldCOnBuffer, NULL);
	Renderer::deviceContext->VSSetConstantBuffers(0, 1, &Renderer::worldCOnBuffer);
	Renderer::deviceContext->DrawIndexed(objectNode.numIndices, objectNode.startIndexLocation, objectNode.startVertLocation);
}

void RenderTextures(RenderNode &_node)
{
	RenderTexture &textureNode = (RenderTexture&)_node;
	Renderer::deviceContext->PSSetShaderResources(0, 1, &textureNode.texture);
	RenderNode *itr = textureNode.child;
	while (itr != nullptr)
	{
		itr->Process();
		itr = itr->next;
	}
}

void RenderMeshes(RenderNode &_node)
{
	RenderMesh &meshNode = (RenderMesh&)_node;
	Renderer::deviceContext->IASetVertexBuffers(0, 1, &meshNode.meshVertexBuffer, &meshNode.stride, &meshNode.offset);
	Renderer::deviceContext->IASetIndexBuffer(meshNode.meshIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	RenderNode *itr = meshNode.child;
	while (itr != nullptr)
	{
		itr->Process();
		itr = itr->next;
	}
}