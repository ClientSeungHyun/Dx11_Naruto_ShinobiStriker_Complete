#include "stdafx.h"
#include "State_Custom_HItSpin.h"
#include "GameInstance.h"
#include "Player_Custom.h"
#include "Model.h"


CState_Custom_HitSpin::CState_Custom_HitSpin(CFsm* pFsm, CPlayer* pPlayer)
	:CState{ pFsm }
	, m_pPlayer{ pPlayer }
{
}

HRESULT CState_Custom_HitSpin::Initialize(_uint iStateNum)
{
	m_iAnimation_Spin = m_pPlayer->Get_Model()->Get_AnimationIndex("CustomMan_Beaten_SpinBlowOff");
	m_iAnimation_FallBehind = m_pPlayer->Get_Model()->Get_AnimationIndex("CustomMan_Beaten_Fall_Behind_Loop");
	m_iAnimation_DownFloor = m_pPlayer->Get_Model()->Get_AnimationIndex("CustomMan_Beaten_DownToFloor");
	m_iAnimation_GetUpReady = m_pPlayer->Get_Model()->Get_AnimationIndex("CustomMan_Beaten_Down_Loop");
	m_iAnimation_GetUp = m_pPlayer->Get_Model()->Get_AnimationIndex("CustomMan_GetUp");

	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Custom_HitSpin::Start_State(void* pArg)
{
	m_pPlayer->SetUp_NextAnimation(m_iAnimation_Spin, false);

	m_pPlayer->Get_RigidBody()->Set_IsFriction(true);
	m_pPlayer->Set_IsFall(true);
	m_pPlayer->Set_IsJump(false);
	m_pPlayer->Set_IsDoubleJump(false);
	m_pPlayer->Set_IsGround(false);
	m_pPlayer->Set_IsWall(false);

	m_isFallEnd = false;


	if (nullptr != pArg)
	{
		_float2* vForceBackPower = static_cast<_float2*>(pArg);
		m_pPlayer->KnockBack(vForceBackPower->x, vForceBackPower->y);
	}
	else
	{
		m_pPlayer->KnockBack(7.f, 10.f);
	}

	m_pPlayer->Get_SoundCom()->Play3D(TEXT("Voice_FlyAway.wav"), g_fVolume + 0.2f);

	return S_OK;
}

void CState_Custom_HitSpin::Update(_float fTimeDelta)
{
	m_fCurrentTime += fTimeDelta;

	if(m_pPlayer->Get_CurrentAnimationIndex() == m_iAnimation_Spin)
		m_pPlayer->Set_IsWall(false);

	if (m_pPlayer->Get_IsEndAnimation(m_iAnimation_Spin))
	{
		m_pPlayer->Set_IsGround(false);
		m_pPlayer->SetUp_NextAnimation(m_iAnimation_FallBehind, true);
	}
	else if (m_pPlayer->Get_IsGround() && !m_isFallEnd)
	{
		m_fCurrentTime = 0.f;
		m_isFallEnd = true;
 		m_pPlayer->Set_IsFall(false);
		m_pPlayer->Get_RigidBody()->Set_IsGravity(false);
		m_pPlayer->SetUp_NextAnimation(m_iAnimation_DownFloor, false, 0.f);
	}
	else if (m_pPlayer->Get_IsEndAnimation(m_iAnimation_DownFloor) && m_fCurrentTime >= m_fDownDuration)
	{
		m_pPlayer->SetUp_NextAnimation(m_iAnimation_GetUpReady, false, 0.01f);
	}
	else if (m_pPlayer->Get_IsEndAnimation(m_iAnimation_GetUpReady))
	{
		m_pPlayer->SetUp_NextAnimation(m_iAnimation_GetUp, false, 0.01f);
	}
	else if (m_pPlayer->Get_IsEndAnimation(m_iAnimation_GetUp))
	{
		m_pPlayer->Change_State(CPlayer_Custom::IDLE);
	}
}

void CState_Custom_HitSpin::End_State()
{
	m_pPlayer->Set_IsFall(false);
}

CState_Custom_HitSpin* CState_Custom_HitSpin::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Custom_HitSpin* pInstance = new CState_Custom_HitSpin(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Custom_Hit2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Custom_HitSpin::Free()
{
	__super::Free();
}
