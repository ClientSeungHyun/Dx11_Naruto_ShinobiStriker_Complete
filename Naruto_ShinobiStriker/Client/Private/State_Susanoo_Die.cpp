#include "stdafx.h"
#include "State_Susanoo_Die.h"
#include "GameInstance.h"
#include "Boss_Susanoo.h"
#include "Model.h"


CState_Susanoo_Die::CState_Susanoo_Die(CFsm* pFsm, CMonster* pMonster)
	:CState{ pFsm }
	, m_pMonster{ pMonster }
{
}

HRESULT CState_Susanoo_Die::Initialize(_uint iStateNum)
{
	m_iAnimation_Die = m_pMonster->Get_Model()->Get_AnimationIndex("RineSusanoPS_Dying_Type03");

	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Susanoo_Die::Start_State(void* pArg)
{
	m_pMonster->SetUp_NextAnimation(m_iAnimation_Die, false);

	m_pMonster->Find_PlayerTarget();


	m_pMonster->Get_SoundCom()->Play3D(TEXT("Voice_Susanoo_Lose.wav"), g_fVolume + 0.2f);

	return S_OK;
}

void CState_Susanoo_Die::Update(_float fTimeDelta)
{
}

void CState_Susanoo_Die::End_State()
{
}

CState_Susanoo_Die* CState_Susanoo_Die::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum)
{
	CState_Susanoo_Die* pInstance = new CState_Susanoo_Die(pFsm, pMonster);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Susanoo_Die"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Susanoo_Die::Free()
{
	__super::Free();
}
