#include "stdafx.h"
#include "RenderSet.h"
#include "RenderMaterial.h"

RenderSet::RenderSet()
{
	stride = offset = 0;
}


RenderSet::~RenderSet()
{
	if (meshIndexBuffer)
		meshIndexBuffer->Release();
	if (meshVertexBuffer)
		meshVertexBuffer->Release();
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