#include "stdafx.h"
#include "RenderMaterial.h"


RenderMaterial::RenderMaterial()
{
}


RenderMaterial::~RenderMaterial()
{

}

void RenderMaterial::Process()
{
	Renderer::deviceContext->PSSetShaderResources(0, 1, &texture);
	for (size_t i = 0; i < objects.size(); i++)
	{
		objects[i].Process();
	}
}
void RenderMaterial::Shutdown()
{
	if (texture)
		texture->Release();
}