#include "stdafx.h"
#include "State_Susanoo_GroundPunch.h"
#include "GameInstance.h"
#include "Boss_Susanoo.h"
#include "Model.h"
#include "ColliderObject.h"

CState_Susanoo_GroundPunch::CState_Susanoo_GroundPunch(CFsm* pFsm, CMonster* pMonster)
	:CState{ pFsm }
	, m_pMonster{ pMonster }
{
}

HRESULT CState_Susanoo_GroundPunch::Initialize(_uint iStateNum)
{
	m_iAnimation_GroundPunch = m_pMonster->Get_Model()->Get_AnimationIndex("RineSusanoPS_Attack_GroundPunch", 1.0);
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Susanoo_GroundPunch::Start_State(void* pArg)
{
	m_pMonster->SetUp_NextAnimation(m_iAnimation_GroundPunch, false);
	m_pMonster->Set_IsStrongAttack(true);
	m_isSummonGroundBreak = false;

	Look_Player();

	return S_OK;
}

void CState_Susanoo_GroundPunch::Update(_float fTimeDelta)
{
	_uint iFrame = m_pMonster->Get_Frame(m_iAnimation_GroundPunch);

	if (m_pMonster->Get_IsEndAnimation(m_iAnimation_GroundPunch))
		m_pMonster->Change_State(CBoss_Susanoo::IDLE);

	if ((iFrame == 20 || iFrame == 21) && !m_isSummonGroundBreak)
	{
		dynamic_cast<CBoss_Susanoo*>(m_pMonster)->Appear_GroundBreak();
		m_isSummonGroundBreak = true;
	}

	Attack_Collider();
}

void CState_Susanoo_GroundPunch::End_State()
{
	m_pMonster->Set_IsStrongAttack(false);
}

void CState_Susanoo_GroundPunch::Attack_Collider()
{
	_uint iFrame = m_pMonster->Get_Frame(m_iAnimation_GroundPunch);

	if (iFrame >= m_iAttackFrame && iFrame <= m_iAttackFrame + 5)
	{
		m_pMonster->Get_AttackCollider()->IsActive(true);
	}
	else
	{
		m_pMonster->Get_AttackCollider()->IsActive(false);
	}

}

void CState_Susanoo_GroundPunch::Look_Player()
{
	m_pMonster->Find_PlayerTarget();

	CGameObject* pTarget = m_pMonster->Get_PlayerTarget();
	if (nullptr != pTarget)
	{
		m_pMonster->Get_Transform()->LookAt_Forward(pTarget->Get_Position());
	}
}

CState_Susanoo_GroundPunch* CState_Susanoo_GroundPunch::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum)
{
	CState_Susanoo_GroundPunch* pInstance = new CState_Susanoo_GroundPunch(pFsm, pMonster);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Susanoo_DoubleSlash"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Susanoo_GroundPunch::Free()
{
	__super::Free();
}
