#include "stdafx.h"
#include "State_Custom_BackBlow.h"
#include "GameInstance.h"
#include "Player_Custom.h"
#include "Model.h"
#include "Collider_PlayerAttack.h"


CState_Custom_BackBlow::CState_Custom_BackBlow(CFsm* pFsm, CPlayer* pPlayer)
	:CState{ pFsm }
	, m_pPlayer{ pPlayer }
{
}

HRESULT CState_Custom_BackBlow::Initialize(_uint iStateNum)
{
	m_iAnimation_BackBlow = m_pPlayer->Get_Model()->Get_AnimationIndex("CustomMan_Attack_cmb07", 1.8);
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Custom_BackBlow::Start_State(void* pArg)
{
	m_pPlayer->SetUp_NextAnimation(m_iAnimation_BackBlow, false, 0.2f);
	m_pPlayer->Find_NearTarget();

	m_isChase = false;
	m_isSound = false;


	return S_OK;
}

void CState_Custom_BackBlow::Update(_float fTimeDelta)
{
	_int iFrame = m_pPlayer->Get_Frame(m_iAnimation_BackBlow);

	if (6 <= iFrame && iFrame <= 9)
	{
		if (!m_isSound)
		{
			m_pPlayer->Get_SoundCom()->Play3D(TEXT("Naruto_Attack3.wav"), g_fVolume);
			m_isSound = true;
		}

		m_pPlayer->Chase_Target(fTimeDelta, 5.f);
		m_isChase = true;
	}

	if (iFrame >= 13)
	{
		if (KEY_TAP(KEY::LBUTTON))
		{
			//m_pPlayer->Change_State(CPlayer_Custom::ATTACK3);
			return;
		}

	}

	if (m_pPlayer->Get_IsEndAnimation(m_iAnimation_BackBlow))
		m_pPlayer->Change_State(CPlayer_Custom::IDLE);


	Attack_Collider();
}

void CState_Custom_BackBlow::End_State()
{
	m_pPlayer->Get_AttackCollider()->IsActive(false);
}

void CState_Custom_BackBlow::Attack_Collider()
{
	_uint iFrame = m_pPlayer->Get_Frame(m_iAnimation_BackBlow);


	if (iFrame >= m_iAttackFrame && iFrame <= m_iAttackFrame + 8)
	{
		m_pPlayer->Get_AttackCollider()->IsActive(true);
	}
	else
	{
		m_pPlayer->Get_AttackCollider()->IsActive(false);
	}

}

CState_Custom_BackBlow* CState_Custom_BackBlow::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Custom_BackBlow* pInstance = new CState_Custom_BackBlow(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Custom_PunchLeft"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Custom_BackBlow::Free()
{
	__super::Free();
}
