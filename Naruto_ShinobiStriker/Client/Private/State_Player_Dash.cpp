#include "stdafx.h"
#include "State_Player_Dash.h"
#include "GameInstance.h"
#include "Player_Custom.h"
#include "Model.h"


CState_Player_Dash::CState_Player_Dash(CFsm* pFsm, CPlayer* pPlayer)
	:CState{ pFsm }
	, m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_Dash::Initialize(_uint iStateNum)
{
	m_iAnimation_DashLeft = m_pPlayer->Get_Model()->Get_AnimationIndex("CustomMan_DashStep_Left", 1.3);
	m_iAnimation_DashRight= m_pPlayer->Get_Model()->Get_AnimationIndex("CustomMan_DashStep_Right", 1.3);
	m_iAnimation_DashFront = m_pPlayer->Get_Model()->Get_AnimationIndex("CustomMan_DashStep_Front", 1.6);
	m_iAnimation_DashBehind = m_pPlayer->Get_Model()->Get_AnimationIndex("CustomMan_DashStep_Behind", 1.6);

	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Player_Dash::Start_State(void* pArg)
{
	_vector vDashDir = XMVectorSet(0.f, 0.f, 0.f, 0.f);

	if (KEY_HOLD(KEY::W) || KEY_TAP(KEY::W))
	{
		m_pPlayer->SetUp_NextAnimation(m_iAnimation_DashFront, false, 0.f);
		vDashDir = XMVector3Normalize(m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_LOOK));
	}
	else if (KEY_HOLD(KEY::A) || KEY_TAP(KEY::A))
	{
		m_pPlayer->SetUp_NextAnimation(m_iAnimation_DashLeft, false, 0.f);
		vDashDir = XMVector3Normalize(m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_RIGHT)) * -1.f;
	}
	else if (KEY_HOLD(KEY::S) || KEY_TAP(KEY::S))
	{
		m_pPlayer->SetUp_NextAnimation(m_iAnimation_DashBehind, false, 0.f);
		vDashDir = XMVector3Normalize(m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_LOOK)) * -1.f;
	}
	else if (KEY_HOLD(KEY::D) || KEY_TAP(KEY::D))
	{
		m_pPlayer->SetUp_NextAnimation(m_iAnimation_DashRight, false, 0.f);
		vDashDir = XMVector3Normalize(m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_RIGHT));
	}

	Dash(vDashDir);

	m_isStop = false;

	m_pPlayer->On_MotionTrail();

	return S_OK;
}

void CState_Player_Dash::Update(_float fTimeDelta)
{
	_uint iCurrentAnimation = m_pPlayer->Get_CurrentAnimationIndex();
	_int iFrame = m_pPlayer->Get_Frame(iCurrentAnimation);

	if (!m_isStop &&(iCurrentAnimation == m_iAnimation_DashFront || iCurrentAnimation == m_iAnimation_DashBehind) && (iFrame == 20 || iFrame == 21))
	{
		m_pPlayer->Get_RigidBody()->Set_IsFriction(true);
		m_isStop = true;
	}
	else if(!m_isStop && (iCurrentAnimation == m_iAnimation_DashLeft || iCurrentAnimation == m_iAnimation_DashRight) && (iFrame == 11 || iFrame == 12))
	{
		m_pPlayer->Get_RigidBody()->Set_IsFriction(true);
		m_isStop = true;
	}

	if (KEY_HOLD(KEY::LSHIFT))
	{
		if (iFrame >= 19)
		{
			if (KEY_TAP(KEY::W) || KEY_TAP(KEY::S) || KEY_TAP(KEY::D) || KEY_TAP(KEY::A))
				m_pPlayer->Change_State(CPlayer::DASH);
		}
	}

	if (iFrame >= 23)
	{
		// 점프
		if (KEY_TAP(KEY::SPACE))
		{
			m_pPlayer->Change_State(CPlayer_Custom::JUMP);
			return;
		}

		// 공격
		if (KEY_TAP(KEY::LBUTTON))
		{
			m_pPlayer->Change_State(CPlayer_Custom::PUNCH_LEFT);
			return;
		}

		Change_SkillState();
	}

	if (m_pPlayer->Get_IsEndAnimation(iCurrentAnimation))
	{
		m_pPlayer->Change_State(CPlayer::IDLE);
	}
}

void CState_Player_Dash::End_State()
{
	m_pPlayer->Off_Motiontrail();
	m_pPlayer->Get_RigidBody()->Set_IsFriction(true);
}

void CState_Player_Dash::Dash(_fvector vDashDir)
{
	CRigidBody* pRigidBody = m_pPlayer->Get_RigidBody();

	m_pPlayer->Get_RigidBody()->Set_IsFriction(false);
	pRigidBody->Clear_All();
	pRigidBody->Add_Force(CRigidBody::FORCE, vDashDir * 10.f);
}

void CState_Player_Dash::Change_SkillState()
{
	if (KEY_TAP(KEY::NUM1) &&
		dynamic_cast<CPlayer_Custom*>(m_pPlayer)->Get_SkillCollTime1() <= 0.f)
	{
		switch (m_pPlayer->Get_SkillType())
		{
		case CPlayer::SKILL_NARUTO:
			m_pPlayer->Change_State(CPlayer_Custom::RASENGUN_CHARGE);
			break;
		case CPlayer::SKILL_SASUKE:
			m_pPlayer->Change_State(CPlayer_Custom::CHIDORI_CHARGE);
			break;
		case CPlayer::SKILL_SARIN:
			m_pPlayer->Change_State(CPlayer_Custom::AMATERAS);
			break;
		case CPlayer::SKILL_MINATO:
			m_pPlayer->Change_State(CPlayer_Custom::RASENKO);
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
			m_pPlayer->Change_State(CPlayer_Custom::RASENSHURIKEN);
			break;
		case CPlayer::SKILL_SASUKE:
			m_pPlayer->Change_State(CPlayer_Custom::FIREBALL);
			break;
		case CPlayer::SKILL_SARIN:
			m_pPlayer->Change_State(CPlayer_Custom::TSUKUYOMI);
			break;
		case CPlayer::SKILL_MINATO:
			m_pPlayer->Change_State(CPlayer_Custom::RAIJIN);
		}

		return;
	}

	if (KEY_TAP(KEY::NUM3))
	{
		switch (m_pPlayer->Get_SkillType())
		{
		case CPlayer::SKILL_NARUTO:
			m_pPlayer->Change_State(CPlayer_Custom::MASSIVERASENGAN);
			break;
		case CPlayer::SKILL_SASUKE:
			m_pPlayer->Change_State(CPlayer_Custom::KIRIN);
			break;
		case CPlayer::SKILL_MINATO:
			m_pPlayer->Change_State(CPlayer_Custom::TYPEZERO);
			break;
		}
		return;
	}
}

CState_Player_Dash* CState_Player_Dash::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Player_Dash* pInstance = new CState_Player_Dash(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Naruto_DashLeft"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Player_Dash::Free()
{
	__super::Free();
}
