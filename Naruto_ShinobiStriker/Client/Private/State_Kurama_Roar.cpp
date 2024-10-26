#include "stdafx.h"
#include "State_Kurama_Roar.h"
#include "GameInstance.h"
#include "Boss_Kurama.h"
#include "Model.h"


CState_Kurama_Roar::CState_Kurama_Roar(CFsm* pFsm, CMonster* pMonster)
	:CState{ pFsm }
	, m_pMonster{ pMonster }
{
}

HRESULT CState_Kurama_Roar::Initialize(_uint iStateNum)
{
	m_iAnimation_Roar = m_pMonster->Get_Model()->Get_AnimationIndex("Kurama_Ninjutsu_Roar",1.4);
	m_iAnimation_Suggest = m_pMonster->Get_Model()->Get_AnimationIndex("Kurama_etc_Suggest_Large", 1.3);

	m_iStateNum = iStateNum;
	m_isFrist = true;

	return S_OK;
}

HRESULT CState_Kurama_Roar::Start_State(void* pArg)
{
	m_PrevStateIndex = m_pFsm->Get_PrevState();

	if (m_PrevStateIndex == CBoss_Kurama::APPEAR && m_isFrist)
		m_pMonster->SetUp_NextAnimation(m_iAnimation_Suggest, false);
	else
		m_pMonster->SetUp_NextAnimation(m_iAnimation_Roar, false);

	m_isStartRoar = false;
	m_isEndRoar = false;
	m_isPlaySound = false;

	return S_OK;
}

void CState_Kurama_Roar::Update(_float fTimeDelta)
{
	_uint iFrame = m_pMonster->Get_Frame(m_pMonster->Get_CurrentAnimationIndex());

	if (m_pMonster->Get_IsEndAnimation(m_iAnimation_Roar)
		|| m_pMonster->Get_IsEndAnimation(m_iAnimation_Suggest))
	{
		m_pMonster->Change_State(CBoss_Kurama::IDLE);
	}

	if (m_pMonster->Get_CurrentAnimationIndex() == m_iAnimation_Roar)
	{
		_uint iFrame = m_pMonster->Get_Frame(m_iAnimation_Roar);
		if ((m_iAttackFrame == iFrame || m_iAttackFrame + 1 == iFrame) && !m_isStartRoar)
		{
			dynamic_cast<CBoss_Kurama*>(m_pMonster)->Start_Roar();
			m_isStartRoar = true;

		}
		else if((m_iAttackEndFrame == iFrame || m_iAttackEndFrame + 1 == iFrame) && !m_isEndRoar)
		{
			dynamic_cast<CBoss_Kurama*>(m_pMonster)->End_Roar();
			m_isEndRoar = true;
		}
	}

	if (!m_isPlaySound)
	{
		if (m_pMonster->Get_CurrentAnimationIndex() == m_iAnimation_Suggest)
		{
			if (iFrame == 61 || iFrame == 62)
			{
				m_pMonster->Get_SoundCom()->Play3D(TEXT("Voice_Kurama_Roar.wav"), g_fVolume + 0.2f);
				m_isPlaySound = true;
			}
		}
		else if (m_pMonster->Get_CurrentAnimationIndex() == m_iAnimation_Roar && (iFrame == m_iAttackFrame || iFrame == m_iAttackFrame + 1))
		{
			m_pMonster->Get_SoundCom()->Play3D(TEXT("Voice_Kurama_Roar.wav"), g_fVolume + 0.2f);
			m_isPlaySound = true;
		}
	}
}

void CState_Kurama_Roar::End_State()
{
	m_isFrist = false;
	dynamic_cast<CBoss_Kurama*>(m_pMonster)->End_Roar();
	dynamic_cast<CBoss_Kurama*>(m_pMonster)->Appear_TargetUI();
}

void CState_Kurama_Roar::Look_Player()
{
	m_pMonster->Find_PlayerTarget();

	CGameObject* pTarget = m_pMonster->Get_PlayerTarget();
	if (nullptr != pTarget)
	{
		m_pMonster->Get_Transform()->LookAt_Forward(pTarget->Get_Position());
	}
}

CState_Kurama_Roar* CState_Kurama_Roar::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum)
{
	CState_Kurama_Roar* pInstance = new CState_Kurama_Roar(pFsm, pMonster);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Kurama_Roar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Kurama_Roar::Free()
{
	__super::Free();
}
