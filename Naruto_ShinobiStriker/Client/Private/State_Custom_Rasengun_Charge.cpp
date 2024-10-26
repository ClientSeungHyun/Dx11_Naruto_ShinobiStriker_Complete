#include "stdafx.h"
#include "State_Custom_Rasengun_Charge.h"
#include "GameInstance.h"
#include "Player_Custom.h"
#include "Model.h"
#include "ObjectPool.h"
#include "Particle_Up.h"


CState_Custom_Rasengun_Charge::CState_Custom_Rasengun_Charge(CFsm* pFsm, CPlayer* pPlayer)
	:CState{ pFsm }
	, m_pPlayer{ pPlayer }
{
}

HRESULT CState_Custom_Rasengun_Charge::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;

	m_iAnimation_Rasengun_Charge1 = m_pPlayer->Get_Model()->Get_AnimationIndex("CustomMan_Ninjutsu_Rasengun_Charge_Lv1", 1.7);
	m_iAnimation_Rasengun_Charge2to3 = m_pPlayer->Get_Model()->Get_AnimationIndex("CustomMan_Ninjutsu_Rasengun_Charge_Lv2toLv3", 1.7);
	m_iAnimation_Rasengun_ChargeLoop = m_pPlayer->Get_Model()->Get_AnimationIndex("CustomMan_Ninjutsu_Rasengun_Charge_Lv2toLv3_Loop", 1.7);

	return S_OK;
}

HRESULT CState_Custom_Rasengun_Charge::Start_State(void* pArg)
{
	m_pPlayer->SetUp_NextAnimation(m_iAnimation_Rasengun_Charge1, false, 0.1f);

	m_pPlayer->Set_IsInvincible(true);

	m_isCharge1 = { false };
	m_isCharge2to3 = { false };
	m_isChargeLoop = { false };

	_uint iSoundRand = rand() % 2;
	
	_vector vPos = m_pPlayer->Get_Position() + m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_UP);
	CObjectPool<CParticle_Up>::Get_GameObject()->Appear(vPos, XMVectorSet(0.05f, 0.05f, 0.9f, 0.5f), CParticle_Up::PARTICLE_UP);

	m_pPlayer->Get_EffectSoundCom()->Play3D(TEXT("Effect_Ninjutsu.wav"), g_fVolume + 0.2f);

	return S_OK;
}

void CState_Custom_Rasengun_Charge::Update(_float fTimeDelta)
{
	if (m_pPlayer->Get_CurrentAnimationIndex() == m_iAnimation_Rasengun_Charge1)
	{
		_uint iFrame = m_pPlayer->Get_Frame(m_iAnimation_Rasengun_Charge1);

		if (iFrame > 9 && !m_isCharge1)
		{
			dynamic_cast<CPlayer_Custom*>(m_pPlayer)->Create_Rasengan();
			m_pPlayer->SetUp_NextAnimation(m_iAnimation_Rasengun_Charge2to3, false, 0.1f);

			m_isCharge1 = true;
		}
	}
	else if (m_pPlayer->Get_CurrentAnimationIndex() == m_iAnimation_Rasengun_Charge2to3)
	{
		if (m_pPlayer->Get_IsEndAnimation(m_iAnimation_Rasengun_Charge2to3) && !m_isCharge2to3)
		{
			if (KEY_HOLD(KEY::NUM1))
			{
				m_pPlayer->SetUp_NextAnimation(m_iAnimation_Rasengun_ChargeLoop, true, 0.1f);
			}
			else
			{
				// 공격력 설정
				m_pPlayer->Change_State(CPlayer_Custom::RASENGUN_RUN);
			}
			m_isCharge2to3 = true;
		}
	}
	else
	{
		if (KEY_AWAY(KEY::NUM1) && !m_isChargeLoop && m_isCharge1)
		{
			// 공격력 설정
			m_pPlayer->Change_State(CPlayer_Custom::RASENGUN_RUN);
			m_isChargeLoop = true;
		}
	}

}

void CState_Custom_Rasengun_Charge::End_State()
{
	//m_pGameInstance->Stop_Sound(SOUND_SKILL);
}

CState_Custom_Rasengun_Charge* CState_Custom_Rasengun_Charge::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Custom_Rasengun_Charge* pInstance = new CState_Custom_Rasengun_Charge(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Custom_Rasengun_Charge"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Custom_Rasengun_Charge::Free()
{
	__super::Free();
}
