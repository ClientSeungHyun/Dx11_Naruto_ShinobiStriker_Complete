#include "stdafx.h"
#include "State_Naruto_DoubleKick.h"
#include "GameInstance.h"
#include "Player_Naruto.h"
#include "Model.h"
#include "Collider_PlayerAttack.h"

CState_Naruto_DoubleKick::CState_Naruto_DoubleKick(CFsm* pFsm, CPlayer* pPlayer)
	:CState{ pFsm }
	, m_pPlayer{ pPlayer }
{
}

HRESULT CState_Naruto_DoubleKick::Initialize(_uint iStateNum)
{
	m_iAnimation_DoubleKick = m_pPlayer->Get_Model()->Get_AnimationIndex("Naruto_Attack_JumpDoubleKick", 2.0);
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Naruto_DoubleKick::Start_State(void* pArg)
{
	m_pPlayer->SetUp_NextAnimation(m_iAnimation_DoubleKick, false, 0.1f);

	m_isChase = false;
	m_isSound = false;


	return S_OK;
}

void CState_Naruto_DoubleKick::Update(_float fTimeDelta)
{
	_uint iFrame = m_pPlayer->Get_Frame(m_iAnimation_DoubleKick);

	if (5 <= iFrame && iFrame <= 14)
	{
		if (!m_isSound)
		{
			m_pPlayer->Get_SoundCom()->Play3D(TEXT("Naruto_Attack2.wav"), g_fVolume);
			m_isSound = true;
		}
		m_pPlayer->Chase_Target(fTimeDelta, 5.f);
		m_isChase = true;
	}

	if (m_pPlayer->Get_IsEndAnimation(m_iAnimation_DoubleKick))
		m_pPlayer->Change_State(CPlayer_Naruto::IDLE);

	Attack_Collider();
}

void CState_Naruto_DoubleKick::End_State()
{
	m_pPlayer->Set_IsStrongAttack(false);
	m_pPlayer->Get_AttackCollider()->IsActive(false);
}

void CState_Naruto_DoubleKick::Attack_Collider()
{
	_uint iFrame = m_pPlayer->Get_Frame(m_iAnimation_DoubleKick);


	if ((iFrame >= m_iAttackFrame1 && iFrame <= m_iAttackFrame1 + 5))
	{
		m_pPlayer->Get_AttackCollider()->IsActive(true);
	}
	else if (iFrame >= m_iAttackFrame2 && iFrame <= m_iAttackFrame2 + 10)
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

CState_Naruto_DoubleKick* CState_Naruto_DoubleKick::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Naruto_DoubleKick* pInstance = new CState_Naruto_DoubleKick(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Naruto_Idle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Naruto_DoubleKick::Free()
{
	__super::Free();
}
