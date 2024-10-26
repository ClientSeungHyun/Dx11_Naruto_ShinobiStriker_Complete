#include "stdafx.h"
#include "State_Custom_WireReady.h"
#include "GameInstance.h"
#include "Player_Custom.h"
#include "Model.h"
#include "WireTrail.h"
#include "PlayerCamera.h"

CState_Custom_WireReady::CState_Custom_WireReady(CFsm* pFsm, CPlayer* pPlayer)
	:CState{ pFsm }
	, m_pPlayer{ pPlayer }
{
}

HRESULT CState_Custom_WireReady::Initialize(_uint iStateNum)
{
	m_iAnimation_WireReady = m_pPlayer->Get_Model()->Get_AnimationIndex("CustomMan_WireJump_Ready", 0.8);
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Custom_WireReady::Start_State(void* pArg)
{
	m_pPlayer->SetUp_NextAnimation(m_iAnimation_WireReady, false);

	m_pPlayer->Get_Transform()->Set_NewUp(XMVectorSet(0.f, 1.f, 0.f, 0.f));

	m_pPlayer->Get_RigidBody()->Set_IsFriction(true);
	m_pPlayer->Set_IsSkill(true);
	m_pPlayer->Set_IsFall(false);
	m_pPlayer->Set_IsWall(false);
	m_pPlayer->Set_IsCanAttachWall(true);

	m_pPlayer->Get_RigidBody()->Set_GravityScale(15.f);

	return S_OK;
}

void CState_Custom_WireReady::Update(_float fTimeDelta)
{
	_uint iFrame = m_pPlayer->Get_Frame(m_iAnimation_WireReady);

	if (iFrame >= m_iWireFrame && iFrame < m_iWireFrame + 2)
	{
		m_pPlayer->Get_WireTrail()->Appear();
		m_pGameInstance->Find_Camera(m_pPlayer->Get_LevelIndex())->ZoomOut(0.5f);
	}

	if (iFrame > 24)
	{
		if(m_pPlayer->Get_WireTrail()->IsActive())
			m_pPlayer->Change_State(CPlayer::WIRE_DASH);
		else
			m_pPlayer->Change_State(CPlayer::FALL_FRONT);
	}
}

void CState_Custom_WireReady::End_State()
{
	m_pGameInstance->Find_Camera(m_pPlayer->Get_LevelIndex())->ZoomOut(0.5f);
	m_pPlayer->Set_IsCanAttachWall(false);
}

CState_Custom_WireReady* CState_Custom_WireReady::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Custom_WireReady* pInstance = new CState_Custom_WireReady(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Custom_WireReady"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Custom_WireReady::Free()
{
	__super::Free();
}
