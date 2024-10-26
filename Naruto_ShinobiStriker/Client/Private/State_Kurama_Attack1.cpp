#include "stdafx.h"
#include "State_Kurama_Attack1.h"
#include "GameInstance.h"
#include "Boss_Kurama.h"
#include "Model.h"
#include "Collider_MonsterAttack.h"

CState_Kurama_Attack1::CState_Kurama_Attack1(CFsm* pFsm, CMonster* pMonster)
	:CState{ pFsm }
	, m_pMonster{ pMonster }
{
}

HRESULT CState_Kurama_Attack1::Initialize(_uint iStateNum)
{
	m_iAnimation_DownRight = m_pMonster->Get_Model()->Get_AnimationIndex("Kurama_Attack_MowDown_Right", 2.5);
	m_iAnimation_DownLeft = m_pMonster->Get_Model()->Get_AnimationIndex("Kurama_Attack_MowDown_Left", 2.5);
	m_iAnimation_KickUp = m_pMonster->Get_Model()->Get_AnimationIndex("Kurama_Attack_KickUp", 2.5);

	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Kurama_Attack1::Start_State(void* pArg)
{
	m_pMonster->SetUp_NextAnimation(m_iAnimation_DownRight, false);

	return S_OK;
}

void CState_Kurama_Attack1::Update(_float fTimeDelta)
{
	_uint iCurrentAnimation = m_pMonster->Get_CurrentAnimationIndex();
	_uint iFrame = m_pMonster->Get_Frame(iCurrentAnimation);

	if (iCurrentAnimation == m_iAnimation_DownRight && iFrame >= 45)
		m_pMonster->SetUp_NextAnimation(m_iAnimation_DownLeft);
	else if (iCurrentAnimation == m_iAnimation_DownLeft && iFrame >= 40)
		m_pMonster->SetUp_NextAnimation(m_iAnimation_KickUp);
	else if (iCurrentAnimation == m_iAnimation_KickUp && m_pMonster->Get_IsEndAnimation(m_iAnimation_KickUp))
		m_pMonster->Change_State(CBoss_Kurama::IDLE);

	Attack_Collider();
}

void CState_Kurama_Attack1::End_State()
{
}

void CState_Kurama_Attack1::Look_Player()
{
	m_pMonster->Find_PlayerTarget();

	CGameObject* pTarget = m_pMonster->Get_PlayerTarget();
	if (nullptr != pTarget)
	{
		m_pMonster->Get_Transform()->LookAt_Forward(pTarget->Get_Position());
	}
}

void CState_Kurama_Attack1::Attack_Collider()
{
	_uint iAnimationIndex = m_pMonster->Get_CurrentAnimationIndex();
	_uint iFrame = m_pMonster->Get_Frame(iAnimationIndex);

	m_pMonster->Get_AttackCollider()->IsActive(false);

	if (iAnimationIndex == m_iAnimation_DownRight)
	{
		if (iFrame >= m_iAttackFrame1 && iFrame <= m_iAttackFrame1 + 10)
		{
			m_pMonster->Get_AttackCollider()->IsActive(true);
		}
	}
	else if (iAnimationIndex == m_iAnimation_DownLeft)
	{
		if (iFrame >= m_iAttackFrame2 && iFrame <= m_iAttackFrame2 + 7)
		{
			m_pMonster->Get_AttackCollider()->IsActive(true);
		}
	}
	else if (iAnimationIndex == m_iAnimation_KickUp)
	{
		if (iFrame >= m_iAttackFrame3 && iFrame <= m_iAttackFrame1 + 10)
		{
			m_pMonster->Set_IsStrongAttack(true);
			m_pMonster->Get_AttackCollider()->IsActive(true);
		}
	}
	else 
	{
		m_pMonster->Set_IsStrongAttack(false);
	}

}

CState_Kurama_Attack1* CState_Kurama_Attack1::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum)
{
	CState_Kurama_Attack1* pInstance = new CState_Kurama_Attack1(pFsm, pMonster);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Kurama_Attack1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Kurama_Attack1::Free()
{
	__super::Free();
}
