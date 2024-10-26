#include "stdafx.h"
#include "State_Susanoo_DoubleSlash.h"
#include "GameInstance.h"
#include "Boss_Susanoo.h"
#include "Model.h"
#include "ColliderObject.h"

CState_Susanoo_DoubleSlash::CState_Susanoo_DoubleSlash(CFsm* pFsm, CMonster* pMonster)
	:CState{ pFsm }
	, m_pMonster{ pMonster }
{
}

HRESULT CState_Susanoo_DoubleSlash::Initialize(_uint iStateNum)
{
	m_iAnimation_DoubleSlash = m_pMonster->Get_Model()->Get_AnimationIndex("RineSusanoPS_Attack_DoubleSlashing", 0.6);
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Susanoo_DoubleSlash::Start_State(void* pArg)
{
	m_pMonster->SetUp_NextAnimation(m_iAnimation_DoubleSlash, false);
	m_pMonster->Set_IsStrongAttack(true);

	Look_Player();

	return S_OK;
}

void CState_Susanoo_DoubleSlash::Update(_float fTimeDelta)
{
	if (m_pMonster->Get_IsEndAnimation(m_iAnimation_DoubleSlash))
		m_pMonster->Change_State(CBoss_Susanoo::IDLE);

	Attack_Collider();
}

void CState_Susanoo_DoubleSlash::End_State()
{
	m_isSlash1 = false;
	m_isSlash2 = false;
	m_pMonster->Set_IsStrongAttack(false);
}

void CState_Susanoo_DoubleSlash::Attack_Collider()
{
	_uint iFrame = m_pMonster->Get_Frame(m_iAnimation_DoubleSlash);

	if (iFrame >= m_iAttackFrame1 && iFrame <= m_iAttackFrame1 + 5 && !m_isSlash1)
	{
		dynamic_cast<CBoss_Susanoo*>(m_pMonster)->Appear_BladeSlsh();
		m_isSlash1 = true;
	}
	else if (iFrame >= m_iAttackFrame2 && iFrame <= m_iAttackFrame2 + 5 && !m_isSlash2)
	{
		dynamic_cast<CBoss_Susanoo*>(m_pMonster)->Appear_BladeSlsh();
		m_isSlash2 = true;
	}

}

void CState_Susanoo_DoubleSlash::Look_Player()
{
	m_pMonster->Find_PlayerTarget();

	CGameObject* pTarget = m_pMonster->Get_PlayerTarget();
	if (nullptr != pTarget)
	{
		m_pMonster->Get_Transform()->LookAt_Forward(pTarget->Get_Position());
	}
}

CState_Susanoo_DoubleSlash* CState_Susanoo_DoubleSlash::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum)
{
	CState_Susanoo_DoubleSlash* pInstance = new CState_Susanoo_DoubleSlash(pFsm, pMonster);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Susanoo_DoubleSlash"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Susanoo_DoubleSlash::Free()
{
	__super::Free();
}
