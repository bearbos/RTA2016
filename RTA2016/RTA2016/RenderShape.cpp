#include "stdafx.h"
#include "RenderShape.h"



RenderShape::RenderShape()
{
	numIndices = startIndexLocation = startVertLocation = 0;
}
void RenderShape::Process()
{
	D3D11_MAPPED_SUBRESOURCE currWorldSubResource;
	Renderer::deviceContext->Map(Renderer::worldCOnBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &currWorldSubResource);
	XMFLOAT4X4 *currWorldVram;
	currWorldVram = (XMFLOAT4X4*)currWorldSubResource.pData;
	*currWorldVram = objectsWorld;
	Renderer::deviceContext->Unmap(Renderer::worldCOnBuffer, NULL);
	Renderer::deviceContext->VSSetConstantBuffers(0, 1, &Renderer::worldCOnBuffer);
	Renderer::deviceContext->DrawIndexed(numIndices, startIndexLocation, startVertLocation);
}
void RenderShape::SetObjectsMatrix(XMFLOAT4X4 _objToAdd)
{
	objectsWorld = _objToAdd;
}
