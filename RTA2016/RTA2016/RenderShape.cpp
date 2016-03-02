#include "stdafx.h"
#include "RenderShape.h"

void RenderShape::Process()
{
	for (unsigned int i = 0; i < objectsWorlds.size(); i++)
	{
		D3D11_MAPPED_SUBRESOURCE currWorldSubResource;
		Renderer::deviceContext->Map(Renderer::worldCOnBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &currWorldSubResource);
		XMFLOAT4X4 *currWorldVram;
		currWorldVram = (XMFLOAT4X4*)currWorldSubResource.pData;
		currWorldVram = &objectsWorlds[i];
		Renderer::deviceContext->Unmap(Renderer::worldCOnBuffer, NULL);

		Renderer::deviceContext->DrawIndexed(numIndices, startIndexLocation, startVertLocation);
	}
}
void RenderShape::AddObjectsMatrix(XMFLOAT4X4 _objToAdd)
{
	objectsWorlds.push_back(_objToAdd);
}
