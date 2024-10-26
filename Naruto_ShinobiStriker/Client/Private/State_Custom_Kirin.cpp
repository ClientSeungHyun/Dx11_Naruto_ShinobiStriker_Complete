#include "stdafx.h"
#include "State_Custom_Kirin.h"
#include "GameInstance.h"
#include "Player_Custom.h"
#include "Model.h"
#include "PlayerCamera.h"
#include "ObjectPool.h"
#include "Particle_Up.h"

CState_Custom_Kirin::CState_Custom_Kirin(CFsm* pFsm, CPlayer* pPlayer)
	:CState{ pFsm }
	, m_pPlayer{ pPlayer }
{
}

HRESULT CState_Custom_Kirin::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;

	m_iAnimation_Kirin = m_pPlayer->Get_Model()->Get_AnimationIndex("CustomMan_Ninjutsu_Kirin", 2.3);

	return S_OK;
}

HRESULT CState_Custom_Kirin::Start_State(void* pArg)
{
	m_pPlayer->SetUp_NextAnimation(m_iAnimation_Kirin, false, 0.2f);

	m_pPlayer->Set_IsInvincible(true);

	m_pPlayer->Find_NearTarget();
	CGameObject* pTarget = m_pPlayer->Get_NearTarget();

	_matrix PlayerWorldMatrix = m_pPlayer->Get_Transform()->Get_WorldMatrix();

	_vector vPlayerPos = m_pPlayer->Get_Position();
	_vector vPlayerRight = XMVector3Normalize(PlayerWorldMatrix.r[0]);
	_vector vPlayerUp = XMVector3Normalize(PlayerWorldMatrix.r[1]);
	_vector vPlayerLook = XMVector3Normalize(PlayerWorldMatrix.r[2]);

	if (nullptr != pTarget)
	{
		_vector vTargetPos = pTarget->Get_Position();

		_float fDistance = XMVectorGetX(XMVector3Length(vPlayerPos - vTargetPos));
		if (fDistance > 18.f)
		{
			XMStoreFloat3(&m_vKirinPos, vPlayerPos + vPlayerLook * 18.f);
		}
		else
		{
			XMStoreFloat3(&m_vKirinPos, vTargetPos);
		}
	}
	else
	{
		XMStoreFloat3(&m_vKirinPos, vPlayerPos + vPlayerLook * 5.f);
	}
	dynamic_cast<CPlayer_Custom*>(m_pPlayer)->Create_Kirin(XMLoadFloat3(&m_vKirinPos));
	m_pPlayer->Get_Transform()->LookAt_Forward(XMLoadFloat3(&m_vKirinPos));

	PlayerWorldMatrix = m_pPlayer->Get_Transform()->Get_WorldMatrix();
	vPlayerRight = XMVector3Normalize(PlayerWorldMatrix.r[0]);
	vPlayerUp = XMVector3Normalize(PlayerWorldMatrix.r[1]);
	vPlayerLook = XMVector3Normalize(PlayerWorldMatrix.r[2]);

	m_pPlayerCamera = dynamic_cast<CPlayerCamera*>(m_pGameInstance->Find_Camera(m_pPlayer->Get_LevelIndex()));

	_vector vCameraStartPos1 = vPlayerPos + vPlayerUp + (vPlayerLook * 0.5f) + (vPlayerRight);
	_vector vCameraEndPos1 = vPlayerPos + vPlayerUp + (vPlayerLook * 1.f) + (vPlayerRight);

	_vector vCameraPos2 = vPlayerPos + (vPlayerUp * 1.5f) - (vPlayerLook * 3.5f) + (vPlayerRight);
	
	XMStoreFloat3(&m_vCameraStartPos1, vCameraStartPos1);
	XMStoreFloat3(&m_vCameraEndPos1, vCameraEndPos1);

	XMStoreFloat3(&m_vCameraPos2, vCameraPos2);
	XMStoreFloat3(&m_vCameraAt1, vPlayerPos + vPlayerUp * 1.5f);
	XMStoreFloat3(&m_vCameraAt2, vPlayerPos + vPlayerUp * 4.f);

	m_isCameraMove1 = false;
	m_isCameraMove2 = false;

	_vector vPos = m_pPlayer->Get_Position() + m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_UP);
	CObjectPool<CParticle_Up>::Get_GameObject()->Appear(vPos, XMVectorSet(0.05f, 0.05f, 0.9f, 0.5f), CParticle_Up::PARTICLE_UP);

	m_pPlayer->Get_SoundCom()->Play3D(TEXT("Voice_Kirin.wav"), g_fVolume + 0.2f);
	m_pPlayer->Get_EffectSoundCom()->Play3D(TEXT("Effect_Ninjutsu.wav"), g_fVolume + 0.2f);

	return S_OK;
}

void CState_Custom_Kirin::Update(_float fTimeDelta)
{
	_uint iFrame = m_pPlayer->Get_Frame(m_iAnimation_Kirin);

	if (m_pPlayer->Get_IsEndAnimation(m_iAnimation_Kirin))
		m_pFsm->Change_State(CPlayer_Custom::IDLE);

	if (m_iCameraFrame1 >= iFrame && !m_isCameraMove1)
	{
		m_pPlayerCamera->Setting_CameraControl(m_vCameraStartPos1, m_vCameraEndPos1, m_vCameraAt1);
		m_pPlayerCamera->Start_CameraControl();
		m_pPlayerCamera->Start_CameraLerp(2.5f);
		m_isCameraMove1 = true;
	}
	else if (m_iCameraFrame2 >= iFrame && m_iCameraFrame1 < iFrame && !m_isCameraMove2)
	{
		m_pPlayerCamera->Setting_CameraControl(m_vCameraPos2, m_vCameraPos2, m_vCameraAt2, 1.5f);
		m_isCameraMove2 = true;
	}
	else if (iFrame >= 110 && iFrame < 120)
	{
		m_vCameraAt2.y -= 15.f * fTimeDelta;
		m_pPlayerCamera->Setting_CameraControl(m_vCameraPos2, m_vCameraPos2, m_vCameraAt2, 1.5f);
	}
}

void CState_Custom_Kirin::End_State()
{
	m_pPlayerCamera->End_CameraControl();
	//m_pGameInstance->Stop_Sound(SOUND_SKILL);
}

CState_Custom_Kirin* CState_Custom_Kirin::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Custom_Kirin* pInstance = new CState_Custom_Kirin(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Custom_FireBall"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Custom_Kirin::Free()
{
	__super::Free();
}
