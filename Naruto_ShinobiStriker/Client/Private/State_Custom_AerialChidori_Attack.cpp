#include "stdafx.h"
#include "State_Custom_AerialChidori_Attack.h"
#include "GameInstance.h"
#include "Player_Custom.h"
#include "Model.h"
#include "Collider_PlayerAttack.h"
#include "ObjectPool.h"
#include "Particle_Up.h"

CState_Custom_AerialChidori_Attack::CState_Custom_AerialChidori_Attack(CFsm* pFsm, CPlayer* pPlayer)
	:CState{ pFsm }
	, m_pPlayer{ pPlayer }
{
}

HRESULT CState_Custom_AerialChidori_Attack::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;

	m_iAnimation_AerialChidori_Attack = m_pPlayer->Get_Model()->Get_AnimationIndex("CustomMan_Ninjutsu_Aerial_Chidori_Attack_End", 1.5);

	return S_OK;
}

HRESULT CState_Custom_AerialChidori_Attack::Start_State(void* pArg)
{
	m_pPlayer->SetUp_NextAnimation(m_iAnimation_AerialChidori_Attack, false, 0.1f);

	m_pPlayer->Get_SoundCom()->Play3D(TEXT("Voice_Chidori.wav"), g_fVolume + 0.2f);

	return S_OK;
}

void CState_Custom_AerialChidori_Attack::Update(_float fTimeDelta)
{
	_uint iFrame = m_pPlayer->Get_Frame(m_iAnimation_AerialChidori_Attack);

	if (iFrame >= 4)
	{
		dynamic_cast<CPlayer_Custom*>(m_pPlayer)->Explosion_Chidori();
	}

	if (m_pPlayer->Get_IsEndAnimation(m_iAnimation_AerialChidori_Attack))
	{
		m_pPlayer->Change_State(CPlayer_Custom::FALL_FRONT);
	}
}

void CState_Custom_AerialChidori_Attack::End_State()
{
	m_pPlayer->Set_IsInvincible(false);
	m_pPlayer->Set_IsGravity(true);
	m_pPlayer->Set_IsSkill(false);

	m_pPlayer->Get_AttackCollider()->IsActive(false);

}

CState_Custom_AerialChidori_Attack* CState_Custom_AerialChidori_Attack::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Custom_AerialChidori_Attack* pInstance = new CState_Custom_AerialChidori_Attack(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Custom_AerialChidori_Attack"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Custom_AerialChidori_Attack::Free()
{
	__super::Free();
}
