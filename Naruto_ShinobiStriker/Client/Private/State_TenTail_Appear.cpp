#include "stdafx.h"
#include "State_TenTail_Appear.h"
#include "GameInstance.h"
#include "TenTail.h"
#include "Model.h"


CState_TenTail_Appear::CState_TenTail_Appear(CFsm* pFsm, CMonster* pMonster)
	:CState{ pFsm }
	, m_pMonster{ pMonster }
{
}

HRESULT CState_TenTail_Appear::Initialize(_uint iStateNum)
{
	m_iAnimation_Appear = m_pMonster->Get_Model()->Get_AnimationIndex("TenTailsCloneLoser01_etc_Appearance_Type01");
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_TenTail_Appear::Start_State(void* pArg)
{
	m_pMonster->SetUp_NextAnimation(m_iAnimation_Appear, false);

	return S_OK;
}

void CState_TenTail_Appear::Update(_float fTimeDelta)
{
	if (m_pMonster->Get_IsEndAnimation(m_iAnimation_Appear))
	{
		Look_Player();
		m_pMonster->Change_State(CTenTail::IDLE);
	}
}

void CState_TenTail_Appear::End_State()
{
}

void CState_TenTail_Appear::Look_Player()
{
	m_pMonster->Find_PlayerTarget();

	CGameObject* pTarget = m_pMonster->Get_PlayerTarget();
	if (nullptr != pTarget)
	{
		m_pMonster->Get_Transform()->LookAt_Forward(pTarget->Get_Position());
	}
}

CState_TenTail_Appear* CState_TenTail_Appear::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum)
{
	CState_TenTail_Appear* pInstance = new CState_TenTail_Appear(pFsm, pMonster);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_TenTail_Appear"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_TenTail_Appear::Free()
{
	__super::Free();
}
