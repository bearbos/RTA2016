#pragma once
#include "stdafx.h"
struct ViewProjectionMatrixs
{
	XMFLOAT4X4 viewMatrix;
	XMFLOAT4X4 projectionMatrix;

};
struct uniqueVertex
{
	XMFLOAT4 position;
	XMFLOAT4 normal;
	XMFLOAT2 texture;
};
struct Vertex
{
	XMFLOAT4 pos;
	XMFLOAT4 norm;
	XMFLOAT2 uv;
};