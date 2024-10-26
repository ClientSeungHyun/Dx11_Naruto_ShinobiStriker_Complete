#include "stdafx.h"
#include "State_Naruto_WallAttach.h"
#include "GameInstance.h"
#include "Player_Naruto.h"
#include "Model.h"


CState_Naruto_WallAttach::CState_Naruto_WallAttach(CFsm* pFsm, CPlayer* pPlayer)
	:CState{ pFsm }
	, m_pPlayer{ pPlayer }
{
}

HRESULT CState_Naruto_WallAttach::Initialize(_uint iStateNum)
{
	m_iAnimation_FallFront = m_pPlayer->Get_Model()->Get_AnimationIndex("Naruto_Fall_Front_Loop");
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Naruto_WallAttach::Start_State(void* pArg)
{
	m_pPlayer->SetUp_NextAnimation(m_iAnimation_FallFront, true);
	
	m_pPlayer->Get_RigidBody()->Clear_All();
	m_pPlayer->Set_IsGravity(false);

	XMStoreFloat3(&m_vPrevPos, m_pPlayer->Get_Position());

	return S_OK;
}

void CState_Naruto_WallAttach::Update(_float fTimeDelta)
{
	CTransform* pTransform = m_pPlayer->Get_Transform();
	Safe_AddRef(pTransform);

	_vector vPlayerUp = XMVector3Normalize(pTransform->Get_State(CTransform::STATE_UP));
	_vector vWallUp = XMVector3Normalize(m_pPlayer->Get_Normal());

	_float fRadain;
 	XMStoreFloat(&fRadain, XMVector3Dot(vPlayerUp, vWallUp));

	/* 거의 평행 */
	if (fRadain >= 0.95f)
	{
		m_pPlayer->Set_IsWall(true);
		m_pPlayer->Set_IsAttachWall(false);
		m_pPlayer->Change_State(CPlayer_Naruto::LAND);
	}
	else
	{
		vPlayerUp = XMVectorLerp(vPlayerUp, vWallUp, 15.0f * fTimeDelta);
		pTransform->Set_NewUp(vPlayerUp);
	}

	Safe_Release(pTransform);

}

void CState_Naruto_WallAttach::End_State()
{
	m_pPlayer->Set_IsAttachWall(false);
}

CState_Naruto_WallAttach* CState_Naruto_WallAttach::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Naruto_WallAttach* pInstance = new CState_Naruto_WallAttach(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Naruto_Idle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Naruto_WallAttach::Free()
{
	__super::Free();
}
