#include "stdafx.h"
#include "State_Kurama_Idle.h"
#include "GameInstance.h"
#include "Boss_Kurama.h"
#include "Model.h"


CState_Kurama_Idle::CState_Kurama_Idle(CFsm* pFsm, CMonster* pMonster)
	:CState{ pFsm }
	, m_pMonster{ pMonster }
{
}

HRESULT CState_Kurama_Idle::Initialize(_uint iStateNum)
{
	m_iAnimation_Idle = m_pMonster->Get_Model()->Get_AnimationIndex("Kurama_Idle_Loop");
	m_iStateNum = iStateNum;
	m_fIdleDuration = 3.3f;

	return S_OK;
}

HRESULT CState_Kurama_Idle::Start_State(void* pArg)
{
	m_pMonster->SetUp_NextAnimation(m_iAnimation_Idle, true, 0.2f);

	m_pMonster->Find_PlayerTarget();

	m_fCurrentTime = 0.f;

	if(m_pFsm->Get_PrevState() == CBoss_Kurama::CHANCE)
	{
		m_isPrevChance = true;
	}

	return S_OK;
}

void CState_Kurama_Idle::Update(_float fTimeDelta)
{
	m_fCurrentTime += fTimeDelta;

	if (m_isPrevChance && !m_isActivatedSpecial)
	{
		if (m_fCurrentTime >= 1.f)
		{
			m_pMonster->Change_State(CBoss_Kurama::JUMP);
			m_isActivatedSpecial = true;
		}
	}
	else
	{
		if (m_iIdleCount > (_uint)(rand() % 4 + 4))
		{
			m_pMonster->Change_State(CBoss_Kurama::LEAVE);
			m_iIdleCount = 0;
			return;
		}

		if (m_fCurrentTime >= m_pGameInstance->Get_Random(m_fIdleDuration, m_fIdleDuration + 1.5f))
			Select_Attack();
	}
}

void CState_Kurama_Idle::End_State()
{
}

void CState_Kurama_Idle::Look_Player()
{
	m_pMonster->Find_PlayerTarget();

	CGameObject* pTarget = m_pMonster->Get_PlayerTarget();
	if (nullptr != pTarget)
	{
		m_pMonster->Get_Transform()->LookAt_Forward(pTarget->Get_Position());
	}
}

void CState_Kurama_Idle::Select_Attack()
{
	_float fDistance = XMVectorGetX(XMVector3Length(m_pMonster->Get_PlayerTarget()->Get_Position() - m_pMonster->Get_Position()));

	if (fDistance > 40.f)
		return;

	++m_iIdleCount;

	if (fDistance <= 6.f)
	{
		_uint iRand = rand() % 120;
		
		switch (iRand / 10)
		{
		case 0:
		case 1:
			m_pMonster->Change_State(CBoss_Kurama::ATTACK1);
			break;
		case 2:
		case 3:
			m_pMonster->Change_State(CBoss_Kurama::ATTACK2);
			break;
		case 4:
		case 5:
		case 6:
			m_pMonster->Change_State(CBoss_Kurama::JUMPCRUSH);
			break;
		case 7:
		case 8:
			m_pMonster->Change_State(CBoss_Kurama::BITE);
			break;
		case 9:
		case 10:
		case 11:
			m_pMonster->Change_State(CBoss_Kurama::ROAR);
			break;
		}
	}
	else
	{
		_uint iRand = rand() % 60;

		switch (iRand / 10)
		{
		case 0:
		case 1:
			m_pMonster->Change_State(CBoss_Kurama::THREE_BLAST);
			break;
		case 2:
		case 3:
			m_pMonster->Change_State(CBoss_Kurama::AERIAL_BLAST);
			break;
		case 4:
		case 5:
			m_pMonster->Change_State(CBoss_Kurama::ROAR);
			break;
		}
	}
}

CState_Kurama_Idle* CState_Kurama_Idle::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum)
{
	CState_Kurama_Idle* pInstance = new CState_Kurama_Idle(pFsm, pMonster);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Kurama_Idle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Kurama_Idle::Free()
{
	__super::Free();
}
