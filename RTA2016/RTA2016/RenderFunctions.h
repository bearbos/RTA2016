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

void DoNothing(RenderNode &_node)
{

}

void RenderTextures(RenderNode &_node)
{
	RenderTexture &textureNode = (RenderTexture&)_node;
	Renderer::deviceContext->PSSetShaderResources(0, 1, &textureNode.texture.p);
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
	Renderer::deviceContext->VSSetShader(meshNode.vertShader.p, NULL, 0);
	Renderer::deviceContext->IASetVertexBuffers(0, 1, &meshNode.meshVertexBuffer.p, &meshNode.stride, &meshNode.offset);
	Renderer::deviceContext->IASetIndexBuffer(meshNode.meshIndexBuffer.p, DXGI_FORMAT_R32_UINT, 0);
	RenderNode *itr = meshNode.child;
	while (itr != nullptr)
	{
		itr->Process();
		itr = itr->next;
	}
}
void RenderAndMovement(RenderNode &_node)
{
	RenderObject &objectNode = (RenderObject&)_node;
	if (GetAsyncKeyState(VK_UP))
	{
		objectNode.objectsWorld._43 += MOVEMENTSPEED * Renderer::delta;
	}
	if (GetAsyncKeyState(VK_DOWN))
	{
		objectNode.objectsWorld._43 += -MOVEMENTSPEED * Renderer::delta;
	}
	if (GetAsyncKeyState(VK_LEFT))
	{
		objectNode.objectsWorld._41 += -MOVEMENTSPEED * Renderer::delta;
	}
	if (GetAsyncKeyState(VK_RIGHT))
	{
		objectNode.objectsWorld._41 += MOVEMENTSPEED * Renderer::delta;
	}
	D3D11_MAPPED_SUBRESOURCE currWorldSubResource;
	Renderer::deviceContext->Map(Renderer::worldCOnBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &currWorldSubResource);
	XMFLOAT4X4 *currWorldVram;
	currWorldVram = (XMFLOAT4X4*)currWorldSubResource.pData;
	*currWorldVram = objectNode.objectsWorld;
	Renderer::deviceContext->Unmap(Renderer::worldCOnBuffer, NULL);
	Renderer::deviceContext->VSSetConstantBuffers(0, 1, &Renderer::worldCOnBuffer);
	Renderer::deviceContext->DrawIndexed(objectNode.numIndices, objectNode.startIndexLocation, objectNode.startVertLocation);
}