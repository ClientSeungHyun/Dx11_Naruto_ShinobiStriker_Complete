#include "stdafx.h"
#include "State_Kurama_Fall.h"
#include "GameInstance.h"
#include "Boss_Kurama.h"
#include "Model.h"


CState_Kurama_Fall::CState_Kurama_Fall(CFsm* pFsm, CMonster* pMonster)
	:CState{ pFsm }
	, m_pMonster{ pMonster }
{
}

HRESULT CState_Kurama_Fall::Initialize(_uint iStateNum)
{
	m_iAnimation_Fall = m_pMonster->Get_Model()->Get_AnimationIndex("Kurama_Fall_Front_Loop", 2.0);
	m_iAnimation_Land = m_pMonster->Get_Model()->Get_AnimationIndex("Kurama_Land", 1.0);
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Kurama_Fall::Start_State(void* pArg)
{
	m_pMonster->SetUp_NextAnimation(m_iAnimation_Fall, true, 0.1f);

	m_pMonster->Get_RigidBody()->Set_IsGravity(true);

	m_pMonster->Set_IsCheckPhysX(true);
	m_isGround = false;

	return S_OK;
}

void CState_Kurama_Fall::Update(_float fTimeDelta)
{
	if (m_pMonster->Get_IsGround() && !m_isGround)
	{
		Look_Player();
		m_isGround = true;
		m_pMonster->SetUp_NextAnimation(m_iAnimation_Land, false, 0.f);
	}
	else if (m_pMonster->Get_IsEndAnimation(m_iAnimation_Land))
	{
		m_pMonster->Change_State(CBoss_Kurama::IDLE);
	}


}

void CState_Kurama_Fall::End_State()
{
	m_pMonster->Set_IsCheckPhysX(false);
	m_pMonster->Get_RigidBody()->Set_IsGravity(false);
	m_pMonster->Get_RigidBody()->Clear_All();
}

void CState_Kurama_Fall::Look_Player()
{
	m_pMonster->Find_PlayerTarget();

	CGameObject* pTarget = m_pMonster->Get_PlayerTarget();
	if (nullptr != pTarget)
	{
		m_pMonster->Get_Transform()->LookAt_Forward(pTarget->Get_Position());
	}
}

CState_Kurama_Fall* CState_Kurama_Fall::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum)
{
	CState_Kurama_Fall* pInstance = new CState_Kurama_Fall(pFsm, pMonster);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Kurama_Fall"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Kurama_Fall::Free()
{
	__super::Free();
}
