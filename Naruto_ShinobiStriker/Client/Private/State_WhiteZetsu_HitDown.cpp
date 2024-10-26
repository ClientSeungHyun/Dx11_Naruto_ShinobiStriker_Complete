#include "stdafx.h"
#include "State_WhiteZetsu_HitDown.h"
#include "GameInstance.h"
#include "WhiteZetsu.h"
#include "Model.h"


CState_WhiteZetsu_HitDown::CState_WhiteZetsu_HitDown(CFsm* pFsm, CMonster* pMonster)
	:CState{ pFsm }
	, m_pMonster{ pMonster }
{
}

HRESULT CState_WhiteZetsu_HitDown::Initialize(_uint iStateNum)
{
	m_iAnimation_HitDown = m_pMonster->Get_Model()->Get_AnimationIndex("WhiteZetsuCrowdForm_Beaten_OneHitDown_Short", 1.5f);
	m_iStateNum = iStateNum;


	return S_OK;
}

HRESULT CState_WhiteZetsu_HitDown::Start_State(void* pArg)
{
	m_pMonster->SetUp_NextAnimation(m_iAnimation_HitDown, false, 0.f);

	m_pMonster->Set_IsInvincible(true);

	return S_OK;
}

void CState_WhiteZetsu_HitDown::Update(_float fTimeDelta)
{

	if (m_pMonster->Get_IsEndAnimation(m_iAnimation_HitDown))
	{

		if (m_pMonster->Get_CurrentHP() <= 0.f)
			m_pFsm->Change_State(CMonster::DIE);
		else
			m_pMonster->Change_State(CWhiteZetsu::GETUP);
	}
}

void CState_WhiteZetsu_HitDown::End_State()
{
	m_pMonster->Set_IsInvincible(false);
}

CState_WhiteZetsu_HitDown* CState_WhiteZetsu_HitDown::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum)
{
	CState_WhiteZetsu_HitDown* pInstance = new CState_WhiteZetsu_HitDown(pFsm, pMonster);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_WhiteZetsu_BackDash"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_WhiteZetsu_HitDown::Free()
{
	__super::Free();
}
