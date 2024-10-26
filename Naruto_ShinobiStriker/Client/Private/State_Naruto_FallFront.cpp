#include "stdafx.h"
#include "State_Naruto_FallFront.h"
#include "GameInstance.h"
#include "Player_Naruto.h"
#include "Model.h"


CState_Naruto_FallFront::CState_Naruto_FallFront(CFsm* pFsm, CPlayer* pPlayer)
	:CState{ pFsm }
	, m_pPlayer{ pPlayer }
{
}

HRESULT CState_Naruto_FallFront::Initialize(_uint iStateNum)
{
	m_iAnimation_FallFront = m_pPlayer->Get_Model()->Get_AnimationIndex("Naruto_Fall_Front_Loop");
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Naruto_FallFront::Start_State(void* pArg)
{
	m_pPlayer->SetUp_NextAnimation(m_iAnimation_FallFront, true, 0.1f);

	m_pPlayer->Get_Transform()->Set_NewUp(XMVectorSet(0.f, 1.f, 0.f, 0.f));

	m_pPlayer->Get_RigidBody()->Set_IsFriction(true);
	m_pPlayer->Set_IsFall(true);
	m_pPlayer->Set_IsWall(false);
	m_pPlayer->Set_IsCanAttachWall(true);

	return S_OK;
}

void CState_Naruto_FallFront::Update(_float fTimeDelta)
{
	Move(fTimeDelta);

	if (KEY_TAP(KEY::SPACE) && !m_pPlayer->Get_IsDoubleJump())
	{
		m_pPlayer->Change_State(CPlayer_Naruto::DOUBLEJUMP);
	}

	if (m_pPlayer->Get_IsGround())
	{
		m_pPlayer->Change_State(CPlayer_Naruto::LAND);
	}

	if (m_pPlayer->Get_IsAttachWall())
	{
		m_pPlayer->Change_State(CPlayer_Naruto::WALLATTACH);
	}

	// 나선환
	if (KEY_TAP(KEY::NUM1) &&
		dynamic_cast<CPlayer_Naruto*>(m_pPlayer)->Get_RasenganCoolTime() <= 0.f)
	{
		m_pPlayer->Change_State(CPlayer_Naruto::AERIAL_RASENGUN_CHARGE);
		return;
	}

	if (KEY_TAP(KEY::NUM2) &&
		dynamic_cast<CPlayer_Naruto*>(m_pPlayer)->Get_RasenShurikenCoolTime() <= 0.f)
	{
		m_pPlayer->Change_State(CPlayer_Naruto::AERIAL_RASENSHURIKEN);
		return;
	}

	// 와이어 액션 추가하기
}

void CState_Naruto_FallFront::End_State()
{
	m_pPlayer->Set_IsCanAttachWall(false);
	m_pPlayer->Set_IsFall(false);
}

void CState_Naruto_FallFront::Move(_float fTimeDelta)
{
	_vector vDir = XMVectorSet(0.f, 0.f, 0.f, 0.f);

	if (KEY_HOLD(KEY::W))
	{
		vDir += m_pPlayer->Calculate_Direction_Straight();
	}
	else if (KEY_HOLD(KEY::S))
	{
		vDir -= m_pPlayer->Calculate_Direction_Straight();
	}

	if (KEY_HOLD(KEY::D))
	{
		vDir += m_pPlayer->Calculate_Direction_Right();
	}

	if (KEY_HOLD(KEY::A))
	{
		vDir -= m_pPlayer->Calculate_Direction_Right();
	}

	if (XMVector3Length(vDir).m128_f32[0] > 0.f)
	{

		m_pPlayer->Move_Dir(vDir, 8.0f, fTimeDelta);
	}
}

CState_Naruto_FallFront* CState_Naruto_FallFront::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Naruto_FallFront* pInstance = new CState_Naruto_FallFront(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Naruto_Idle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Naruto_FallFront::Free()
{
	__super::Free();
}
