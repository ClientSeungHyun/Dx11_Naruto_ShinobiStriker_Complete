#include "stdafx.h"
#include "State_Custom_Tsukuyomi.h"
#include "GameInstance.h"
#include "Player_Custom.h"
#include "Model.h"
#include "ObjectPool.h"
#include "Particle_Up.h"

CState_Custom_Tsukuyomi::CState_Custom_Tsukuyomi(CFsm* pFsm, CPlayer* pPlayer)
	:CState{ pFsm }
	, m_pPlayer{ pPlayer }
{
}

HRESULT CState_Custom_Tsukuyomi::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;

	m_iAnimation_Amateras_Start = m_pPlayer->Get_Model()->Get_AnimationIndex("CustomMan_Ninjutsu_Amaterasu_Start", 1.3);
	m_iAnimation_Amateras_Loop = m_pPlayer->Get_Model()->Get_AnimationIndex("CustomMan_Ninjutsu_Amaterasu_Loop", 1.3);
	m_iAnimation_Amateras_End = m_pPlayer->Get_Model()->Get_AnimationIndex("CustomMan_Ninjutsu_Amaterasu_End", 1.3);

	return S_OK;
}

HRESULT CState_Custom_Tsukuyomi::Start_State(void* pArg)
{
	m_pPlayer->SetUp_NextAnimation(m_iAnimation_Amateras_Start, false, 0.2f);

	m_pPlayer->Set_IsInvincible(true);
	m_pPlayer->Find_NearTarget();

	m_pPlayer->Find_NearTarget();
	if(nullptr != m_pPlayer->Get_NearTarget())
		m_pPlayer->Get_Transform()->LookAt_Forward(m_pPlayer->Get_NearTarget()->Get_Position());

	m_isCreate = false;

	m_pPlayer->Get_SoundCom()->Play3D(TEXT("Voice_Tsukuyomi0.wav"), g_fVolume + 0.2f);
	m_pPlayer->Get_EffectSoundCom()->Play3D(TEXT("Effect_Ninjutsu.wav"), g_fVolume + 0.2f);

	return S_OK;
}

void CState_Custom_Tsukuyomi::Update(_float fTimeDelta)
{
	_uint iAnimationIndex = m_pPlayer->Get_CurrentAnimationIndex();
	_uint iFrame = m_pPlayer->Get_Frame(iAnimationIndex);

	if (m_pPlayer->Get_IsEndAnimation(m_iAnimation_Amateras_End))
		m_pFsm->Change_State(CPlayer_Custom::IDLE);

	if (m_pPlayer->Get_IsEndAnimation(m_iAnimation_Amateras_Start))
		m_pPlayer->SetUp_NextAnimation(m_iAnimation_Amateras_Loop, true);

	if (iAnimationIndex == m_iAnimation_Amateras_Loop)
	{
		if (KEY_NONE(KEY::NUM2))
		{
			m_pPlayer->Get_SoundCom()->Play3D(TEXT("Voice_Tsukuyomi1.wav"), g_fVolume + 0.2f);
			m_pPlayer->SetUp_NextAnimation(m_iAnimation_Amateras_End, false);
			dynamic_cast<CPlayer_Custom*>(m_pPlayer)->Create_Tsukuyomi();
		}
	}
}

void CState_Custom_Tsukuyomi::End_State()
{
	m_pPlayer->Set_IsInvincible(false);
	//m_pGameInstance->Stop_Sound(SOUND_SKILL);
}

CState_Custom_Tsukuyomi* CState_Custom_Tsukuyomi::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Custom_Tsukuyomi* pInstance = new CState_Custom_Tsukuyomi(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Custom_Amateras"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Custom_Tsukuyomi::Free()
{
	__super::Free();
}
