#include "stdafx.h"
#include "..\Public\Level_Ame.h"

#include "Level_Loading.h"

#include "GameInstance.h"
#include "FreeCamera.h"
#include "PlayerCamera.h"

#include "NonAnimModel.h"

#include "Player_Naruto.h"
#include "Player_Custom.h"
#include "WhiteZetsu.h"

CLevel_Ame::CLevel_Ame(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Ame::Initialize()
{
	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_LandObjects()))
		return E_FAIL;
	if (FAILED(Ready_Layer_Camera()))
		return E_FAIL;
	if (FAILED(Ready_Layer_BackGround()))
		return E_FAIL;
	if (FAILED(Ready_Layer_Effect()))
		return E_FAIL;
	
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Find_Player(LEVEL_AME));
	CCamera* pCamera = m_pGameInstance->Find_Camera(LEVEL_AME);

	//pPlayer->Set_Camera(pCamera);

	m_pGameInstance->Set_Listener(pPlayer);

	return S_OK;
}

void CLevel_Ame::Update(_float fTimeDelta)
{
	if (KEY_TAP(KEY::ENTER))
	{
		if (FAILED(m_pGameInstance->Change_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_KONOHA))))
			return;
	}
}

HRESULT CLevel_Ame::Render()
{
	_float3 vPlayerPos;
	XMStoreFloat3(&vPlayerPos, (m_pGameInstance->Find_Player(LEVEL_AME)->Get_Position()));

	_tchar	szPlayerPosText[MAX_PATH] = {};
	swprintf(szPlayerPosText, MAX_PATH, TEXT("X : %.3f, Y : %.3f, Z : %.3f"), vPlayerPos.x, vPlayerPos.y, vPlayerPos.z);

	SetWindowText(g_hWnd, szPlayerPosText);
	return S_OK;
}

HRESULT CLevel_Ame::Ready_Lights()
{
	/* 게임플레이 레벨에 필요한 광원을 준비한다. */
	

	return S_OK;
}

HRESULT CLevel_Ame::Ready_Layer_Camera()
{
	//CFreeCamera::CAMERA_FREE_DESC		Desc{};

	//Desc.fSensor = 0.2f;
	//Desc.vEye = _float4(0.f, 10.f, -10.f, 1.f);
	//Desc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	//Desc.fFovy = XMConvertToRadians(60.0f);
	//Desc.fAspect = g_iWinSizeX / g_iWinSizeY;
	//Desc.fNear = 0.1f;
	//Desc.fFar = 1000.f;
	//Desc.fSpeedPerSec = 30.f;
	//Desc.fRotationPerSec = XMConvertToRadians(90.0f);

	//if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Camera"), TEXT("Prototype_GameObject_FreeCamera"), &Desc)))
	//	return E_FAIL;
	
	CPlayerCamera::CAMERA_PLAYER_DESC	Desc{};

	Desc.vEye = _float4(0.f, 10.f, -10.f, 1.f);
	Desc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	Desc.fFovy = XMConvertToRadians(60.0f);
	Desc.fAspect = (_float)g_iWinSizeX / (_float)g_iWinSizeY;
	Desc.fNear = 0.1f;
	Desc.fFar = 1000.f;
	Desc.fSpeedPerSec = 30.f;
	Desc.fRotationPerSec = XMConvertToRadians(90.0f);
	Desc.iLevelIndex = LEVEL_AME;

	Desc.fSpeed = 4.f;
	Desc.pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Find_Player(LEVEL_AME));

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_AME, TEXT("Layer_Camera"), TEXT("Prototype_GameObject_PlayerCamera"), &Desc)))
		return E_FAIL;

	return S_OK;
}


HRESULT CLevel_Ame::Ready_Layer_BackGround()
{
	CGameObject::GAMEOBJECT_DESC	Desc{};

	Desc.fRotationPerSec = 0.f;
	Desc.fSpeedPerSec = 0.f;
	Desc.iLevelIndex = LEVEL_AME;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_AME, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_Terrain"), &Desc)))
		return E_FAIL;

	if (FAILED(Load_Map()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Ame::Ready_Layer_Effect()
{
	//for (size_t i = 0; i < 50; i++)
	//{
	//	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_Effect"))))
	//		return E_FAIL;
	//}
	

	return S_OK;
}

HRESULT CLevel_Ame::Ready_Layer_Monster()
{
	CMonster::MONSTER_DESC MonsterDesc;
	MonsterDesc.fRotationPerSec = 10.f;
	MonsterDesc.fSpeedPerSec = 10.f;
	MonsterDesc.vScale = _float3(1.f, 1.f, 1.f);
	MonsterDesc.iLevelIndex = LEVEL_AME;

	// 흰 제츠
	strcpy_s(MonsterDesc.szModelTag, "Prototype_Component_Model_WhiteZetsu");
	MonsterDesc.vPosition = _float3(-2.f, 2.f, -25.f);
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_AME, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_WhitZetsu"), &MonsterDesc)))
		return E_FAIL;

	// 십미
	strcpy_s(MonsterDesc.szModelTag, "Prototype_Component_Model_TenTail");
	MonsterDesc.vPosition = _float3(0.f, 2.f, -25.f);
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_AME, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_TenTail"), &MonsterDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Ame::Ready_LandObjects()
{
	/*CPlayer::PLAYER_DESC PlayerDesc;
	PlayerDesc.vPosition = _float3(0.f, 5.f, 0.f);
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Prototype_GameObject_Naruto"), &PlayerDesc)))
		return E_FAIL;*/

	if (FAILED(Ready_Layer_Monster()))
		return E_FAIL;

	if (FAILED(Ready_CustomPlayer()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Ame::Ready_CustomPlayer()
{
	CPlayer_Custom::CUSTOM_DESC PlayerDesc;
	PlayerDesc.vPosition = _float3(-2.f, 2.f, -26.f);
	PlayerDesc.iLevelIndex = LEVEL_AME;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_AME, TEXT("Layer_Player"), TEXT("Prototype_GameObject_PlayerCustom"), &PlayerDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Ame::Load_Map()
{
	_tchar szFinalPath[MAX_PATH] = TEXT("../Bin/DataFiles/MapData/AmeVillageMap.dat");

	HANDLE hFile = 0;
	_ulong dwByte = 0;

	CNonAnimModel::NONMODEL_DESC  NonAnimModelDesc;
	NonAnimModelDesc.iLevelIndex = LEVEL_AME;

	LOAD_DESC LoadDesc;

	_int iNum;

	hFile = CreateFile(szFinalPath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
	{
		CloseHandle(hFile);
		return E_FAIL;
	}

	ReadFile(hFile, &(iNum), sizeof(_int), &dwByte, nullptr);

	for (size_t i = 0; i < iNum; ++i)
	{
		ReadFile(hFile, &LoadDesc, sizeof(LOAD_DESC), &dwByte, nullptr);

		NonAnimModelDesc.fRotationPerSec = LoadDesc.fRotationPerSec;
		NonAnimModelDesc.fSpeedPerSec = LoadDesc.fSpeedPerSec;
		NonAnimModelDesc.isCollision = LoadDesc.isCollision;
		strcpy_s(NonAnimModelDesc.szLayerTag, LoadDesc.szLayerTag);
		strcpy_s(NonAnimModelDesc.szModelTag, LoadDesc.szModelTag);
		strcpy_s(NonAnimModelDesc.szPrototypeTag, LoadDesc.szPrototypeTag);
		NonAnimModelDesc.vPosition = LoadDesc.vPosition;
		NonAnimModelDesc.vRotation = LoadDesc.vRotation;
		NonAnimModelDesc.vScale = LoadDesc.vScale;

		_tchar szPrototypeTag[MAX_PATH];
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, NonAnimModelDesc.szPrototypeTag, MAX_PATH, szPrototypeTag, MAX_PATH);

		NonAnimModelDesc.iLevelIndex = LEVEL_AME;
		m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_AME, TEXT("Layer_GameObject_Map"), TEXT("Prototype_GameObject_NonAnim"), &NonAnimModelDesc);
	}

	CloseHandle(hFile);

	return S_OK;
}

CLevel_Ame * CLevel_Ame::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Ame*		pInstance = new CLevel_Ame(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : LEVEL_AME"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Ame::Free()
{

	__super::Free();

	//m_pGameInstance->Reset_PhsyX();

}
