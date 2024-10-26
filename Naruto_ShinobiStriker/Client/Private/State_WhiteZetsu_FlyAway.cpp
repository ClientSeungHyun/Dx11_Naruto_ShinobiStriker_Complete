#include "stdafx.h"
#include "State_WhiteZetsu_FlyAway.h"
#include "GameInstance.h"
#include "WhiteZetsu.h"
#include "Model.h"


CState_WhiteZetsu_FlyAway::CState_WhiteZetsu_FlyAway(CFsm* pFsm, CMonster* pMonster)
	:CState{ pFsm }
	, m_pMonster{ pMonster }
{
}

HRESULT CState_WhiteZetsu_FlyAway::Initialize(_uint iStateNum)
{
	m_iAnimation_SpinBlow = m_pMonster->Get_Model()->Get_AnimationIndex("WhiteZetsuCrowdForm_Beaten_SpinBlowOff", 1.7);
	m_iStateNum = iStateNum;

	m_pMonster->Set_IsInvincible(true);

	return S_OK;
}

HRESULT CState_WhiteZetsu_FlyAway::Start_State(void* pArg)
{
	m_pMonster->SetUp_NextAnimation(m_iAnimation_SpinBlow, false, 0.1f);

	m_pMonster->Set_IsInvincible(true);
	m_pMonster->Set_IsCheckPhysX(true);
	m_pMonster->Set_IsGravity(true);

	m_pMonster->Force_BackUp(m_pGameInstance->Get_Random(13.f, 18.f), m_pGameInstance->Get_Random(12.f, 17.f));

	return S_OK;
}

void CState_WhiteZetsu_FlyAway::Update(_float fTimeDelta)
{

	if (m_pMonster->Get_IsEndAnimation(m_iAnimation_SpinBlow))
	{
		m_pMonster->Change_State(CWhiteZetsu::FALL);
	}

}

void CState_WhiteZetsu_FlyAway::End_State()
{
	m_pMonster->Set_IsInvincible(false);
}

CState_WhiteZetsu_FlyAway* CState_WhiteZetsu_FlyAway::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum)
{
	CState_WhiteZetsu_FlyAway* pInstance = new CState_WhiteZetsu_FlyAway(pFsm, pMonster);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_WhiteZetsu_FlyAway"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_WhiteZetsu_FlyAway::Free()
{
	__super::Free();
}
