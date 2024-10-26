#include "stdafx.h"
#include "State_Naruto_PunchLeft.h"
#include "GameInstance.h"
#include "Player_Naruto.h"
#include "Model.h"
#include "Collider_PlayerAttack.h"

CState_Naruto_PunchLeft::CState_Naruto_PunchLeft(CFsm* pFsm, CPlayer* pPlayer)
	:CState{ pFsm }
	, m_pPlayer{ pPlayer }
{
}

HRESULT CState_Naruto_PunchLeft::Initialize(_uint iStateNum)
{
	m_iAnimation_PunchLeft = m_pPlayer->Get_Model()->Get_AnimationIndex("Naruto_Attack_Punch_Left", 2.3);
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Naruto_PunchLeft::Start_State(void* pArg)
{
	m_pPlayer->SetUp_NextAnimation(m_iAnimation_PunchLeft, false, 0.3f);
	m_pPlayer->Find_NearTarget();
	
	m_isChase = false;
	m_isSound = false;


	return S_OK;
}

void CState_Naruto_PunchLeft::Update(_float fTimeDelta)
{
	_uint iFrame = m_pPlayer->Get_Frame(m_iAnimation_PunchLeft);

	if (5 <= iFrame && iFrame <= 14)
	{
		if (!m_isSound)
		{
			m_pPlayer->Get_SoundCom()->Play3D(TEXT("Naruto_Attack3.wav"), g_fVolume);
			m_isSound = true;
		}

		m_pPlayer->Chase_Target(fTimeDelta, 5.f);
		m_isChase = true;
	}

	if (iFrame >= 13)
	{
		if (KEY_TAP(KEY::LBUTTON))
		{
			//m_pPlayer->Change_State(CPlayer_Naruto::ATTACK2);
			return;
		}

	}

	if (m_pPlayer->Get_IsEndAnimation(m_iAnimation_PunchLeft))
		m_pPlayer->Change_State(CPlayer_Naruto::IDLE);


	Attack_Collider();
}

void CState_Naruto_PunchLeft::End_State()
{
	m_pPlayer->Get_AttackCollider()->IsActive(false);
}

void CState_Naruto_PunchLeft::Attack_Collider()
{
	_uint iFrame = m_pPlayer->Get_Frame(m_iAnimation_PunchLeft);


	if (iFrame >= m_iAttackFrame && iFrame <= m_iAttackFrame + 10)
	{
		m_pPlayer->Get_AttackCollider()->IsActive(true);
	}
	else
	{
		m_pPlayer->Get_AttackCollider()->IsActive(false);
	}

}

CState_Naruto_PunchLeft* CState_Naruto_PunchLeft::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Naruto_PunchLeft* pInstance = new CState_Naruto_PunchLeft(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Naruto_Idle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Naruto_PunchLeft::Free()
{
	__super::Free();
}
