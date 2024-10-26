#include "stdafx.h"
#include "State_Naruto_RunEnd.h"
#include "GameInstance.h"
#include "Player_Naruto.h"
#include "Model.h"


CState_Naruto_RunEnd::CState_Naruto_RunEnd(CFsm* pFsm, CPlayer* pPlayer)
	:CState{ pFsm }
	, m_pPlayer{ pPlayer }
{
}

HRESULT CState_Naruto_RunEnd::Initialize(_uint iStateNum)
{
	m_iAnimation_RunnEnd = m_pPlayer->Get_Model()->Get_AnimationIndex("Naruto_Run_End", 2.0);
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Naruto_RunEnd::Start_State(void* pArg)
{
	m_pPlayer->SetUp_NextAnimation(m_iAnimation_RunnEnd, false, 0.05f);

	return S_OK;
}

void CState_Naruto_RunEnd::Update(_float fTimeDelta)
{
	if (m_pPlayer->Get_IsEndAnimation(m_iAnimation_RunnEnd))
	{
		m_pPlayer->Change_State(CPlayer_Naruto::IDLE);
		return;
	}
}

void CState_Naruto_RunEnd::End_State()
{
}

_bool CState_Naruto_RunEnd::Move(_float fTimeDelta)
{
	_bool isMoving = false;
	_vector vDir = XMVectorSet(0.f, 0.f, 0.f, 0.f);

	if (KEY_HOLD(KEY::W))
	{
		vDir += m_pPlayer->Calculate_Direction_Straight();
		isMoving = true;
	}
	else if (KEY_HOLD(KEY::S))
	{
		vDir -= m_pPlayer->Calculate_Direction_Straight();
		isMoving = true;
	}

	if (KEY_HOLD(KEY::D))
	{
		vDir += m_pPlayer->Calculate_Direction_Right();

		isMoving = true;
	}

	if (KEY_HOLD(KEY::A))
	{
		vDir -= m_pPlayer->Calculate_Direction_Right();

		isMoving = true;
	}

	m_pPlayer->Move_Dir(vDir, 10.0f, fTimeDelta);

	return isMoving;
}

CState_Naruto_RunEnd* CState_Naruto_RunEnd::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Naruto_RunEnd* pInstance = new CState_Naruto_RunEnd(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Naruto_Idle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Naruto_RunEnd::Free()
{
	__super::Free();
}
