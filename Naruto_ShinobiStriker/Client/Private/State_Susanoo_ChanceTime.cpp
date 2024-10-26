#include "stdafx.h"
#include "State_Susanoo_ChanceTime.h"
#include "GameInstance.h"
#include "Boss_Susanoo.h"
#include "Model.h"


CState_Susanoo_ChanceTime::CState_Susanoo_ChanceTime(CFsm* pFsm, CMonster* pMonster)
	:CState{ pFsm }
	, m_pMonster{ pMonster }
{
}

HRESULT CState_Susanoo_ChanceTime::Initialize(_uint iStateNum)
{
	m_iAnimation_ChanceStart = m_pMonster->Get_Model()->Get_AnimationIndex("RineSusanoPS_etc_ChanceTime_Start", 1.5);
	m_iAnimation_ChanceLoop = m_pMonster->Get_Model()->Get_AnimationIndex("RineSusanoPS_etc_ChanceTime_Loop", 1.0);
	m_iAnimation_ChanceEnd = m_pMonster->Get_Model()->Get_AnimationIndex("RineSusanoPS_etc_ChanceTime_End", 1.5);

	m_iStateNum = iStateNum;
	m_fChaneDuration = 3.f;

	return S_OK;
}

HRESULT CState_Susanoo_ChanceTime::Start_State(void* pArg)
{
	m_pMonster->SetUp_NextAnimation(m_iAnimation_ChanceStart, false);

	if (m_pFsm->Get_PrevState() != CBoss_Susanoo::HIT)
	{
		m_fChanceTime = 0.f;
		m_pMonster->SetUp_NextAnimation(m_iAnimation_ChanceStart, false);
	}
	else
	{
		m_pMonster->SetUp_NextAnimation(m_iAnimation_ChanceLoop, true, 0.05f);
	}

	m_isEnd = false;

	return S_OK;
}

void CState_Susanoo_ChanceTime::Update(_float fTimeDelta)
{
	m_fChanceTime += fTimeDelta;

	if (m_pMonster->Get_IsEndAnimation(m_iAnimation_ChanceStart))
	{
		m_pMonster->SetUp_NextAnimation(m_iAnimation_ChanceLoop, true, 0.1f);
	}
	else if (m_pMonster->Get_IsEndAnimation(m_iAnimation_ChanceLoop))
	{
		m_pMonster->SetUp_NextAnimation(m_iAnimation_ChanceEnd, false, 0.1f);
	}
	else if (m_pMonster->Get_IsEndAnimation(m_iAnimation_ChanceEnd))
	{
		m_pMonster->Change_State(CBoss_Susanoo::IDLE);
	}

	if (m_fChanceTime >= m_fChaneDuration && !m_isEnd)
	{
		m_isEnd = true;
		m_pMonster->SetUp_NextAnimation(m_iAnimation_ChanceEnd, false);
	}

	if (KEY_TAP(KEY::NUM6))
		m_pMonster->Change_State(CBoss_Susanoo::HIT);
}

void CState_Susanoo_ChanceTime::End_State()
{
}

void CState_Susanoo_ChanceTime::Look_Player()
{
	m_pMonster->Find_PlayerTarget();

	CGameObject* pTarget = m_pMonster->Get_PlayerTarget();
	if (nullptr != pTarget)
	{
		m_pMonster->Get_Transform()->LookAt_Forward(pTarget->Get_Position());
	}
}

CState_Susanoo_ChanceTime* CState_Susanoo_ChanceTime::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum)
{
	CState_Susanoo_ChanceTime* pInstance = new CState_Susanoo_ChanceTime(pFsm, pMonster);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Susanoo_ChanceTime"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Susanoo_ChanceTime::Free()
{
	__super::Free();
}
