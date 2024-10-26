#include "stdafx.h"
#include "State_Kurama_Bite.h"
#include "GameInstance.h"
#include "Boss_Kurama.h"
#include "Model.h"
#include "ColliderObject.h"

CState_Kurama_Bite::CState_Kurama_Bite(CFsm* pFsm, CMonster* pMonster)
	:CState{ pFsm }
	, m_pMonster{ pMonster }
{
}

HRESULT CState_Kurama_Bite::Initialize(_uint iStateNum)
{
	m_iAnimation_Bite = m_pMonster->Get_Model()->Get_AnimationIndex("Kurama_Attack_HugeBite", 1.7);
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Kurama_Bite::Start_State(void* pArg)
{
	m_pMonster->SetUp_NextAnimation(m_iAnimation_Bite, false);
	m_pMonster->Set_IsStrongAttack(true);

	return S_OK;
}

void CState_Kurama_Bite::Update(_float fTimeDelta)
{
	if (m_pMonster->Get_IsEndAnimation(m_iAnimation_Bite))
		m_pMonster->Change_State(CBoss_Kurama::IDLE);

	Attack_Collider();
}

void CState_Kurama_Bite::End_State()
{
	m_pMonster->Set_IsStrongAttack(false);
}

void CState_Kurama_Bite::Attack_Collider()
{
	_uint iFrame = m_pMonster->Get_Frame(m_iAnimation_Bite);

	if (iFrame >= m_iAttackFrame && iFrame <= m_iAttackFrame + 10)
	{
		m_pMonster->Get_AttackCollider()->IsActive(true);
	}
	else
	{
		m_pMonster->Get_AttackCollider()->IsActive(false);
	}

}

CState_Kurama_Bite* CState_Kurama_Bite::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum)
{
	CState_Kurama_Bite* pInstance = new CState_Kurama_Bite(pFsm, pMonster);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Kurama_Bite"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Kurama_Bite::Free()
{
	__super::Free();
}
