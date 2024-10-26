#include "stdafx.h"
#include "State_WhiteZetsu_GetUp.h"
#include "GameInstance.h"
#include "WhiteZetsu.h"
#include "Model.h"


CState_WhiteZetsu_GetUp::CState_WhiteZetsu_GetUp(CFsm* pFsm, CMonster* pMonster)
	:CState{ pFsm }
	, m_pMonster{ pMonster }
{
}

HRESULT CState_WhiteZetsu_GetUp::Initialize(_uint iStateNum)
{
	m_iAnimation_GetUp = m_pMonster->Get_Model()->Get_AnimationIndex("WhiteZetsuCrowdForm_GetUp");
	m_iStateNum = iStateNum;

	m_pMonster->Set_IsInvincible(true);

	return S_OK;
}

HRESULT CState_WhiteZetsu_GetUp::Start_State(void* pArg)
{
	m_pMonster->SetUp_NextAnimation(m_iAnimation_GetUp, false, 0.6f);

	//m_pMonster->Set_IsCheckPhysX(false);

	return S_OK;
}

void CState_WhiteZetsu_GetUp::Update(_float fTimeDelta)
{
	if (m_pMonster->Get_IsEndAnimation(m_iAnimation_GetUp))
		m_pMonster->Change_State(CWhiteZetsu::IDLE);
}

void CState_WhiteZetsu_GetUp::End_State()
{
	m_pMonster->Set_IsInvincible(false);
}

CState_WhiteZetsu_GetUp* CState_WhiteZetsu_GetUp::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum)
{
	CState_WhiteZetsu_GetUp* pInstance = new CState_WhiteZetsu_GetUp(pFsm, pMonster);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_WhiteZetsu_GetUp"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_WhiteZetsu_GetUp::Free()
{
	__super::Free();
}
