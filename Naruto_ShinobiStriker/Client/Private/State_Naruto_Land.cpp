#include "stdafx.h"
#include "State_Naruto_Land.h"
#include "GameInstance.h"
#include "Player_Naruto.h"
#include "Model.h"


CState_Naruto_Land::CState_Naruto_Land(CFsm* pFsm, CPlayer* pPlayer)
	:CState{ pFsm }
	, m_pPlayer{ pPlayer }
{
}

HRESULT CState_Naruto_Land::Initialize(_uint iStateNum)
{
	m_iAnimation_Land = m_pPlayer->Get_Model()->Get_AnimationIndex("Naruto_Land", 2.5);
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Naruto_Land::Start_State(void* pArg)
{
	m_pPlayer->SetUp_NextAnimation(m_iAnimation_Land, false, 0.05f);

	m_pPlayer->Set_IsJump(false);
	m_pPlayer->Set_IsDoubleJump(false);

	m_pPlayer->Get_RigidBody()->Clear_All();
	m_pPlayer->Set_IsGravity(false);

	return S_OK;
}

void CState_Naruto_Land::Update(_float fTimeDelta)
{
	if (m_pPlayer->Get_IsEndAnimation(m_iAnimation_Land))
	{
		m_pPlayer->Change_State(CPlayer_Naruto::IDLE);
	}
}

void CState_Naruto_Land::End_State()
{
}

CState_Naruto_Land* CState_Naruto_Land::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Naruto_Land* pInstance = new CState_Naruto_Land(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Naruto_Land"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Naruto_Land::Free()
{
	__super::Free();
}
