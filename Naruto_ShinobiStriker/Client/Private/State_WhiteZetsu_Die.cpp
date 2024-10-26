#include "stdafx.h"
#include "State_WhiteZetsu_Die.h"
#include "GameInstance.h"
#include "WhiteZetsu.h"
#include "Model.h"


CState_WhiteZetsu_Die::CState_WhiteZetsu_Die(CFsm* pFsm, CMonster* pMonster)
	:CState{ pFsm }
	, m_pMonster{ pMonster }
{
}

HRESULT CState_WhiteZetsu_Die::Initialize(_uint iStateNum)
{
	m_iAnimation_Die = m_pMonster->Get_Model()->Get_AnimationIndex("WhiteZetsuCrowdForm_Dying_Type01", 1.3);
	m_iAnimation_DieBlast = m_pMonster->Get_Model()->Get_AnimationIndex("WhiteZetsuCrowdForm_Dying_Blasted", 1.3);
	m_iStateNum = iStateNum;


	return S_OK;
}

HRESULT CState_WhiteZetsu_Die::Start_State(void* pArg)
{
	_uint iRandDie = rand() % 2;

	if (m_pFsm->Get_PrevState() == CWhiteZetsu::FALL || m_pFsm->Get_PrevState() == CWhiteZetsu::HIT_FLYGROUND
		|| m_pFsm->Get_PrevState() == CWhiteZetsu::HIT_DOWN)
	{
		m_isPrevDie = true;
	}
	else
	{
		m_isPrevDie = false;

		switch (iRandDie)
		{
		case 0:
			m_pMonster->SetUp_NextAnimation(m_iAnimation_Die, false, 0.1f);
			break;
		case 1:
			m_pMonster->SetUp_NextAnimation(m_iAnimation_DieBlast, false, 0.1f);
			break;
		}


		m_pMonster->Force_BackWard(10.f);
	}

	m_fDieTime = 0.f;

	m_pMonster->Set_IsInvincible(true);

	return S_OK;
}

void CState_WhiteZetsu_Die::Update(_float fTimeDelta)
{
	_uint iCurrenAnimationIndex = m_pMonster->Get_CurrentAnimationIndex();

	if (m_isPrevDie)
	{
		m_fDieTime += fTimeDelta;
		if (m_fDieTime >= 1.f)
		{
			m_pGameInstance->Event_DestoryObject(m_pMonster);
		}
	}
	else
	{
		if (iCurrenAnimationIndex == m_iAnimation_Die && m_pMonster->Get_IsEndAnimation(m_iAnimation_Die))
		{
			m_pGameInstance->Event_DestoryObject(m_pMonster);
		}

		if (iCurrenAnimationIndex == m_iAnimation_DieBlast && m_pMonster->Get_IsEndAnimation(m_iAnimation_DieBlast))
		{
			//m_pMonster->Set_Dead(true);
			m_pGameInstance->Event_DestoryObject(m_pMonster);
		}
	}
}

void CState_WhiteZetsu_Die::End_State()
{
	m_pMonster->Set_IsInvincible(false);
}

CState_WhiteZetsu_Die* CState_WhiteZetsu_Die::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum)
{
	CState_WhiteZetsu_Die* pInstance = new CState_WhiteZetsu_Die(pFsm, pMonster);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_WhiteZetsu_Die"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_WhiteZetsu_Die::Free()
{
	__super::Free();
}
