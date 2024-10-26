#include "stdafx.h"
#include "..\Public\Level_Chunin.h"
#include "Level_Loading.h"
#include "GameInstance.h"
#include "FreeCamera.h"
#include "PlayerCamera.h"

#include "Layer.h"

#include "NonAnimModel.h"
#include "Player_Naruto.h"
#include "Player_Custom.h"
#include "WhiteZetsu.h"

#include "ObjectPool.h"
#include "Lightning.h"
#include "Amateras.h"
#include "Fire05.h"
#include "Smoke11.h"
#include "Effect_GroundBreak.h"
#include "SmokeRing.h"
#include "Effect_Distortion.h"

#include "Sasuke_Next.h"
#include "Boss_Susanoo.h"

#include "RasenganRing.h"
#include "Particle_Hit.h"
#include "Particle_Snow.h"
#include "Particle_Up.h"
#include "ExplosionCircle.h"
#include "BladeSlash.h"
#include "Effect_RasenkoHit.h"
#include "Effect_TypeZero.h"
#include "Effect_Tsukuyomi01.h"

#include "Particle_FireBall.h"

CLevel_Chunin::CLevel_Chunin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Chunin::Initialize()
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
	m_fSecondTextTime = 0.f;

	m_pGameInstance->Play_BGM(TEXT("BGM_Chunin.wav"), 0.3f);

	return S_OK;
}

void CLevel_Chunin::Update(_float fTimeDelta)
{
	m_pGameInstance->Set_Listener(m_pPlayer);

	_float4 vPlayerPos;
	XMStoreFloat4(&vPlayerPos, m_pPlayer->Get_Position());

	_float4 vLightPos = _float4(31.f, vPlayerPos.y + 35.f, 6.7f, 1.f);
	_float4 vLightAt = vPlayerPos;
	m_pGameInstance->Set_LigthDepth(XMLoadFloat4(&vLightPos), XMLoadFloat4(&vLightAt));

	if (KEY_TAP(KEY::L))
	{
		if (FAILED(m_pGameInstance->Change_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_KONOHA))))
			return;
		return;
	}

	if (m_isControlCamera)
	{
		Start_Camera();
	}

	if (KEY_TAP(KEY::UP))
	{
		CGameObject::GAMEOBJECT_DESC Desc;
		Desc.fRotationPerSec = 0.f;
		Desc.fSpeedPerSec = 0.;
		Desc.iLevelIndex = LEVEL_CHUNIN;

		_float3 vPos;
		XMStoreFloat3(&vPos, m_pPlayer->Get_Position());

		CParticle_FireBall* ppp = dynamic_cast<CParticle_FireBall*>(m_pGameInstance->Get_CloneObject_AddLayer(LEVEL_CHUNIN, TEXT("Layer_Particle"), TEXT("Prototype_GameObject_Particle_FireBall"), &Desc));
		ppp->Appear(m_pPlayer->Get_Position());
	}

	// 대화1
	if (KEY_TAP(KEY::ENTER) && nullptr != m_pNpc_Sasuke)
	{
		if (!m_isEnd_FirstText)
		{
			FirstText();
		}
	}

	// 대화2
	if (m_isSummonMonster && m_pGameInstance->Find_Layer(LEVEL_CHUNIN, TEXT("Layer_Monster"))->Get_ObjectList().size() <= 0)
	{
		m_fSecondTextTime += fTimeDelta;
		SecondText();
	}

	// 보스 소환 카메라
	if (m_isBossAppearCamera)
	{
		_matrix vBossWorldMatrix = m_pSusanoo->Get_Transform()->Get_WorldMatrix();
		if (m_pSusanoo->Get_Fsm()->Get_CurrentState() == CBoss_Susanoo::ROAR)
		{
			_uint iFrame = m_pSusanoo->Get_Model()->Get_Frame(m_pSusanoo->Get_Model()->Get_CurrentAnimationIndex());

			if (iFrame <= 30 && !m_isBossCameraView1)
			{
				_vector vStartCameraPos = vBossWorldMatrix.r[3] + XMVector3Normalize(vBossWorldMatrix.r[2]) * 7.f + XMVector3Normalize(vBossWorldMatrix.r[1]) * 8.f;
				_vector vEndCameraPos = vBossWorldMatrix.r[3] + XMVector3Normalize(vBossWorldMatrix.r[2]) * 3.f + XMVector3Normalize(vBossWorldMatrix.r[1]) * 8.f;

				m_pPlayerCamera->Setting_CameraControl(vStartCameraPos, vEndCameraPos, vStartCameraPos - XMVector3Normalize(vBossWorldMatrix.r[2]) * 10.f);
				m_pPlayerCamera->Start_CameraControl();
				m_pPlayerCamera->Start_CameraLerp(1.f);
				m_isBossCameraView1 = true;
			}
			else if(iFrame > 30 && !m_isBossCameraView2)
			{
				_vector vStartCameraPos = vBossWorldMatrix.r[3] + XMVector3Normalize(vBossWorldMatrix.r[2]) * 7.f + XMVector3Normalize(vBossWorldMatrix.r[1]) * 7.f;
				_vector vEndCameraPos = vBossWorldMatrix.r[3] + XMVector3Normalize(vBossWorldMatrix.r[2]) * 12.f + XMVector3Normalize(vBossWorldMatrix.r[1]) * 7.f;

				m_pPlayerCamera->Setting_CameraControl(vStartCameraPos, vEndCameraPos, vStartCameraPos - XMVector3Normalize(vBossWorldMatrix.r[2]) * 10.f);
				m_pPlayerCamera->Start_CameraControl();
				m_pPlayerCamera->Start_CameraLerp(0.3f);
				m_pPlayerCamera->Start_Shake(0.3f, 1.3f);
				m_isBossCameraView2 = true;
			}
		}
		else
		{
			m_fBossCameraTime += fTimeDelta;
			if (m_fBossCameraTime >= 2.f)
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
		//CMonster::MONSTER_DESC MonsterDesc;
		//MonsterDesc.fRotationPerSec = 10.f;
		//MonsterDesc.fSpeedPerSec = 10.f;
		//MonsterDesc.vScale = _float3(1.f, 1.f, 1.f);
		//MonsterDesc.iLevelIndex = LEVEL_CHUNIN;
		//strcpy_s(MonsterDesc.szModelTag, "Prototype_Component_Model_WhiteZetsu");

		//MonsterDesc.vPosition = _float3(-14.f, -25.f, 9.f);
		//if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_CHUNIN, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_WhitZetsu"), &MonsterDesc)))
		//	return;
		
		Summon_Boss();
	}
}

HRESULT CLevel_Chunin::Render()
{
	_float3 vPlayerPos;
	XMStoreFloat3(&vPlayerPos, (m_pGameInstance->Find_Player(LEVEL_CHUNIN)->Get_Position()));

	_tchar	szPlayerPosText[MAX_PATH] = {};
	swprintf(szPlayerPosText, MAX_PATH, TEXT("X : %.3f, Y : %.3f, Z : %.3f"), vPlayerPos.x, vPlayerPos.y, vPlayerPos.z);

	SetWindowText(g_hWnd, szPlayerPosText);
	return S_OK;
}

HRESULT CLevel_Chunin::Ready_Lights()
{
	m_pGameInstance->Clear_Light();

	LIGHT_DESC			LightDesc{};

	ZeroMemory(&LightDesc, sizeof LightDesc);
	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(0.6f, 0.6f, 0.6f, 1.f);
	LightDesc.vAmbient = _float4(0.7f, 0.7f, 0.7f, 0.7f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	// 스태틱 라이트 설정
	m_vStaticLightDepthPos = _float4(31.f, -5.f, 6.7f, 1.f);
	m_vStaticLightDepthAt = _float4(0.f, -35.f, 0.f, 1.f);
	m_pGameInstance->Set_StaticLightViewMatrix(XMMatrixLookAtLH(XMLoadFloat4(&m_vStaticLightDepthPos), XMLoadFloat4(&m_vStaticLightDepthAt), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	m_pGameInstance->Load_StaticLightDepthTexture(TEXT("../Bin/DataFiles/StaticLight/StaticLighDepth_Chunin.dds"));

	//ZeroMemory(&LightDesc, sizeof LightDesc);
	//LightDesc.eType = LIGHT_DESC::TYPE_POINT;
	//LightDesc.vPosition = _float4(-7.f, -23.f, 13.f, 1.f);
	//LightDesc.fRange = 7.f;
	//LightDesc.vDiffuse = _float4(1.f, 0.f, 0.f, 1.f);
	//LightDesc.vAmbient = /*_float4(0.4f, 0.2f, 0.2f, 1.f);*/_float4(0.f, 0.f, 0.f, 0.f);
	//LightDesc.vSpecular = LightDesc.vDiffuse;

	//if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
	//	return E_FAIL;

	//ZeroMemory(&LightDesc, sizeof LightDesc);
	//LightDesc.eType = LIGHT_DESC::TYPE_POINT;
	//LightDesc.vPosition = _float4(-2.f, -23.f, 13.f, 1.f);
	//LightDesc.fRange = 7.f;
	//LightDesc.vDiffuse = _float4(0.f, 1.f, 0.f, 1.f);
	//LightDesc.vAmbient = /*_float4(0.2f, 0.4f, 0.2f, 1.f);*/_float4(0.f, 0.f, 0.f, 0.f);
	//LightDesc.vSpecular = LightDesc.vDiffuse;

	//if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Chunin::Ready_Layer_Camera()
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

	Desc.fSpeed = 4.f;
	Desc.pPlayer = dynamic_cast<CPlayer*>(m_pPlayer);

	m_pPlayerCamera = dynamic_cast<CPlayerCamera*>(m_pGameInstance->Get_CloneObject_AddLayer(LEVEL_CHUNIN, TEXT("Layer_Camera"), TEXT("Prototype_GameObject_PlayerCamera"), &Desc));
	if (nullptr == m_pPlayerCamera)
		return E_FAIL;

	Safe_AddRef(m_pPlayerCamera);

	return S_OK;
}


HRESULT CLevel_Chunin::Ready_Layer_BackGround()
{
	CGameObject::GAMEOBJECT_DESC	Desc{};

	Desc.fRotationPerSec = 0.f;
	Desc.fSpeedPerSec = 0.f;
	Desc.iLevelIndex = LEVEL_CHUNIN;

	//if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_CHUNIN, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_Terrain"), &Desc)))
	//	return E_FAIL;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_KONOHA, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_Sky"), &Desc)))
		return E_FAIL;

	if (FAILED(Load_Map()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Chunin::Ready_Layer_Effect()
{
	CGameObject::GAMEOBJECT_DESC Desc;
	Desc.fRotationPerSec = 0.f;
	Desc.fSpeedPerSec = 0.;
	Desc.iLevelIndex = LEVEL_CHUNIN;

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
		CEffect_RasenkoHit* pRasenkoHit = dynamic_cast<CEffect_RasenkoHit*>(m_pGameInstance->Get_CloneObject_AddLayer(LEVEL_CHUNIN, TEXT("Layer_Skill_Effect"), TEXT("Prototype_GameObject_RasenkoHIt"), &Desc));
		CObjectPool<CEffect_RasenkoHit>::Return_GameObject(pRasenkoHit);
	}

	CLightning::LIGHTNING_DESC	LightningDesc{};
	memcpy(&LightningDesc, &Desc, sizeof(CGameObject::GAMEOBJECT_DESC));

	for (_uint i = 0; i < CLightning::LIGHTNING_END; ++i)
	{
		for (_uint j = 0; j < 80; ++j)
		{
			LightningDesc.eType = CLightning::LIGHTNING_TYPE(j % CLightning::LIGHTNING_END);
			CLightning* pLightning = dynamic_cast<CLightning*>(m_pGameInstance->Get_CloneObject_AddLayer(LEVEL_CHUNIN, TEXT("Layer_Skill_Effect"), TEXT("Prototype_GameObject_Lightning"), &LightningDesc));
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
			CAmateras* pAmateras = dynamic_cast<CAmateras*>(m_pGameInstance->Get_CloneObject_AddLayer(LEVEL_CHUNIN, TEXT("Layer_Skill_Effect"), TEXT("Prototype_GameObject_Amateras"), &AmaterasDesc));
			CObjectPool<CAmateras>::Return_GameObject(pAmateras);
		}
	}

	for (_uint i = 0; i < 100; ++i)
	{
		CRasenganRing* pRasenganRing = dynamic_cast<CRasenganRing*>(m_pGameInstance->Get_CloneObject_AddLayer(LEVEL_CHUNIN, TEXT("Layer_Skill_Effect"), TEXT("Prototype_GameObject_RasenganRing"), &Desc));
		CObjectPool<CRasenganRing>::Return_GameObject(pRasenganRing);
	}

	for (_uint i = 0; i < 1000; ++i)
	{
		CFire05* pFire = dynamic_cast<CFire05*>(m_pGameInstance->Get_CloneObject_AddLayer(LEVEL_CHUNIN, TEXT("Layer_Skill_Effect"), TEXT("Prototype_GameObject_Fire"), &Desc));
		CObjectPool<CFire05>::Return_GameObject(pFire);
	}

	for (_uint i = 0; i < 50; ++i)
	{
		CExplosionCircle* pExplosionCircle = dynamic_cast<CExplosionCircle*>(m_pGameInstance->Get_CloneObject_AddLayer(LEVEL_CHUNIN, TEXT("Layer_Skill_Effect"), TEXT("Prototype_GameObject_ExplosionCircle"), &Desc));
		CObjectPool<CExplosionCircle>::Return_GameObject(pExplosionCircle);
	}

	for (_uint i = 0; i < 10; ++i)
	{
		CBladeSlash* pBladeSlash = dynamic_cast<CBladeSlash*>(m_pGameInstance->Get_CloneObject_AddLayer(LEVEL_CHUNIN, TEXT("Layer_Skill_Effect"), TEXT("Prototype_GameObject_BladeSlash"), &Desc));
		CObjectPool<CBladeSlash>::Return_GameObject(pBladeSlash);
	}

	for (_uint i = 0; i < 300; ++i)
	{
		CParticle_Hit* pParticleHit = dynamic_cast<CParticle_Hit*>(m_pGameInstance->Get_CloneObject_AddLayer(LEVEL_CHUNIN, TEXT("Layer_Particle"), TEXT("Prototype_GameObject_HitParticle"), &LightningDesc));
		CObjectPool<CParticle_Hit>::Return_GameObject(pParticleHit);
	}

	for (_uint i = 0; i < 100; ++i)
	{
		CParticle_Up* pParticleUp = dynamic_cast<CParticle_Up*>(m_pGameInstance->Get_CloneObject_AddLayer(LEVEL_CHUNIN, TEXT("Layer_Particle"), TEXT("Prototype_GameObject_UpParticle"), &LightningDesc));
		CObjectPool<CParticle_Up>::Return_GameObject(pParticleUp);
	}

	for (_uint i = 0; i < 10; ++i)
	{
		CEffect_TypeZero* pTypeZero = dynamic_cast<CEffect_TypeZero*>(m_pGameInstance->Get_CloneObject_AddLayer(LEVEL_CHUNIN, TEXT("Layer_Particle"), TEXT("Prototype_GameObject_TypeZero"), &LightningDesc));
		CObjectPool<CEffect_TypeZero>::Return_GameObject(pTypeZero);
	}

	return S_OK;
}

HRESULT CLevel_Chunin::Ready_LandObjects()
{
	if (FAILED(Ready_Player()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Chunin::Ready_Player()
{
	CPlayer_Custom::CUSTOM_DESC PlayerDesc;
	PlayerDesc.fRotationPerSec = 10.f;
	PlayerDesc.fSpeedPerSec = 10.f;
	PlayerDesc.iLevelIndex = LEVEL_CHUNIN;
	PlayerDesc.vPosition = _float3(-2.f, -25.f, 13.f);

	m_pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_CloneObject_AddLayer(LEVEL_CHUNIN, TEXT("Layer_Player"), TEXT("Prototype_GameObject_PlayerCustom"), &PlayerDesc));
	if (nullptr == m_pPlayer)
		return E_FAIL;

	Safe_AddRef(m_pPlayer);
	m_pPlayer->Get_Transform()->Rotation(0.f, XMConvertToRadians(180.f), 0.f);

	return S_OK;
}

HRESULT CLevel_Chunin::Summon_Boss()
{
	CMonster::MONSTER_DESC MonsterDesc;
	MonsterDesc.fRotationPerSec = 10.f;
	MonsterDesc.fSpeedPerSec = 10.f;
	MonsterDesc.vScale = _float3(1.f, 1.f, 1.f);
	MonsterDesc.iLevelIndex = LEVEL_CHUNIN;
	MonsterDesc.vPosition = _float3(2.f, -25.f, -5.f);

	strcpy_s(MonsterDesc.szModelTag, "Prototype_Component_Model_Susanoo");
	m_pSusanoo = dynamic_cast<CBoss_Susanoo*>(m_pGameInstance->Get_CloneObject_AddLayer(LEVEL_CHUNIN, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Susanoo"), &MonsterDesc));
	if (nullptr == m_pSusanoo)
		return E_FAIL;

	_matrix vBossWorldMatrix = m_pSusanoo->Get_Transform()->Get_WorldMatrix();

	_vector vCameraPos = vBossWorldMatrix.r[3] + XMVector3Normalize(vBossWorldMatrix.r[2]) * 10.f + XMVector3Normalize(vBossWorldMatrix.r[1]) * 4.f;

	m_pPlayerCamera->Setting_CameraControl(vCameraPos, vCameraPos, vCameraPos - XMVector3Normalize(vBossWorldMatrix.r[2]) * 10.f);
	m_pPlayerCamera->Start_CameraControl();

	m_pPlayer->Change_IsRender(false);
	m_pPlayer->Set_IsControl(false);
	m_pPlayer->Get_Transform()->Set_State(CTransform::STATE_POSITION, XMVectorSet(-2.f, -25.f, 13.f, 1.f));

	m_isBossAppearCamera = true;

	return S_OK;
}

HRESULT CLevel_Chunin::Ready_NPC()
{
	CSasuke_Next::NPC_DESC NpcDesc{};
	NpcDesc.fRotationPerSec = 10.f;
	NpcDesc.fSpeedPerSec = 10.f;
	NpcDesc.iLevelIndex = LEVEL_CHUNIN;
	NpcDesc.vPosition = _float3(-3.5f, -25.f, -3.5f);

	m_pNpc_Sasuke = dynamic_cast<CSasuke_Next*>(m_pGameInstance->Get_CloneObject_AddLayer(LEVEL_CHUNIN, TEXT("Layer_Npc"), TEXT("Prototype_GameObject_SasukeNext"), &NpcDesc));
	if (nullptr == m_pNpc_Sasuke)
		return E_FAIL;

	Safe_AddRef(m_pNpc_Sasuke);

	if (FAILED(Ready_NpcText()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Chunin::Load_Map()
{
	_tchar szFinalPath[MAX_PATH] = TEXT("../Bin/DataFiles/MapData/ChuninExam_New.dat");

	HANDLE hFile = 0;
	_ulong dwByte = 0;

	CNonAnimModel::NONMODEL_DESC  NonAnimModelDesc;
	NonAnimModelDesc.iLevelIndex = LEVEL_CHUNIN;

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

		m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_CHUNIN, TEXT("Layer_GameObject_Map"), TEXT("Prototype_GameObject_NonAnim"), &NonAnimModelDesc);
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CLevel_Chunin::Summon_WhiteZetsu()
{
	CMonster::MONSTER_DESC MonsterDesc;
	MonsterDesc.fRotationPerSec = 10.f;
	MonsterDesc.fSpeedPerSec = 10.f;
	MonsterDesc.vScale = _float3(1.f, 1.f, 1.f);
	MonsterDesc.iLevelIndex = LEVEL_CHUNIN;
	strcpy_s(MonsterDesc.szModelTag, "Prototype_Component_Model_WhiteZetsu");

	MonsterDesc.vPosition = _float3(-14.f, -25.f, 9.f);
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_CHUNIN, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_WhitZetsu"), &MonsterDesc)))
		return E_FAIL;

	MonsterDesc.vPosition = _float3(-8.f, -25.f, -15.f);
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_CHUNIN, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_WhitZetsu"), &MonsterDesc)))
		return E_FAIL;

	MonsterDesc.vPosition = _float3(15.f, -25.f, -3.f);
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_CHUNIN, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_WhitZetsu"), &MonsterDesc)))
		return E_FAIL;

	MonsterDesc.vPosition = _float3(10.f, -25.f, 9.f);
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_CHUNIN, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_WhitZetsu"), &MonsterDesc)))
		return E_FAIL;

	MonsterDesc.vPosition = _float3(-20.f, -25.f, 16.f);
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_CHUNIN, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_WhitZetsu"), &MonsterDesc)))
		return E_FAIL;

	MonsterDesc.vPosition = _float3(-20.f, -25.f, 0.f);
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_CHUNIN, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_WhitZetsu"), &MonsterDesc)))
		return E_FAIL;

	m_isSummonMonster = true;

	return S_OK;
}

HRESULT CLevel_Chunin::Ready_NpcText()
{
	m_TextList01.emplace_back(TEXT("너가 이번 일을 도와줄 실력 있는 닌자인가"));
	m_TextList01.emplace_back(TEXT("나도 무슨 일인지 모르지만\n갑자기 나루토가 이상해졌어"));
	m_TextList01.emplace_back(TEXT("나를 도와 그 바보녀석의 정신을 들게 하자."));
	m_TextList01.emplace_back(TEXT("아무리 추천이 있었다고는 해도\n실력을 한 번 봐야겠지?"));
	m_TextList01.emplace_back(TEXT("준비해"));

	m_TextList02.emplace_back(TEXT("역시 그런 잡졸들은 간단히 물리치는군"));
	m_TextList02.emplace_back(TEXT("지금부터는 내가 직접 나선다"));
	m_TextList02.emplace_back(TEXT("쉽지는 않을거다..."));

	return S_OK;
}

void CLevel_Chunin::Start_Camera()
{
	if (!m_isCameraView1)
	{
		m_pPlayerCamera->Setting_CameraControl(_float3(22.f, -5.f, 21.f), _float3(-20.f, -5.f, 30.f), _float3(-2.f, -7.f, -28.f));
		m_pPlayerCamera->Start_CameraControl();
		m_pPlayerCamera->Start_CameraLerp(3.f);

		m_pPlayer->Change_IsRender(false);
		m_pPlayer->Set_IsControl(false);

		m_isCameraView1 = true;
	}
	else if (m_isCameraView1 && !m_isCameraView2 && m_pPlayerCamera->Get_IsLerpEnd())
	{
		m_pPlayerCamera->Setting_CameraControl(_float3(-30.f, -2.f, 10.f), _float3(-13.f, -10.f, -5.f), _float3(-2.f, -25.f, -2.f));
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

void CLevel_Chunin::FirstText()
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

			//Safe_Release(m_pNpc_Sasuke);
			//m_pNpc_Sasuke = nullptr;

			Summon_WhiteZetsu();
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

void CLevel_Chunin::SecondText()
{
	m_pNpc_Sasuke->Change_SpeakMode(m_pPlayerCamera, false);

	if (m_TextList02.size() > m_iTextNum)
	{
		if (m_fSecondTextTime >= 2.5f)
		{
			m_pNpc_Sasuke->Change_TextWindow(m_TextList02[m_iTextNum++]);
			m_fSecondTextTime = 0.f;
		}
	}
	else if (m_TextList02.size() <= m_iTextNum)
	{
		if (m_fSecondTextTime >= 7.f)
		{
			Summon_Boss();
			m_isEnd_SecondText = true;
		}
		else if (m_fSecondTextTime >= 3.f)
		{
			m_pNpc_Sasuke->End_SpeakMode();
		}
	}
}

void CLevel_Chunin::Clear_ObjectPool()
{
	CObjectPool<CEffect_TypeZero>::Clear();
	CObjectPool<CLightning>::Clear();
	CObjectPool<CAmateras>::Clear();
	CObjectPool<CRasenganRing>::Clear();
	CObjectPool<CFire05>::Clear();
	CObjectPool<CExplosionCircle>::Clear();
	CObjectPool<CBladeSlash>::Clear();
	CObjectPool<CParticle_Hit>::Clear();
	CObjectPool<CParticle_Up>::Clear();
	CObjectPool<CEffect_RasenkoHit>::Clear();
	CObjectPool<CEffect_TypeZero>::Clear(); 
	CObjectPool<CSmoke11>::Clear();
	CObjectPool<CSmokeRing>::Clear();
	CObjectPool<CEffect_GroundBreak>::Clear();
	CObjectPool<CEffect_Distortion>::Clear();
}

CLevel_Chunin* CLevel_Chunin::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Chunin* pInstance = new CLevel_Chunin(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Chunin"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Chunin::Free()
{
	m_pGameInstance->Reset_PhsyX();

	__super::Free();

	Clear_ObjectPool();

	Safe_Release(m_pPlayerCamera);
	Safe_Release(m_pNpc_Sasuke);
	Safe_Release(m_pPlayer);
}
