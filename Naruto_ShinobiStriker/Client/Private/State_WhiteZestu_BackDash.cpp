#include "stdafx.h"
#include "State_WhiteZestu_BackDash.h"
#include "GameInstance.h"
#include "WhiteZetsu.h"
#include "Model.h"


CState_WhiteZetsu_BackDash::CState_WhiteZetsu_BackDash(CFsm* pFsm, CMonster* pMonster)
	:CState{ pFsm }
	, m_pMonster{ pMonster }
{
}

HRESULT CState_WhiteZetsu_BackDash::Initialize(_uint iStateNum)
{
	m_iAnimation_BackDash = m_pMonster->Get_Model()->Get_AnimationIndex("WhiteZetsuCrowdForm_DashStep_Behind", 1.5f);
	m_iStateNum = iStateNum;

	m_pMonster->Set_IsInvincible(true);

	return S_OK;
}

HRESULT CState_WhiteZetsu_BackDash::Start_State(void* pArg)
{
	m_pMonster->SetUp_NextAnimation(m_iAnimation_BackDash, false, 0.f);
	m_pMonster->Set_IsInvincible(true);

	m_pMonster->Force_RandomBackWard(15.f);

	return S_OK;
}

void CState_WhiteZetsu_BackDash::Update(_float fTimeDelta)
{
	if (m_pMonster->Get_IsEndAnimation(m_iAnimation_BackDash))
	{
		m_pMonster->Change_State(CWhiteZetsu::IDLE);
	}

	if (m_pMonster->Get_PlayerTarget() != nullptr)
		m_pMonster->Get_Transform()->LookAt_Forward(m_pMonster->Get_PlayerTarget()->Get_Position());
}

void CState_WhiteZetsu_BackDash::End_State()
{
	m_pMonster->Set_IsInvincible(false);
}

CState_WhiteZetsu_BackDash* CState_WhiteZetsu_BackDash::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum)
{
	CState_WhiteZetsu_BackDash* pInstance = new CState_WhiteZetsu_BackDash(pFsm, pMonster);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_WhiteZetsu_BackDash"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_WhiteZetsu_BackDash::Free()
{
	__super::Free();
}
