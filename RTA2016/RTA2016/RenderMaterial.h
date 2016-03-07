#pragma once
class RenderShape;
class RenderMaterial
{
	std::vector<RenderShape> objects;
	ID3D11ShaderResourceView *texture;
public:
	RenderMaterial();
	~RenderMaterial();
	void Process();
};

