#include "stdafx.h"
#include "State_WhiteZetsu_Fall.h"
#include "GameInstance.h"
#include "WhiteZetsu.h"
#include "Model.h"


CState_WhiteZetsu_Fall::CState_WhiteZetsu_Fall(CFsm* pFsm, CMonster* pMonster)
	:CState{ pFsm }
	, m_pMonster{ pMonster }
{
}

HRESULT CState_WhiteZetsu_Fall::Initialize(_uint iStateNum)
{
	m_iAnimation_Fall = m_pMonster->Get_Model()->Get_AnimationIndex("WhiteZetsuCrowdForm_Beaten_Fall_Behind_Loop", 1.3);
	m_iAnimation_DownToFloor = m_pMonster->Get_Model()->Get_AnimationIndex("WhiteZetsuCrowdForm_Beaten_DownToFloor", 1.);

	m_iStateNum = iStateNum;

	m_pMonster->Set_IsInvincible(true);

	return S_OK;
}

HRESULT CState_WhiteZetsu_Fall::Start_State(void* pArg)
{
	m_pMonster->SetUp_NextAnimation(m_iAnimation_Fall, true);

	m_pMonster->Set_IsCheckPhysX(true);
	m_pMonster->Set_IsGravity(true);

	m_isGround = false;

	return S_OK;
}

void CState_WhiteZetsu_Fall::Update(_float fTimeDelta)
{
	if (m_pMonster->Get_IsGround() && !m_isGround)
	{
		m_isGround = true;
		m_pMonster->SetUp_NextAnimation(m_iAnimation_DownToFloor);
	}
	else if (m_pMonster->Get_IsEndAnimation(m_iAnimation_DownToFloor))
	{
		if (m_pMonster->Get_CurrentHP() <= 0.f)
		{
			m_pMonster->Change_State(CWhiteZetsu::DIE);
			return;
		}
		m_pMonster->Change_State(CWhiteZetsu::GETUP);
	}
}

void CState_WhiteZetsu_Fall::End_State()
{
	//m_pMonster->Set_IsCheckPhysX(false);
	m_pMonster->Set_IsGravity(false);
}

CState_WhiteZetsu_Fall* CState_WhiteZetsu_Fall::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum)
{
	CState_WhiteZetsu_Fall* pInstance = new CState_WhiteZetsu_Fall(pFsm, pMonster);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Naruto_Idle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_WhiteZetsu_Fall::Free()
{
	__super::Free();
}
