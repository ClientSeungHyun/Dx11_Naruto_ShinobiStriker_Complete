#include "stdafx.h"
#include "State_Custom_MassiveRasengan.h"
#include "GameInstance.h"
#include "Player_Custom.h"
#include "Model.h"
#include "PlayerCamera.h"

#include "ObjectPool.h"
#include "Particle_Up.h"

CState_Custom_MassiveRasengan::CState_Custom_MassiveRasengan(CFsm* pFsm, CPlayer* pPlayer)
	:CState{ pFsm }
	, m_pPlayer{ pPlayer }
{
}

HRESULT CState_Custom_MassiveRasengan::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;

	m_iAnimation_MassiveRasengan = m_pPlayer->Get_Model()->Get_AnimationIndex("CustomMan_Ninjutsu_MassiveRasengan", 0.7);


	return S_OK;
}

HRESULT CState_Custom_MassiveRasengan::Start_State(void* pArg)
{
	m_pPlayer->SetUp_NextAnimation(m_iAnimation_MassiveRasengan, false, 0.1f);

	m_pPlayer->Set_IsGravity(false);
	m_pPlayer->Set_IsInvincible(true);
	m_pPlayer->Set_IsSkill(true);

	m_pPlayer->Get_RigidBody()->Set_IsGravity(false);
	m_pPlayer->Get_RigidBody()->Clear_All();

	m_pPlayer->Find_NearTarget();

	Setting_TargetPos();

	m_pPlayerCamera = dynamic_cast<CPlayerCamera*>(m_pGameInstance->Find_Camera(m_pPlayer->Get_LevelIndex()));

	m_fMoveTime = 0.f;
	m_isShoot = false;
	m_isCameraMove1 = false;
	m_isCameraMove2 = false;


	m_pPlayer->Get_SoundCom()->Play3D(TEXT("Voice_MassiveRasengan.wav"), g_fVolume + 0.2f);
	m_pPlayer->Get_EffectSoundCom()->Play3D(TEXT("Effect_Ninjutsu.wav"), g_fVolume + 0.2f);

	return S_OK;
}

void CState_Custom_MassiveRasengan::Update(_float fTimeDelta)
{
	_uint iFrame = m_pPlayer->Get_Frame(m_iAnimation_MassiveRasengan);

	Control_Camera(fTimeDelta);

	//// 이 프레임 사이에서는 적을 따라서 회전함
	//if (45 < iFrame && iFrame < m_iFrame_Shoot - 5)
	//{
	//	Chase_Target(fTimeDelta);
	//}

	if (m_iFrame_Create < iFrame && iFrame < m_iFrame_Create + 2)
	{
		dynamic_cast<CPlayer_Custom*>(m_pPlayer)->Create_MassiveRasengan();
		Create_Shadow();
	}
	else if (iFrame >= m_iFrame_Shoot && iFrame <= m_iFrame_Shoot + 5 &&!m_isShoot)
	{
		dynamic_cast<CPlayer_Custom*>(m_pPlayer)->Shoot_MassiveRasengan();

		m_isShoot = true;
	}

	if (m_pPlayer->Get_IsEndAnimation(m_iAnimation_MassiveRasengan))
		m_pPlayer->Change_State(CPlayer_Custom::FALL_FRONT);
}

void CState_Custom_MassiveRasengan::End_State()
{
	m_pPlayer->Set_IsInvincible(false);
	m_pPlayerCamera->End_CameraControl();
	//m_pGameInstance->Stop_Sound(SOUND_SKILL);
	m_pPlayer->Set_IsSkill(false);
	//dynamic_cast<CPlayer_Naruto*>(m_pPlayer)->Set_IsSenninEye(false);
}

void CState_Custom_MassiveRasengan::Control_Camera(_float fTimeDelta)
{
	_uint iFrame = m_pPlayer->Get_Frame(m_iAnimation_MassiveRasengan);

	_matrix PlayerWorldMatrix = m_pPlayer->Get_Transform()->Get_WorldMatrix();

	_vector vPlayerRight = XMVector3Normalize(PlayerWorldMatrix.r[0]);
	_vector vPlayerUp = XMVector3Normalize(PlayerWorldMatrix.r[1]);
	_vector vPlayerLook = XMVector3Normalize(PlayerWorldMatrix.r[2]);
	_vector vPlayerPos = PlayerWorldMatrix.r[3];

	_vector vInitPos = XMLoadFloat3(&m_vInitPos);
	_vector vTargetPos = XMLoadFloat3(&m_vTargetPos);

	_vector vCameraInitPos = vInitPos + (vPlayerUp * 1.8f) + (vPlayerLook * 1.f) + (vPlayerRight) * 0.4f;
	_vector vCameraTargetPos = vInitPos + (vPlayerUp * 2.3f) + (vPlayerLook * 1.f) + (vPlayerRight) * 0.4f;

	if (iFrame >= 8 && iFrame <= 15)
	{
		m_fMoveTime += 3.5f * fTimeDelta;
		if (m_fMoveTime >= 1.f)
		{
			_vector vPos = m_pPlayer->Get_Position() + m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_UP);
			CObjectPool<CParticle_Up>::Get_GameObject()->Appear(vPos, XMVectorSet(0.05f, 0.05f, 0.9f, 0.5f), CParticle_Up::PARTICLE_UP);
			m_isCameraMove1 = true;
			m_fMoveTime = 1.f;
		}
		vPlayerPos = XMVectorLerp(vInitPos, vTargetPos, m_fMoveTime / 1.5f);
		m_pPlayer->Get_Transform()->Set_State(CTransform::STATE_POSITION, vPlayerPos);
	}

	if (0 <= iFrame && iFrame <= 20 && !m_isCameraMove1)
	{
		_vector vCameraPos = XMVectorLerp(vCameraInitPos, vCameraTargetPos, m_fMoveTime);

		m_pPlayerCamera->Setting_CameraControl(vCameraPos, vCameraPos, vPlayerPos + vPlayerUp * 1.8f);
		m_pPlayerCamera->Start_CameraControl();
	}
	else if(m_isCameraMove1 && !m_isCameraMove2 &&iFrame > 20 && iFrame <= 60)
	{
		_vector vCameraPos = vCameraTargetPos;
		_vector vTargetCamerPos = vPlayerPos + (vPlayerUp * 1.5f) + (vPlayerLook * 5.5f) + (vPlayerRight) * 0.4f;;

		m_pPlayerCamera->Setting_CameraControl(vCameraPos, vTargetCamerPos, vPlayerPos + vPlayerUp * 1.8f);
		m_pPlayerCamera->Start_CameraControl();
		m_pPlayerCamera->Start_CameraLerp(0.2f);

		m_isCameraMove2 = true;
	}
	else if(iFrame >= 61 && iFrame < 70 && m_isCameraMove2)
	{
		_vector vCameraPos = vPlayerPos + (vPlayerUp * 5.5f) + (vPlayerLook * -1.5f);

		m_pPlayerCamera->Setting_CameraControl(vCameraPos, vCameraPos, vPlayerPos + vPlayerUp * 0.4f);
	}

}

void CState_Custom_MassiveRasengan::Chase_Target(_float fTimeDelta)
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

void CState_Custom_MassiveRasengan::Setting_TargetPos()
{
	_vector vPos = m_pPlayer->Get_Position();
	XMStoreFloat3(&m_vInitPos, vPos);

	_vector vUp = XMVector3Normalize(m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_UP));
	vPos += vUp * 3.2f;
	XMStoreFloat3(&m_vTargetPos, vPos);
}

void CState_Custom_MassiveRasengan::Create_Shadow()
{
	CShadow_Custom::SHADOW_DESC tDesc = {};
	tDesc.fRotationPerSec = 0.f;
	tDesc.fSpeedPerSec = 0.f;
	tDesc.eSkill = CShadow_Custom::MASSIVERASENGAN;

	dynamic_cast<CPlayer_Custom*>(m_pPlayer)->Create_Shadow(tDesc);
	//dynamic_cast<CPlayer_Naruto*>(m_pPlayer)->Set_IsSenninEye(true);
}

CState_Custom_MassiveRasengan* CState_Custom_MassiveRasengan::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Custom_MassiveRasengan* pInstance = new CState_Custom_MassiveRasengan(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Naruto_Idle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Custom_MassiveRasengan::Free()
{
	__super::Free();
}
