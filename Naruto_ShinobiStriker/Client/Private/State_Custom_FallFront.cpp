#include "stdafx.h"
#include "State_Custom_FallFront.h"
#include "GameInstance.h"
#include "Player_Custom.h"
#include "Model.h"

#include "WireTrail.h"
#include "PlayerCamera.h"

CState_Custom_FallFront::CState_Custom_FallFront(CFsm* pFsm, CPlayer* pPlayer)
	:CState{ pFsm }
	, m_pPlayer{ pPlayer }
{
}

HRESULT CState_Custom_FallFront::Initialize(_uint iStateNum)
{
	m_iAnimation_FallFront = m_pPlayer->Get_Model()->Get_AnimationIndex("CustomMan_Fall_Front_Loop");
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Custom_FallFront::Start_State(void* pArg)
{
	m_pPlayer->SetUp_NextAnimation(m_iAnimation_FallFront, true);

	m_pPlayer->Get_Transform()->Set_NewUp(XMVectorSet(0.f, 1.f, 0.f, 0.f));

	m_pPlayer->Get_RigidBody()->Set_IsFriction(true);
	m_pPlayer->Get_RigidBody()->Set_GravityScale(15.f);

	m_pPlayer->Set_IsSkill(false);
	m_pPlayer->Set_IsFall(true);
	m_pPlayer->Set_IsWall(false);
	m_pPlayer->Set_IsCanAttachWall(true);

	return S_OK;
}

void CState_Custom_FallFront::Update(_float fTimeDelta)
{
	Move(fTimeDelta);

 	if (KEY_TAP(KEY::SPACE) && !m_pPlayer->Get_IsDoubleJump())
	{
		m_pPlayer->Change_State(CPlayer_Custom::DOUBLEJUMP);
	}

	if (m_pPlayer->Get_IsGround())
	{
		m_pPlayer->Change_State(CPlayer_Custom::LAND);
	}

	if (m_pPlayer->Get_IsAttachWall())
	{
		m_pPlayer->Change_State(CPlayer_Custom::WALLATTACH);
	}

	if (KEY_TAP(KEY::NUM1) &&
		dynamic_cast<CPlayer_Custom*>(m_pPlayer)->Get_SkillCollTime1() <= 0.f)
	{
		switch (m_pPlayer->Get_SkillType())
		{
		case CPlayer::SKILL_NARUTO:
			m_pPlayer->Change_State(CPlayer_Custom::AERIAL_RASENGUN_CHARGE);
			break;
		case CPlayer::SKILL_SASUKE:
			m_pPlayer->Change_State(CPlayer_Custom::AERIAL_CHIDORI_CHARGE);
			break;
		default:
			dynamic_cast<CPlayer_Custom*>(m_pPlayer)->Clear_SkillCollTime1();
			break;
		}

		return;
	}

	if (KEY_TAP(KEY::NUM2) &&
		dynamic_cast<CPlayer_Custom*>(m_pPlayer)->Get_SkillCollTime2() <= 0.f)
	{
		switch (m_pPlayer->Get_SkillType())
		{
		case CPlayer::SKILL_NARUTO:
			m_pPlayer->Change_State(CPlayer_Custom::AERIAL_RASENSHURIKEN);
			break;
		case CPlayer::SKILL_SASUKE:
			m_pPlayer->Change_State(CPlayer_Custom::AERIAL_FIREBALL);
			break;
		default:
			dynamic_cast<CPlayer_Custom*>(m_pPlayer)->Clear_SkillCollTime2();
			break;
		}

		return;
	}

	if (KEY_TAP(KEY::NUM3) &&
		dynamic_cast<CPlayer_Custom*>(m_pPlayer)->Get_SkillCollTimeSP() <= 0.f)
	{
		switch (m_pPlayer->Get_SkillType())
		{
		case CPlayer::SKILL_SARIN:
			m_pPlayer->Change_State(CPlayer_Custom::SUSANOO);
			break;
		default:
			dynamic_cast<CPlayer_Custom*>(m_pPlayer)->Clear_SkillCollTimeSP();
			break;
		}
		return;
	}

	if (KEY_TAP(KEY::E) || KEY_HOLD(KEY::E))
	{
		m_pPlayer->Get_RigidBody()->Set_GravityScale(1.f);
		m_pGameInstance->Find_Camera(m_pPlayer->Get_LevelIndex())->ZoomIn(-0.5f, 0.6f);
	}
	else if (KEY_AWAY(KEY::E))
	{
		m_pPlayer->Get_RigidBody()->Set_GravityScale(15.f);
		m_pPlayer->Change_State(CPlayer_Custom::WIRE_READY);
	}
}

void CState_Custom_FallFront::End_State()
{
	m_pPlayer->Get_RigidBody()->Set_GravityScale(15.f);
	m_pGameInstance->Find_Camera(m_pPlayer->Get_LevelIndex())->ZoomOut(0.5f);
	m_pPlayer->Set_IsCanAttachWall(false);
	m_pPlayer->Set_IsFall(false);
}

void CState_Custom_FallFront::Move(_float fTimeDelta)
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

CState_Custom_FallFront* CState_Custom_FallFront::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Custom_FallFront* pInstance = new CState_Custom_FallFront(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Custom_FallFront"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Custom_FallFront::Free()
{
	__super::Free();
}
