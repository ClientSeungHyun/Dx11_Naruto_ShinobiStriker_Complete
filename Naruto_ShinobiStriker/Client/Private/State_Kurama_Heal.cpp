#include "stdafx.h"
#include "State_Kurama_Heal.h"
#include "GameInstance.h"
#include "Boss_Kurama.h"
#include "Model.h"


CState_Kurama_Heal::CState_Kurama_Heal(CFsm* pFsm, CMonster* pMonster)
	:CState{ pFsm }
	, m_pMonster{ pMonster }
{
}

HRESULT CState_Kurama_Heal::Initialize(_uint iStateNum)
{
	m_iAnimation_HealStart = m_pMonster->Get_Model()->Get_AnimationIndex("Kurama_HandSeal_RecoveryChakra_Start", 1.0);
	m_iAnimation_HealLoop = m_pMonster->Get_Model()->Get_AnimationIndex("Kurama_HandSeal_RecoveryChakra_Loop", 1.0);
	m_iAnimation_HealEnd = m_pMonster->Get_Model()->Get_AnimationIndex("Kurama_HandSeal_RecoveryChakra_End", 1.0);

	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Kurama_Heal::Start_State(void* pArg)
{
	m_pMonster->SetUp_NextAnimation(m_iAnimation_HealStart, false, 0.3f);
	m_isHealEnd = false;

	return S_OK;
}

void CState_Kurama_Heal::Update(_float fTimeDelta)
{
	_float fDistance = XMVectorGetX(XMVector3Length(m_pMonster->Get_PlayerTarget()->Get_Position() - m_pMonster->Get_Position()));

	if (fDistance < 30.f && !m_isHealEnd)
	{
		m_pMonster->SetUp_NextAnimation(m_iAnimation_HealEnd, false);
		m_isHealEnd = true;
	}

	if (m_pMonster->Get_CurrentAnimationIndex() == m_iAnimation_HealLoop)
	{
		m_pMonster->Damaged(-0.05f);
	}

	if (m_pMonster->Get_IsEndAnimation(m_iAnimation_HealStart))
	{
		m_pMonster->SetUp_NextAnimation(m_iAnimation_HealLoop, true);
	}
	if (m_pMonster->Get_CurrentAnimationIndex() == m_iAnimation_HealEnd && m_pMonster->Get_IsEndAnimation(m_iAnimation_HealEnd))
	{
		m_pMonster->Change_State(CBoss_Kurama::IDLE);
	}
	
}

void CState_Kurama_Heal::End_State()
{
}

void CState_Kurama_Heal::Look_Player()
{
	m_pMonster->Find_PlayerTarget();

	CGameObject* pTarget = m_pMonster->Get_PlayerTarget();
	if (nullptr != pTarget)
	{
		m_pMonster->Get_Transform()->LookAt_Forward(pTarget->Get_Position());
	}
}

CState_Kurama_Heal* CState_Kurama_Heal::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum)
{
	CState_Kurama_Heal* pInstance = new CState_Kurama_Heal(pFsm, pMonster);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Kurama_Heal"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Kurama_Heal::Free()
{
	__super::Free();
}
