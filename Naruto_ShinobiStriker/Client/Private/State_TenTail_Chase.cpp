#include "stdafx.h"
#include "State_TenTail_Chase.h"
#include "GameInstance.h"
#include "TenTail.h"
#include "Model.h"


CState_TenTail_Chase::CState_TenTail_Chase(CFsm* pFsm, CMonster* pMonster)
	:CState{ pFsm }
	, m_pMonster{ pMonster }
{
}

HRESULT CState_TenTail_Chase::Initialize(_uint iStateNum)
{
	m_iAnimation_RunLoop = m_pMonster->Get_Model()->Get_AnimationIndex("TenTailsCloneLoser01_Run_Loop");
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_TenTail_Chase::Start_State(void* pArg)
{
	m_pMonster->SetUp_NextAnimation(m_iAnimation_RunLoop, true);

	return S_OK;
}

void CState_TenTail_Chase::Update(_float fTimeDelta)
{
	CTransform* pTransform = m_pMonster->Get_Transform();
	_vector vTargetPos = m_pMonster->Get_PlayerTarget()->Get_Position();
	_vector vPos = pTransform->Get_State(CTransform::STATE_POSITION);

	_vector vChaserDir = vTargetPos - vPos;

	_float fDistance = -1.f;
	XMStoreFloat(&fDistance, XMVector3Length(vChaserDir));

	if (fDistance <= 1.5f)
	{
		m_pMonster->Change_State(CTenTail::ATTACK1);
		return;
	}

	vChaserDir = XMVector3Normalize(vChaserDir);

	m_pMonster->Move_Dir(vChaserDir, m_fMoveSpeed, fTimeDelta);
}

void CState_TenTail_Chase::End_State()
{
}

CState_TenTail_Chase* CState_TenTail_Chase::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum)
{
	CState_TenTail_Chase* pInstance = new CState_TenTail_Chase(pFsm, pMonster);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Naruto_Idle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_TenTail_Chase::Free()
{
	__super::Free();
}
