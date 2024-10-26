#include "stdafx.h"
#include "State_Custom_ChakraJump.h"
#include "GameInstance.h"
#include "Player_Custom.h"
#include "Model.h"
#include "Camera.h"

CState_Custom_ChakraJump::CState_Custom_ChakraJump(CFsm* pFsm, CPlayer* pPlayer)
	:CState{ pFsm }
	, m_pPlayer{ pPlayer }
{
}

HRESULT CState_Custom_ChakraJump::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;

	m_iAnimation_ChakraJump_Start = m_pPlayer->Get_Model()->Get_AnimationIndex("CustomMan_ChakraJump_Charge_Start", 1.0);
	m_iAnimation_ChakraJump_Loop = m_pPlayer->Get_Model()->Get_AnimationIndex("CustomMan_ChakraJump_Charge_Loop", 1.0);
	m_iAnimation_ChakraJump_End = m_pPlayer->Get_Model()->Get_AnimationIndex("CustomMan_ChakraJump_Charge_End", 1.0);

	return S_OK;
}

HRESULT CState_Custom_ChakraJump::Start_State(void* pArg)
{
	m_pPlayer->SetUp_NextAnimation(m_iAnimation_ChakraJump_Start, false, 0.1f);

	XMStoreFloat3(&m_vStartPos, m_pPlayer->Get_Position());

	m_isStart = { false };
	m_isLoop = { false };
	m_isEnd = { false };
	m_fJumpTime = 0.f;
	m_fJumpPower = 30.f;

	m_pPlayer->Appear_JumpGuide();
	m_pPlayer->Get_RigidBody()->Set_IsFriction(false);

	m_pPlayer->Get_EffectSoundCom()->Play3D(TEXT("Effect_ChakraJump.wav"), g_fVolume + 0.2f);

	return S_OK;
}

void CState_Custom_ChakraJump::Update(_float fTimeDelta)
{
	m_fJumpPower += fTimeDelta;
	if (m_fJumpPower >= 45.f)
		m_fJumpPower = 45.f;

	if (m_pPlayer->Get_IsEndAnimation(m_iAnimation_ChakraJump_Start) && !m_isStart)
	{
		if (KEY_HOLD(KEY::CTRL))
		{
			m_pPlayer->SetUp_NextAnimation(m_iAnimation_ChakraJump_Loop, true, 0.1f);
		}
		m_isStart = true;
	}
	else
	{
		if (KEY_AWAY(KEY::CTRL))
		{
			m_pPlayer->Set_IsJump(true);
			m_pPlayer->Set_IsWall(false);
			ChakraJump();
			m_pPlayer->Disappear_JumpGuide();
			m_pPlayer->SetUp_NextAnimation(m_iAnimation_ChakraJump_End, false, 0.1f);
		}
	}

	if (m_pPlayer->Get_CurrentAnimationIndex() != m_iAnimation_ChakraJump_End)
		Set_JumpDir();
	else
	{
		if (m_pPlayer->Get_RigidBody()->Get_Velocity().y <= -1.5f)
		{
			m_pPlayer->Change_State(CPlayer_Custom::FALL_FRONT);
		}
	}
}

void CState_Custom_ChakraJump::End_State()
{
	//m_pPlayer->Disappear_JumpGuide();
	m_pPlayer->Set_IsGravity(true);
}

void CState_Custom_ChakraJump::Set_JumpDir()
{
	_vector vCameraLook = XMVector3Normalize(m_pGameInstance->Find_Camera(m_pPlayer->Get_LevelIndex())->Get_Transform()->Get_State(CTransform::STATE_LOOK)) * 1.2f;
	_vector vPos = m_pPlayer->Get_Position();
	_vector vUp = XMVectorSet(0.f, 1.3f, 0.f, 0.f);


	XMStoreFloat3(&m_vJumpDir, XMVector3Normalize(vCameraLook + vUp));
}

void CState_Custom_ChakraJump::ChakraJump()
{
	m_pPlayer->Get_Transform()->LookAt(m_pPlayer->Get_Position() + XMLoadFloat3(&m_vJumpDir));
	m_pPlayer->Get_RigidBody()->Add_Force(CRigidBody::FORCE, XMVector3Normalize(XMLoadFloat3(&m_vJumpDir)) * m_fJumpPower);
}

CState_Custom_ChakraJump* CState_Custom_ChakraJump::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Custom_ChakraJump* pInstance = new CState_Custom_ChakraJump(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Custom_ChakraJump"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Custom_ChakraJump::Free()
{
	__super::Free();
}
