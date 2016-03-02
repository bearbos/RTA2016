#pragma once
#include "stdafx.h"
class RenderShape
{
	std::vector<XMFLOAT4X4> objectsWorlds;
	unsigned int numIndices, startIndexLocation, startVertLocation;
public:
	void Process();
	void AddObjectsMatrix(XMFLOAT4X4 _objToAdd);
};

