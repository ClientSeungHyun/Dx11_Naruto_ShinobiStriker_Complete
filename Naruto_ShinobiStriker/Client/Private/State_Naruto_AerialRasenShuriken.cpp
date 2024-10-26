#include "stdafx.h"
#include "State_Naruto_AerialRasenShuriken.h"
#include "GameInstance.h"
#include "Player_Naruto.h"
#include "Shadow_Naruto.h"
#include "Model.h"


CState_Naruto_AerialRasenShuriken::CState_Naruto_AerialRasenShuriken(CFsm* pFsm, CPlayer* pPlayer)
	:CState{ pFsm }
	, m_pPlayer{ pPlayer }
{
}

HRESULT CState_Naruto_AerialRasenShuriken::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;

	m_iAnimation_AerialRasenShuriken = m_pPlayer->Get_Model()->Get_AnimationIndex("Naruto_Ninjutsu_Aerial_TrueRasenShuriken", 1.5);

	m_iFrame_Create = 10;
	m_iFrame_Shoot = 70;

	return S_OK;
}

HRESULT CState_Naruto_AerialRasenShuriken::Start_State(void* pArg)
{
	m_pPlayer->SetUp_NextAnimation(m_iAnimation_AerialRasenShuriken, false, 0.1f);

	m_pPlayer->Set_IsGravity(false);
	m_pPlayer->Set_IsInvincible(true);

	m_pPlayer->Get_RigidBody()->Set_IsGravity(false);
	m_pPlayer->Get_RigidBody()->Clear_All();

	CShadow_Naruto::SHADOW_NARUTO_DESC tDesc = {};
	tDesc.fRotationPerSec = 0.f;
	tDesc.fSpeedPerSec = 0.f;
	tDesc.eSkill = CShadow_Naruto::RASENSHURIKEN_AERIAL;

	dynamic_cast<CPlayer_Naruto*>(m_pPlayer)->Create_ShadowNaruto(tDesc);
	dynamic_cast<CPlayer_Naruto*>(m_pPlayer)->Set_IsSenninEye(true);

	m_isShoot = false;

	m_pPlayer->Get_SoundCom()->Play3D(TEXT("Naruto_RasenShuriken.wav"), g_fVolume);

	m_pPlayer->Set_IsSkill(true);

	return S_OK;
}

void CState_Naruto_AerialRasenShuriken::Update(_float fTimeDelta)
{
	_uint iFrame = m_pPlayer->Get_Frame(m_iAnimation_AerialRasenShuriken);

	// 이 프레임 사이에서는 적을 따라서 회전함
	if (45 < iFrame && iFrame < m_iFrame_Shoot - 5)
	{
		Chase_Target(fTimeDelta);
	}

	if (m_iFrame_Create < iFrame && iFrame < m_iFrame_Shoot)
	{
		dynamic_cast<CPlayer_Naruto*>(m_pPlayer)->Create_RasenShuriken();
	}
	else if (iFrame >= m_iFrame_Shoot && !m_isShoot)
	{
		//m_pGameInstance->Stop_Sound(SOUND_SKILL);
		
		dynamic_cast<CPlayer_Naruto*>(m_pPlayer)->Shoot_RasenShuriken();

		//m_pGameInstance->Play3D(TEXT("RasenShuriken_Start.wav"), SOUND_SKILL, g_fVolume, m_pPlayer->Get_Position());

		m_isShoot = true;
	}

	if (m_pPlayer->Get_IsEndAnimation(m_iAnimation_AerialRasenShuriken))
		m_pPlayer->Change_State(CPlayer_Naruto::FALL_FRONT);
}

void CState_Naruto_AerialRasenShuriken::End_State()
{
	m_pPlayer->Set_IsInvincible(false);
	//m_pGameInstance->Stop_Sound(SOUND_SKILL);
	m_pPlayer->Set_IsSkill(false);
	dynamic_cast<CPlayer_Naruto*>(m_pPlayer)->Set_IsSenninEye(false);
}

void CState_Naruto_AerialRasenShuriken::Chase_Target(_float fTimeDelta)
{
	CGameObject* pTarget = m_pPlayer->Get_NearTarget();

	if (nullptr != pTarget)
	{
		_vector vTargetPos = pTarget->Get_Position();
		_vector vPlayerPos = m_pPlayer->Get_Position();
		_vector vChaseDir = vTargetPos - vPlayerPos;

		m_pPlayer->Get_Transform()->LookAt(vTargetPos);
	}
}

CState_Naruto_AerialRasenShuriken* CState_Naruto_AerialRasenShuriken::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Naruto_AerialRasenShuriken* pInstance = new CState_Naruto_AerialRasenShuriken(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Naruto_Idle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Naruto_AerialRasenShuriken::Free()
{
	__super::Free();
}
