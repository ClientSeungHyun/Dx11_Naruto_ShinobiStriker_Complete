#pragma once

#pragma warning (disable : 4251)
#pragma warning(disable : 5208) //����ü ��� ����

#include <d3d11.h>

#include "Effects11/d3dx11effect.h"
#include "DirectXTK\DDSTextureLoader.h"
#include "DirectXTK\WICTextureLoader.h"

#include "assimp/scene.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"

#include <DirectXCollision.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
using namespace DirectX;

/* d3dx11�� ���ԵǾ��־����� �ʹ� ���̴�. */
/* ���� d3dx11���ԵǾ��־��� �پ��� ����� �����ϴ� ��� + ���̺귯�� ������ ���������� �߰��Ѵ�. */
//#include <d3dx11.h>

#include <stdlib.h>
#include <map>
#include <unordered_map>
#include <list>
#include <vector>
#include <algorithm>
#include <string>
#include <commdlg.h>
#include <filesystem>

using namespace std;

namespace fs = std::filesystem;

#include "Engine_Enum.h"
#include "Engine_Macro.h"
#include "Engine_Function.h"
#include "Engine_Typedef.h"
#include "Engine_Struct.h"

namespace Engine
{
	const _wstring		g_strTransformTag = TEXT("Com_Transform");
	const _uint			g_iMaxMeshBones = 512;
}

using namespace Engine;

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif

#endif // _DEBUG


