#include "stdafx.h"
#include "State_Custom_Susanoo.h"
#include "GameInstance.h"
#include "Player_Custom.h"
#include "Model.h"
#include "PlayerCamera.h"
#include "Skill_Susanoo.h"
#include "ObjectPool.h"
#include "Particle_Up.h"

CState_Custom_Susanoo::CState_Custom_Susanoo(CFsm* pFsm, CPlayer* pPlayer)
	:CState{ pFsm }
	, m_pPlayer{ pPlayer }
{
}

HRESULT CState_Custom_Susanoo::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;

	m_iAnimation_SusanooStart = m_pPlayer->Get_Model()->Get_AnimationIndex("D23_Ninjutsu_D23NJ3_Start", 1.5f);
	m_iAnimation_SusanooLoop = m_pPlayer->Get_Model()->Get_AnimationIndex("D23_Ninjutsu_D23NJ3_Loop", 1.5f);
	m_iAnimation_SusanooEnd = m_pPlayer->Get_Model()->Get_AnimationIndex("D23_Ninjutsu_D23NJ3_End", 1.3f);

	return S_OK;
}

HRESULT CState_Custom_Susanoo::Start_State(void* pArg)
{
	m_pPlayer->SetUp_NextAnimation(m_iAnimation_SusanooStart, false, 0.1f);

	m_pPlayer->Set_IsInvincible(true);
	m_pPlayer->Set_IsSkill(true);

	m_isCameraControl = false;

	m_pPlayerCamera = dynamic_cast<CPlayerCamera*>(m_pGameInstance->Find_Camera(m_pPlayer->Get_LevelIndex()));

	Control_Camera();

	_vector vPos = m_pPlayer->Get_Position();
	_vector vUp = m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_UP);

	m_pPlayer->Get_Transform()->Set_State(CTransform::STATE_POSITION, vPos + vUp);

	m_pPlayer->Find_NearTarget();
	if (nullptr != m_pPlayer->Get_NearTarget())
		m_pPlayer->Get_Transform()->LookAt_Forward(m_pPlayer->Get_NearTarget()->Get_Position());

	m_fLoopTime = 0.f;
	m_fLoopDuration = 1.3f;

	m_pSusanoo = dynamic_cast<CPlayer_Custom*>(m_pPlayer)->Get_Susanoo();
	m_pSusanoo->Appear();

	vPos = m_pPlayer->Get_Position() + m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_UP);
	CObjectPool<CParticle_Up>::Get_GameObject()->Appear(vPos, XMVectorSet(0.05f, 0.05f, 0.9f, 0.5f), CParticle_Up::PARTICLE_UP);

	_uint iRandVoice = rand() % 2;
	switch (iRandVoice)
	{
	case 0:
		m_pPlayer->Get_SoundCom()->Play3D(TEXT("Voice_Susanoo0.wav"), g_fVolume + 0.2f);
		break;
	default:
		m_pPlayer->Get_SoundCom()->Play3D(TEXT("Voice_Susanoo1.wav"), g_fVolume + 0.2f);
		break;
	}
	m_pPlayer->Get_EffectSoundCom()->Play3D(TEXT("Effect_Ninjutsu.wav"), g_fVolume + 0.2f);

	return S_OK;
}

void CState_Custom_Susanoo::Update(_float fTimeDelta)
{
	_uint iCurrentAnimation = m_pPlayer->Get_CurrentAnimationIndex();
	_uint iFrame = m_pPlayer->Get_Frame(iCurrentAnimation);

	if (iCurrentAnimation == m_iAnimation_SusanooStart )
	{
		if(iFrame == 30)
			m_pPlayer->Change_IsRender(false);

		if (m_pPlayer->Get_IsEndAnimation(m_iAnimation_SusanooStart))
		{
			m_pSusanoo->Change_Animation(CSkill_Susanoo::SKIL_LOOP);
			m_pPlayer->SetUp_NextAnimation(m_iAnimation_SusanooLoop, true, 0.f);
		}
	}
	else if (iCurrentAnimation == m_iAnimation_SusanooLoop)
	{
		m_fLoopTime += fTimeDelta;
		if (m_fLoopTime >= m_fLoopDuration)
		{
			m_pPlayer->Change_IsRender(true);
			m_pSusanoo->Change_Animation(CSkill_Susanoo::SKILL_END);
			m_pPlayer->SetUp_NextAnimation(m_iAnimation_SusanooEnd, false, 0.f);
		}

		m_pPlayer->Get_Transform()->Go_Straight(fTimeDelta, 20.f);
	}
	else if (iCurrentAnimation == m_iAnimation_SusanooEnd && m_pPlayer->Get_IsEndAnimation(m_iAnimation_SusanooEnd))
	{	
		m_pPlayer->Change_State(CPlayer::FALL_FRONT);
	}
}

void CState_Custom_Susanoo::End_State()
{
	m_pPlayer->Set_IsSkill(false);
	m_pPlayer->Change_IsRender(true);
	m_pPlayerCamera->End_CameraControl();
	m_pPlayer->Set_IsInvincible(false);
}

void CState_Custom_Susanoo::Control_Camera()
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

void CState_Custom_Susanoo::Create_HitEffect()
{
	//_float3 vPos = _float3(m_vInitWorldMatrix._41, m_vInitWorldMatrix._42, m_vInitWorldMatrix._43);
	//_float3 vRootBonePos = m_pPlayer->Get_RootBonePos();

	//vPos.x = vPos.x + vRootBonePos.x;
	//vPos.y = vPos.y + vRootBonePos.y + 1.f;
	//vPos.z = vPos.z + vRootBonePos.z;
	//CObjectPool<CExplosionCircle>::Get_GameObject()->Appear(vPos, _float4(1.f, 0.9f, 0.f, 0.9f), 2.f, 7.f, 25.f, 0.04f);
	//CObjectPool<CExplosionCircle>::Get_GameObject()->Appear(vPos, _float4(1.f, 0.9f, 0.f, 0.9f), 1.f, 5.f, 15.f, 0.07f);
}

void CState_Custom_Susanoo::Setting_TargetPos()
{
	CMonster* pMonster = dynamic_cast<CMonster*>(m_pPlayer->Get_KuaniHitObject_One());

	_vector vPos = pMonster->Get_Transform()->Get_State(CTransform::STATE_POSITION) + XMVector3Normalize(pMonster->Get_Transform()->Get_State(CTransform::STATE_UP)) * 1.2f;

	XMStoreFloat3(&m_vTargetPos, vPos);
}

CState_Custom_Susanoo* CState_Custom_Susanoo::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Custom_Susanoo* pInstance = new CState_Custom_Susanoo(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Custom_Susanoo"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Custom_Susanoo::Free()
{
	__super::Free();
}
