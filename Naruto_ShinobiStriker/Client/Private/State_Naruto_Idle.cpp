#include "stdafx.h"
#include "State_Naruto_Idle.h"
#include "GameInstance.h"
#include "Player_Naruto.h"
#include "Model.h"


CState_Naruto_Idle::CState_Naruto_Idle(CFsm* pFsm, CPlayer* pPlayer)
    :CState{pFsm}
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Naruto_Idle::Initialize(_uint iStateNum)
{
    m_iAnimation_Idle = m_pPlayer->Get_Model()->Get_AnimationIndex("Naruto_Idle_Loop");
	m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Naruto_Idle::Start_State(void* pArg)
{
	m_pPlayer->SetUp_NextAnimation(m_iAnimation_Idle, true, 0.2f);
	m_pPlayer->Disappear_FootTrail();

    return S_OK;
}

void CState_Naruto_Idle::Update(_float fTimeDelta)
{
	if (KEY_HOLD(KEY::LSHIFT))
	{
		if (KEY_TAP(KEY::W) || KEY_TAP(KEY::S) || KEY_TAP(KEY::D) || KEY_TAP(KEY::A))
			m_pPlayer->Change_State(CPlayer_Naruto::DASH);
	}
	else
	{
		// 이동
		if (KEY_HOLD(KEY::W) || KEY_HOLD(KEY::S) || KEY_HOLD(KEY::D) || KEY_HOLD(KEY::A))
		{
			m_pPlayer->Change_State(CPlayer_Naruto::RUN);
			return;
		}
	}

	// 점프
	if (KEY_TAP(KEY::SPACE))
	{
		m_pPlayer->Change_State(CPlayer_Naruto::JUMP);
		return;
	}

	// 공격
	if (KEY_TAP(KEY::LBUTTON))
	{
		//m_pPlayer->Change_State(CPlayer_Naruto::ATTACK1);
		return;
	}

	// 나선환
	if (KEY_TAP(KEY::NUM1)&&
		dynamic_cast<CPlayer_Naruto*>(m_pPlayer)->Get_RasenganCoolTime() <= 0.f)
	{
		m_pPlayer->Change_State(CPlayer_Naruto::RASENGUN_CHARGE);
		return;
	}

	// 나선수리검
	if (KEY_TAP(KEY::NUM2) && 
		dynamic_cast<CPlayer_Naruto*>(m_pPlayer)->Get_RasenShurikenCoolTime() <= 0.f)
	{
		m_pPlayer->Change_State(CPlayer_Naruto::RASENSHURIKEN);
		return;
	}

	// 차크라점프
	if (KEY_TAP(KEY::CTRL))
	{
		m_pPlayer->Change_State(CPlayer_Naruto::CHAKRAJUMP);
		return;
	}
}

void CState_Naruto_Idle::End_State()
{
	m_pPlayer->Appear_FootTrail();
}

CState_Naruto_Idle* CState_Naruto_Idle::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Naruto_Idle* pInstance = new CState_Naruto_Idle(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Naruto_Idle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Naruto_Idle::Free()
{
	__super::Free();
}
