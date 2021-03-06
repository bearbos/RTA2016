#pragma once
//#include "stdafx.h"
#include <fbxsdk.h>


struct ViewProjectionMatrixs
{
	XMFLOAT4X4 viewMatrix;
	XMFLOAT4X4 projectionMatrix;

};
struct uniqueVertex
{
	XMFLOAT4 position;
	XMFLOAT4 normal;
	unsigned int indexs[4];
	float weights[4];
	XMFLOAT2 texture;
};
struct Vertex
{
	XMFLOAT4 pos;
	XMFLOAT4 norm;
	XMFLOAT2 uv;
};
struct BlendingIndexWeightPair
{
	float BlendingIndex;
	float BlendingWeight;
};
struct JointKeys
{
	float time;
	XMFLOAT4X4 local;
};
struct Joint
{
	XMFLOAT4X4 World;
	XMFLOAT4X4 Local;
	XMFLOAT4X4 GlobalBind;
	unsigned int parentIndex;
	char name[128];
	bool bDirty;
	std::vector<BlendingIndexWeightPair> SkinWeight;
	std::vector<JointKeys> frames;
};
struct ArrayOfMatrixes
{
	XMFLOAT4X4 matrix[50];
};
