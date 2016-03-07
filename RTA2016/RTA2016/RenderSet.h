#pragma once
class RenderMaterial;
class RenderSet
{
	ID3D11Buffer *meshVertexBuffer;
	ID3D11Buffer *meshIndexBuffer;
	std::vector<RenderMaterial> texturesMaterials;
	unsigned int stride, offset;
public:

	RenderSet();
	~RenderSet();
	void Process();
};
