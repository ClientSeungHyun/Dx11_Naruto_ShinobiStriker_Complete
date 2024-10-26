#include "stdafx.h"
#include "State_TenTail_Idle.h"
#include "GameInstance.h"
#include "TenTail.h"
#include "Model.h"


CState_TenTail_Idle::CState_TenTail_Idle(CFsm* pFsm, CMonster* pMonster)
	:CState{ pFsm }
	, m_pMonster{ pMonster }
{
}

HRESULT CState_TenTail_Idle::Initialize(_uint iStateNum)
{
	m_iAnimation_Idle = m_pMonster->Get_Model()->Get_AnimationIndex("TenTailsCloneLoser01_Idle_Loop");
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_TenTail_Idle::Start_State(void* pArg)
{
	m_pMonster->SetUp_NextAnimation(m_iAnimation_Idle, true);

	m_fIdleTime = 0.f;

	return S_OK;
}

void CState_TenTail_Idle::Update(_float fTimeDelta)
{
	m_fIdleTime += fTimeDelta;

	_vector vTargetPos = m_pMonster->Get_PlayerTarget()->Get_Position();
	_vector vDir = vTargetPos - m_pMonster->Get_Position();

	_float fDistance = -1.f;
	XMStoreFloat(&fDistance, XMVector3Length(vDir));

	if (fDistance <= 0.8f)
		m_pMonster->Change_State(CTenTail::ATTACK1);

	if (m_fIdleTime >= m_fIdleDuration)
	{
		//m_pMonster->Change_State(CWhiteZetsu::CHASE);
	}

	//if (KEY_TAP(KEY::F))
	//	m_pMonster->Change_State(CWhiteZetsu::HIT_FLYAWAY);

	//if (KEY_TAP(KEY::H))
	//{
	//	m_pMonster->Change_State(CWhiteZetsu::HIT1);
	//}

	//if (KEY_TAP(KEY::D))
	//{
	//	m_pMonster->Change_State(CWhiteZetsu::DIE);
	//}

}

void CState_TenTail_Idle::End_State()
{
}

CState_TenTail_Idle* CState_TenTail_Idle::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum)
{
	CState_TenTail_Idle* pInstance = new CState_TenTail_Idle(pFsm, pMonster);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_TenTail_Idle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_TenTail_Idle::Free()
{
	__super::Free();
}
