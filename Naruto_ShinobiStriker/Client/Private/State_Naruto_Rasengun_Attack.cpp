#include "stdafx.h"
#include "State_Naruto_Rasengun_Attack.h"
#include "GameInstance.h"
#include "Player_Naruto.h"
#include "Model.h"


CState_Naruto_Rasengun_Attack::CState_Naruto_Rasengun_Attack(CFsm* pFsm, CPlayer* pPlayer)
	:CState{ pFsm }
	, m_pPlayer{ pPlayer }
{
}

HRESULT CState_Naruto_Rasengun_Attack::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;

	m_iAnimation_Rasengun_Attack = m_pPlayer->Get_Model()->Get_AnimationIndex("Naruto_Ninjutsu_Rasengun_Attack_Lv1_End", 1.5);

	return S_OK;
}

HRESULT CState_Naruto_Rasengun_Attack::Start_State(void* pArg)
{
	m_pPlayer->SetUp_NextAnimation(m_iAnimation_Rasengun_Attack, false, 0.05f);

	m_pPlayer->Get_SoundCom()->Play3D(TEXT("Naruto_Rasengan.wav"), g_fVolume);


	return S_OK;
}

void CState_Naruto_Rasengun_Attack::Update(_float fTimeDelta)
{
	_uint iFrame = m_pPlayer->Get_Frame(m_iAnimation_Rasengun_Attack);

	if (iFrame >= 4)
	{
		dynamic_cast<CPlayer_Naruto*>(m_pPlayer)->Explosion_Rasengan();
	}

	if (m_pPlayer->Get_IsEndAnimation(m_iAnimation_Rasengun_Attack))
	{
		m_pPlayer->Change_State(CPlayer_Naruto::IDLE);
	}
}

void CState_Naruto_Rasengun_Attack::End_State()
{
	m_pPlayer->Set_IsInvincible(false);
}

CState_Naruto_Rasengun_Attack* CState_Naruto_Rasengun_Attack::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Naruto_Rasengun_Attack* pInstance = new CState_Naruto_Rasengun_Attack(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Naruto_Rasengun_Attack"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Naruto_Rasengun_Attack::Free()
{
	__super::Free();
}
