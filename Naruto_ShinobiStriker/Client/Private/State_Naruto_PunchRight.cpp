#include "stdafx.h"
#include "State_Naruto_PunchRight.h"
#include "GameInstance.h"
#include "Player_Naruto.h"
#include "Model.h"
#include "Collider_PlayerAttack.h"

CState_Naruto_PunchRight::CState_Naruto_PunchRight(CFsm* pFsm, CPlayer* pPlayer)
	:CState{ pFsm }
	, m_pPlayer{ pPlayer }
{
}

HRESULT CState_Naruto_PunchRight::Initialize(_uint iStateNum)
{
	m_iAnimation_PunchRight = m_pPlayer->Get_Model()->Get_AnimationIndex("Naruto_Attack_Punch_Right", 2.3);
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Naruto_PunchRight::Start_State(void* pArg)
{
	m_pPlayer->SetUp_NextAnimation(m_iAnimation_PunchRight, false, 0.1f);

	m_isChase = false;

	return S_OK;
}

void CState_Naruto_PunchRight::Update(_float fTimeDelta)
{
	_int iFrame = m_pPlayer->Get_Frame(m_iAnimation_PunchRight);

	if (5 <= iFrame && iFrame <= 14)
	{
		m_pPlayer->Chase_Target(fTimeDelta, 5.f);
		m_isChase = true;
	}

	if (iFrame >= 15)
	{
		if (KEY_TAP(KEY::LBUTTON))
		{
			//m_pPlayer->Change_State(CPlayer_Naruto::ATTACK3);
			return;
		}

	}

	if (m_pPlayer->Get_IsEndAnimation(m_iAnimation_PunchRight))
		m_pPlayer->Change_State(CPlayer_Naruto::IDLE);


	Attack_Collider();
}

void CState_Naruto_PunchRight::End_State()
{
	m_pPlayer->Get_AttackCollider()->IsActive(false);
}

void CState_Naruto_PunchRight::Attack_Collider()
{
	_uint iFrame = m_pPlayer->Get_Frame(m_iAnimation_PunchRight);


	if (iFrame >= m_iAttackFrame && iFrame <= m_iAttackFrame + 10)
	{
		m_pPlayer->Get_AttackCollider()->IsActive(true);
	}
	else
	{
		m_pPlayer->Get_AttackCollider()->IsActive(false);
	}

}

CState_Naruto_PunchRight* CState_Naruto_PunchRight::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Naruto_PunchRight* pInstance = new CState_Naruto_PunchRight(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Naruto_Idle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Naruto_PunchRight::Free()
{
	__super::Free();
}
