#include "stdafx.h"
#include "State_Custom_AerialFireBall.h"
#include "GameInstance.h"
#include "Player_Custom.h"
#include "Model.h"
#include "ObjectPool.h"
#include "Particle_Up.h"


CState_Custom_AerialFireBall::CState_Custom_AerialFireBall(CFsm* pFsm, CPlayer* pPlayer)
	:CState{ pFsm }
	, m_pPlayer{ pPlayer }
{
}

HRESULT CState_Custom_AerialFireBall::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;

	m_iAnimation_FireBall = m_pPlayer->Get_Model()->Get_AnimationIndex("CustomMan_Ninjutsu_Aerial_Fireball_Lv3", 1.3);

	return S_OK;
}

HRESULT CState_Custom_AerialFireBall::Start_State(void* pArg)
{
	m_pPlayer->SetUp_NextAnimation(m_iAnimation_FireBall, false, 0.2f);

	m_pPlayer->Set_IsGravity(false);
	m_pPlayer->Set_IsInvincible(true);

	m_pPlayer->Get_RigidBody()->Set_IsGravity(false);
	m_pPlayer->Get_RigidBody()->Clear_All();

	m_pPlayer->Find_NearTarget();
	m_pPlayer->Set_IsSkill(true);

	m_isCreate = false;


	_vector vPos = m_pPlayer->Get_Position() + m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_UP);
	CObjectPool<CParticle_Up>::Get_GameObject()->Appear(vPos, XMVectorSet(1.f, 0.05f, 0.f, 0.f), CParticle_Up::PARTICLE_UP);

	m_pPlayer->Get_SoundCom()->Play3D(TEXT("Voice_FrieBall.wav"), g_fVolume + 0.2f);
	m_pPlayer->Get_EffectSoundCom()->Play3D(TEXT("Effect_Ninjutsu.wav"), g_fVolume + 0.2f);

	return S_OK;
}

void CState_Custom_AerialFireBall::Update(_float fTimeDelta)
{
	_uint iFrame = m_pPlayer->Get_Frame(m_iAnimation_FireBall);
	if (m_pPlayer->Get_IsEndAnimation(m_iAnimation_FireBall))
		m_pFsm->Change_State(CPlayer_Custom::FALL_FRONT);

	if (iFrame < 12)
	{
		if (nullptr != m_pPlayer->Get_NearTarget())
		{
			_vector vPlayerPos = m_pPlayer->Get_Position();
			_vector vTargetPos = m_pPlayer->Get_NearTarget()->Get_Position();

			_vector vTargetDir = XMVector3Normalize(vTargetPos - vPlayerPos);
			m_pPlayer->Get_Transform()->LookAt(vTargetPos);
		}
	}

	if ((iFrame == 12 || iFrame == 13) && !m_isCreate)
	{
		dynamic_cast<CPlayer_Custom*>(m_pPlayer)->Create_FireBall();
		m_isCreate = true;
	}
}

void CState_Custom_AerialFireBall::End_State()
{
	m_pPlayer->Set_IsSkill(false);
	m_pPlayer->Set_IsInvincible(false);
	//m_pGameInstance->Stop_Sound(SOUND_SKILL);
}

CState_Custom_AerialFireBall* CState_Custom_AerialFireBall::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Custom_AerialFireBall* pInstance = new CState_Custom_AerialFireBall(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Custom_AerialFireBall"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Custom_AerialFireBall::Free()
{
	__super::Free();
}
