#include "Mesh.h"

// Taken from RTABase

Mesh::Mesh()
{
	jointIndex = 0;
}

Mesh::~Mesh()
{
}


//bool Mesh::LoadTexture(const char* fileName)
//{
//	OGLTexture texture;
//	std::string s;
//	bool result;
//
//	s = fileName;
//	s = "..\\Assets\\" + s;
//
//	result = texture.Load(s.c_str());
//	textures.push_back(texture);
//
//	return result;
//}

std::string& Mesh::GetName()
{
	return name;
}

const std::string& Mesh::GetName() const
{
	return name;
}

std::vector< Mesh::UniqueMeshVertex >& Mesh::GetVertices()
{
	return vertices;
}

const std::vector< Mesh::UniqueMeshVertex >& Mesh::GetVertices() const
{
	return vertices;
}

std::vector< unsigned int >& Mesh::GetIndices()
{
	return indices;
}

const std::vector< unsigned int >& Mesh::GetIndices() const
{
	return indices;
}

std::vector< std::vector< Mesh::JointInfluence >>& Mesh::GetInfluences()
{
	return influences;
}

const std::vector< std::vector< Mesh::JointInfluence >>& Mesh::GetInfluences() const
{
	return influences;
}

std::vector< std::string >& Mesh::GetTextureNames()
{
	return textureNames;
}

const std::vector< std::string >& Mesh::GetTextureNames() const
{
	return textureNames;
}

unsigned int& Mesh::GetJointIndex()
{
	return jointIndex;
}

const unsigned int& Mesh::GetJointIndex() const
{
	return jointIndex;
}

std::vector<Joint>& Mesh::GetSkeleton()
{
	return skeleton;
}

const std::vector<Joint>& Mesh::GetSkeleton() const
{
	return skeleton;
}