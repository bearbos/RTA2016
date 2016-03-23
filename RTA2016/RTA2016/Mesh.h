#pragma once

// taken from RTABase

//struct Joint;

class Mesh
{
public:

	struct TextureCoord
	{
		float u, v;
	};

	struct JointInfluence	// not sure what this is yet
	{
		unsigned int jointIndex;
		float weight;
	};

	struct UniqueMeshVertex
	{
		XMFLOAT3 uVPos;
		XMFLOAT3 uVNorm;
		TextureCoord textCoord;

		bool operator==(const UniqueMeshVertex& v) const
		{
			return uVPos.x == v.uVPos.x &&
				uVPos.y == v.uVPos.y &&
				uVPos.z == v.uVPos.z /*&&
				uVNorm.x == v.uVNorm.x &&
				uVNorm.y == v.uVNorm.y &&
				uVNorm.z == v.uVNorm.z &&
				textCoord.u == v.textCoord.u &&
				textCoord.v == v.textCoord.v*/;
		}
	};

	Mesh();
	~Mesh();


	bool LoadTexture(const char* fileName);

	std::string& GetName();
	const std::string& GetName() const;

	std::vector< UniqueMeshVertex >& GetVertices();
	const std::vector< UniqueMeshVertex >& GetVertices() const;

	std::vector< unsigned int >& GetIndices();
	const std::vector< unsigned int >& GetIndices() const;

	std::vector< std::vector< JointInfluence >>& GetInfluences();
	const std::vector< std::vector< JointInfluence >>& GetInfluences() const;

	std::vector< std::string >& GetTextureNames();
	const std::vector< std::string >& GetTextureNames() const;

	unsigned int& GetJointIndex();
	const unsigned int& GetJointIndex() const;

	std::vector<Joint>& GetSkeleton();
	const std::vector<Joint>& GetSkeleton() const;


private:
	std::string name;

	std::vector<Joint> skeleton;

	std::vector< UniqueMeshVertex > vertices;

	std::vector< unsigned int > indices;

	std::vector< std::vector< JointInfluence >> influences;

	std::vector< std::string > textureNames;
	//std::vector< ID3D11Texture2D > textures;

	unsigned int jointIndex;
};


