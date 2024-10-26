#include "stdafx.h"
#include "State_Susanoo_Rising.h"
#include "GameInstance.h"
#include "Boss_Susanoo.h"
#include "Model.h"
#include "ColliderObject.h"

CState_Susanoo_Rising::CState_Susanoo_Rising(CFsm* pFsm, CMonster* pMonster)
	:CState{ pFsm }
	, m_pMonster{ pMonster }
{
}

HRESULT CState_Susanoo_Rising::Initialize(_uint iStateNum)
{
	m_iAnimation_Rising = m_pMonster->Get_Model()->Get_AnimationIndex("RineSusanoPS_Attack_RisingRotationSlash", 1.0);
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Susanoo_Rising::Start_State(void* pArg)
{
	m_pMonster->SetUp_NextAnimation(m_iAnimation_Rising, false);
	m_pMonster->Set_IsStrongAttack(true);

	return S_OK;
}

void CState_Susanoo_Rising::Update(_float fTimeDelta)
{
	_uint iFrame = m_pMonster->Get_Frame(m_iAnimation_Rising);

	if (m_pMonster->Get_IsEndAnimation(m_iAnimation_Rising))
	{
		//dynamic_cast<CBoss_Susanoo*>(m_pMonster)->Contorl_RootBone();
		m_pMonster->Change_State(CBoss_Susanoo::IDLE);
	}

	if (iFrame >= 33 && iFrame <= 50)
	{
		_vector vPos = m_pMonster->Get_Position();
		vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + 20.f * fTimeDelta);

		m_pMonster->Get_Transform()->Set_State(CTransform::STATE_POSITION, vPos);
	}
	else if(iFrame > 50 && iFrame <=75)
	{
		_vector vPos = m_pMonster->Get_Position();
		vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + 5.f * fTimeDelta);

		m_pMonster->Get_Transform()->Set_State(CTransform::STATE_POSITION, vPos);
	}
	else if (iFrame > 75 && iFrame <= 110)
	{
		_vector vPos = m_pMonster->Get_Position();
		vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) - 3.f * fTimeDelta);

		m_pMonster->Get_Transform()->Set_State(CTransform::STATE_POSITION, vPos);
	}

	Attack_Collider();
}

void CState_Susanoo_Rising::End_State()
{
	m_pMonster->Set_IsStrongAttack(false);
}

void CState_Susanoo_Rising::Attack_Collider()
{
	_uint iFrame = m_pMonster->Get_Frame(m_iAnimation_Rising);

	if (iFrame >= m_iAttackFrame && iFrame <= m_iAttackFrame + 10)
	{
		m_pMonster->Get_AttackCollider()->IsActive(true);
	}
	else
	{
		m_pMonster->Get_AttackCollider()->IsActive(false);
	}

}

CState_Susanoo_Rising* CState_Susanoo_Rising::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum)
{
	CState_Susanoo_Rising* pInstance = new CState_Susanoo_Rising(pFsm, pMonster);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Susanoo_Rising"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Susanoo_Rising::Free()
{
	__super::Free();
}
