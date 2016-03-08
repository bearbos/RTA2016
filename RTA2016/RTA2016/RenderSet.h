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
	void SetVertexBuffer(std::vector<uniqueVertex> _vertexes);
	void SetIndexBuffer(std::vector<unsigned int> _indices);
	inline void AddMaterial(RenderMaterial _mat){ texturesMaterials.push_back(_mat); }
	void Shutdown();
};
