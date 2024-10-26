#include "stdafx.h"
#include "State_Custom_AerialChidori_Charge.h"
#include "GameInstance.h"
#include "Player_Custom.h"
#include "Model.h"
#include "Collider_PlayerAttack.h"
#include "ObjectPool.h"
#include "Particle_Up.h"

CState_Custom_AerialChidori_Charge::CState_Custom_AerialChidori_Charge(CFsm* pFsm, CPlayer* pPlayer)
	:CState{ pFsm }
	, m_pPlayer{ pPlayer }
{
}

HRESULT CState_Custom_AerialChidori_Charge::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;

	m_iAnimation_AerialChidori_Charge1 = m_pPlayer->Get_Model()->Get_AnimationIndex("CustomMan_Ninjutsu_Aerial_Chidori_Charge_Lv1", 1.7);
	m_iAnimation_AerialChidori_Charge2to3 = m_pPlayer->Get_Model()->Get_AnimationIndex("CustomMan_Ninjutsu_Aerial_Chidori_Charge_Lv2toLv3", 1.7);
	m_iAnimation_AerialChidori_ChargeLoop = m_pPlayer->Get_Model()->Get_AnimationIndex("CustomMan_Ninjutsu_Aerial_Chidori_Charge_Lv2toLv3_Loop", 1.7);


	return S_OK;
}

HRESULT CState_Custom_AerialChidori_Charge::Start_State(void* pArg)
{
	m_pPlayer->SetUp_NextAnimation(m_iAnimation_AerialChidori_Charge1, false, 0.1f);

	m_pPlayer->Set_IsGravity(false);
	m_pPlayer->Set_IsInvincible(true);

	m_pPlayer->Get_RigidBody()->Set_IsGravity(false);
	m_pPlayer->Get_RigidBody()->Clear_All();

	m_isCharge1 = { false };
	m_isCharge2to3 = { false };
	m_isChargeLoop = { false };

	dynamic_cast<CPlayer_Custom*>(m_pPlayer)->Create_Chidori();

	_uint iSoundRand = rand() % 2;

	_vector vPos = m_pPlayer->Get_Position() + m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_UP);
	CObjectPool<CParticle_Up>::Get_GameObject()->Appear(vPos, XMVectorSet(0.05f, 0.05f, 0.9f, 0.5f), CParticle_Up::PARTICLE_UP);

	m_pPlayer->Set_IsSkill(true);

	m_pPlayer->Get_EffectSoundCom()->Play3D(TEXT("Effect_Ninjutsu.wav"), g_fVolume + 0.2f);

	return S_OK;
}

void CState_Custom_AerialChidori_Charge::Update(_float fTimeDelta)
{
	if (!m_isChargeLoop)
	{
		if (m_pPlayer->Get_CurrentAnimationIndex() == m_iAnimation_AerialChidori_Charge1)
		{
			_uint iFrame = m_pPlayer->Get_Frame(m_iAnimation_AerialChidori_Charge1);

			if (iFrame > 9 && !m_isCharge1)
			{
				if (KEY_HOLD(KEY::NUM1))
				{
					m_pPlayer->SetUp_NextAnimation(m_iAnimation_AerialChidori_Charge2to3, false, 0.1f);
				}
				else
				{
					// 공격력 설정
					m_pPlayer->Change_State(CPlayer_Custom::AERIAL_CHIDORI_RUN);
				}
				m_isCharge1 = true;
			}
		}
		else if (m_pPlayer->Get_CurrentAnimationIndex() == m_iAnimation_AerialChidori_Charge2to3)
		{
			if (m_pPlayer->Get_IsEndAnimation(m_iAnimation_AerialChidori_Charge2to3) && !m_isCharge2to3)
			{
				if (KEY_HOLD(KEY::NUM1))
				{
					m_pPlayer->SetUp_NextAnimation(m_iAnimation_AerialChidori_ChargeLoop, true, 0.1f);
				}
				else
				{
					// 공격력 설정
					m_pPlayer->Change_State(CPlayer_Custom::AERIAL_CHIDORI_RUN);
				}
				m_isCharge2to3 = true;
			}
		}


		if (KEY_AWAY(KEY::NUM1))
		{
			// 공격력 설정
			m_pPlayer->Change_State(CPlayer_Custom::AERIAL_CHIDORI_RUN);
			m_isChargeLoop = true;
		}
	}

}

void CState_Custom_AerialChidori_Charge::End_State()
{
	//m_pGameInstance->Stop_Sound(SOUND_SKILL);
}

CState_Custom_AerialChidori_Charge* CState_Custom_AerialChidori_Charge::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Custom_AerialChidori_Charge* pInstance = new CState_Custom_AerialChidori_Charge(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Custom_AerialChidori_Charge"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Custom_AerialChidori_Charge::Free()
{
	__super::Free();
}
