#include "stdafx.h"
#include "State_Custom_FireBall.h"
#include "GameInstance.h"
#include "Player_Custom.h"
#include "Model.h"
#include "ObjectPool.h"
#include "Particle_Up.h"


CState_Custom_FireBall::CState_Custom_FireBall(CFsm* pFsm, CPlayer* pPlayer)
	:CState{ pFsm }
	, m_pPlayer{ pPlayer }
{
}

HRESULT CState_Custom_FireBall::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;

	m_iAnimation_FireBall = m_pPlayer->Get_Model()->Get_AnimationIndex("CustomMan_Ninjutsu_Fireball_Lv3", 1.6);

	return S_OK;
}

HRESULT CState_Custom_FireBall::Start_State(void* pArg)
{
	m_pPlayer->SetUp_NextAnimation(m_iAnimation_FireBall, false, 0.2f);

	m_pPlayer->Set_IsInvincible(true);
	m_pPlayer->Find_NearTarget();

	m_isCreate = false;

	_vector vPos = m_pPlayer->Get_Position() + m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_UP);
	CObjectPool<CParticle_Up>::Get_GameObject()->Appear(vPos, XMVectorSet(1.f, 0.05f, 0.f, 0.f), CParticle_Up::PARTICLE_UP);

	m_pPlayer->Get_SoundCom()->Play3D(TEXT("Voice_FrieBall.wav"), g_fVolume + 0.2f);
	m_pPlayer->Get_EffectSoundCom()->Play3D(TEXT("Effect_Ninjutsu.wav"), g_fVolume + 0.2f);

	return S_OK;
}

void CState_Custom_FireBall::Update(_float fTimeDelta)
{
	_uint iFrame = m_pPlayer->Get_Frame(m_iAnimation_FireBall);
	if (m_pPlayer->Get_IsEndAnimation(m_iAnimation_FireBall))
		m_pFsm->Change_State(CPlayer_Custom::IDLE);

	if (iFrame < 12)
	{
		if (nullptr != m_pPlayer->Get_NearTarget())
		{
			_vector vPlayerPos = m_pPlayer->Get_Position();
			_vector vTargetPos = m_pPlayer->Get_NearTarget()->Get_Position();

			_vector vTargetDir = XMVector3Normalize(vTargetPos - vPlayerPos);
			m_pPlayer->Get_Transform()->Turn_Lerp(vTargetDir, 1.5f, fTimeDelta);
		}
	}

	if ((iFrame == 12 || iFrame == 13) && !m_isCreate)
	{
		dynamic_cast<CPlayer_Custom*>(m_pPlayer)->Create_FireBall();
		m_isCreate = true;
	}
}

void CState_Custom_FireBall::End_State()
{
	m_pPlayer->Set_IsInvincible(false);
	//m_pGameInstance->Stop_Sound(SOUND_SKILL);
}

CState_Custom_FireBall* CState_Custom_FireBall::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Custom_FireBall* pInstance = new CState_Custom_FireBall(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Custom_FireBall"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Custom_FireBall::Free()
{
	__super::Free();
}
