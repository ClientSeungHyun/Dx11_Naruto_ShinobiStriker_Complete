#include "stdafx.h"
#include "State_Custom_WindMill.h"
#include "GameInstance.h"
#include "Player_Custom.h"
#include "Model.h"
#include "Collider_PlayerAttack.h"

CState_Custom_WindMill::CState_Custom_WindMill(CFsm* pFsm, CPlayer* pPlayer)
	:CState{ pFsm }
	, m_pPlayer{ pPlayer }
{
}

HRESULT CState_Custom_WindMill::Initialize(_uint iStateNum)
{
	m_iAnimation_Windmill = m_pPlayer->Get_Model()->Get_AnimationIndex("CustomMan_Attack_Hand_WindMill", 1.8);
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Custom_WindMill::Start_State(void* pArg)
{
	m_pPlayer->SetUp_NextAnimation(m_iAnimation_Windmill, false, 0.3f);

	m_isChase = false;
	m_isSound = false;


	return S_OK;
}

void CState_Custom_WindMill::Update(_float fTimeDelta)
{
	_int iFrame = m_pPlayer->Get_Frame(m_iAnimation_Windmill);

	if ((5 <= iFrame && iFrame <= 12)
		|| (50 <= iFrame && iFrame <= 60))
	{
		if (!m_isSound)
		{
			m_pPlayer->Get_SoundCom()->Play3D(TEXT("Naruto_Attack2.wav"), g_fVolume);
			m_isSound = true;
		}

		m_pPlayer->Chase_Target(fTimeDelta, 5.f);
		m_isChase = true;
	}

	if (m_pPlayer->Get_IsEndAnimation(m_iAnimation_Windmill))
		m_pPlayer->Change_State(CPlayer_Custom::IDLE);

	Attack_Collider();
}

void CState_Custom_WindMill::End_State()
{
	m_pPlayer->Set_IsStrongAttack(false);
	m_pPlayer->Get_AttackCollider()->IsActive(false);
}

void CState_Custom_WindMill::Attack_Collider()
{
	_uint iFrame = m_pPlayer->Get_Frame(m_iAnimation_Windmill);

	//(iFrame >= m_iAttackFrame1 && iFrame <= m_iAttackFrame1 + 8) ||
	if ((iFrame >= m_iAttackFrame2 && iFrame <= m_iAttackFrame2 + 10))
	{
		m_pPlayer->Set_IsStrongAttack(true);
		m_pPlayer->Get_AttackCollider()->IsActive(true);
	}
	else
	{
		m_pPlayer->Set_IsStrongAttack(false);
		m_pPlayer->Get_AttackCollider()->IsActive(false);
	}
}

CState_Custom_WindMill* CState_Custom_WindMill::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Custom_WindMill* pInstance = new CState_Custom_WindMill(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Custom_WindMill"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Custom_WindMill::Free()
{
	__super::Free();
}
