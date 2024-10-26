#include "stdafx.h"
#include "State_Kurama_FoxBlast.h"
#include "GameInstance.h"
#include "Boss_Kurama.h"
#include "Model.h"


CState_Kurama_FoxBlast::CState_Kurama_FoxBlast(CFsm* pFsm, CMonster* pMonster)
	:CState{ pFsm }
	, m_pMonster{ pMonster }
{
}

HRESULT CState_Kurama_FoxBlast::Initialize(_uint iStateNum)
{
	m_iAnimation_Charge = m_pMonster->Get_Model()->Get_AnimationIndex("Kurama_Ninjutsu_FakeFoxBlast_Charge", 2.0);
	m_iAnimation_Blast = m_pMonster->Get_Model()->Get_AnimationIndex("Kurama_Ninjutsu_FakeFoxBlast", 2.0);

	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Kurama_FoxBlast::Start_State(void* pArg)
{
	m_pMonster->SetUp_NextAnimation(m_iAnimation_Charge, false);
	m_pMonster->Find_PlayerTarget();


	m_isShoot = false;

	return S_OK;
}

void CState_Kurama_FoxBlast::Update(_float fTimeDelta)
{
	_uint iCurrentAnimation = m_pMonster->Get_CurrentAnimationIndex();
	_uint iFrame = m_pMonster->Get_Frame(iCurrentAnimation);

	if (iFrame == 60 || iFrame == 61)
		XMStoreFloat3(&m_vTargetPos, m_pMonster->Get_PlayerTarget()->Get_Position());

	if (m_pMonster->Get_IsEndAnimation(m_iAnimation_Blast))
	{
		m_pMonster->Change_State(CBoss_Kurama::IDLE);
	}
	else if (m_pMonster->Get_IsEndAnimation(m_iAnimation_Charge))
	{
		m_pMonster->SetUp_NextAnimation(m_iAnimation_Blast, false);
	}

	if (iCurrentAnimation == m_iAnimation_Blast)
	{
		if (!m_isShoot && iFrame >= 67 && iFrame <= 68)
		{
			_uint iRand = rand() % 10;
			if (iRand > 7)
			{
				dynamic_cast<CBoss_Kurama*>(m_pMonster)->Shoot_FoxBlast(XMLoadFloat3(&m_vTargetPos));
			}
			else
			{
				dynamic_cast<CBoss_Kurama*>(m_pMonster)->Shoot_FoxSphere();
			}
			m_isShoot = true;
		}
		
	}
}

void CState_Kurama_FoxBlast::End_State()
{
}

void CState_Kurama_FoxBlast::Look_Player()
{
	m_pMonster->Find_PlayerTarget();

	CGameObject* pTarget = m_pMonster->Get_PlayerTarget();
	if (nullptr != pTarget)
	{
		m_pMonster->Get_Transform()->LookAt_Forward(pTarget->Get_Position());
	}
}

CState_Kurama_FoxBlast* CState_Kurama_FoxBlast::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum)
{
	CState_Kurama_FoxBlast* pInstance = new CState_Kurama_FoxBlast(pFsm, pMonster);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Kurama_Roar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Kurama_FoxBlast::Free()
{
	__super::Free();
}
