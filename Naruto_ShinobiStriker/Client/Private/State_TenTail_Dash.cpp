#include "stdafx.h"
#include "State_TenTail_Dash.h"
#include "GameInstance.h"
#include "TenTail.h"
#include "Model.h"


CState_TenTail_Dash::CState_TenTail_Dash(CFsm* pFsm, CMonster* pMonster)
	:CState{ pFsm }
	, m_pMonster{ pMonster }
{
}

HRESULT CState_TenTail_Dash::Initialize(_uint iStateNum)
{
	m_iAnimation_Dash = m_pMonster->Get_Model()->Get_AnimationIndex("TenTailsCloneLoser01_Attack_MowDown", 1.5);

	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_TenTail_Dash::Start_State(void* pArg)
{
	m_pMonster->SetUp_NextAnimation(m_iAnimation_Dash, false);

	XMStoreFloat3(&m_vDashDir, XMVector3Normalize(m_pMonster->Get_Transform()->Get_State(CTransform::STATE_LOOK)));

	return S_OK;
}

void CState_TenTail_Dash::Update(_float fTimeDelta)
{
	_uint iFrame = m_pMonster->Get_Model()->Get_Frame(m_iAnimation_Dash);

	if (m_pMonster->Get_IsEndAnimation(m_iAnimation_Dash))
	{
		m_pMonster->Change_State(CTenTail::IDLE);
	}

	m_pMonster->Move_Dir(XMLoadFloat3(&m_vDashDir), 16.f, fTimeDelta);
}

void CState_TenTail_Dash::End_State()
{
}

CState_TenTail_Dash* CState_TenTail_Dash::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum)
{
	CState_TenTail_Dash* pInstance = new CState_TenTail_Dash(pFsm, pMonster);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_TenTail_Attack"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_TenTail_Dash::Free()
{
	__super::Free();
}
