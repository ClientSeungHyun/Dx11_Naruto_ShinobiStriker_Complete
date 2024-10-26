#include "stdafx.h"
#include "State_Custom_TypeZeroAttack.h"
#include "GameInstance.h"
#include "Player_Custom.h"
#include "Model.h"
#include "PlayerCamera.h"
#include "ObjectPool.h"
#include "ExplosionCircle.h"
#include "Collider_PlayerAttack.h"
#include "Particle_Up.h"

CState_Custom_TypeZeroAttack::CState_Custom_TypeZeroAttack(CFsm* pFsm, CPlayer* pPlayer)
	:CState{ pFsm }
	, m_pPlayer{ pPlayer }
{
}

HRESULT CState_Custom_TypeZeroAttack::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;

	m_iAnimation_TypeZero = m_pPlayer->Get_Model()->Get_AnimationIndex("CustomMan_Ninjutsu_D61NJ1_Attack", 1.8f);

	m_iEffetFrames[0] = 0;
	m_iEffetFrames[1] = 5;
	m_iEffetFrames[2] = 17;
	m_iEffetFrames[3] = 26;
	m_iEffetFrames[4] = 34;
	m_iEffetFrames[5] = 40;
	m_iEffetFrames[6] = 50;
	m_iEffetFrames[7] = 58;
	m_iEffetFrames[8] = 64;
	m_iEffetFrames[9] = 72;

	return S_OK;
}

HRESULT CState_Custom_TypeZeroAttack::Start_State(void* pArg)
{
	m_pPlayer->SetUp_NextAnimation(m_iAnimation_TypeZero, false, 0.1f);

	m_pPlayer->Set_IsInvincible(true);

	for (_uint i = 1; i < 10; ++i)
	{
		m_isActiveEffects[i] = false;
	}
	m_isCameraControl = false;

	m_pPlayerCamera = dynamic_cast<CPlayerCamera*>(m_pGameInstance->Find_Camera(m_pPlayer->Get_LevelIndex()));

	XMStoreFloat4x4(&m_vInitWorldMatrix, m_pPlayer->Get_Transform()->Get_WorldMatrix());

	dynamic_cast<CPlayer_Custom*>(m_pPlayer)->Create_EffectTypeZero(0, m_vInitWorldMatrix);

	Control_Camera();

	_vector vPos = m_pPlayer->Get_Position();
	_vector vLook = m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_LOOK);

	_vector vColliderPos = vPos - vLook * 5.f;

	_float3 vOffsetPos;
	XMStoreFloat3(&vOffsetPos, vColliderPos - vPos );
	m_pPlayer->Scaled_AttackCollider(7.f, 3.f, 7.f);
	m_pPlayer->Offset_AttackCollider(vOffsetPos.x, vOffsetPos.y, vOffsetPos.z);

	_uint iRandVoice = rand() % 2;
	switch (iRandVoice)
	{
	case 0 :
		m_pPlayer->Get_SoundCom()->Play3D(TEXT("Voice_TypeZero0.wav"), g_fVolume + 0.2f);
		break;
	default:
		m_pPlayer->Get_SoundCom()->Play3D(TEXT("Voice_TypeZero1.wav"), g_fVolume + 0.2f);
		break;
	}
	m_pPlayer->Get_EffectSoundCom()->Play3D(TEXT("Effect_Ninjutsu.wav"), g_fVolume + 0.2f);


	vPos = m_pPlayer->Get_Position() + m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_UP);
	CObjectPool<CParticle_Up>::Get_GameObject()->Appear(vPos, XMVectorSet(0.05f, 0.05f, 0.9f, 0.5f), CParticle_Up::PARTICLE_UP);


	m_pPlayer->On_MotionTrail();

	return S_OK;
}

void CState_Custom_TypeZeroAttack::Update(_float fTimeDelta)
{
	_uint iFrame = m_pPlayer->Get_Frame(m_iAnimation_TypeZero);

	if (m_isActiveEffects[1] && iFrame == 15 && !m_isCameraControl)
	{
		m_pPlayerCamera->Start_CameraLerp(5.f);
		m_isCameraControl = true;
	}

	if (m_pPlayer->Get_IsEndAnimation(m_iAnimation_TypeZero))
		m_pPlayer->Change_State(CPlayer_Custom::IDLE);

	if (iFrame == 81)
	{
		m_pPlayer->Get_AttackCollider()->IsActive(true);
		Create_HitEffect();
	}
	else
	{
		m_pPlayer->Get_AttackCollider()->IsActive(false);
	}
	if (iFrame == 83)
	{
		m_pPlayer->Scaled_AttackCollider(1.f, 1.f, 1.f);
	}

	for (_uint i = 1; i < 10; ++i)
	{
		if (iFrame == m_iEffetFrames[i] && !m_isActiveEffects[i])
		{
			Create_HitEffect();
			m_pPlayer->Get_EffectSoundCom()->Play3D(TEXT("TypeZeroHit.wav"), g_fVolume);

			m_pPlayer->Get_AttackCollider()->IsActive(true);
			dynamic_cast<CPlayer_Custom*>(m_pPlayer)->Create_EffectTypeZero(i, m_vInitWorldMatrix);
			m_isActiveEffects[i] = true;
		}

	}

	if (iFrame >= 73)
		m_pPlayer->Set_IsStrongAttack(true);
	else
		m_pPlayer->Set_IsStrongAttack(false);
}

void CState_Custom_TypeZeroAttack::End_State()
{
	m_pPlayer->Off_Motiontrail();
	m_pPlayer->Scaled_AttackCollider(1.f, 1.f, 1.f);
	m_pPlayer->Offset_AttackCollider(0.f, 0.f, 0.f);
	m_pPlayer->Get_AttackCollider()->IsActive(false);

	m_pPlayerCamera->End_CameraControl();
	m_pPlayer->Set_IsStrongAttack(false);
	m_pPlayer->Set_IsInvincible(false);
}

void CState_Custom_TypeZeroAttack::Control_Camera()
{
	_uint iFrame = m_pPlayer->Get_Frame(m_iAnimation_TypeZero);

	_matrix PlayerWorldMatrix = XMLoadFloat4x4(&m_vInitWorldMatrix);

	_vector vPlayerRight = XMVector3Normalize(PlayerWorldMatrix.r[0]);
	_vector vPlayerUp = XMVector3Normalize(PlayerWorldMatrix.r[1]);
	_vector vPlayerLook = XMVector3Normalize(PlayerWorldMatrix.r[2]);
	_vector vPlayerPos = PlayerWorldMatrix.r[3];

	_vector vCameraInitPos = vPlayerPos + (vPlayerUp * 1.f) - (vPlayerLook * 8.f) - (vPlayerRight) * 0.4f;
	_vector vCameraTargetPos = vPlayerPos + (vPlayerUp * 1.f) - (vPlayerLook * 7.8f) - (vPlayerRight) * 0.7f;

	m_pPlayerCamera->Setting_CameraControl(vCameraInitPos, vCameraTargetPos, vPlayerPos + vPlayerUp);
	m_pPlayerCamera->Start_CameraControl();
	m_pPlayerCamera->Start_CameraLerp(5.f);
}

void CState_Custom_TypeZeroAttack::Create_HitEffect()
{
	_float3 vPos = _float3(m_vInitWorldMatrix._41, m_vInitWorldMatrix._42, m_vInitWorldMatrix._43);
	_float3 vRootBonePos = m_pPlayer->Get_RootBonePos();

	vPos.x = vPos.x + vRootBonePos.x;
	vPos.y = vPos.y + vRootBonePos.y + 1.f;
	vPos.z = vPos.z + vRootBonePos.z;
	/*CObjectPool<CExplosionCircle>::Get_GameObject()->Appear(vPos, _float4(1.f, 0.9f, 0.f, 0.9f), 2.f, 7.f, 25.f, 0.04f);
	CObjectPool<CExplosionCircle>::Get_GameObject()->Appear(vPos, _float4(1.f, 0.9f, 0.f, 0.9f), 1.f, 5.f, 15.f, 0.07f);*/
}

CState_Custom_TypeZeroAttack* CState_Custom_TypeZeroAttack::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Custom_TypeZeroAttack* pInstance = new CState_Custom_TypeZeroAttack(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Custom_TypeZero"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Custom_TypeZeroAttack::Free()
{
	__super::Free();
}
