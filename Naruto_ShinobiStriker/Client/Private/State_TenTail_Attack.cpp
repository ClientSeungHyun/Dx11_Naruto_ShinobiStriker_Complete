#include "stdafx.h"
#include "State_TenTail_Attack.h"
#include "GameInstance.h"
#include "TenTail.h"
#include "Model.h"


CState_TenTail_Attack::CState_TenTail_Attack(CFsm* pFsm, CMonster* pMonster)
	:CState{ pFsm }
	, m_pMonster{ pMonster }
{
}

HRESULT CState_TenTail_Attack::Initialize(_uint iStateNum)
{
	m_iAnimation_Punch = m_pMonster->Get_Model()->Get_AnimationIndex("TenTailsCloneLoser01_Attack_MowDown", 1.5);
	m_iAnimation_DownStrike = m_pMonster->Get_Model()->Get_AnimationIndex("TenTailsCloneLoser01_Attack_DownStrike", 2.0);

	m_iStateNum = iStateNum;

	//125
	return S_OK;
}

HRESULT CState_TenTail_Attack::Start_State(void* pArg)
{
	m_pMonster->SetUp_NextAnimation(m_iAnimation_Punch, false);

	m_isPunch = false;

	return S_OK;
}

void CState_TenTail_Attack::Update(_float fTimeDelta)
{
	_uint iCurrentAnimationIndex = m_pMonster->Get_Model()->Get_CurrentAnimationIndex();
	_uint iFrame = m_pMonster->Get_Model()->Get_Frame(iCurrentAnimationIndex);

	if (iCurrentAnimationIndex == m_iAnimation_Punch && iFrame >= 106 && !m_isPunch)
	{
		m_pMonster->SetUp_NextAnimation(m_iAnimation_DownStrike, false, 0.1f);
		m_isPunch = true;
	}
	else if (m_pMonster->Get_IsEndAnimation(m_iAnimation_DownStrike))
	{
		m_pMonster->Change_State(CTenTail::IDLE);
	}
	
	if (iCurrentAnimationIndex == m_iAnimation_Punch)
	{
		if (m_iAttackFrameStart1 <= iFrame && iFrame <= m_iAttackFrameEnd1)
		{
			m_pMonster->Chase_Player(fTimeDelta, m_AttackMoveSpped);
		}

	}
	else if (iCurrentAnimationIndex == m_iAnimation_DownStrike)
	{
		if ((m_iAttackFrameStart2 <= iFrame && iFrame <= m_iAttackFrameEnd2))
		{
			m_pMonster->Chase_Player(fTimeDelta, m_AttackMoveSpped);
		}
	}
}

void CState_TenTail_Attack::End_State()
{
}

CState_TenTail_Attack* CState_TenTail_Attack::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum)
{
	CState_TenTail_Attack* pInstance = new CState_TenTail_Attack(pFsm, pMonster);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_TenTail_Attack"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_TenTail_Attack::Free()
{
	__super::Free();
}
