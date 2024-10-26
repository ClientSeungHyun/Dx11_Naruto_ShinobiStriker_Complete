#include "stdafx.h"
#include "State_Custom_Rasengun_Attack.h"
#include "GameInstance.h"
#include "Player_Custom.h"
#include "Model.h"
#include "Collider_PlayerAttack.h"

CState_Custom_Rasengun_Attack::CState_Custom_Rasengun_Attack(CFsm* pFsm, CPlayer* pPlayer)
	:CState{ pFsm }
	, m_pPlayer{ pPlayer }
{
}

HRESULT CState_Custom_Rasengun_Attack::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;

	m_iAnimation_Rasengun_Attack = m_pPlayer->Get_Model()->Get_AnimationIndex("CustomMan_Ninjutsu_Rasengun_Attack_Lv1_End", 1.5);

	return S_OK;
}

HRESULT CState_Custom_Rasengun_Attack::Start_State(void* pArg)
{
	m_pPlayer->SetUp_NextAnimation(m_iAnimation_Rasengun_Attack, false, 0.05f);

	m_pPlayer->Get_SoundCom()->Play3D(TEXT("Voice_Rasengan.wav"), g_fVolume + 0.2f);


	return S_OK;
}

void CState_Custom_Rasengun_Attack::Update(_float fTimeDelta)
{
	_uint iFrame = m_pPlayer->Get_Frame(m_iAnimation_Rasengun_Attack);

	if (iFrame >= 7 && iFrame <= 8)
	{
		dynamic_cast<CPlayer_Custom*>(m_pPlayer)->Explosion_Rasengan();
	}
	else if (m_pPlayer->Get_IsEndAnimation(m_iAnimation_Rasengun_Attack))
	{
		m_pPlayer->Change_State(CPlayer_Custom::IDLE);
	}
}

void CState_Custom_Rasengun_Attack::End_State()
{
	m_pPlayer->Set_IsInvincible(false);
	m_pPlayer->Get_AttackCollider()->IsActive(false);
}

CState_Custom_Rasengun_Attack* CState_Custom_Rasengun_Attack::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Custom_Rasengun_Attack* pInstance = new CState_Custom_Rasengun_Attack(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Custom_Rasengun_Attack"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Custom_Rasengun_Attack::Free()
{
	__super::Free();
}
