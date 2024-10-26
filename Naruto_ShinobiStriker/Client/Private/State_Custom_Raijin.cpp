#include "stdafx.h"
#include "State_Custom_Raijin.h"
#include "GameInstance.h"
#include "Player_Custom.h"
#include "Model.h"
#include "PlayerCamera.h"
#include "ObjectPool.h"
#include "Particle_Up.h"
#include "ExplosionCircle.h"
#include "Effect_RasenkoHit.h"

#include "Collider_PlayerAttack.h"

CState_Custom_Raijin::CState_Custom_Raijin(CFsm* pFsm, CPlayer* pPlayer)
	:CState{ pFsm }
	, m_pPlayer{ pPlayer }
{
}

HRESULT CState_Custom_Raijin::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;

	m_iAnimation_RaijinStart = m_pPlayer->Get_Model()->Get_AnimationIndex("CustomMan_Ninjutsu_FlyingRaijinLv2_Start", 1.5f);
	m_iAnimation_RaijinHit = m_pPlayer->Get_Model()->Get_AnimationIndex("CustomMan_Ninjutsu_FlyingRaijinLv2_Hit", 1.5f);
	m_iAnimation_RaijinEnd = m_pPlayer->Get_Model()->Get_AnimationIndex("CustomMan_Ninjutsu_FlyingRaijinLv2_End", 1.f);

	return S_OK;
}

HRESULT CState_Custom_Raijin::Start_State(void* pArg)
{
	m_pPlayer->SetUp_NextAnimation(m_iAnimation_RaijinStart, false, 0.1f);

	m_pPlayer->Set_IsInvincible(true);

	m_isCameraControl = false;

	m_pPlayerCamera = dynamic_cast<CPlayerCamera*>(m_pGameInstance->Find_Camera(m_pPlayer->Get_LevelIndex()));

	Control_Camera();

	_vector vPos = m_pPlayer->Get_Position();
	_vector vLook = m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_LOOK);

	_vector vColliderPos = vPos - vLook * 5.f;

	_float3 vOffsetPos;
	XMStoreFloat3(&vOffsetPos, vColliderPos - vPos);
	m_pPlayer->Offset_AttackCollider(vOffsetPos.x, vOffsetPos.y, vOffsetPos.z);

	m_pPlayer->Find_NearTarget();
	if (nullptr != m_pPlayer->Get_NearTarget())
		m_pPlayer->Get_Transform()->LookAt_Forward(m_pPlayer->Get_NearTarget()->Get_Position());

	m_isCreateKunai = false;
	m_isBackJump = false;
	m_isMoveTarget = false;

	vPos = m_pPlayer->Get_Position() + m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_UP);
	CObjectPool<CParticle_Up>::Get_GameObject()->Appear(vPos, XMVectorSet(0.05f, 0.05f, 0.9f, 0.5f), CParticle_Up::PARTICLE_UP);

	return S_OK;
}

void CState_Custom_Raijin::Update(_float fTimeDelta)
{
	_uint iCurrentAnimation = m_pPlayer->Get_CurrentAnimationIndex();
	_uint iFrame = m_pPlayer->Get_Frame(iCurrentAnimation);

	if (iCurrentAnimation == m_iAnimation_RaijinStart)
	{
		if ((iFrame == 14 || iFrame == 15) && !m_isCreateKunai)
		{
			m_pPlayer->Appear_MinatoKunai_One();
			m_pPlayer->Shoot_MinatoKunai_One();
			m_isCreateKunai = true;
		}

		if (m_pPlayer->Get_IsKunaiCollision_One() && m_isCreateKunai)
		{
			Setting_TargetPos();
			m_pPlayer->SetUp_NextAnimation(m_iAnimation_RaijinHit, false, 0.f);
		}
		else if (m_pPlayer->Get_IsEndAnimation(m_iAnimation_RaijinStart))
		{
			//m_pPlayer->SetUp_NextAnimation(m_iAnimation_RaijinHit, false, 0.f);
			m_pPlayer->Change_State(CPlayer::IDLE);
		}
	}
	else if (iCurrentAnimation == m_iAnimation_RaijinHit)
	{
		if (!m_isMoveTarget)
		{
			m_pPlayer->Get_Transform()->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_vTargetPos));

			_vector vPos = m_pPlayer->Get_Position();
			_float3 vStorePos;
			XMStoreFloat3(&vStorePos, vPos + m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_LOOK) * m_pGameInstance->Get_Random(-0.8f, 0.8f));

			CObjectPool<CExplosionCircle>::Get_GameObject()->Appear(vStorePos, _float4(1.f, 0.9f, 0.f, 0.9f), 1.2f, 7.f, 3.f, 0.03f);
			CObjectPool<CExplosionCircle>::Get_GameObject()->Appear(vStorePos, _float4(1.f, 0.9f, 0.f, 0.9f), 0.1f, 5.f, 2.f, 0.06f);

			CObjectPool<CEffect_RasenkoHit>::Get_GameObject()->Appear(m_pPlayer->Get_Position());

			m_pPlayer->Get_SoundCom()->Play3D(TEXT("Voice_Raijin.wav"), g_fVolume + 0.2f);
			m_pPlayer->Get_EffectSoundCom()->Play3D(TEXT("Effect_Ninjutsu.wav"), g_fVolume + 0.2f);

			m_isMoveTarget = true;
		}

		if (iFrame == 4 || iFrame == 5)
		{
			dynamic_cast<CPlayer_Custom*>(m_pPlayer)->Create_Rasengan();
		}
		else if (m_pPlayer->Get_IsEndAnimation(m_iAnimation_RaijinHit))
		{
			_float3 vPos;
			XMStoreFloat3(&vPos, m_pPlayer->Get_Position());

			CObjectPool<CExplosionCircle>::Get_GameObject()->Appear(vPos, _float4(1.f, 0.9f, 0.f, 0.9f), 1.2f, 7.f, 3.f, 0.03f);
			CObjectPool<CExplosionCircle>::Get_GameObject()->Appear(vPos, _float4(1.f, 0.9f, 0.f, 0.9f), 0.1f, 5.f, 2.f, 0.06f);

			CObjectPool<CEffect_RasenkoHit>::Get_GameObject()->Appear(m_pPlayer->Get_Position());

			dynamic_cast<CPlayer_Custom*>(m_pPlayer)->Explosion_Rasengan();
			m_pPlayer->SetUp_NextAnimation(m_iAnimation_RaijinEnd, false, 0.f);
		}
	}
	else if (iCurrentAnimation == m_iAnimation_RaijinEnd)
	{
		if ((iFrame == 14 || iFrame == 15) && !m_isBackJump)
		{
			CRigidBody* pRigidBody = m_pPlayer->Get_RigidBody();
			_vector vDashDir = vDashDir = XMVector3Normalize(m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_LOOK)) * -1.f;

			m_pPlayer->Get_RigidBody()->Set_IsFriction(true);
			pRigidBody->Clear_All();
			pRigidBody->Add_Force(CRigidBody::FORCE, vDashDir * 15.f);

			m_isBackJump = true;
		}

		if (m_pPlayer->Get_IsEndAnimation(m_iAnimation_RaijinEnd))
		{
			m_pPlayer->Change_State(CPlayer::FALL_FRONT);
		}

	}
}

void CState_Custom_Raijin::End_State()
{
	m_pPlayer->Scaled_AttackCollider(1.f, 1.f, 1.f);
	m_pPlayer->Offset_AttackCollider(0.f, 0.f, 0.f);
	m_pPlayer->Get_AttackCollider()->IsActive(false);

	m_pPlayerCamera->End_CameraControl();
	m_pPlayer->Set_IsInvincible(false);
}

void CState_Custom_Raijin::Control_Camera()
{
	//_uint iFrame = m_pPlayer->Get_Frame(m_iAnimation_TypeZero);

	//_matrix PlayerWorldMatrix = XMLoadFloat4x4(&m_vInitWorldMatrix);

	//_vector vPlayerRight = XMVector3Normalize(PlayerWorldMatrix.r[0]);
	//_vector vPlayerUp = XMVector3Normalize(PlayerWorldMatrix.r[1]);
	//_vector vPlayerLook = XMVector3Normalize(PlayerWorldMatrix.r[2]);
	//_vector vPlayerPos = PlayerWorldMatrix.r[3];

	//_vector vCameraInitPos = vPlayerPos + (vPlayerUp * 1.f) - (vPlayerLook * 8.f) - (vPlayerRight) * 0.4f;
	//_vector vCameraTargetPos = vPlayerPos + (vPlayerUp * 1.f) - (vPlayerLook * 7.8f) - (vPlayerRight) * 0.7f;

	//m_pPlayerCamera->Setting_CameraControl(vCameraInitPos, vCameraTargetPos, vPlayerPos + vPlayerUp);
	//m_pPlayerCamera->Start_CameraControl();
	//m_pPlayerCamera->Start_CameraLerp(5.f);
}

void CState_Custom_Raijin::Create_HitEffect()
{
	//_float3 vPos = _float3(m_vInitWorldMatrix._41, m_vInitWorldMatrix._42, m_vInitWorldMatrix._43);
	//_float3 vRootBonePos = m_pPlayer->Get_RootBonePos();

	//vPos.x = vPos.x + vRootBonePos.x;
	//vPos.y = vPos.y + vRootBonePos.y + 1.f;
	//vPos.z = vPos.z + vRootBonePos.z;
	//CObjectPool<CExplosionCircle>::Get_GameObject()->Appear(vPos, _float4(1.f, 0.9f, 0.f, 0.9f), 2.f, 7.f, 25.f, 0.04f);
	//CObjectPool<CExplosionCircle>::Get_GameObject()->Appear(vPos, _float4(1.f, 0.9f, 0.f, 0.9f), 1.f, 5.f, 15.f, 0.07f);
}

void CState_Custom_Raijin::Setting_TargetPos()
{
	CMonster* pMonster = dynamic_cast<CMonster*>(m_pPlayer->Get_KuaniHitObject_One());

	_vector vPos = pMonster->Get_Transform()->Get_State(CTransform::STATE_POSITION) + XMVector3Normalize(pMonster->Get_Transform()->Get_State(CTransform::STATE_UP)) * 1.2f;

	XMStoreFloat3(&m_vTargetPos, vPos);
}

CState_Custom_Raijin* CState_Custom_Raijin::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Custom_Raijin* pInstance = new CState_Custom_Raijin(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Custom_Raijin"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Custom_Raijin::Free()
{
	__super::Free();
}
