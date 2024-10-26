#include "stdafx.h"
#include "State_WhiteZetsu_Hit1.h"
#include "GameInstance.h"
#include "WhiteZetsu.h"
#include "Model.h"


CState_WhiteZetsu_Hit1::CState_WhiteZetsu_Hit1(CFsm* pFsm, CMonster* pMonster)
	:CState{ pFsm }
	, m_pMonster{ pMonster }
{
}

HRESULT CState_WhiteZetsu_Hit1::Initialize(_uint iStateNum)
{
	m_iAnimation_HitShoulder = m_pMonster->Get_Model()->Get_AnimationIndex("WhiteZetsuCrowdForm_Beaten_UpLeft_Shoulder");
	m_iStateNum = iStateNum;


	return S_OK;
}

HRESULT CState_WhiteZetsu_Hit1::Start_State(void* pArg)
{
	m_pMonster->SetUp_NextAnimation(m_iAnimation_HitShoulder, false, 0.05f);

	m_pMonster->Set_IsCheckPhysX(true);
	m_pMonster->Set_IsInvincible(true);

	if (m_pMonster->Get_CurrentHP() <= 0.f)
		m_pFsm->Change_State(CMonster::DIE);

	return S_OK;
}

void CState_WhiteZetsu_Hit1::Update(_float fTimeDelta)
{
	if (m_pMonster->Get_IsEndAnimation(m_iAnimation_HitShoulder))
		m_pMonster->Change_State(CWhiteZetsu::IDLE);

	if (KEY_TAP(KEY::H))
	{
		m_pMonster->Change_State(CWhiteZetsu::HIT2);
	}
}

void CState_WhiteZetsu_Hit1::End_State()
{
	//m_pMonster->Set_IsCheckPhysX(false);
	m_pMonster->Set_IsInvincible(false);
}

CState_WhiteZetsu_Hit1* CState_WhiteZetsu_Hit1::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum)
{
	CState_WhiteZetsu_Hit1* pInstance = new CState_WhiteZetsu_Hit1(pFsm, pMonster);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_WhiteZetsu_Hit1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_WhiteZetsu_Hit1::Free()
{
	__super::Free();
}
