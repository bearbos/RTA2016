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
	inline void AddShape(RenderShape _object) { objects.push_back(_object); }
	void Shutdown();
};

