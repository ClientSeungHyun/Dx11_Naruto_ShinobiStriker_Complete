#include "stdafx.h"
#include "State_Custom_DoubleJump.h"
#include "GameInstance.h"
#include "Player_Custom.h"
#include "Model.h"

#include "RigidBody.h"

CState_Custom_DoubleJump::CState_Custom_DoubleJump(CFsm* pFsm, CPlayer* pPlayer)
	:CState{ pFsm }
	, m_pPlayer{ pPlayer }
{
}

HRESULT CState_Custom_DoubleJump::Initialize(_uint iStateNum)
{
	m_iAnimation_DoubleJump = m_pPlayer->Get_Model()->Get_AnimationIndex("CustomMan_DoubleJump", 2.6);
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Custom_DoubleJump::Start_State(void* pArg)
{
	m_pPlayer->SetUp_NextAnimation(m_iAnimation_DoubleJump, false, 0.05f);

	m_pPlayer->Get_Transform()->Set_NewUp(XMVectorSet(0.f, 1.f, 0.f, 0.f));

	Set_JumpTargetPos();
	Jump();

	m_pPlayer->Set_IsDoubleJump(true);
	m_pPlayer->Set_IsWall(false);

	m_pPlayer->Get_EffectSoundCom()->Play3D(TEXT("Jump.wav"), g_fVolume);

	return S_OK;
}

void CState_Custom_DoubleJump::Update(_float fTimeDelta)
{
	Move(fTimeDelta);

	if (m_pPlayer->Get_IsEndAnimation(m_iAnimation_DoubleJump))
		m_pPlayer->Change_State(CPlayer_Custom::FALL_FRONT);

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
}

void CState_Custom_DoubleJump::End_State()
{
}

void CState_Custom_DoubleJump::Set_JumpTargetPos()
{
	_vector vCurrentPos = m_pPlayer->Get_Position();
	_vector vCurrentLook = XMVector3Normalize(m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_LOOK));

	_vector vJumpPos = vCurrentPos + XMVectorSet(0.f, 4.f, 0.f, 0.f) + vCurrentLook * 8.f;

	m_pPlayer->Set_TargetPos(vJumpPos);
}

void CState_Custom_DoubleJump::Jump()
{
	_vector vJumpDir;
	_vector vJumpPower = XMVectorSet(1.f, 20.f, 1.f, 0.f);

	if (m_pPlayer->Get_IsWall())
	{
		_vector vPlayerUp = XMVector3Normalize(m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_UP));
		_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

		vJumpDir = XMVector3Normalize(vPlayerUp + vUp);
	}
	else
	{
		vJumpDir = XMVector3Normalize(m_pPlayer->Get_TargetPos() - m_pPlayer->Get_Position());
	}

	vJumpDir *= vJumpPower;

	m_pPlayer->Get_RigidBody()->Clear_All();
	m_pPlayer->Get_RigidBody()->Add_Force(CRigidBody::FORCE, vJumpDir);
}

void CState_Custom_DoubleJump::Move(_float fTimeDelta)
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

CState_Custom_DoubleJump* CState_Custom_DoubleJump::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Custom_DoubleJump* pInstance = new CState_Custom_DoubleJump(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Custom_DoubleJump"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Custom_DoubleJump::Free()
{
	__super::Free();
}
