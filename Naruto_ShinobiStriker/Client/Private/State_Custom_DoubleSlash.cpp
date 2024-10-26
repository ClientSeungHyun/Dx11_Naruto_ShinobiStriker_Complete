#include "stdafx.h"
#include "State_Custom_DoubleSlash.h"
#include "GameInstance.h"
#include "Player_Custom.h"
#include "Model.h"
#include "Collider_PlayerAttack.h"


CState_Custom_DoubleSlash::CState_Custom_DoubleSlash(CFsm* pFsm, CPlayer* pPlayer)
	:CState{ pFsm }
	, m_pPlayer{ pPlayer }
{
}

HRESULT CState_Custom_DoubleSlash::Initialize(_uint iStateNum)
{
	m_iAnimation_DoubleSlash = m_pPlayer->Get_Model()->Get_AnimationIndex("D34_Attack_KunaiDoubleSlashing", 1.7);
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Custom_DoubleSlash::Start_State(void* pArg)
{
	m_pPlayer->SetUp_NextAnimation(m_iAnimation_DoubleSlash, false, 0.1f);
	m_pPlayer->Find_NearTarget();

	m_isChase = false;
	m_isSound = false;

	return S_OK;
}

void CState_Custom_DoubleSlash::Update(_float fTimeDelta)
{
	_uint iFrame = m_pPlayer->Get_Frame(m_iAnimation_DoubleSlash);

	if (iFrame >= m_iAttackFrame1 - 3 && iFrame <= 40)
	{
		if (!m_isSound)
		{
			m_pPlayer->Get_SoundCom()->Play3D(TEXT("Naruto_Attack3.wav"), g_fVolume);
			m_isSound = true;
		}

		m_pPlayer->Chase_Target(fTimeDelta, 8.f);
		m_isChase = true;
	}

	if (iFrame >= 47 && iFrame <= 55)
	{
		if (KEY_TAP(KEY::LBUTTON))
		{
			m_pPlayer->Change_State(CPlayer_Custom::GROUNDPUNCH);
			return;
		}

	}

	if (m_pPlayer->Get_IsEndAnimation(m_iAnimation_DoubleSlash))
		m_pPlayer->Change_State(CPlayer_Custom::IDLE);


	Attack_Collider();
}

void CState_Custom_DoubleSlash::End_State()
{
	m_pPlayer->Get_AttackCollider()->IsActive(false);
}

void CState_Custom_DoubleSlash::Attack_Collider()
{
	_uint iFrame = m_pPlayer->Get_Frame(m_iAnimation_DoubleSlash);


	if (iFrame == m_iAttackFrame1 || iFrame == m_iAttackFrame1 + 1)
	{
		m_pPlayer->Get_AttackCollider()->IsActive(true);
	}
	else if (iFrame == m_iAttackFrame2 || iFrame == m_iAttackFrame2 + 1)
	{
		m_pPlayer->Get_AttackCollider()->IsActive(true);
	}
	else
	{
		m_pPlayer->Get_AttackCollider()->IsActive(false);
	}

}

CState_Custom_DoubleSlash* CState_Custom_DoubleSlash::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Custom_DoubleSlash* pInstance = new CState_Custom_DoubleSlash(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Custom_DoubleSlash"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Custom_DoubleSlash::Free()
{
	__super::Free();
}
