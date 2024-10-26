#include "stdafx.h"
#include "State_Custom_Rasenko.h"
#include "GameInstance.h"
#include "Player_Custom.h"
#include "Model.h"
#include "PlayerCamera.h"
#include "ObjectPool.h"
#include "Particle_Up.h"
#include "ExplosionCircle.h"
#include "Collider_PlayerAttack.h"

CState_Custom_Rasenko::CState_Custom_Rasenko(CFsm* pFsm, CPlayer* pPlayer)
	:CState{ pFsm }
	, m_pPlayer{ pPlayer }
{
}

HRESULT CState_Custom_Rasenko::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;

	m_iAnimation_Rasenko_Start = m_pPlayer->Get_Model()->Get_AnimationIndex("CustomMan_Ninjutsu_RasenSenkoCBS_Start", 1.8f);
	m_iAnimation_Rasenko_Loop = m_pPlayer->Get_Model()->Get_AnimationIndex("CustomMan_Ninjutsu_RasenSenkoCBS_Loop", 1.8f);
	m_iAnimation_Rasenko_End = m_pPlayer->Get_Model()->Get_AnimationIndex("CustomMan_Ninjutsu_RasenSenkoCBS_End", 1.0f);

	return S_OK;
}

HRESULT CState_Custom_Rasenko::Start_State(void* pArg)
{
	m_pPlayer->SetUp_NextAnimation(m_iAnimation_Rasenko_Start, false, 0.1f);

	m_pPlayer->Set_IsInvincible(true);

	m_fLoopTime = 0.f;

	m_isStartCamera = false;
	m_isSecondCamera = false;

	_vector vPos = m_pPlayer->Get_Position();
	_vector vLook = m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_LOOK);

	_vector vColliderPos = vPos - vLook * 3.f;

	_float3 vOffsetPos;
	XMStoreFloat3(&vOffsetPos, vColliderPos - vPos);

	m_pPlayer->Scaled_AttackCollider(7.f, 3.f, 3.f);
	m_pPlayer->Offset_AttackCollider(vOffsetPos.x, vOffsetPos.y, vOffsetPos.z);

	m_pPlayerCamera = dynamic_cast<CPlayerCamera*>(m_pGameInstance->Find_Camera(m_pPlayer->Get_LevelIndex()));

	m_fCurrentTime = 0.f;
	m_isCreateRasengan = false;
	m_isCreateStar = false;
	m_isMoveCenter = false;
	m_isShootKunai = false;
	m_isAppearKunai = false;

	vPos = m_pPlayer->Get_Position() + m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_UP);
	CObjectPool<CParticle_Up>::Get_GameObject()->Appear(vPos, XMVectorSet(0.05f, 0.05f, 0.9f, 0.5f), CParticle_Up::PARTICLE_UP);

	m_pPlayer->Get_SoundCom()->Play3D(TEXT("Voice_Rasenko.wav"), g_fVolume + 0.2f);
	m_pPlayer->Get_EffectSoundCom()->Play3D(TEXT("Effect_Ninjutsu.wav"), g_fVolume + 0.2f);

	return S_OK;
}

void CState_Custom_Rasenko::Update(_float fTimeDelta)
{
	_uint iCurrentAnimation = m_pPlayer->Get_CurrentAnimationIndex();
	_uint iFrame = m_pPlayer->Get_Frame(iCurrentAnimation);


	Control_Camera();

	m_pPlayer->Change_IsRender(true);
	if (iCurrentAnimation == m_iAnimation_Rasenko_Start)
	{
		if (m_pPlayer->Get_IsEndAnimation(m_iAnimation_Rasenko_Start))
		{
			if (!m_isMoveCenter)
			{
				m_pPlayer->Move_KunaiCenter();
				m_isMoveCenter = true;
			}

			dynamic_cast<CPlayer_Custom*>(m_pPlayer)->Disappear_Rasengan();
			m_pPlayer->SetUp_NextAnimation(m_iAnimation_Rasenko_Loop, true, 0.f);
		}

		if ((iFrame == 33 || iFrame == 34) && !m_isAppearKunai)
		{
			m_pPlayer->Appear_MinatoKunai();
			m_isAppearKunai = true;
		}
		else if ((iFrame == 35 || iFrame == 36) && !m_isShootKunai)
		{
			m_pPlayer->Shoot_MinatoKunai();
			m_isShootKunai = true;
		}
		else if ((iFrame == 54 || iFrame == 55) && !m_isCreateRasengan)
		{
			dynamic_cast<CPlayer_Custom*>(m_pPlayer)->Create_Rasengan();
			m_isCreateRasengan = true;
		}
	}
	else if (iCurrentAnimation == m_iAnimation_Rasenko_Loop)
	{
		m_pPlayer->Get_AttackCollider()->IsActive(true);
		m_fCurrentTime += fTimeDelta;
		if (m_fCurrentTime > 0.3f)
		{
			m_pPlayer->Get_AttackCollider()->IsActive(false);
			m_fCurrentTime = 0.f;
		}

		m_pPlayer->Change_IsRender(false);
		m_fLoopTime += fTimeDelta;
		if (m_fLoopTime >= 3.f)
		{
			m_pPlayer->Disappear_MinatoKunai();
			dynamic_cast<CPlayer_Custom*>(m_pPlayer)->Disappear_RasnkoStar();
			m_pPlayer->SetUp_NextAnimation(m_iAnimation_Rasenko_End, false, 0.f);
		}

		if (!m_isCreateStar)
		{
			dynamic_cast<CPlayer_Custom*>(m_pPlayer)->Create_RasnkoStar();
			m_isCreateStar = true;
		}
		m_pPlayer->Get_Transform()->Go_Straight(fTimeDelta, 5.f);
	}
	else if (iCurrentAnimation == m_iAnimation_Rasenko_End)
	{
		if (m_pPlayer->Get_IsEndAnimation(m_iAnimation_Rasenko_End))
			m_pPlayer->Change_State(CPlayer::IDLE);
	}
}

void CState_Custom_Rasenko::End_State()
{
	m_pPlayer->Scaled_AttackCollider(1.f, 1.f, 1.f);
	m_pPlayer->Offset_AttackCollider(0.f, 0.f, 0.f);
	m_pPlayer->Get_AttackCollider()->IsActive(false);

	m_pPlayer->Change_IsRender(true);
	m_pPlayerCamera->End_CameraControl();
	m_pPlayer->Set_IsInvincible(false);
}

void CState_Custom_Rasenko::Control_Camera()
{
	_uint iCurrentAnimation = m_pPlayer->Get_CurrentAnimationIndex();

	_matrix PlayerWorldMatrix = m_pPlayer->Get_Transform()->Get_WorldMatrix();

	_vector vPlayerRight = XMVector3Normalize(PlayerWorldMatrix.r[0]);
	_vector vPlayerUp = XMVector3Normalize(PlayerWorldMatrix.r[1]);
	_vector vPlayerLook = XMVector3Normalize(PlayerWorldMatrix.r[2]);
	_vector vPlayerPos = PlayerWorldMatrix.r[3];

	if (iCurrentAnimation == m_iAnimation_Rasenko_Start && !m_isStartCamera)
	{
		_vector vCameraInitPos = vPlayerPos + (vPlayerUp * 1.f) - (vPlayerLook * 1.f) - (vPlayerRight) * 0.4f;
		_vector vCameraTargetPos = vPlayerPos + (vPlayerUp * 1.f) - (vPlayerLook * 3.8f) - (vPlayerRight) * 0.4f;

		m_pPlayerCamera->Setting_CameraControl(vCameraInitPos, vCameraTargetPos, vPlayerPos + vPlayerUp);
		m_pPlayerCamera->Start_CameraControl();
		m_pPlayerCamera->Start_CameraLerp(1.f);

		_vector vPos = vPlayerPos + vPlayerUp;
		CObjectPool<CParticle_Up>::Get_GameObject()->Appear(vPos, XMVectorSet(0.05f, 0.05f, 0.9f, 0.5f), CParticle_Up::PARTICLE_SPRED);

		m_isStartCamera = true;
	}
	else if (iCurrentAnimation == m_iAnimation_Rasenko_Loop )
	{
		_vector vCameraPos = vPlayerPos + (vPlayerUp * 2.5f) - (vPlayerLook * 6.f);
		m_pPlayerCamera->Setting_CameraControl(vCameraPos, vCameraPos, vPlayerPos + (vPlayerUp * 1.5f));
	}
}

CState_Custom_Rasenko* CState_Custom_Rasenko::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Custom_Rasenko* pInstance = new CState_Custom_Rasenko(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Custom_Rasenko"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Custom_Rasenko::Free()
{
	__super::Free();
}
