#include "stdafx.h"
#include "State_Custom_RasenShuriken.h"
#include "GameInstance.h"
#include "Player_Custom.h"
#include "Shadow_Custom.h"
#include "Model.h"
#include "ObjectPool.h"
#include "Particle_Up.h"


CState_Custom_RasenShuriken::CState_Custom_RasenShuriken(CFsm* pFsm, CPlayer* pPlayer)
	:CState{ pFsm }
	, m_pPlayer{ pPlayer }
{
}

HRESULT CState_Custom_RasenShuriken::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;

	m_iAnimation_RasenShuriken = m_pPlayer->Get_Model()->Get_AnimationIndex("CustomMan_Ninjutsu_TrueRasenShuriken", 1.5);

	m_iFrame_Create = 10;
	m_iFrame_Shoot = 70;

	return S_OK;
}

HRESULT CState_Custom_RasenShuriken::Start_State(void* pArg)
{
	m_pPlayer->SetUp_NextAnimation(m_iAnimation_RasenShuriken, false, 0.1f);

	m_pPlayer->Set_IsInvincible(true);
	m_pPlayer->Find_NearTarget();

	CShadow_Custom::SHADOW_DESC tDesc = {};
	tDesc.fRotationPerSec = 0.f;
	tDesc.fSpeedPerSec = 0.f;
	tDesc.eSkill = CShadow_Custom::RASENSHURIKEN;

	dynamic_cast<CPlayer_Custom*>(m_pPlayer)->Create_Shadow(tDesc);
	//dynamic_cast<CPlayer_Naruto*>(m_pPlayer)->Set_IsSenninEye(true);

	m_isShoot = false;

	_vector vPos = m_pPlayer->Get_Position() + m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_UP);
	CObjectPool<CParticle_Up>::Get_GameObject()->Appear(vPos, XMVectorSet(0.05f, 0.05f, 0.9f, 0.5f), CParticle_Up::PARTICLE_UP);

	m_pPlayer->Get_SoundCom()->Play3D(TEXT("Voice_RasenShuriken.wav"), g_fVolume + 0.2f);
	m_pPlayer->Get_EffectSoundCom()->Play3D(TEXT("Effect_Ninjutsu.wav"), g_fVolume + 0.2f);

	return S_OK;
}

void CState_Custom_RasenShuriken::Update(_float fTimeDelta)
{
	_uint iFrame = m_pPlayer->Get_Frame(m_iAnimation_RasenShuriken);

	// 이 프레임 사이에서는 적을 따라서 회전함
	if (47 < iFrame && iFrame < m_iFrame_Shoot - 5)
	{
		Chase_Target(fTimeDelta);
	}

	if (m_iFrame_Create < iFrame && iFrame < m_iFrame_Shoot)
	{
		dynamic_cast<CPlayer_Custom*>(m_pPlayer)->Create_RasenShuriken();

	}
	else if (iFrame >= m_iFrame_Shoot && !m_isShoot)
	{
		dynamic_cast<CPlayer_Custom*>(m_pPlayer)->Shoot_RasenShuriken();

		m_isShoot = true;
	}

	if (m_pPlayer->Get_IsEndAnimation(m_iAnimation_RasenShuriken))
		m_pPlayer->Change_State(CPlayer_Custom::IDLE);
}

void CState_Custom_RasenShuriken::End_State()
{
	m_pPlayer->Set_IsInvincible(false);
	//dynamic_cast<CPlayer_Custom*>(m_pPlayer)->Set_IsSenninEye(false);
}

void CState_Custom_RasenShuriken::Chase_Target(_float fTimeDelta)
{
	CGameObject* pTarget = m_pPlayer->Get_NearTarget();

	if (nullptr != pTarget)
	{
		_vector vTargetPos = pTarget->Get_Position();
		_vector vPlayerPos = m_pPlayer->Get_Position();
		_vector vChaseDir = vTargetPos - vPlayerPos;

		m_pPlayer->Get_Transform()->LookAt(vTargetPos);
	}
}

CState_Custom_RasenShuriken* CState_Custom_RasenShuriken::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Custom_RasenShuriken* pInstance = new CState_Custom_RasenShuriken(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Custom_RasenShuriken"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Custom_RasenShuriken::Free()
{
	__super::Free();
}
