#include "stdafx.h"
#include "State_Naruto_Jump.h"
#include "GameInstance.h"
#include "Player_Naruto.h"
#include "Model.h"

#include "RigidBody.h"

CState_Naruto_Jump::CState_Naruto_Jump(CFsm* pFsm, CPlayer* pPlayer)
	:CState{ pFsm }
	, m_pPlayer{ pPlayer }
{
}

HRESULT CState_Naruto_Jump::Initialize(_uint iStateNum)
{
	m_iAnimation_Jump = m_pPlayer->Get_Model()->Get_AnimationIndex("Naruto_Jump_Front", 2.6);
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Naruto_Jump::Start_State(void* pArg)
{
	m_pPlayer->SetUp_NextAnimation(m_iAnimation_Jump, false, 0.1f);

	m_pPlayer->Get_Transform()->Set_NewUp(XMVectorSet(0.f, 1.f, 0.f, 0.f));

	Set_JumpTargetPos();
	Jump();

	m_pPlayer->Set_IsJump(true);
	m_pPlayer->Set_IsWall(false);

	m_pPlayer->Get_SoundCom()->Play3D(TEXT("Jump.wav"), g_fVolume);

	return S_OK;
}

void CState_Naruto_Jump::Update(_float fTimeDelta)
{
	Move(fTimeDelta);

	if (XMVectorGetY(m_pPlayer->Get_TargetPos()) <= (XMVectorGetY(m_pPlayer->Get_Position()))
		|| m_pPlayer->Get_IsEndAnimation(m_iAnimation_Jump))
		m_pPlayer->Change_State(CPlayer_Naruto::FALL_FRONT);

	if (KEY_TAP(KEY::SPACE) && !m_pPlayer->Get_IsDoubleJump())
	{
		m_pPlayer->Change_State(CPlayer_Naruto::DOUBLEJUMP);
	}

	// ³ª¼±È¯
	if (KEY_TAP(KEY::NUM1) &&
		dynamic_cast<CPlayer_Naruto*>(m_pPlayer)->Get_RasenganCoolTime() <= 0.f)
	{
		m_pPlayer->Change_State(CPlayer_Naruto::AERIAL_RASENGUN_CHARGE);
		return;
	}

	if (KEY_TAP(KEY::NUM2) &&
		dynamic_cast<CPlayer_Naruto*>(m_pPlayer)->Get_RasenShurikenCoolTime() <= 0.f)
	{
		m_pPlayer->Change_State(CPlayer_Naruto::AERIAL_RASENSHURIKEN);
		return;
	}
}

void CState_Naruto_Jump::End_State()
{
}

void CState_Naruto_Jump::Set_JumpTargetPos()
{
	_vector vCurrentPos = m_pPlayer->Get_Position();
	_vector vCurrentLook = XMVector3Normalize(m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_LOOK));

	_vector vJumpPos = vCurrentPos + XMVectorSet(0.f, 4.f, 0.f, 0.f) + vCurrentLook * 5.f;

	m_pPlayer->Set_TargetPos(vJumpPos);
}

void CState_Naruto_Jump::Jump()
{
	_vector vJumpDir;
	_vector vJumpPower = XMVectorSet(1.f, 10.f, 1.f, 0.f);

	if (m_pPlayer->Get_IsWall())
	{
		_vector vPlayerUp = XMVector3Normalize(m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_UP));
		_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

		vJumpDir = XMVector3Normalize(vPlayerUp + vUp);
	}
	else
	{
		vJumpDir = XMVector3Normalize(m_pPlayer->Get_TargetPos() - m_pPlayer->Get_Position());
	}

	vJumpDir *= vJumpPower;

	m_pPlayer->Get_RigidBody()->Clear_All();
	m_pPlayer->Get_RigidBody()->Add_Force(CRigidBody::FORCE, vJumpDir);
}

void CState_Naruto_Jump::Move(_float fTimeDelta)
{
	_vector vDir = XMVectorSet(0.f, 0.f, 0.f, 0.f);

	if (KEY_HOLD(KEY::W))
	{
		vDir += m_pPlayer->Calculate_Direction_Straight();
	}
	else if (KEY_HOLD(KEY::S))
	{
		vDir -= m_pPlayer->Calculate_Direction_Straight();
	}

	if (KEY_HOLD(KEY::D))
	{
		vDir += m_pPlayer->Calculate_Direction_Right();
	}

	if (KEY_HOLD(KEY::A))
	{
		vDir -= m_pPlayer->Calculate_Direction_Right();
	}

	if (XMVector3Length(vDir).m128_f32[0] > 0.f)
	{

		m_pPlayer->Move_Dir(vDir, 8.0f, fTimeDelta);
	}
}

CState_Naruto_Jump* CState_Naruto_Jump::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Naruto_Jump* pInstance = new CState_Naruto_Jump(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Naruto_Idle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Naruto_Jump::Free()
{
	__super::Free();
}
