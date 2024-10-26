#include "stdafx.h"
#include "State_WhiteZetsu_Attack1.h"
#include "GameInstance.h"
#include "WhiteZetsu.h"
#include "Model.h"
#include "Collider_MonsterAttack.h"

CState_WhiteZetsu_Attack1::CState_WhiteZetsu_Attack1(CFsm* pFsm, CMonster* pMonster)
	:CState{ pFsm }
	, m_pMonster{ pMonster }
{
}

HRESULT CState_WhiteZetsu_Attack1::Initialize(_uint iStateNum)
{
	m_iAnimation_DownStrike = m_pMonster->Get_Model()->Get_AnimationIndex("WhiteZetsuCrowdForm_Attack_DownStrike", 1.5);
	m_iAnimation_JumpKick = m_pMonster->Get_Model()->Get_AnimationIndex("WhiteZetsuCrowdForm_Attack_JumpTurnKick", 1.5);

	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_WhiteZetsu_Attack1::Start_State(void* pArg)
{
	m_pMonster->SetUp_NextAnimation(m_iAnimation_DownStrike, false);

	m_isDownStrike = false;

	return S_OK;
}

void CState_WhiteZetsu_Attack1::Update(_float fTimeDelta)
{
	_uint iCurrentAnimationIndex = m_pMonster->Get_Model()->Get_CurrentAnimationIndex();
	_uint iFrame = m_pMonster->Get_Model()->Get_Frame(iCurrentAnimationIndex);

	if (iCurrentAnimationIndex == m_iAnimation_DownStrike && iFrame >= 26 && !m_isDownStrike)
	{
		m_pMonster->SetUp_NextAnimation(m_iAnimation_JumpKick, false, 0.1f);
		m_isDownStrike = true;
	}
	else if (m_pMonster->Get_IsEndAnimation(m_iAnimation_JumpKick))
	{
		m_pMonster->Change_State(CWhiteZetsu::IDLE);
	}

	if (iCurrentAnimationIndex == m_iAnimation_DownStrike)
	{
		if (m_iAttackFrameStart1 <= iFrame && iFrame <= m_iAttackFrameEnd1)
		{
			m_pMonster->Get_AttackCollider()->IsActive(true);
			m_pMonster->Chase_Player(fTimeDelta, m_AttackMoveSpped);
		}
		else
		{
			m_pMonster->Get_AttackCollider()->IsActive(false);
		}

	}
	else if (iCurrentAnimationIndex == m_iAnimation_JumpKick)
	{
		if (m_iAttackFrameStart2 <= iFrame && iFrame <= m_iAttackFrameEnd2) 
		{
			m_pMonster->Get_AttackCollider()->IsActive(true);
			m_pMonster->Chase_Player(fTimeDelta, m_AttackMoveSpped);
		}
		else if (m_iAttackFrameStart3 <= iFrame && iFrame <= m_iAttackFrameEnd3)
		{
			m_pMonster->Set_IsStrongAttack(true);
			m_pMonster->Get_AttackCollider()->IsActive(true);
			m_pMonster->Chase_Player(fTimeDelta, m_AttackMoveSpped);
		}
		else
		{
			m_pMonster->Set_IsStrongAttack(false);
			m_pMonster->Get_AttackCollider()->IsActive(false);
		}
	}
}

void CState_WhiteZetsu_Attack1::End_State()
{
	m_pMonster->Set_IsStrongAttack(false);
	m_pMonster->Get_AttackCollider()->IsActive(false);
}

CState_WhiteZetsu_Attack1* CState_WhiteZetsu_Attack1::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum)
{
	CState_WhiteZetsu_Attack1* pInstance = new CState_WhiteZetsu_Attack1(pFsm, pMonster);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_WhiteZetsu_Attack1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_WhiteZetsu_Attack1::Free()
{
	__super::Free();
}
