#include "stdafx.h"
#include "..\Public\Loader.h"

#include "Terrain.h"
#include "BackGround.h"
#include "GameInstance.h"

#include "ModelController.h"

#include "AnimModel.h"
#include "NonAnimModel.h"
#include "FreeCamera.h"

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}



_uint APIENTRY LoadingMain(void* pArg)
{
	CoInitializeEx(nullptr, 0);

	CLoader*		pLoader = static_cast<CLoader*>(pArg);

	if(FAILED(pLoader->Loading()))
		return 1;
		
	return 0;
}


HRESULT CLoader::Initialize(LEVELID eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingMain, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;


	return S_OK;
}

HRESULT CLoader::Loading()
{
	EnterCriticalSection(&m_CriticalSection);

	HRESULT			hr = {};

	switch (m_eNextLevelID)
	{
	case LEVEL_LOGO:
		hr = Ready_Resources_For_LogoLevel();
		break;
	case LEVEL_GAMEPLAY:
		hr = Ready_Resources_For_GamePlayLevel();
		break;
	}

	LeaveCriticalSection(&m_CriticalSection);

	return hr;
}

void CLoader::Draw_LoadingText()
{
	SetWindowText(g_hWnd, m_szLoadingText);
}

HRESULT CLoader::Ready_Resources_For_LogoLevel()
{
	lstrcpy(m_szLoadingText, TEXT("�ؽ��ĸ� �ε����Դϴ�."));

	/* For. Prototype_Component_Texture_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_BackGround"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Default%d.jpg"), 2))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("����(��) �ε����Դϴ�."));


	lstrcpy(m_szLoadingText, TEXT("������(��) �ε����Դϴ�."));


	lstrcpy(m_szLoadingText, TEXT("��ü������(��) �ε����Դϴ�."));
	/* For. Prototype_GameObject_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"),
		CBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("�ε��� �Ϸ�Ǿ����ϴ�."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Ready_Resources_For_GamePlayLevel()
{
	lstrcpy(m_szLoadingText, TEXT("�ؽ��ĸ� �ε����Դϴ�."));
	/* For. Prototype_Component_Texture_Terrain*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Grass_%d.dds"), 2))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("����(��) �ε����Դϴ�."));
	/* For. Prototype_Component_VIBuffer_Terrain*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain_Height"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain_Normal"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, 10, 10))))
		return E_FAIL;

	_matrix		PreTransformMatrix = XMMatrixIdentity();

	/* For. Prototype_Component_Model_Fiona*/
	//PreTransformMatrix =  XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Fiona"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Fiona/Fiona.fbx", PreTransformMatrix))))
	//	return E_FAIL;
	//CModelController::Get_Instance()->Add_ProtytypeTag(TEXT("Fiona"));

	///* For. Prototype_Component_Model_ForkLift*/
	//PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("ForkLift"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/ForkLift/ForkLift.fbx", PreTransformMatrix))))
	//	return E_FAIL;
	//CModelController::Get_Instance()->Add_ProtytypeTag(TEXT("ForkLift"));

	//PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Zetsu"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/WhiteZetsu/WhiteZetsu.fbx", PreTransformMatrix))))
	//	return E_FAIL;
	//CModelController::Get_Instance()->Add_ProtytypeTag(TEXT("Zetsu"));

	//Ready_Resources_For_AmeVillage();
	Ready_Resources_For_KonohaVillage();
	//Ready_Resources_For_ChuninExam();

	lstrcpy(m_szLoadingText, TEXT("���̴���(��) �ε����Դϴ�."));
	/* For. Prototype_Component_Shader_VtxNorTex*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxModel */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Model"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxModel.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For. Prototype_Component_Shader_VtxAnimModel */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimModel.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("������(��) �ε����Դϴ�."));


	lstrcpy(m_szLoadingText, TEXT("��ü������(��) �ε����Դϴ�."));
	/* For. Prototype_GameObject_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Monster */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster"),
		CNonAnimModel::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_FreeCamera */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FreeCamera"),
		CFreeCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("�ε��� �Ϸ�Ǿ����ϴ�."));

	m_isFinished = true;


	return S_OK;
}

HRESULT CLoader::Ready_Resources_For_AmeVillage()
{
	_matrix		PreTransformMatrix = XMMatrixIdentity();

	// _finddata_t : <io.h>���� �����ϸ� ���� ������ �����ϴ� ����ü
	_finddata_t fd;

	// _findfirst : <io.h>���� �����ϸ� ����ڰ� ������ ��� ������ ���� ù ��° ������ ã�� �Լ�
	intptr_t handle = _findfirst("../Bin/ModelData/NonAnim/AmeVillage_Parts/*", &fd);

	if (handle == -1)
		return E_FAIL;

	int iResult = 0;

	char szCurPath[128] = "../Bin/ModelData/NonAnim/AmeVillage_Parts/";	 // ��� ���
	char szFullPath[128] = "";

	_wstring strPrototype = TEXT("Part");
	_uint iNum = 0;

	while (iResult != -1)
	{
		strcpy_s(szFullPath, szCurPath);
		strcat_s(szFullPath, fd.name);

		_char szFileName[MAX_PATH] = "";
		_char szExt[MAX_PATH] = "";
		_splitpath_s(szFullPath, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

		if (!strcmp(fd.name, ".") || !strcmp(fd.name, "..")
			|| strcmp(szExt, ".dat"))
		{
			iResult = _findnext(handle, &fd);
			continue;
		}

		string strFileName = szFileName;
		_wstring strPrototypeName;

		strPrototypeName.assign(strFileName.begin(), strFileName.end());
		wprintf(strPrototypeName.c_str());

		PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, strPrototypeName,
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, szFullPath, PreTransformMatrix))))
			return E_FAIL;

		CModelController::Get_Instance()->Add_ProtytypeTag(strPrototypeName);

		//_findnext : <io.h>���� �����ϸ� ���� ��ġ�� ������ ã�� �Լ�, ���̻� ���ٸ� -1�� ����
		iResult = _findnext(handle, &fd);
	}

	return S_OK;
}

HRESULT CLoader::Ready_Resources_For_KonohaVillage()
{
	_matrix		PreTransformMatrix = XMMatrixIdentity();

	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("NewKonoha"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/NewKonoha/NewKonoha.dat", PreTransformMatrix))))
		return E_FAIL;
	CModelController::Get_Instance()->Add_ProtytypeTag(TEXT("NewKonoha"));

	//// _finddata_t : <io.h>���� �����ϸ� ���� ������ �����ϴ� ����ü
	//_finddata_t fd;

	//// _findfirst : <io.h>���� �����ϸ� ����ڰ� ������ ��� ������ ���� ù ��° ������ ã�� �Լ�
	//intptr_t handle = _findfirst("../Bin/ModelData/NonAnim/KonohaVillage_Parts/*", &fd);

	//if (handle == -1)
	//	return E_FAIL;

	//int iResult = 0;

	//char szCurPath[128] = "../Bin/ModelData/NonAnim/KonohaVillage_Parts/";	 // ��� ���
	//char szFullPath[128] = "";

	//_wstring strPrototype = TEXT("Part");
	//_uint iNum = 0;

	//while (iResult != -1)
	//{
	//	strcpy_s(szFullPath, szCurPath);
	//	strcat_s(szFullPath, fd.name);

	//	_char szFileName[MAX_PATH] = "";
	//	_char szExt[MAX_PATH] = "";
	//	_splitpath_s(szFullPath, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

	//	if (!strcmp(fd.name, ".") || !strcmp(fd.name, "..")
	//		|| strcmp(szExt, ".dat"))
	//	{
	//		iResult = _findnext(handle, &fd);
	//		continue;
	//	}

	//	string strFileName = szFileName;
	//	_wstring strPrototypeName;

	//	strPrototypeName.assign(strFileName.begin(), strFileName.end());
	//	wprintf(strPrototypeName.c_str());

	//	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	//	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, strPrototypeName,
	//		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, szFullPath, PreTransformMatrix))))
	//		return E_FAIL;

	//	CModelController::Get_Instance()->Add_ProtytypeTag(strPrototypeName);

	//	//_findnext : <io.h>���� �����ϸ� ���� ��ġ�� ������ ã�� �Լ�, ���̻� ���ٸ� -1�� ����
	//	iResult = _findnext(handle, &fd);
	//}

	return S_OK;
}

HRESULT CLoader::Ready_Resources_For_ChuninExam()
{
	_matrix		PreTransformMatrix = XMMatrixIdentity();

	// _finddata_t : <io.h>���� �����ϸ� ���� ������ �����ϴ� ����ü
	_finddata_t fd;

	// _findfirst : <io.h>���� �����ϸ� ����ڰ� ������ ��� ������ ���� ù ��° ������ ã�� �Լ�
	intptr_t handle = _findfirst("../Bin/ModelData/NonAnim/Chunin_Parts/*", &fd);

	if (handle == -1)
		return E_FAIL;

	int iResult = 0;

	char szCurPath[128] = "../Bin/ModelData/NonAnim/Chunin_Parts/";	 // ��� ���
	char szFullPath[128] = "";

	_wstring strPrototype = TEXT("Part");
	_uint iNum = 0;

	while (iResult != -1)
	{
		strcpy_s(szFullPath, szCurPath);
		strcat_s(szFullPath, fd.name);

		_char szFileName[MAX_PATH] = "";
		_char szExt[MAX_PATH] = "";
		_splitpath_s(szFullPath, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

		if (!strcmp(fd.name, ".") || !strcmp(fd.name, "..")
			|| strcmp(szExt, ".dat"))
		{
			iResult = _findnext(handle, &fd);
			continue;
		}

		string strFileName = szFileName;
		_wstring strPrototypeName;

		strPrototypeName.assign(strFileName.begin(), strFileName.end());
		wprintf(strPrototypeName.c_str());

		PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, strPrototypeName,
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, szFullPath, PreTransformMatrix))))
			return E_FAIL;

		CModelController::Get_Instance()->Add_ProtytypeTag(strPrototypeName);

		//_findnext : <io.h>���� �����ϸ� ���� ��ġ�� ������ ã�� �Լ�, ���̻� ���ٸ� -1�� ����
		iResult = _findnext(handle, &fd);
	}

	return S_OK;
}

CLoader * CLoader::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, LEVELID eNextLevelID)
{
	CLoader*		pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX(TEXT("Failed to Created : CLoader"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	__super::Free();

	WaitForSingleObject(m_hThread, INFINITE);

	DeleteObject(m_hThread);

	CloseHandle(m_hThread);

	DeleteCriticalSection(&m_CriticalSection);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
