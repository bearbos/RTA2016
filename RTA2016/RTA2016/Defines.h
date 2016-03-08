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
	XMFLOAT2 texture;
	XMFLOAT4 normal;
};