#include "stdafx.h"
#include "State_Naruto_ElbowStrike.h"
#include "GameInstance.h"
#include "Player_Naruto.h"
#include "Model.h"
#include "Collider_PlayerAttack.h"

CState_Naruto_ElbowStrike::CState_Naruto_ElbowStrike(CFsm* pFsm, CPlayer* pPlayer)
	:CState{ pFsm }
	, m_pPlayer{ pPlayer }
{
}

HRESULT CState_Naruto_ElbowStrike::Initialize(_uint iStateNum)
{
	m_iAnimation_ElbowStrike = m_pPlayer->Get_Model()->Get_AnimationIndex("Naruto_Attack_ElbowStrike", 2.3);
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Naruto_ElbowStrike::Start_State(void* pArg)
{
	m_pPlayer->SetUp_NextAnimation(m_iAnimation_ElbowStrike, false, 0.1f);

	m_isChase = false;

	return S_OK;
}

void CState_Naruto_ElbowStrike::Update(_float fTimeDelta)
{
	_uint iFrame = m_pPlayer->Get_Frame(m_iAnimation_ElbowStrike);

	if (35 <= iFrame && iFrame <= 45)
	{
		m_pPlayer->Chase_Target(fTimeDelta, 5.f);
		m_isChase = true;
	}

	if (iFrame >= 45)
	{
		if (KEY_TAP(KEY::LBUTTON))
		{
			//m_pPlayer->Change_State(CPlayer_Naruto::ATTACK_STRONG);
			return;
		}

	}

	if (m_pPlayer->Get_IsEndAnimation(m_iAnimation_ElbowStrike))
		m_pPlayer->Change_State(CPlayer_Naruto::IDLE);


	Attack_Collider(fTimeDelta);
}

void CState_Naruto_ElbowStrike::End_State()
{
	m_pPlayer->Get_AttackCollider()->IsActive(false);
}

void CState_Naruto_ElbowStrike::Attack_Collider(_float fTimeDelta)
{
	_uint iFrame = m_pPlayer->Get_Frame(m_iAnimation_ElbowStrike);


	if (iFrame >= m_iAttackFrame && iFrame <= m_iAttackFrame + 10)
	{
		m_pPlayer->Get_AttackCollider()->IsActive(true);
	}
	else
	{
		m_pPlayer->Get_AttackCollider()->IsActive(false);
	}

}

CState_Naruto_ElbowStrike* CState_Naruto_ElbowStrike::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Naruto_ElbowStrike* pInstance = new CState_Naruto_ElbowStrike(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Naruto_ElbowStrike"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Naruto_ElbowStrike::Free()
{
	__super::Free();
}
