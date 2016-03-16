#pragma once

#include "stdafx.h"

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
	vector<Joint>* skeletonPTR;
	vector<FbxNode *> skeletonNodes;
	

private:
	vector<string> m_filePaths;
	vector<XMFLOAT4X4> m_worldMatrices;
	void FBXBinaryConvert(const char * _fileName, const char * _binName);
	bool LoadMesh(FbxMesh* meshAttribute, Mesh& mesh, vector<unsigned int>& controlPointIndices);
	bool LoadTexture(FbxMesh* meshAttribute, Mesh& mesh);
	void LoadBinary(const char * _binName);
	void ProcessSkeletonHierarchy(FbxNode* _rootNodeIn);
	void ProcessSkeletonHierarchyRecursively(FbxNode* _nodeIn, unsigned int _myIndex, int _parentIndexIn);
	void ProcessJointsAndAnimations(FbxMesh* _nodeIn);
	FbxAMatrix GetGeometryTransformation(FbxNode* inNode);
	unsigned int FindJointIndexUsingName(const string& _JointNameIn);
	XMFLOAT4X4 fbxToFloatMatrix(FbxAMatrix& _matrixIn);
};

