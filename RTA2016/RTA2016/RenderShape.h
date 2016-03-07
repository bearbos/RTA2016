#pragma once
#include "stdafx.h"
class RenderShape
{
	XMFLOAT4X4 objectsWorld;
	unsigned int numIndices, startIndexLocation, startVertLocation;
public:
	RenderShape();
	void Process();
	void AddObjectsMatrix(XMFLOAT4X4 _objToAdd);
};

