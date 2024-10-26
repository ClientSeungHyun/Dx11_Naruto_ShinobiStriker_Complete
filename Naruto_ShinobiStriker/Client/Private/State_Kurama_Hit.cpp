#include "stdafx.h"
#include "State_Kurama_Hit.h"
#include "GameInstance.h"
#include "Boss_Kurama.h"
#include "Model.h"


CState_Kurama_Hit::CState_Kurama_Hit(CFsm* pFsm, CMonster* pMonster)
	:CState{ pFsm }
	, m_pMonster{ pMonster }
{
}

HRESULT CState_Kurama_Hit::Initialize(_uint iStateNum)
{
	m_iAnimation_Hit1 = m_pMonster->Get_Model()->Get_AnimationIndex("Kurama_Beaten_Type01", 2.0);
	m_iAnimation_Hit2 = m_pMonster->Get_Model()->Get_AnimationIndex("Kurama_Beaten_Type02", 2.0);

	m_iAnimation_ChanceHitLeft = m_pMonster->Get_Model()->Get_AnimationIndex("Kurama_Beaten_ChanceTimeHit_Left", 1.5);
	m_iAnimation_ChanceHitRight = m_pMonster->Get_Model()->Get_AnimationIndex("Kurama_Beaten_ChanceTimeHit_Right", 1.5);

	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Kurama_Hit::Start_State(void* pArg)
{
	m_PrevStateIndex = m_pFsm->Get_PrevState();

	if (m_PrevStateIndex == CBoss_Kurama::CHANCE)
	{
		_float fDirection = m_pMonster->Carculate_TargetDir();

		if (fDirection > 0)
			m_pMonster->SetUp_NextAnimation(m_iAnimation_ChanceHitRight, false);
		else
			m_pMonster->SetUp_NextAnimation(m_iAnimation_ChanceHitLeft, false);
	}
	else
	{
		_uint iRand = rand() % 2;

		switch (iRand)
		{
		case 0:
			m_pMonster->SetUp_NextAnimation(m_iAnimation_Hit1, false, 0.1f);
			break;
		case 1:
			m_pMonster->SetUp_NextAnimation(m_iAnimation_Hit2, false, 0.1f);
		}
	}


	return S_OK;
}

void CState_Kurama_Hit::Update(_float fTimeDelta)
{
	_uint iCurrentAnimation = m_pMonster->Get_CurrentAnimationIndex();
	_uint iFrame = m_pMonster->Get_Frame(iCurrentAnimation);

	if (m_pMonster->Get_IsEndAnimation(iCurrentAnimation))
	{
		if(m_PrevStateIndex == CBoss_Kurama::CHANCE)
			m_pMonster->Change_State(CBoss_Kurama::CHANCE);
		else
			m_pMonster->Change_State(CBoss_Kurama::IDLE);
	}
}

void CState_Kurama_Hit::End_State()
{
}

void CState_Kurama_Hit::Look_Player()
{
	m_pMonster->Find_PlayerTarget();

	CGameObject* pTarget = m_pMonster->Get_PlayerTarget();
	if (nullptr != pTarget)
	{
		m_pMonster->Get_Transform()->LookAt_Forward(pTarget->Get_Position());
	}
}

CState_Kurama_Hit* CState_Kurama_Hit::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum)
{
	CState_Kurama_Hit* pInstance = new CState_Kurama_Hit(pFsm, pMonster);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Kurama_Idle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Kurama_Hit::Free()
{
	__super::Free();
}
