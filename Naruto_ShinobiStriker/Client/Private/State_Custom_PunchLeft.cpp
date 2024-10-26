#include "stdafx.h"
#include "State_Custom_PunchLeft.h"
#include "GameInstance.h"
#include "Player_Custom.h"
#include "Model.h"
#include "Collider_PlayerAttack.h"


CState_Custom_PunchLeft::CState_Custom_PunchLeft(CFsm* pFsm, CPlayer* pPlayer)
	:CState{ pFsm }
	, m_pPlayer{ pPlayer }
{
}

HRESULT CState_Custom_PunchLeft::Initialize(_uint iStateNum)
{
	m_iAnimation_PunchLeft = m_pPlayer->Get_Model()->Get_AnimationIndex("D34_Attack_Punch_Left", 1.7);
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Custom_PunchLeft::Start_State(void* pArg)
{
	m_pPlayer->SetUp_NextAnimation(m_iAnimation_PunchLeft, false, 0.f);
	m_pPlayer->Find_NearTarget();

	m_isChase = false;
	m_isSound = false;

	m_pPlayer->Get_SoundCom()->Play3D(TEXT("Voice_Attack0.wav"), g_fVolume + 0.2f);

	return S_OK;
}

void CState_Custom_PunchLeft::Update(_float fTimeDelta)
{
	_int iFrame = m_pPlayer->Get_Frame(m_iAnimation_PunchLeft);

	if (7 <= iFrame && iFrame <= 11)
	{
		if (!m_isSound)
		{
			m_pPlayer->Get_SoundCom()->Play3D(TEXT("Naruto_Attack3.wav"), g_fVolume);
			m_isSound = true;
		}

		m_pPlayer->Chase_Target(fTimeDelta, 8.f);
		m_isChase = true;
	}

	if (iFrame >= 17)
	{
		if (KEY_TAP(KEY::LBUTTON))
		{
			m_pPlayer->Change_State(CPlayer_Custom::PUNCH_RIGHT);
			return;
		}
		else if (KEY_TAP(KEY::RBUTTON))
		{
			m_pPlayer->Change_State(CPlayer_Custom::SLASH);
			return;
		}

	}

	if (m_pPlayer->Get_IsEndAnimation(m_iAnimation_PunchLeft))
		m_pPlayer->Change_State(CPlayer_Custom::IDLE);


	Attack_Collider();
}

void CState_Custom_PunchLeft::End_State()
{
	m_pPlayer->Get_AttackCollider()->IsActive(false);
}

void CState_Custom_PunchLeft::Attack_Collider()
{
	_uint iFrame = m_pPlayer->Get_Frame(m_iAnimation_PunchLeft);


	if (iFrame == m_iAttackFrame || iFrame == m_iAttackFrame + 1)
	{
		m_pPlayer->Get_AttackCollider()->IsActive(true);
	}
	else
	{
		m_pPlayer->Get_AttackCollider()->IsActive(false);
	}

}

CState_Custom_PunchLeft* CState_Custom_PunchLeft::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Custom_PunchLeft* pInstance = new CState_Custom_PunchLeft(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Custom_PunchLeft"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Custom_PunchLeft::Free()
{
	__super::Free();
}
