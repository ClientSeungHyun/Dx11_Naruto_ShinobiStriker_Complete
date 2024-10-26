#include "stdafx.h"
#include "State_Kurama_Die.h"
#include "GameInstance.h"
#include "Boss_Kurama.h"
#include "Model.h"


CState_Kurama_Die::CState_Kurama_Die(CFsm* pFsm, CMonster* pMonster)
	:CState{ pFsm }
	, m_pMonster{ pMonster }
{
}

HRESULT CState_Kurama_Die::Initialize(_uint iStateNum)
{
	m_iAnimation_Die = m_pMonster->Get_Model()->Get_AnimationIndex("Kurama_Dying_Type01");
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Kurama_Die::Start_State(void* pArg)
{
	m_pMonster->SetUp_NextAnimation(m_iAnimation_Die, false);

	m_pMonster->Find_PlayerTarget();

	m_pMonster->Get_SoundCom()->Play3D(TEXT("Voice_Kurama_Lose.wav"), g_fVolume + 0.2f);

	return S_OK;
}

void CState_Kurama_Die::Update(_float fTimeDelta)
{
}

void CState_Kurama_Die::End_State()
{
}

CState_Kurama_Die* CState_Kurama_Die::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum)
{
	CState_Kurama_Die* pInstance = new CState_Kurama_Die(pFsm, pMonster);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Kurama_Die"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Kurama_Die::Free()
{
	__super::Free();
}
