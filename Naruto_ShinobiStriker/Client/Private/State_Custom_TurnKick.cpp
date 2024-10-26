#include "stdafx.h"
#include "State_Custom_TurnKick.h"
#include "GameInstance.h"
#include "Player_Custom.h"
#include "Model.h"
#include "Collider_PlayerAttack.h"

CState_Custom_TurnKick::CState_Custom_TurnKick(CFsm* pFsm, CPlayer* pPlayer)
	:CState{ pFsm }
	, m_pPlayer{ pPlayer }
{
}

HRESULT CState_Custom_TurnKick::Initialize(_uint iStateNum)
{
	m_iAnimation_TurnKick = m_pPlayer->Get_Model()->Get_AnimationIndex("CustomMan_Attack_DoubleTurnKick", 1.8);
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Custom_TurnKick::Start_State(void* pArg)
{
	m_pPlayer->SetUp_NextAnimation(m_iAnimation_TurnKick, false, 0.1f);

	m_isChase = false;
	m_isSound = false;


	return S_OK;
}

void CState_Custom_TurnKick::Update(_float fTimeDelta)
{
	_int iFrame = m_pPlayer->Get_Frame(m_iAnimation_TurnKick);

	if ((25 <= iFrame && iFrame <= 28)
		|| (60 <= iFrame && iFrame <= 64))
	{
		if (!m_isSound)
		{
			m_pPlayer->Get_SoundCom()->Play3D(TEXT("Naruto_Attack2.wav"), g_fVolume);
			m_isSound = true;
		}

		m_pPlayer->Chase_Target(fTimeDelta, 5.f);
		m_isChase = true;
	}

	if (iFrame >= 64)
	{
		if (KEY_TAP(KEY::LBUTTON))
		{
			//m_pPlayer->Change_State(CPlayer_Custom::ATTACK_STRONG);
			return;
		}

	}

	if (m_pPlayer->Get_IsEndAnimation(m_iAnimation_TurnKick))
		m_pPlayer->Change_State(CPlayer_Custom::IDLE);

	Attack_Collider();
}

void CState_Custom_TurnKick::End_State()
{
	m_pPlayer->Get_AttackCollider()->IsActive(false);
}

void CState_Custom_TurnKick::Attack_Collider()
{
	_uint iFrame = m_pPlayer->Get_Frame(m_iAnimation_TurnKick);


	if ((iFrame >= m_iAttackFrame1 && iFrame <= m_iAttackFrame1 + 5) ||
		(iFrame >= m_iAttackFrame2 && iFrame <= m_iAttackFrame2 + 5))
	{
		m_pPlayer->Get_AttackCollider()->IsActive(true);
	}
	else
	{
		m_pPlayer->Get_AttackCollider()->IsActive(false);
	}
}

CState_Custom_TurnKick* CState_Custom_TurnKick::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Custom_TurnKick* pInstance = new CState_Custom_TurnKick(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Custom_TurnKick"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Custom_TurnKick::Free()
{
	__super::Free();
}
