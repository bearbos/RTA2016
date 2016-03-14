#pragma once
#include "stdafx.h"
class RenderShape
{
	XMFLOAT4X4 objectsWorld;
public:
	unsigned int numIndices, startIndexLocation, startVertLocation;
	RenderShape();
	void Process();
	void SetObjectsMatrix(XMFLOAT4X4 _objToAdd);
};

