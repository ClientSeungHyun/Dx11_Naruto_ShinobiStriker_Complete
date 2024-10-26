#include "stdafx.h"
#include "State_WhiteZetsu_Appear.h"
#include "GameInstance.h"
#include "WhiteZetsu.h"
#include "Model.h"


CState_WhiteZetsu_Appear::CState_WhiteZetsu_Appear(CFsm* pFsm, CMonster* pMonster)
	:CState{ pFsm }
	, m_pMonster{ pMonster }
{
}

HRESULT CState_WhiteZetsu_Appear::Initialize(_uint iStateNum)
{
	m_iAnimation_Appear = m_pMonster->Get_Model()->Get_AnimationIndex("WhiteZetsuCrowdForm_etc_Appearance_Type01");
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_WhiteZetsu_Appear::Start_State(void* pArg)
{
	m_pMonster->SetUp_NextAnimation(m_iAnimation_Appear, false, 0.f);
	m_pMonster->Set_IsInvincible(true);

	return S_OK;
}

void CState_WhiteZetsu_Appear::Update(_float fTimeDelta)
{
	if (m_pMonster->Get_IsEndAnimation(m_iAnimation_Appear))
	{
		Look_Player();
		m_pMonster->Change_State(CWhiteZetsu::IDLE);
	}
}

void CState_WhiteZetsu_Appear::End_State()
{
	m_pMonster->Set_IsInvincible(false);
}

void CState_WhiteZetsu_Appear::Look_Player()
{
	m_pMonster->Find_PlayerTarget();

	CGameObject* pTarget = m_pMonster->Get_PlayerTarget();
	if (nullptr != pTarget)
	{
		m_pMonster->Get_Transform()->LookAt_Forward(pTarget->Get_Position());
	}
}

CState_WhiteZetsu_Appear* CState_WhiteZetsu_Appear::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum)
{
	CState_WhiteZetsu_Appear* pInstance = new CState_WhiteZetsu_Appear(pFsm, pMonster);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Naruto_Idle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_WhiteZetsu_Appear::Free()
{
	__super::Free();
}
