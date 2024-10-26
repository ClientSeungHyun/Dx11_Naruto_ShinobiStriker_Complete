#include "stdafx.h"
#include "State_WhiteZetsu_AttackFar.h"
#include "GameInstance.h"
#include "WhiteZetsu.h"
#include "Model.h"
#include "Collider_MonsterAttack.h"

CState_WhiteZetsu_AttackFar::CState_WhiteZetsu_AttackFar(CFsm* pFsm, CMonster* pMonster)
	:CState{ pFsm }
	, m_pMonster{ pMonster }
{
}

HRESULT CState_WhiteZetsu_AttackFar::Initialize(_uint iStateNum)
{
	m_iAnimation_GroundPunch = m_pMonster->Get_Model()->Get_AnimationIndex("WhiteZetsuCrowdForm_Attack_GroundPunch");

	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_WhiteZetsu_AttackFar::Start_State(void* pArg)
{
	m_pMonster->SetUp_NextAnimation(m_iAnimation_GroundPunch, false);

	return S_OK;
}

void CState_WhiteZetsu_AttackFar::Update(_float fTimeDelta)
{
	_uint iFrame = m_pMonster->Get_Model()->Get_Frame(m_iAnimation_GroundPunch);

	if (m_pMonster->Get_IsEndAnimation(m_iAnimation_GroundPunch))
	{
		m_pMonster->Change_State(CWhiteZetsu::IDLE);
	}

	if (m_iAttackFrameStart <= iFrame && iFrame <= m_iAttackFrameEnd)
	{
		m_pMonster->Get_AttackCollider()->IsActive(true);
	}
	else
	{
		m_pMonster->Get_AttackCollider()->IsActive(false);
	}
}

void CState_WhiteZetsu_AttackFar::End_State()
{
}

CState_WhiteZetsu_AttackFar* CState_WhiteZetsu_AttackFar::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum)
{
	CState_WhiteZetsu_AttackFar* pInstance = new CState_WhiteZetsu_AttackFar(pFsm, pMonster);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_WhiteZetsu_AttackFar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_WhiteZetsu_AttackFar::Free()
{
	__super::Free();
}
