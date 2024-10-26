#include "stdafx.h"
#include "State_Custom_Idle.h"
#include "GameInstance.h"
#include "Player_Custom.h"
#include "Model.h"


CState_Custom_Idle::CState_Custom_Idle(CFsm* pFsm, CPlayer* pPlayer)
    :CState{pFsm}
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Custom_Idle::Initialize(_uint iStateNum)
{
    m_iAnimation_Idle = m_pPlayer->Get_Model()->Get_AnimationIndex("CustomMan_Idle_Loop");
	m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Custom_Idle::Start_State(void* pArg)
{
	m_pPlayer->SetUp_NextAnimation(m_iAnimation_Idle, true, 0.3f);

    return S_OK;
}

void CState_Custom_Idle::Update(_float fTimeDelta)
{
	// 이동
	if (KEY_HOLD(KEY::LSHIFT))
	{
		if (KEY_TAP(KEY::W) || KEY_TAP(KEY::S) || KEY_TAP(KEY::D) || KEY_TAP(KEY::A))
			m_pPlayer->Change_State(CPlayer_Custom::DASH);
	}
	else
	{
		// 이동
		if (KEY_HOLD(KEY::W) || KEY_HOLD(KEY::S) || KEY_HOLD(KEY::D) || KEY_HOLD(KEY::A))
		{
			m_pPlayer->Change_State(CPlayer_Custom::RUN);
			return;
		}
	}

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

	// 차크라점프
	if (KEY_TAP(KEY::CTRL))
	{
		m_pPlayer->Change_State(CPlayer_Custom::CHAKRAJUMP);
		return;
	}
}

void CState_Custom_Idle::End_State()
{
}

void CState_Custom_Idle::Change_SkillState()
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
		case CPlayer::SKILL_NARUTO:
			m_pPlayer->Change_State(CPlayer_Custom::MASSIVERASENGAN);
			break;
		case CPlayer::SKILL_SASUKE:
			m_pPlayer->Change_State(CPlayer_Custom::KIRIN);
			break;
		case CPlayer::SKILL_SARIN:
			m_pPlayer->Change_State(CPlayer_Custom::SUSANOO);
			break;
		case CPlayer::SKILL_MINATO:
			m_pPlayer->Change_State(CPlayer_Custom::TYPEZERO);
			break;
		default:
			dynamic_cast<CPlayer_Custom*>(m_pPlayer)->Clear_SkillCollTimeSP();
			break;
		}
		return;
	}
}

CState_Custom_Idle* CState_Custom_Idle::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Custom_Idle* pInstance = new CState_Custom_Idle(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Custom_Idle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Custom_Idle::Free()
{
	__super::Free();
}
