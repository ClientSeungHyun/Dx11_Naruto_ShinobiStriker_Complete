#include "stdafx.h"
#include "State_Custom_RisingKick.h"
#include "GameInstance.h"
#include "Player_Custom.h"
#include "Model.h"
#include "Collider_PlayerAttack.h"
#include "PlayerCamera.h"

CState_Custom_RisingKick::CState_Custom_RisingKick(CFsm* pFsm, CPlayer* pPlayer)
	:CState{ pFsm }
	, m_pPlayer{ pPlayer }
{
}

HRESULT CState_Custom_RisingKick::Initialize(_uint iStateNum)
{
	m_iAnimation_Rising = m_pPlayer->Get_Model()->Get_AnimationIndex("D34_Attack_Aerial_RisingKick", 2.5);
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Custom_RisingKick::Start_State(void* pArg)
{
	m_pPlayer->SetUp_NextAnimation(m_iAnimation_Rising, false, 0.f);
	m_pPlayer->Find_NearTarget();
	m_pPlayer->Set_IsJump(true);

	m_isChase = false;
	m_isSound = false;
	m_isRising = false;

	return S_OK;
}

void CState_Custom_RisingKick::Update(_float fTimeDelta)
{
	_int iFrame = m_pPlayer->Get_Frame(m_iAnimation_Rising);

	if (14 <= iFrame && iFrame <= 18)
	{
		if (!m_isSound)
		{
			m_pPlayer->Get_SoundCom()->Play3D(TEXT("Voice_Attack2.wav"), g_fVolume + 0.2f);
			m_isSound = true;
		}

		
		m_pPlayer->Chase_Target(fTimeDelta, 8.f);
		m_isChase = true;
	}

	if (iFrame >= 50 && iFrame <= 55)
	{
		m_pPlayer->Change_State(CPlayer_Custom::HANDDOUBLEPUNCH);
		return;
	}
	Attack_Collider();
}

void CState_Custom_RisingKick::End_State()
{
	//m_pPlayer->Get_Camera()->Start_CameraLerp(0.3f);
	m_pPlayer->Get_RigidBody()->Clear_All();
	m_pPlayer->Get_AttackCollider()->IsActive(false);
}

void CState_Custom_RisingKick::Attack_Collider()
{
	_uint iFrame = m_pPlayer->Get_Frame(m_iAnimation_Rising);


	if (iFrame == m_iAttackFrame || iFrame == m_iAttackFrame + 1)
	{
		if (!m_isRising)
		{

			_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
			_vector vLook = XMVector3Normalize(m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_LOOK));

			m_pPlayer->Get_RigidBody()->Clear_All();
			m_pPlayer->Get_RigidBody()->Add_Force(CRigidBody::FORCE, vLook * 3.f);
			m_pPlayer->Get_RigidBody()->Add_Force(CRigidBody::FORCE, vUp * 8.f);

			m_isRising = true;
		}

		m_pPlayer->Get_AttackCollider()->IsActive(true);
	}
	else
	{
		m_pPlayer->Get_AttackCollider()->IsActive(false);
	}

}

CState_Custom_RisingKick* CState_Custom_RisingKick::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Custom_RisingKick* pInstance = new CState_Custom_RisingKick(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Custom_RisingKick"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Custom_RisingKick::Free()
{
	__super::Free();
}
