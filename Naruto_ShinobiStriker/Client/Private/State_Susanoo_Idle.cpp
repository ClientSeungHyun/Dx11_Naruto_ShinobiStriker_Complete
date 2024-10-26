#include "stdafx.h"
#include "State_Susanoo_Idle.h"
#include "GameInstance.h"
#include "Boss_Susanoo.h"
#include "Model.h"


CState_Susanoo_Idle::CState_Susanoo_Idle(CFsm* pFsm, CMonster* pMonster)
	:CState{ pFsm }
	, m_pMonster{ pMonster }
{
}

HRESULT CState_Susanoo_Idle::Initialize(_uint iStateNum)
{
	m_iAnimation_Idle = m_pMonster->Get_Model()->Get_AnimationIndex("RineSusanoPS_Idle_Type01");
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Susanoo_Idle::Start_State(void* pArg)
{
	m_pMonster->SetUp_NextAnimation(m_iAnimation_Idle, true);

	m_pMonster->Find_PlayerTarget();

	m_fCurrentTime = 0.f;
	if (m_pFsm->Get_PrevState() == CBoss_Susanoo::RISING)
		m_fCurrentTime = 2.5f;

	return S_OK;
}

void CState_Susanoo_Idle::Update(_float fTimeDelta)
{
	m_fCurrentTime += fTimeDelta;

	if (m_fCurrentTime >= m_fIdleDuration)
		Select_Attack();

	//_vector vDir = XMVector3Normalize(m_pMonster->Get_PlayerTarget()->Get_Position() - m_pMonster->Get_Position());
	//m_pMonster->Get_Transform()->Turn_Lerp(vDir, 5.0f, fTimeDelta);
}

void CState_Susanoo_Idle::End_State()
{
}

void CState_Susanoo_Idle::Look_Player()
{
	m_pMonster->Find_PlayerTarget();

	CGameObject* pTarget = m_pMonster->Get_PlayerTarget();
	if (nullptr != pTarget)
	{
		m_pMonster->Get_Transform()->LookAt_Forward(pTarget->Get_Position());
	}
}

void CState_Susanoo_Idle::Select_Attack()
{
	if (m_pFsm->Get_PrevState() == CBoss_Susanoo::RISING)
	{
		m_pMonster->Change_State(CBoss_Susanoo::GROUNDSLASH);
		return;
	}

	_float fDistance = XMVectorGetX(XMVector3Length(m_pMonster->Get_PlayerTarget()->Get_Position() - m_pMonster->Get_Position()));

	_uint iRand = rand() % 40;
	if (fDistance >= 6.f)
	{
		switch (iRand / 10)
		{
		case 0:
		case 1:
			m_pMonster->Change_State(CBoss_Susanoo::DOUBLESLASH);
			break;
		case 2:
		case 3:
			m_pMonster->Change_State(CBoss_Susanoo::DOUBLESLASH);
			break;
		}
	}
	else
	{
		switch (iRand / 10)
		{
		case 0:
		case 1:
			m_pMonster->Change_State(CBoss_Susanoo::RISING);
			break;
		case 2:
		case 3:
			m_pMonster->Change_State(CBoss_Susanoo::GROUNDPUNCH);
			break;
		}
	}
}

CState_Susanoo_Idle* CState_Susanoo_Idle::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum)
{
	CState_Susanoo_Idle* pInstance = new CState_Susanoo_Idle(pFsm, pMonster);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Susanoo_Idle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Susanoo_Idle::Free()
{
	__super::Free();
}
