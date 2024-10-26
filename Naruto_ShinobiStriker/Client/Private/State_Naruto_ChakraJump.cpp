#include "stdafx.h"
#include "State_Naruto_ChakraJump.h"
#include "GameInstance.h"
#include "Player_Naruto.h"
#include "Model.h"
#include "Camera.h"

CState_Naruto_ChakraJump::CState_Naruto_ChakraJump(CFsm* pFsm, CPlayer* pPlayer)
	:CState{ pFsm }
	, m_pPlayer{ pPlayer }
{
}

HRESULT CState_Naruto_ChakraJump::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;

	m_iAnimation_ChakraJump_Start = m_pPlayer->Get_Model()->Get_AnimationIndex("Naruto_ChakraJump_Charge_Start", 1.0);
	m_iAnimation_ChakraJump_Loop = m_pPlayer->Get_Model()->Get_AnimationIndex("Naruto_ChakraJump_Charge_Loop", 1.0);
	m_iAnimation_ChakraJump_End = m_pPlayer->Get_Model()->Get_AnimationIndex("Naruto_ChakraJump_Charge_End", 1.0);

	return S_OK;
}

HRESULT CState_Naruto_ChakraJump::Start_State(void* pArg)
{
	m_pPlayer->SetUp_NextAnimation(m_iAnimation_ChakraJump_Start, false, 0.1f);

	XMStoreFloat3(&m_vStartPos, m_pPlayer->Get_Position());

	m_isStart = { false };
	m_isLoop = { false };
	m_isEnd = { false };
	m_fJumpTime = 0.f;
	m_fJumpPower = 30.f;

	m_pPlayer->Appear_JumpGuide();
	
	return S_OK;
}

void CState_Naruto_ChakraJump::Update(_float fTimeDelta)
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
		if (m_pPlayer->Get_RigidBody()->Get_Velocity().y <= -1.f)
		{
			m_pPlayer->Change_State(CPlayer_Naruto::FALL_FRONT);
		}
	}
}

void CState_Naruto_ChakraJump::End_State()
{
	m_pPlayer->Set_IsGravity(true);
}

void CState_Naruto_ChakraJump::Set_JumpDir()
{
	_vector vCameraLook = XMVector3Normalize(m_pGameInstance->Find_Camera(m_pPlayer->Get_LevelIndex())->Get_Transform()->Get_State(CTransform::STATE_LOOK));
	_vector vPos = m_pPlayer->Get_Position();
	_vector vUp = XMVectorSet(0.f, 1.5f, 0.f, 0.f);


	XMStoreFloat3(&m_vJumpDir, XMVector3Normalize(vCameraLook + vUp));
}

void CState_Naruto_ChakraJump::ChakraJump()
{
	m_pPlayer->Get_Transform()->LookAt(m_pPlayer->Get_Position() + XMLoadFloat3(&m_vJumpDir));
	m_pPlayer->Get_RigidBody()->Add_Force(CRigidBody::FORCE, XMVector3Normalize(XMLoadFloat3(&m_vJumpDir)) * m_fJumpPower);
}


CState_Naruto_ChakraJump* CState_Naruto_ChakraJump::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Naruto_ChakraJump* pInstance = new CState_Naruto_ChakraJump(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Naruto_ChakraJump"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Naruto_ChakraJump::Free()
{
	__super::Free();
}
