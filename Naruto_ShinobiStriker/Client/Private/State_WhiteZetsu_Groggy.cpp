#include "stdafx.h"
#include "State_WhiteZetsu_Groggy.h"
#include "GameInstance.h"
#include "WhiteZetsu.h"
#include "Model.h"


CState_WhiteZetsu_Groggy::CState_WhiteZetsu_Groggy(CFsm* pFsm, CMonster* pMonster)
	:CState{ pFsm }
	, m_pMonster{ pMonster }
{
}

HRESULT CState_WhiteZetsu_Groggy::Initialize(_uint iStateNum)
{
	m_iAnimation_Groggy = m_pMonster->Get_Model()->Get_AnimationIndex("WhiteZetsuCrowdForm_Beaten_ElectricShock_Loop", 1.5f);
	m_iStateNum = iStateNum;


	return S_OK;
}

HRESULT CState_WhiteZetsu_Groggy::Start_State(void* pArg)
{
	m_pMonster->SetUp_NextAnimation(m_iAnimation_Groggy, true, 0.f);


	//m_pMonster->Set_IsInvincible(true);

	return S_OK;
}

void CState_WhiteZetsu_Groggy::Update(_float fTimeDelta)
{

}

void CState_WhiteZetsu_Groggy::End_State()
{
	m_pMonster->Set_IsInvincible(false);
}

CState_WhiteZetsu_Groggy* CState_WhiteZetsu_Groggy::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum)
{
	CState_WhiteZetsu_Groggy* pInstance = new CState_WhiteZetsu_Groggy(pFsm, pMonster);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_WhiteZetsu_BackDash"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_WhiteZetsu_Groggy::Free()
{
	__super::Free();
}
