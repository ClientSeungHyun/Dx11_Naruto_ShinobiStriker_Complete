#include "stdafx.h"
#include "State_Susanoo_GroundSlash.h"
#include "GameInstance.h"
#include "Boss_Susanoo.h"
#include "Model.h"
#include "ColliderObject.h"

CState_Susanoo_GroundSlash::CState_Susanoo_GroundSlash(CFsm* pFsm, CMonster* pMonster)
	:CState{ pFsm }
	, m_pMonster{ pMonster }
{
}

HRESULT CState_Susanoo_GroundSlash::Initialize(_uint iStateNum)
{
	m_iAnimation_GroundSlash = m_pMonster->Get_Model()->Get_AnimationIndex("RineSusanoPS_Attack_GroundSlash", 1.0);
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Susanoo_GroundSlash::Start_State(void* pArg)
{
	m_pMonster->SetUp_NextAnimation(m_iAnimation_GroundSlash, false);
	m_pMonster->Set_IsStrongAttack(true);

	return S_OK;
}

void CState_Susanoo_GroundSlash::Update(_float fTimeDelta)
{
	_uint iFrame = m_pMonster->Get_Frame(m_iAnimation_GroundSlash);

	if (m_pMonster->Get_IsEndAnimation(m_iAnimation_GroundSlash))
		m_pMonster->Change_State(CBoss_Susanoo::IDLE);

	if (iFrame >= 5 && iFrame <= 15)
	{
		_vector vPos = m_pMonster->Get_Position();
		vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + 5.f * fTimeDelta);

		m_pMonster->Get_Transform()->Set_State(CTransform::STATE_POSITION, vPos);
	}
	else if (iFrame > 20 && iFrame < 25)
	{
		m_fDownTime += 3.f * fTimeDelta;
		_vector vCurrentPos = m_pMonster->Get_Position();
		_vector vTargetPos = XMVectorSetY(vCurrentPos, -25.f);
		
		_vector vPos = XMVectorLerp(vCurrentPos, vTargetPos, m_fDownTime);

		m_pMonster->Get_Transform()->Set_State(CTransform::STATE_POSITION, vPos);
	}
	else if (iFrame >= 26 && iFrame < 28 && !m_isAttack)
	{
		dynamic_cast<CBoss_Susanoo*>(m_pMonster)->Appear_GroundSlash();
		m_isAttack = true;
	}
	
}

void CState_Susanoo_GroundSlash::End_State()
{
	m_isAttack = false;
	m_fDownTime = 0.f;
	m_pMonster->Set_IsStrongAttack(false);
}

CState_Susanoo_GroundSlash* CState_Susanoo_GroundSlash::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum)
{
	CState_Susanoo_GroundSlash* pInstance = new CState_Susanoo_GroundSlash(pFsm, pMonster);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Susanoo_DoubleSlash"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Susanoo_GroundSlash::Free()
{
	__super::Free();
}
