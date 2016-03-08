// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// TODO: reference additional headers your program requires here
#include <string>
#include <vector>
#include <d3d11.h>
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "Winmm.lib")
#include <DirectXMath.h>
#define FBXSDK_SHARED

using namespace DirectX;
#include "Renderer.h"

#include "Defines.h"
#include "RenderShape.h"
#include "RenderMaterial.h"
#include "RenderSet.h"

#include <stdint.h>
#include <sys/stat.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <fbxsdk.h>
#include "Mesh.h"

#include "FBXLoader.h"
