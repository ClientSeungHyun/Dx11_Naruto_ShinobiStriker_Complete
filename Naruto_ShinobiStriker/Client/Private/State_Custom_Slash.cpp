#include "stdafx.h"
#include "State_Custom_Slash.h"
#include "GameInstance.h"
#include "Player_Custom.h"
#include "Model.h"
#include "Collider_PlayerAttack.h"


CState_Custom_Slash::CState_Custom_Slash(CFsm* pFsm, CPlayer* pPlayer)
	:CState{ pFsm }
	, m_pPlayer{ pPlayer }
{
}

HRESULT CState_Custom_Slash::Initialize(_uint iStateNum)
{
	m_iAnimation_Slash = m_pPlayer->Get_Model()->Get_AnimationIndex("D34_Attack_KunaiSlashingHorizontally", 2.5);
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Custom_Slash::Start_State(void* pArg)
{
	m_pPlayer->SetUp_NextAnimation(m_iAnimation_Slash, false, 0.f);
	m_pPlayer->Find_NearTarget();

	m_isChase = false;
	m_isSound = false;


	return S_OK;
}

void CState_Custom_Slash::Update(_float fTimeDelta)
{
	_int iFrame = m_pPlayer->Get_Frame(m_iAnimation_Slash);

	if (5 <= iFrame && iFrame <= 10)
	{
		if (!m_isSound)
		{
			m_pPlayer->Get_SoundCom()->Play3D(TEXT("Naruto_Attack3.wav"), g_fVolume);
			m_isSound = true;
		}

		m_pPlayer->Chase_Target(fTimeDelta, 8.f);
		m_isChase = true;
	}

	if (iFrame >= 35 && iFrame <= 43)
	{
		if (KEY_TAP(KEY::RBUTTON))
		{
			m_pPlayer->Change_State(CPlayer_Custom::RISINGKICK);
			return;
		}

	}

	if (m_pPlayer->Get_IsEndAnimation(m_iAnimation_Slash))
		m_pPlayer->Change_State(CPlayer_Custom::IDLE);


	Attack_Collider();
}

void CState_Custom_Slash::End_State()
{
	m_pPlayer->Get_AttackCollider()->IsActive(false);
}

void CState_Custom_Slash::Attack_Collider()
{
	_uint iFrame = m_pPlayer->Get_Frame(m_iAnimation_Slash);


	if (iFrame == m_iAttackFrame || iFrame == m_iAttackFrame + 1)
	{
		m_pPlayer->Get_AttackCollider()->IsActive(true);
	}
	else
	{
		m_pPlayer->Get_AttackCollider()->IsActive(false);
	}

}

CState_Custom_Slash* CState_Custom_Slash::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Custom_Slash* pInstance = new CState_Custom_Slash(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Custom_Slash"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Custom_Slash::Free()
{
	__super::Free();
}
