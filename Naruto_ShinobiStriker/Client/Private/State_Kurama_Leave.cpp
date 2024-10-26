#include "stdafx.h"
#include "State_Kurama_Leave.h"
#include "GameInstance.h"
#include "Boss_Kurama.h"
#include "Model.h"


CState_Kurama_Leave::CState_Kurama_Leave(CFsm* pFsm, CMonster* pMonster)
	:CState{ pFsm }
	, m_pMonster{ pMonster }
{
}

HRESULT CState_Kurama_Leave::Initialize(_uint iStateNum)
{
	m_iAnimation_Leave = m_pMonster->Get_Model()->Get_AnimationIndex("Kurama_etc_Leave", 1.0);
	m_iStateNum = iStateNum;
	m_isFirst = true;

	m_vLeavePos[0] = _float3(62.f, -2.6f, -42.8f);
	m_vLeavePos[1] = _float3(-13.414f, 8.65f, -2.245f);
	m_vLeavePos[2] = _float3(58.f, 3.73f, 23.56f);

	m_iPositionIndex = m_iPrevPositionIndex = 0;

	return S_OK;
}

HRESULT CState_Kurama_Leave::Start_State(void* pArg)
{
	m_iAnimation_Leave = m_pMonster->Get_Model()->Get_AnimationIndex("Kurama_etc_Leave", 2.0);
	m_pMonster->SetUp_NextAnimation(m_iAnimation_Leave, false, 0.1f);

	return S_OK;
}

void CState_Kurama_Leave::Update(_float fTimeDelta)
{
	_uint iFrame = m_pMonster->Get_Frame(m_iAnimation_Leave);

	if(iFrame > 10)
		m_pMonster->Set_IsRender(false);

	if (m_pMonster->Get_IsEndAnimation(m_iAnimation_Leave))
	{
		while (1)
		{
			_uint iRand = rand() % 3;

			if (iRand != m_iPositionIndex && iRand != m_iPrevPositionIndex)
			{
				m_iPrevPositionIndex = m_iPositionIndex;
				m_iPositionIndex = iRand;
				m_pMonster->Get_Transform()->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_vLeavePos[iRand]));
				break;
			}
		}

		m_pMonster->Change_State(CBoss_Kurama::APPEAR);
	}

	
}

void CState_Kurama_Leave::End_State()
{
	m_pMonster->Get_Transform()->LookAt_Forward(m_pMonster->Get_PlayerTarget()->Get_Position());
}

void CState_Kurama_Leave::Look_Player()
{
	m_pMonster->Find_PlayerTarget();

	CGameObject* pTarget = m_pMonster->Get_PlayerTarget();
	if (nullptr != pTarget)
	{
		m_pMonster->Get_Transform()->LookAt_Forward(pTarget->Get_Position());
	}
}

CState_Kurama_Leave* CState_Kurama_Leave::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum)
{
	CState_Kurama_Leave* pInstance = new CState_Kurama_Leave(pFsm, pMonster);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Kurama_Heal"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Kurama_Leave::Free()
{
	__super::Free();
}
