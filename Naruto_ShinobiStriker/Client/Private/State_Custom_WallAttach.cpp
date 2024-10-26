#include "stdafx.h"
#include "State_Custom_WallAttach.h"
#include "GameInstance.h"
#include "Player_Custom.h"
#include "Model.h"


CState_Custom_WallAttach::CState_Custom_WallAttach(CFsm* pFsm, CPlayer* pPlayer)
	:CState{ pFsm }
	, m_pPlayer{ pPlayer }
{
}

HRESULT CState_Custom_WallAttach::Initialize(_uint iStateNum)
{
	m_iAnimation_FallFront = m_pPlayer->Get_Model()->Get_AnimationIndex("CustomMan_Fall_Front_Loop");
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Custom_WallAttach::Start_State(void* pArg)
{
	m_pPlayer->SetUp_NextAnimation(m_iAnimation_FallFront, true);

	m_pPlayer->Get_RigidBody()->Clear_All();
	m_pPlayer->Set_IsGravity(false);

	XMStoreFloat3(&m_vPrevPos, m_pPlayer->Get_Position());

	return S_OK;
}

void CState_Custom_WallAttach::Update(_float fTimeDelta)
{
	CTransform* pTransform = m_pPlayer->Get_Transform();
	Safe_AddRef(pTransform);

	_vector vPlayerPos = pTransform->Get_State(CTransform::STATE_POSITION);
	_vector vPrevPlayerPos = XMLoadFloat3(&m_vPrevPos);

	_vector vPlayerUp = XMVector3Normalize(pTransform->Get_State(CTransform::STATE_UP));
	_vector vWallUp = XMVector3Normalize(m_pPlayer->Get_Normal());

	_float fMove = XMVectorGetX(XMVector3Length(vPlayerPos - vPrevPlayerPos));
	if (fMove > 0.0001f)
	{
		_vector vUp = XMVector3Normalize(pTransform->Get_State(CTransform::STATE_UP));
		_vector vWallUp = XMVector3Normalize(m_pPlayer->Get_Normal());

		_float fRadain = XMVectorGetX(XMVector3Dot(vPlayerUp, vWallUp));
		if (fRadain >= 0.95f)
		{
			m_pPlayer->Set_IsWall(true);
			m_pPlayer->Set_IsAttachWall(false);
			m_pPlayer->Change_State(CPlayer_Custom::LAND);
		}
		else
		{
			vPlayerUp = XMVectorLerp(vPlayerUp, vWallUp, 10.f * fTimeDelta);
			pTransform->Set_NewUp(vPlayerUp);
		}
		return;
	}

	_vector vWallPos = m_pPlayer->Get_WallPos();
	_vector vDistance = vWallPos - vPlayerPos;
	_vector vDir = XMVector3Normalize(vDistance);

	_vector vMove = vDir * 15.0f * fTimeDelta;

	if (XMVectorGetX(XMVector3Length(vDistance)) <= XMVectorGetX(XMVector3Length(vMove)))
	{
		m_pPlayer->Set_IsWall(true);
		m_pPlayer->Set_IsAttachWall(false);
		m_pPlayer->Change_State(CPlayer_Custom::LAND);
	}
	else
		vPlayerPos += vMove;

	vPlayerUp = XMVectorLerp(vPlayerUp, vWallUp, 10.f * fTimeDelta);

	pTransform->Set_State(CTransform::STATE_POSITION, vPlayerPos);
	pTransform->Set_NewUp(vPlayerUp);

	XMStoreFloat3(&m_vPrevPos, vPrevPlayerPos);

	Safe_Release(pTransform);

}

void CState_Custom_WallAttach::End_State()
{
	m_pPlayer->Set_IsAttachWall(false);
}

CState_Custom_WallAttach* CState_Custom_WallAttach::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Custom_WallAttach* pInstance = new CState_Custom_WallAttach(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Naruto_Idle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Custom_WallAttach::Free()
{
	__super::Free();
}
