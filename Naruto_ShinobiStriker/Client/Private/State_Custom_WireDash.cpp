#include "stdafx.h"
#include "State_Custom_WireDash.h"
#include "GameInstance.h"
#include "Player_Custom.h"
#include "Model.h"
#include "WireTrail.h"
#include "PlayerCamera.h"

CState_Custom_WireDash::CState_Custom_WireDash(CFsm* pFsm, CPlayer* pPlayer)
	:CState{ pFsm }
	, m_pPlayer{ pPlayer }
{
}

HRESULT CState_Custom_WireDash::Initialize(_uint iStateNum)
{
	m_iAnimation_AerialDash_Start = m_pPlayer->Get_Model()->Get_AnimationIndex("CustomMan_Aerial_Dash_Start", 1.3f);
	m_iAnimation_AerialDash_Loop = m_pPlayer->Get_Model()->Get_AnimationIndex("CustomMan_Aerial_Dash_Loop", 1.6f);
	m_iAnimation_AerialDash_End = m_pPlayer->Get_Model()->Get_AnimationIndex("CustomMan_Aerial_Dash_End", 1.3f);

	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Custom_WireDash::Start_State(void* pArg)
{
	m_pPlayer->SetUp_NextAnimation(m_iAnimation_AerialDash_Loop, true, 0.f);

	m_pPlayer->Get_Transform()->Set_NewUp(XMVectorSet(0.f, 1.f, 0.f, 0.f));

	m_pPlayer->Get_RigidBody()->Set_IsFriction(true);
	m_pPlayer->Set_IsFall(false);
	m_pPlayer->Set_IsWall(false);
	m_pPlayer->Set_IsCanAttachWall(true);

	_vector vPlayerPos = m_pPlayer->Get_Position();
	_vector vTargetPos = XMLoadFloat3(&m_pPlayer->Get_WireTrail()->Get_EndPos());
	_vector vDir = XMVector3Normalize(vTargetPos - vPlayerPos);
	XMStoreFloat3(&m_vTargetDir, vDir);

	m_pPlayer->Get_Transform()->LookAt(XMLoadFloat3(&m_pPlayer->Get_WireTrail()->Get_EndPos()));

	m_pPlayer->Get_WireTrail()->DisAppear();

	return S_OK;
}

void CState_Custom_WireDash::Update(_float fTimeDelta)
{
	_uint iCurrentAnimation = m_pPlayer->Get_CurrentAnimationIndex();
	_uint iFrame = m_pPlayer->Get_Frame(iCurrentAnimation);

	_vector vPlayerPos = m_pPlayer->Get_Position();
	_vector vTargetPos = XMLoadFloat3(&m_pPlayer->Get_WireTrail()->Get_EndPos());
	_vector vDir = XMVector3Normalize(vTargetPos - vPlayerPos);

	m_pPlayer->Get_Transform()->Move_Dir(fTimeDelta, m_vTargetDir, 40.f);
	

	if (!m_pPlayer->Get_WireTrail()->IsActive())
	{
		m_pPlayer->Change_State(CPlayer_Custom::FALL_FRONT);
		return;
	}

	if (m_pPlayer->Get_IsAttachWall())
	{
		m_pPlayer->Get_WireTrail()->IsActive(false);
		m_pPlayer->Change_State(CPlayer_Custom::WALLATTACH);
	}
}

void CState_Custom_WireDash::End_State()
{
	m_pPlayer->Get_WireTrail()->IsActive(false);
	m_pGameInstance->Find_Camera(m_pPlayer->Get_LevelIndex())->ZoomOut(0.5f);
	m_pPlayer->Set_IsCanAttachWall(false);
}

CState_Custom_WireDash* CState_Custom_WireDash::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Custom_WireDash* pInstance = new CState_Custom_WireDash(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Custom_WireReady"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Custom_WireDash::Free()
{
	__super::Free();
}
