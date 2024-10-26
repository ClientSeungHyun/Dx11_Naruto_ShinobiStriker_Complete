#include "stdafx.h"
#include "State_Kurama_Jump.h"
#include "GameInstance.h"
#include "Boss_Kurama.h"
#include "Model.h"


CState_Kurama_Jump::CState_Kurama_Jump(CFsm* pFsm, CMonster* pMonster)
	:CState{ pFsm }
	, m_pMonster{ pMonster }
{
}

HRESULT CState_Kurama_Jump::Initialize(_uint iStateNum)
{
	m_iAnimation_Jump= m_pMonster->Get_Model()->Get_AnimationIndex("Kurama_Jump_Front", 2.0);
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Kurama_Jump::Start_State(void* pArg)
{
	m_pMonster->SetUp_NextAnimation(m_iAnimation_Jump, false, 0.1f);

	_vector vJumpDir = XMVectorSet(0.f, 20.f, 0.f, 0.f);

	m_pMonster->Get_RigidBody()->Clear_All();
	m_pMonster->Get_RigidBody()->Set_IsGravity(true);
	m_pMonster->Get_RigidBody()->Add_Force(CRigidBody::FORCE, vJumpDir);

		return S_OK;
}

void CState_Kurama_Jump::Update(_float fTimeDelta)
{
	_uint iFrame = m_pMonster->Get_Frame(m_iAnimation_Jump);

	if (m_pMonster->Get_IsEndAnimation(m_iAnimation_Jump))
	{

		m_pMonster->Change_State(CBoss_Kurama::SPECIALATTACK);
	}


}

void CState_Kurama_Jump::End_State()
{
	m_pMonster->Get_RigidBody()->Set_IsGravity(false);
	m_pMonster->Get_RigidBody()->Clear_All();
}

void CState_Kurama_Jump::Look_Player()
{
	m_pMonster->Find_PlayerTarget();

	CGameObject* pTarget = m_pMonster->Get_PlayerTarget();
	if (nullptr != pTarget)
	{
		m_pMonster->Get_Transform()->LookAt_Forward(pTarget->Get_Position());
	}
}

CState_Kurama_Jump* CState_Kurama_Jump::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum)
{
	CState_Kurama_Jump* pInstance = new CState_Kurama_Jump(pFsm, pMonster);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Kurama_Fall"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Kurama_Jump::Free()
{
	__super::Free();
}
