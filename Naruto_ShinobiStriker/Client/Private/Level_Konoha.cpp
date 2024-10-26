#include "stdafx.h"
#include "..\Public\Level_Konoha.h"

#include "Level_Loading.h"

#include "GameInstance.h"
#include "Layer.h"
#include "FreeCamera.h"
#include "PlayerCamera.h"

#include "ObjectPool.h"
#include "Lightning.h"
#include "Amateras.h"
#include "Fire05.h"
#include "Smoke11.h"
#include "Effect_GroundBreak.h"
#include "SmokeRing.h"
#include "Effect_Distortion.h"

#include "Boss_Kurama.h"
#include "Sasuke_Next.h"
#include "NonAnimModel.h"

#include "Player_Custom.h"
#include "WhiteZetsu.h"
#include "ObjectPool.h"
#include "Lightning.h"
#include "Particle_Hit.h"
#include "Particle_Snow.h"
#include "RasenganRing.h"
#include "Particle_Hit.h"
#include "Particle_Snow.h"
#include "Particle_Up.h"
#include "ExplosionCircle.h"
#include "FoxSphere.h"
#include "FoxBlast.h"
#include "AreaCircle.h"
#include "Effect_RasenkoHit.h"
#include "Effect_TypeZero.h"


CLevel_Konoha::CLevel_Konoha(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Konoha::Initialize()
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

	if (FAILED(Ready_NPC()))
		return E_FAIL;

	m_pPlayer->Set_Camera(m_pPlayerCamera);
	m_pGameInstance->Set_Listener(m_pPlayer);

	m_vAreaCirclePos[0] = _float3(-6.2f, -11.44f, 19.42f);
	m_vAreaCirclePos[1] = _float3(104.f, -11.46f, 102.f);
	m_vAreaCirclePos[2] = _float3(67.46f, -2.6f, -28.f);


	m_pGameInstance->Play_BGM(TEXT("BGM_Konoha.wav"), 0.3f);
	// 85 70 -14
	return S_OK;
}

void CLevel_Konoha::Update(_float fTimeDelta)
{
	m_pGameInstance->Set_Listener(m_pPlayer);

	_float4 vPlayerPos;
	XMStoreFloat4(&vPlayerPos, m_pPlayer->Get_Position());

	_float4 vLightPos = _float4(vPlayerPos.x + 20.f, vPlayerPos.y + 35.f, vPlayerPos.z + 20.f, 1.f);
	_float4 vLightAt = vPlayerPos;
	m_pGameInstance->Set_LigthDepth(XMLoadFloat4(&vLightPos), XMLoadFloat4(&vLightAt));

	if (KEY_TAP(KEY::L))
	{
		if (FAILED(m_pGameInstance->Change_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_AME))))
			return;
	}

	if (m_isControlCamera)
	{
		Start_Camera();
	}

	// 대화1
	if (KEY_TAP(KEY::ENTER) && nullptr != m_pNpc_Sasuke)
	{
		if (!m_isEnd_FirstText)
		{
			FirstText();
		}
	}

	if (m_isEnd_FirstText)
	{
		if (m_isFirstSummonMonster && m_pGameInstance->Find_Layer(LEVEL_KONOHA, TEXT("Layer_Monster"))->Get_ObjectList().size() <= 0)
		{
			if (!m_isFirstAreaCircle)
			{
 				m_pAreaCircle->Appear(XMLoadFloat3(&m_vAreaCirclePos[0]));
				m_isFirstAreaCircle = true;
			}
			else if (!m_isSecondAreaCircle && m_isSecondSummonMonster)
			{
				m_pAreaCircle->Appear(XMLoadFloat3(&m_vAreaCirclePos[1]));
				m_isSecondAreaCircle = true;
			}
			else if (!m_isThirdAreaCircle && m_isThirdSummonMonster)
			{
				m_pAreaCircle->Appear(XMLoadFloat3(&m_vAreaCirclePos[2]));
				m_isThirdAreaCircle = true;
			}

			if (m_pAreaCircle->Get_IsCollision())
			{
				Summon_Monster();
			}
		}
	}

	// 보스 소환 카메라
	if (m_isBossAppearCamera)
	{
		_matrix vBossWorldMatrix = m_pKurama->Get_Transform()->Get_WorldMatrix();
		if (m_pKurama->Get_Fsm()->Get_CurrentState() == CBoss_Kurama::ROAR)
		{
			_uint iFrame = m_pKurama->Get_Model()->Get_Frame(m_pKurama->Get_Model()->Get_CurrentAnimationIndex());

			if (iFrame <= 63 && !m_isBossCameraView1)
			{
				_vector vStartCameraPos = vBossWorldMatrix.r[3] + XMVector3Normalize(vBossWorldMatrix.r[2]) * 7.f + XMVector3Normalize(vBossWorldMatrix.r[1]) * 5.f;
				_vector vEndCameraPos = vBossWorldMatrix.r[3] + XMVector3Normalize(vBossWorldMatrix.r[2]) * 3.f + XMVector3Normalize(vBossWorldMatrix.r[1]) * 5.f;

				m_pPlayerCamera->Setting_CameraControl(vStartCameraPos, vEndCameraPos, vStartCameraPos - XMVector3Normalize(vBossWorldMatrix.r[2]) * 10.f);
				m_pPlayerCamera->Start_CameraControl();
				m_pPlayerCamera->Start_CameraLerp(1.f);
				m_isBossCameraView1 = true;
			}
			else if (iFrame > 63 && !m_isBossCameraView2)
			{
				_vector vStartCameraPos = vBossWorldMatrix.r[3] + XMVector3Normalize(vBossWorldMatrix.r[2]) * 7.f + XMVector3Normalize(vBossWorldMatrix.r[1]) * 3.f;
				_vector vEndCameraPos = vBossWorldMatrix.r[3] + XMVector3Normalize(vBossWorldMatrix.r[2]) * 12.f + XMVector3Normalize(vBossWorldMatrix.r[1]) * 3.f;

				m_pPlayerCamera->Setting_CameraControl(vStartCameraPos, vEndCameraPos, vStartCameraPos - XMVector3Normalize(vBossWorldMatrix.r[2]) * 10.f);
				m_pPlayerCamera->Start_CameraControl();
				m_pPlayerCamera->Start_CameraLerp(0.3f);
				m_pPlayerCamera->Start_Shake(0.3f, 2.f);
				m_isBossCameraView2 = true;
			}
		}
		else
		{
			m_fBossCameraTime += fTimeDelta;
			if (m_fBossCameraTime >= 4.f)
			{
				m_pPlayer->Change_IsRender(true);
				m_pPlayer->Set_IsControl(true);
				m_pPlayerCamera->End_CameraControl();

				m_isBossAppearCamera = false;
			}
		}
	}

	if (KEY_TAP(KEY::NUM9))
	{
		Summon_Boss();
	}
}

HRESULT CLevel_Konoha::Render()
{
	_float3 vPlayerPos;
	XMStoreFloat3(&vPlayerPos, (m_pGameInstance->Find_Player(LEVEL_KONOHA)->Get_Position()));

	_tchar	szPlayerPosText[MAX_PATH] = {};
	swprintf(szPlayerPosText, MAX_PATH, TEXT("X : %.3f, Y : %.3f, Z : %.3f"), vPlayerPos.x, vPlayerPos.y, vPlayerPos.z);

	SetWindowText(g_hWnd, szPlayerPosText);
	return S_OK;
}

HRESULT CLevel_Konoha::Ready_Lights()
{
	LIGHT_DESC			LightDesc{};

	ZeroMemory(&LightDesc, sizeof LightDesc);
	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(0.9f, 0.9f, 0.9f, 1.f);
	LightDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 0.4f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	// 스태틱 라이트 설정
	m_vStaticLightDepthPos = _float4(85.f, 70.f, -14.f, 1.f);
	m_vStaticLightDepthAt = _float4(0.f, -35.f, 0.f, 1.f);
	m_pGameInstance->Set_StaticLightViewMatrix(XMMatrixLookAtLH(XMLoadFloat4(&m_vStaticLightDepthPos), XMLoadFloat4(&m_vStaticLightDepthAt), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	m_pGameInstance->Load_StaticLightDepthTexture(TEXT("../Bin/DataFiles/StaticLight/StaticLighDepth_Konoha.dds"));

	return S_OK;
}

HRESULT CLevel_Konoha::Ready_Layer_Camera()
{
	CPlayerCamera::CAMERA_PLAYER_DESC	Desc{};

	Desc.vEye = _float4(0.f, 10.f, -10.f, 1.f);
	Desc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	Desc.fFovy = XMConvertToRadians(70.0f);
	Desc.fAspect = (_float)g_iWinSizeX / (_float)g_iWinSizeY;
	Desc.fNear = 0.1f;
	Desc.fFar = 2000.f;
	Desc.fSpeedPerSec = 30.f;
	Desc.fRotationPerSec = XMConvertToRadians(90.0f);
	Desc.iLevelIndex = LEVEL_KONOHA;

	Desc.fSpeed = 4.f;
	Desc.pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Find_Player(LEVEL_KONOHA));

	m_pPlayerCamera = dynamic_cast<CPlayerCamera*>(m_pGameInstance->Get_CloneObject_AddLayer(LEVEL_KONOHA, TEXT("Layer_Camera"), TEXT("Prototype_GameObject_PlayerCamera"), &Desc));
	if (nullptr == m_pPlayerCamera)
		return E_FAIL;

	Safe_AddRef(m_pPlayerCamera);

	return S_OK;
}


HRESULT CLevel_Konoha::Ready_Layer_BackGround()
{
	CGameObject::GAMEOBJECT_DESC	Desc{};

	Desc.fRotationPerSec = 0.f;
	Desc.fSpeedPerSec = 0.f;
	Desc.iLevelIndex = LEVEL_KONOHA;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_KONOHA, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_Sky"), &Desc)))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_KONOHA, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_Terrain"), &Desc)))
	//	return E_FAIL;

	if (FAILED(Load_Map()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Konoha::Ready_Layer_Effect()
{
	CGameObject::GAMEOBJECT_DESC Desc;
	Desc.fRotationPerSec = 0.f;
	Desc.fSpeedPerSec = 0.;
	Desc.iLevelIndex = LEVEL_KONOHA;

	CAreaCircle::AREACIRCLE_DESC AreaCircleDesc{};
	memcpy(&AreaCircleDesc, &Desc, sizeof(CGameObject::GAMEOBJECT_DESC));
	AreaCircleDesc.vPos = _float3(-14.f, -25.f, 9.f);

	m_pAreaCircle = dynamic_cast<CAreaCircle*>(m_pGameInstance->Get_CloneObject_AddLayer(LEVEL_KONOHA, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_AreaiCircle"), &AreaCircleDesc));
	if (nullptr == m_pAreaCircle)
		return E_FAIL;

	for (_uint i = 0; i < 50; ++i)
	{
		CEffect_Distortion* pDistortion = dynamic_cast<CEffect_Distortion*>(m_pGameInstance->Get_CloneObject_AddLayer(LEVEL_CHUNIN, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Distortion"), &Desc));
		CObjectPool<CEffect_Distortion>::Return_GameObject(pDistortion);
	}

	for (_uint i = 0; i < 20; ++i)
	{
		CSmokeRing* pSmokeRing = dynamic_cast<CSmokeRing*>(m_pGameInstance->Get_CloneObject_AddLayer(LEVEL_CHUNIN, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_SmokeRing"), &Desc));
		CObjectPool<CSmokeRing>::Return_GameObject(pSmokeRing);
	}

	for (_uint i = 0; i < 20; ++i)
	{
		CEffect_GroundBreak* pGroundBreak = dynamic_cast<CEffect_GroundBreak*>(m_pGameInstance->Get_CloneObject_AddLayer(LEVEL_CHUNIN, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_GroundBreak"), &Desc));
		CObjectPool<CEffect_GroundBreak>::Return_GameObject(pGroundBreak);
	}

	for (_uint i = 0; i < 200; ++i)
	{
		CSmoke11* pSomke11 = dynamic_cast<CSmoke11*>(m_pGameInstance->Get_CloneObject_AddLayer(LEVEL_CHUNIN, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Smoke11"), &Desc));
		CObjectPool<CSmoke11>::Return_GameObject(pSomke11);
	}

	for (_uint i = 0; i < 50; ++i)
	{
		CEffect_RasenkoHit* pRasenkoHit = dynamic_cast<CEffect_RasenkoHit*>(m_pGameInstance->Get_CloneObject_AddLayer(LEVEL_KONOHA, TEXT("Layer_Skill_Effect"), TEXT("Prototype_GameObject_RasenkoHIt"), &Desc));
		CObjectPool<CEffect_RasenkoHit>::Return_GameObject(pRasenkoHit);
	}

	CLightning::LIGHTNING_DESC	LightningDesc{};
	memcpy(&LightningDesc, &Desc, sizeof(CGameObject::GAMEOBJECT_DESC));

	for (_uint i = 0; i < CLightning::LIGHTNING_END; ++i)
	{
		for (_uint j = 0; j < 80; ++j)
		{
			LightningDesc.eType = CLightning::LIGHTNING_TYPE(j % CLightning::LIGHTNING_END);
			CLightning* pLightning = dynamic_cast<CLightning*>(m_pGameInstance->Get_CloneObject_AddLayer(LEVEL_KONOHA, TEXT("Layer_Skill_Effect"), TEXT("Prototype_GameObject_Lightning"), &LightningDesc));
			CObjectPool<CLightning>::Return_GameObject(pLightning);
		}
	}

	CAmateras::AMATERAS_DESC	AmaterasDesc{};
	memcpy(&AmaterasDesc, &Desc, sizeof(CGameObject::GAMEOBJECT_DESC));
	for (_uint i = 0; i < CAmateras::AMATERAS_END; ++i)
	{
		for (_uint j = 0; j < 300; ++j)
		{
			AmaterasDesc.eType = CAmateras::AMATERAS_TYPE(j % CAmateras::AMATERAS_END);
			CAmateras* pAmateras = dynamic_cast<CAmateras*>(m_pGameInstance->Get_CloneObject_AddLayer(LEVEL_KONOHA, TEXT("Layer_Skill_Effect"), TEXT("Prototype_GameObject_Amateras"), &AmaterasDesc));
			CObjectPool<CAmateras>::Return_GameObject(pAmateras);
		}
	}

	for (_uint i = 0; i < 150; ++i)
	{
		CRasenganRing* pRasenganRing = dynamic_cast<CRasenganRing*>(m_pGameInstance->Get_CloneObject_AddLayer(LEVEL_KONOHA, TEXT("Layer_Skill_Effect"), TEXT("Prototype_GameObject_RasenganRing"), &Desc));
		CObjectPool<CRasenganRing>::Return_GameObject(pRasenganRing);
	}

	for (_uint i = 0; i < 1000; ++i)
	{
		CFire05* pFire = dynamic_cast<CFire05*>(m_pGameInstance->Get_CloneObject_AddLayer(LEVEL_KONOHA, TEXT("Layer_Skill_Effect"), TEXT("Prototype_GameObject_Fire"), &Desc));
		CObjectPool<CFire05>::Return_GameObject(pFire);
	}

	for (_uint i = 0; i < 50; ++i)
	{
		CExplosionCircle* pExplosionCircle = dynamic_cast<CExplosionCircle*>(m_pGameInstance->Get_CloneObject_AddLayer(LEVEL_KONOHA, TEXT("Layer_Skill_Effect"), TEXT("Prototype_GameObject_ExplosionCircle"), &Desc));
		CObjectPool<CExplosionCircle>::Return_GameObject(pExplosionCircle);
	}

	for (_uint i = 0; i < 300; ++i)
	{
		CParticle_Hit* pParticleHit = dynamic_cast<CParticle_Hit*>(m_pGameInstance->Get_CloneObject_AddLayer(LEVEL_KONOHA, TEXT("Layer_Particle"), TEXT("Prototype_GameObject_HitParticle"), &LightningDesc));
		CObjectPool<CParticle_Hit>::Return_GameObject(pParticleHit);
	}

	for (_uint i = 0; i < 100; ++i)
	{
		CParticle_Up* pParticleUp = dynamic_cast<CParticle_Up*>(m_pGameInstance->Get_CloneObject_AddLayer(LEVEL_KONOHA, TEXT("Layer_Particle"), TEXT("Prototype_GameObject_UpParticle"), &LightningDesc));
		CObjectPool<CParticle_Up>::Return_GameObject(pParticleUp);
	}

	for (_uint i = 0; i < 50; ++i)
	{
		CFoxSphere* pFoxSphere = dynamic_cast<CFoxSphere*>(m_pGameInstance->Get_CloneObject_AddLayer(LEVEL_KONOHA, TEXT("Layer_Particle"), TEXT("Prototype_GameObject_FoxSphere"), &LightningDesc));
		CObjectPool<CFoxSphere>::Return_GameObject(pFoxSphere);
	}

	for (_uint i = 0; i < 10; ++i)
	{
		CFoxBlast* pFoxBlast = dynamic_cast<CFoxBlast*>(m_pGameInstance->Get_CloneObject_AddLayer(LEVEL_KONOHA, TEXT("Layer_Particle"), TEXT("Prototype_GameObject_FoxBlast"), &LightningDesc));
		CObjectPool<CFoxBlast>::Return_GameObject(pFoxBlast);
	}

	for (_uint i = 0; i < 10; ++i)
	{
		CEffect_TypeZero* pTypeZero = dynamic_cast<CEffect_TypeZero*>(m_pGameInstance->Get_CloneObject_AddLayer(LEVEL_KONOHA, TEXT("Layer_Particle"), TEXT("Prototype_GameObject_FoxBlast"), &LightningDesc));
		CObjectPool<CEffect_TypeZero>::Return_GameObject(pTypeZero);
	}

	return S_OK;
}

HRESULT CLevel_Konoha::Ready_LandObjects()
{
	//if (FAILED(Ready_Layer_Monster()))
	//	return E_FAIL;

	if (FAILED(Ready_Player()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Konoha::Ready_Player()
{
	CPlayer_Custom::CUSTOM_DESC PlayerDesc;
	PlayerDesc.fRotationPerSec = 10.f;
	PlayerDesc.fSpeedPerSec = 10.f;
	PlayerDesc.iLevelIndex = LEVEL_KONOHA;
	PlayerDesc.vPosition = _float3(-75.6f, -19.51f, 61.74f);
	PlayerDesc.vRotation = _float3(0.f, 180.f, 0.f);

	m_pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_CloneObject_AddLayer(LEVEL_KONOHA, TEXT("Layer_Player"), TEXT("Prototype_GameObject_PlayerCustom"), &PlayerDesc));
	if (nullptr == m_pPlayer)
		return E_FAIL;

	Safe_AddRef(m_pPlayer);
	m_pPlayer->Get_Transform()->Rotation(0.f, XMConvertToRadians(180.f), 0.f);

	return S_OK;
}

HRESULT CLevel_Konoha::Summon_Boss()
{
	CMonster::MONSTER_DESC MonsterDesc;
	MonsterDesc.fRotationPerSec = 10.f;
	MonsterDesc.fSpeedPerSec = 10.f;
	strcpy_s(MonsterDesc.szModelTag, "Prototype_Component_Model_Kurama");
	MonsterDesc.vScale = _float3(1.f, 1.f, 1.f);
	MonsterDesc.iLevelIndex = LEVEL_KONOHA;
	MonsterDesc.vPosition = _float3(62.f, -2.6f, -42.8f);

	m_pKurama = dynamic_cast<CBoss_Kurama*>(m_pGameInstance->Get_CloneObject_AddLayer(LEVEL_KONOHA, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Kurama"), &MonsterDesc));
	if (nullptr == m_pKurama)
		return E_FAIL;

	_matrix vBossWorldMatrix = m_pKurama->Get_Transform()->Get_WorldMatrix();

	_vector vCameraPos = vBossWorldMatrix.r[3] + XMVector3Normalize(vBossWorldMatrix.r[2]) * 10.f + XMVector3Normalize(vBossWorldMatrix.r[1]) * 4.f;

	m_pPlayerCamera->Setting_CameraControl(vCameraPos, vCameraPos, vCameraPos - XMVector3Normalize(vBossWorldMatrix.r[2]) * 10.f);
	m_pPlayerCamera->Start_CameraControl();

	m_pPlayer->Change_IsRender(false);
	m_pPlayer->Get_EffectSoundCom()->Stop();
	m_pPlayer->Change_State(CPlayer::IDLE);
	m_pPlayer->Set_IsControl(false);
	m_pPlayer->Get_Transform()->Set_State(CTransform::STATE_POSITION, XMVectorSet(68.f, -2.6f, -19.2f, 1.f));
	
	m_isBossAppearCamera = true;

	return S_OK;
}

HRESULT CLevel_Konoha::Ready_NPC()
{
	CSasuke_Next::NPC_DESC NpcDesc{};
	NpcDesc.fRotationPerSec = 10.f;
	NpcDesc.fSpeedPerSec = 10.f;
	NpcDesc.iLevelIndex = LEVEL_KONOHA;
	NpcDesc.vPosition = _float3(-74.f, -19.469f, 44.5);
	NpcDesc.vRotation = _float3(0.f, 0.f, 0.f);

	m_pNpc_Sasuke = dynamic_cast<CSasuke_Next*>(m_pGameInstance->Get_CloneObject_AddLayer(LEVEL_KONOHA, TEXT("Layer_Npc"), TEXT("Prototype_GameObject_SasukeNext"), &NpcDesc));
	if (nullptr == m_pNpc_Sasuke)
		return E_FAIL;

	Safe_AddRef(m_pNpc_Sasuke);

	if (FAILED(Ready_NpcText()))
		return E_FAIL;

	//m_pNpc_Sasuke->Get_Transform()->LookAt(m_pPlayer->Get_Position());

	return S_OK;
}

HRESULT CLevel_Konoha::Load_Map()
{
	_tchar szFinalPath[MAX_PATH] = TEXT("../Bin/DataFiles/MapData/KonohaVillageMap_New.dat");

	CNonAnimModel::NONMODEL_DESC  NonAnimModelDesc;
	NonAnimModelDesc.iLevelIndex = LEVEL_KONOHA;
	NonAnimModelDesc.fRotationPerSec = 0.f;
		NonAnimModelDesc.fSpeedPerSec = 0.f;
		NonAnimModelDesc.isCollision = true;
		strcpy_s(NonAnimModelDesc.szModelTag, "Prototype_Model_MapKonoha");
		NonAnimModelDesc.vPosition = _float3(0.f,0.f,0.f);
		NonAnimModelDesc.vRotation = _float3(0.f, 0.f, 0.f);
		NonAnimModelDesc.vScale = _float3(1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_KONOHA, TEXT("Layer_GameObject_Map"), TEXT("Prototype_GameObject_NonAnim"), &NonAnimModelDesc)))
		return E_FAIL;
	
	//HANDLE hFile = 0;
	//_ulong dwByte = 0;

	

	//LOAD_DESC LoadDesc;

	//_int iNum;

	//hFile = CreateFile(szFinalPath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	//if (0 == hFile)
	//{
	//	CloseHandle(hFile);
	//	return E_FAIL;
	//}

	//ReadFile(hFile, &(iNum), sizeof(_int), &dwByte, nullptr);

	//for (size_t i = 0; i < iNum; ++i)
	//{
	//	ReadFile(hFile, &LoadDesc, sizeof(LOAD_DESC), &dwByte, nullptr);

	//	NonAnimModelDesc.fRotationPerSec = LoadDesc.fRotationPerSec;
	//	NonAnimModelDesc.fSpeedPerSec = LoadDesc.fSpeedPerSec;
	//	NonAnimModelDesc.isCollision = LoadDesc.isCollision;
	//	strcpy_s(NonAnimModelDesc.szLayerTag, LoadDesc.szLayerTag);
	//	strcpy_s(NonAnimModelDesc.szModelTag, LoadDesc.szModelTag);
	//	strcpy_s(NonAnimModelDesc.szPrototypeTag, LoadDesc.szPrototypeTag);
	//	NonAnimModelDesc.vPosition = LoadDesc.vPosition;
	//	NonAnimModelDesc.vRotation = LoadDesc.vRotation;
	//	NonAnimModelDesc.vScale = LoadDesc.vScale;

	//	_tchar szPrototypeTag[MAX_PATH];
	//	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, NonAnimModelDesc.szPrototypeTag, MAX_PATH, szPrototypeTag, MAX_PATH);

	//	m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_KONOHA, TEXT("Layer_GameObject_Map"), TEXT("Prototype_GameObject_NonAnim"), &NonAnimModelDesc);
	//}

	//CloseHandle(hFile);

	return S_OK;
}

HRESULT CLevel_Konoha::Ready_NpcText()
{
	m_TextList01.emplace_back(TEXT("이 근처에 나루토가 날뛰고 있을거야"));
	m_TextList01.emplace_back(TEXT("먼저 가서 대비하고 있을게"));
	m_TextList01.emplace_back(TEXT("오면서 마을을 습격하고 있는 적들을 물리치면서 와줘"));
	m_TextList01.emplace_back(TEXT("후딱 처리해버리자고"));

	return S_OK;
}

HRESULT CLevel_Konoha::Summon_Monster()
{
	CMonster::MONSTER_DESC MonsterDesc;
	MonsterDesc.fRotationPerSec = 10.f;
	MonsterDesc.fSpeedPerSec = 10.f;
	MonsterDesc.vScale = _float3(1.f, 1.f, 1.f);
	MonsterDesc.iLevelIndex = LEVEL_KONOHA;
	strcpy_s(MonsterDesc.szModelTag, "Prototype_Component_Model_WhiteZetsu");


	if (!m_isFirstSummonMonster)
	{
		MonsterDesc.vPosition = _float3(-69.8f, -19.46f, 37.4f);
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_KONOHA, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_WhitZetsu"), &MonsterDesc)))
			return E_FAIL;

		MonsterDesc.vPosition = _float3(-85.4f, -19.46f, 47.7f);
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_KONOHA, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_WhitZetsu"), &MonsterDesc)))
			return E_FAIL;

		MonsterDesc.vPosition = _float3(-79.f, -19.46f, 35.3f);
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_KONOHA, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_WhitZetsu"), &MonsterDesc)))
			return E_FAIL;

		m_isFirstSummonMonster = true;
	}
	else if (!m_isSecondSummonMonster)
	{
		MonsterDesc.vPosition = _float3(-1.1f, -11.43f, 11.35f);
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_KONOHA, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_WhitZetsu"), &MonsterDesc)))
			return E_FAIL;

		MonsterDesc.vPosition = _float3(-13.1f, -11.43f, 9.08f);
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_KONOHA, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_WhitZetsu"), &MonsterDesc)))
			return E_FAIL;

		MonsterDesc.vPosition = _float3(0.31f, -11.43f, 38.f);
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_KONOHA, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_WhitZetsu"), &MonsterDesc)))
			return E_FAIL;

		m_isSecondSummonMonster = true;
	}
	else if (!m_isThirdSummonMonster)
	{
		MonsterDesc.vPosition = _float3(113.3f, -11.43f, 93.88f);
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_KONOHA, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_WhitZetsu"), &MonsterDesc)))
			return E_FAIL;

		MonsterDesc.vPosition = _float3(119.8f, -11.43f, 109.27f);
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_KONOHA, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_WhitZetsu"), &MonsterDesc)))
			return E_FAIL;

		MonsterDesc.vPosition = _float3(89.73f, -11.43f, 112.4f);
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_KONOHA, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_WhitZetsu"), &MonsterDesc)))
			return E_FAIL;

		MonsterDesc.vPosition = _float3(94.f, -11.43f, 96.68f);
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_KONOHA, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_WhitZetsu"), &MonsterDesc)))
			return E_FAIL;

		MonsterDesc.vPosition = _float3(97.66f, -11.43f, 83.68f);
		//if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_KONOHA, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_WhitZetsu"), &MonsterDesc)))
		//	return E_FAIL;

		m_isThirdSummonMonster = true;
	}
	else
	{
		Summon_Boss();
	}

	return S_OK;
}

void CLevel_Konoha::Start_Camera()
{
	if (!m_isCameraView1)
	{
		m_pPlayerCamera->Setting_CameraControl(_float3(80.f, 50.f, -58.f), _float3(72.f, 50.f, 9.f), _float3(30.f, -5.f, -8.f));
		m_pPlayerCamera->Start_CameraControl();
		m_pPlayerCamera->Start_CameraLerp(4.f);

		m_pPlayer->Change_IsRender(false);
		m_pPlayer->Set_IsControl(false);

		m_isCameraView1 = true;
	}
	else if (m_isCameraView1 && !m_isCameraView2 && m_pPlayerCamera->Get_IsLerpEnd())
	{
		m_pPlayerCamera->Setting_CameraControl(_float3(81.8f, 50.f, 53.f), _float3(33.7f, 50.f, 50.f), _float3(62.f, -35.f, -14.f));
		m_pPlayerCamera->Start_CameraLerp(3.f);
		m_isCameraView2 = true;
	}
	else if (m_isCameraView2 && !m_isCaemraViewPlayer && m_pPlayerCamera->Get_IsLerpEnd())
	{
		_matrix PlayerWorldMatrix = m_pPlayer->Get_Transform()->Get_WorldMatrix();

		_vector vStartPos = PlayerWorldMatrix.r[3] + XMVector3Normalize(PlayerWorldMatrix.r[0]) * 3.f + XMVector3Normalize(PlayerWorldMatrix.r[1]) * 3.f; +XMVector3Normalize(PlayerWorldMatrix.r[2]) * 3.f;
		_vector vEndPos = PlayerWorldMatrix.r[3] + XMVector3Normalize(PlayerWorldMatrix.r[0]) * -2.f + XMVector3Normalize(PlayerWorldMatrix.r[1]) * 3.f + XMVector3Normalize(PlayerWorldMatrix.r[2]) * 3.f;

		m_pPlayer->Change_IsRender(true);
		m_pPlayerCamera->Setting_CameraControl(vStartPos, vEndPos, PlayerWorldMatrix.r[3]);
		m_pPlayerCamera->Start_CameraLerp(3.f);

		m_isCaemraViewPlayer = true;
	}
	else if (m_pPlayerCamera->Get_IsLerpEnd() && m_isCaemraViewPlayer && !m_isCameraInit)
	{
		_matrix PlayerWorldMatrix = m_pPlayer->Get_Transform()->Get_WorldMatrix();
		_vector vStartPos = PlayerWorldMatrix.r[3] + PlayerWorldMatrix.r[2] * -5.f + XMVector3Normalize(PlayerWorldMatrix.r[1]) * 5.f;
		m_pPlayerCamera->Setting_CameraControl(vStartPos, vStartPos, PlayerWorldMatrix.r[3] + PlayerWorldMatrix.r[2]);
		m_pPlayerCamera->Start_CameraLerp(0.1f);
		m_isCameraInit = true;
	}
	else if (m_pPlayerCamera->Get_IsLerpEnd() && m_isCameraInit)
	{
		m_pPlayer->Set_IsControl(true);
		m_pNpc_Sasuke->Appear();
		m_pPlayer->Start_AppearUI();
		m_pPlayerCamera->End_CameraControl();
		m_isControlCamera = false;
	}


	//m_pNpc_Sasuke->Appear();
	//m_pPlayer->Start_AppearUI();
	//m_isControlCamera = false;
}

void CLevel_Konoha::FirstText()
{
	if (CSasuke_Next::ST_SPEAK == m_pNpc_Sasuke->Get_State())
	{
		if (m_TextList01.size() > m_iTextNum)
			m_pNpc_Sasuke->Change_TextWindow(m_TextList01[m_iTextNum++]);
		else
		{
			m_pNpc_Sasuke->End_SpeakMode();
			m_pPlayerCamera->End_CameraControl();
			m_pPlayer->Start_AppearUI();
			m_pPlayer->Change_IsRender(true);
			m_pPlayer->Set_IsControl(true);

			if (FAILED(Summon_Monster()))
				return;

			m_isEnd_FirstText = true;
			m_iTextNum = 0;
		}
	}
	else if (CSasuke_Next::ST_EXIT == m_pNpc_Sasuke->Get_State())
	{
		_float fDistance = XMVectorGetX(XMVector3Length(m_pPlayer->Get_Position() - m_pNpc_Sasuke->Get_Position()));
		if (fDistance <= 2.f)
		{
			m_pPlayer->Start_DisappearUI();
			m_pPlayer->Change_IsRender(false);
			m_pPlayer->Set_IsControl(false);
			m_pNpc_Sasuke->Change_SpeakMode(m_pPlayerCamera);

			if (m_TextList01.size() > m_iTextNum)
				m_pNpc_Sasuke->Change_TextWindow(m_TextList01[m_iTextNum++]);
		}
	}
}

void CLevel_Konoha::SecondText()
{
}

CLevel_Konoha* CLevel_Konoha::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Konoha* pInstance = new CLevel_Konoha(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Konoha"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Konoha::Free()
{
	//m_pGameInstance->Reset_PhsyX();

	__super::Free();

	Safe_Release(m_pPlayerCamera);
	Safe_Release(m_pNpc_Sasuke);
	Safe_Release(m_pPlayer);
}
