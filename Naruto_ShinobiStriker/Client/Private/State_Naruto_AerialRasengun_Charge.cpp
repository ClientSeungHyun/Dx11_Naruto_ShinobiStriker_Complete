#include "stdafx.h"
#include "State_Naruto_AerialRasengun_Charge.h"
#include "GameInstance.h"
#include "Player_Naruto.h"
#include "Model.h"


CState_Naruto_AerialRasengun_Charge::CState_Naruto_AerialRasengun_Charge(CFsm* pFsm, CPlayer* pPlayer)
	:CState{ pFsm }
	, m_pPlayer{ pPlayer }
{
}

HRESULT CState_Naruto_AerialRasengun_Charge::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;

	m_iAnimation_AerialRasengun_Charge1 = m_pPlayer->Get_Model()->Get_AnimationIndex("Naruto_Ninjutsu_Aerial_Rasengun_Charge_Lv1", 1.7);
	m_iAnimation_AerialRasengun_Charge2to3 = m_pPlayer->Get_Model()->Get_AnimationIndex("Naruto_Ninjutsu_Aerial_Rasengun_Charge_Lv2toLv3", 1.7);
	m_iAnimation_AerialRasengun_ChargeLoop = m_pPlayer->Get_Model()->Get_AnimationIndex("Naruto_Ninjutsu_Aerial_Rasengun_Charge_Lv2toLv3_Loop", 1.7);


	return S_OK;
}

HRESULT CState_Naruto_AerialRasengun_Charge::Start_State(void* pArg)
{
	m_pPlayer->SetUp_NextAnimation(m_iAnimation_AerialRasengun_Charge1, false, 0.1f);

	m_pPlayer->Set_IsGravity(false);
	m_pPlayer->Set_IsInvincible(true);

	m_pPlayer->Get_RigidBody()->Set_IsGravity(false);
	m_pPlayer->Get_RigidBody()->Clear_All();

	m_isCharge1 = { false };
	m_isCharge2to3 = { false };
	m_isChargeLoop = { false };

	dynamic_cast<CPlayer_Naruto*>(m_pPlayer)->Create_Rasengan();

	_uint iSoundRand = rand() % 2;

	switch (iSoundRand)
	{
	case 0:
		m_pPlayer->Get_SoundCom()->Play3D(TEXT("Nartuo_Now.wav"), g_fVolume);
		break;

	case 1:
		m_pPlayer->Get_SoundCom()->Play3D(TEXT("Naruto_GetIt.wav"), g_fVolume);
		break;
	}

	m_pPlayer->Set_IsSkill(true);

	return S_OK;
}

void CState_Naruto_AerialRasengun_Charge::Update(_float fTimeDelta)
{
	if (!m_isChargeLoop)
	{
		if (m_pPlayer->Get_CurrentAnimationIndex() == m_iAnimation_AerialRasengun_Charge1)
		{
			_uint iFrame = m_pPlayer->Get_Frame(m_iAnimation_AerialRasengun_Charge1);

			if (iFrame > 9 && !m_isCharge1)
			{
				if (KEY_HOLD(KEY::NUM1))
				{
					m_pPlayer->SetUp_NextAnimation(m_iAnimation_AerialRasengun_Charge2to3, false, 0.1f);
				}
				else
				{
					// 공격력 설정
					m_pPlayer->Change_State(CPlayer_Naruto::AERIAL_RASENGUN_RUN);
				}
				m_isCharge1 = true;
			}
		}
		else if (m_pPlayer->Get_CurrentAnimationIndex() == m_iAnimation_AerialRasengun_Charge2to3)
		{
			if (m_pPlayer->Get_IsEndAnimation(m_iAnimation_AerialRasengun_Charge2to3) && !m_isCharge2to3)
			{
				if (KEY_HOLD(KEY::NUM1))
				{
					m_pPlayer->SetUp_NextAnimation(m_iAnimation_AerialRasengun_ChargeLoop, true, 0.1f);
				}
				else
				{
					// 공격력 설정
					m_pPlayer->Change_State(CPlayer_Naruto::AERIAL_RASENGUN_RUN);
				}
				m_isCharge2to3 = true;
			}
		}


		if (KEY_AWAY(KEY::NUM1))
		{
			// 공격력 설정
			m_pPlayer->Change_State(CPlayer_Naruto::AERIAL_RASENGUN_RUN);
			m_isChargeLoop = true;
		}
	}

}

void CState_Naruto_AerialRasengun_Charge::End_State()
{

	//m_pGameInstance->Stop_Sound(SOUND_SKILL);
}

CState_Naruto_AerialRasengun_Charge* CState_Naruto_AerialRasengun_Charge::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Naruto_AerialRasengun_Charge* pInstance = new CState_Naruto_AerialRasengun_Charge(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Naruto_Idle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Naruto_AerialRasengun_Charge::Free()
{
	__super::Free();
}
