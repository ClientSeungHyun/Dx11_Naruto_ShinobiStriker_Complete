#include "stdafx.h"
#include "State_Custom_DoubleHandPunch.h"
#include "GameInstance.h"
#include "Player_Custom.h"
#include "Model.h"
#include "Collider_PlayerAttack.h"
#include "PlayerCamera.h"

CState_Custom_DoubleHandPunch::CState_Custom_DoubleHandPunch(CFsm* pFsm, CPlayer* pPlayer)
	:CState{ pFsm }
	, m_pPlayer{ pPlayer }
{
}

HRESULT CState_Custom_DoubleHandPunch::Initialize(_uint iStateNum)
{
	m_iAnimation_DoublePunch = m_pPlayer->Get_Model()->Get_AnimationIndex("D34_Attack_Aerial_Hand_DoublePunch", 1.5);
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Custom_DoubleHandPunch::Start_State(void* pArg)
{
	m_pPlayer->SetUp_NextAnimation(m_iAnimation_DoublePunch, false, 0.1f);
	m_pPlayer->Find_NearTarget();
	m_pPlayer->Set_IsSkill(true);

	m_isChase = false;
	m_isSound = false;

	return S_OK;
}

void CState_Custom_DoubleHandPunch::Update(_float fTimeDelta)
{
	_int iFrame = m_pPlayer->Get_Frame(m_iAnimation_DoublePunch);

	if (7 <= iFrame && iFrame <= 11)
	{
		if (!m_isSound)
		{
			m_pPlayer->Get_SoundCom()->Play3D(TEXT("Voice_Orokana.wav"), g_fVolume + 0.2f);
			m_isSound = true;
		}

		//m_pPlayer->Chase_Target(fTimeDelta, 8.f);
		m_isChase = true;
	}

	if (m_pPlayer->Get_IsEndAnimation(m_iAnimation_DoublePunch))
		m_pPlayer->Change_State(CPlayer_Custom::FALL_FRONT);


	Attack_Collider();
}

void CState_Custom_DoubleHandPunch::End_State()
{
	m_pPlayer->Get_Camera()->End_CameraControl();
	m_pPlayer->Set_IsSkill(false);
	m_pPlayer->Get_AttackCollider()->IsActive(false);
}

void CState_Custom_DoubleHandPunch::Attack_Collider()
{
	_uint iFrame = m_pPlayer->Get_Frame(m_iAnimation_DoublePunch);


	if (iFrame == m_iAttackFrame || iFrame == m_iAttackFrame + 1)
	{
		m_pPlayer->Get_AttackCollider()->IsActive(true);
	}
	else
	{
		m_pPlayer->Get_AttackCollider()->IsActive(false);
	}

}

CState_Custom_DoubleHandPunch* CState_Custom_DoubleHandPunch::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Custom_DoubleHandPunch* pInstance = new CState_Custom_DoubleHandPunch(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Custom_PunchLeft"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Custom_DoubleHandPunch::Free()
{
	__super::Free();
}
