#pragma once

using std::fstream;
using std::vector;
using std::string;
using std::FILE;

class FBXLoader
{
public:
	FBXLoader();
	~FBXLoader();
	void ReadIn(const char * _fileName);
	void FBXBinaryCheck();

	

private:
	vector<string> m_filePaths;
	void FBXBinaryConvert(const char * _fileName, const char * _binName);
	bool LoadMesh(FbxMesh* meshAttribute, Mesh& mesh, vector<unsigned int>& controlPointIndices);
	bool LoadTexture(FbxMesh* meshAttribute, Mesh& mesh);
	void LoadBinary(const char * _binName);
};

