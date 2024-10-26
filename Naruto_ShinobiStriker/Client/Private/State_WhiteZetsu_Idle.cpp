#include "stdafx.h"
#include "State_WhiteZetsu_Idle.h"
#include "GameInstance.h"
#include "WhiteZetsu.h"
#include "Model.h"


CState_WhiteZetsu_Idle::CState_WhiteZetsu_Idle(CFsm* pFsm, CMonster* pMonster)
	:CState{ pFsm }
	, m_pMonster{ pMonster }
{
}

HRESULT CState_WhiteZetsu_Idle::Initialize(_uint iStateNum)
{
	m_iAnimation_Idle = m_pMonster->Get_Model()->Get_AnimationIndex("WhiteZetsuCrowdForm_Idle_Type02_Loop" , 1.5);
	m_iStateNum = iStateNum;
	m_fHitTime = 0.5f;

	return S_OK;
}

HRESULT CState_WhiteZetsu_Idle::Start_State(void* pArg)
{
	m_pMonster->SetUp_NextAnimation(m_iAnimation_Idle, true);
	m_fIdleTime = 0.f;
	return S_OK;
}

void CState_WhiteZetsu_Idle::Update(_float fTimeDelta)
{
	m_fIdleTime += fTimeDelta;
	m_fHitTime += fTimeDelta;

	if (nullptr == m_pMonster->Get_PlayerTarget())
		m_pMonster->Find_PlayerTarget();

	_vector vTargetPos = m_pMonster->Get_PlayerTarget()->Get_Position();
	_vector vDir = vTargetPos - m_pMonster->Get_Position();

	_float fDistance = -1.f;
	XMStoreFloat(&fDistance, XMVector3Length(vDir));

	if (fDistance <= m_pGameInstance->Get_Random(0.7f, 1.5f) && m_fHitTime >= m_pGameInstance->Get_Random(3.5f, 4.5f))
	{
		_uint iRand = rand() % 6;
		switch (iRand)
		{
		case 0:
		case 1:
		case 2:
			m_pMonster->Change_State(CWhiteZetsu::BACKDASH);
			break;

		case 3:
		case 4:
		case 5:
			m_pMonster->Change_State(CWhiteZetsu::ATTACK1);
			m_fHitTime = 0.f;
		default:
			break;
		}
		return;
	}
	else if (fDistance > 1.8f && m_fIdleTime >= m_fIdleDuration)
	{
		m_pMonster->Change_State(CWhiteZetsu::CHASE);
	}

}

void CState_WhiteZetsu_Idle::End_State()
{
	m_fHitTime = 0.f;
}

CState_WhiteZetsu_Idle* CState_WhiteZetsu_Idle::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum)
{
	CState_WhiteZetsu_Idle* pInstance = new CState_WhiteZetsu_Idle(pFsm, pMonster);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Naruto_Idle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_WhiteZetsu_Idle::Free()
{
	__super::Free();
}
