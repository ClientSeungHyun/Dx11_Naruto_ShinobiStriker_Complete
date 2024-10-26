#include "stdafx.h"
#include "State_Susanoo_Roar.h"
#include "GameInstance.h"
#include "Boss_Susanoo.h"
#include "Model.h"


CState_Susanoo_Roar::CState_Susanoo_Roar(CFsm* pFsm, CMonster* pMonster)
	:CState{ pFsm }
	, m_pMonster{ pMonster }
{
}

HRESULT CState_Susanoo_Roar::Initialize(_uint iStateNum)
{
	m_iAnimation_Roar = m_pMonster->Get_Model()->Get_AnimationIndex("RineSusanoPS_etc_Suggest_Large");
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Susanoo_Roar::Start_State(void* pArg)
{
	m_pMonster->SetUp_NextAnimation(m_iAnimation_Roar, false, 0.5f);

	m_pMonster->Find_PlayerTarget();

	return S_OK;
}

void CState_Susanoo_Roar::Update(_float fTimeDelta)
{
	_uint iFrame = m_pMonster->Get_Frame(m_iAnimation_Roar);

	if (m_pMonster->Get_IsEndAnimation(m_iAnimation_Roar))
	{
		m_pMonster->Change_State(CBoss_Susanoo::IDLE);
	}

	if (!m_isSummonBalde && iFrame >= 80 && iFrame < 82)
	{
		dynamic_cast<CBoss_Susanoo*>(m_pMonster)->Summon_Blade();
		m_isSummonBalde = true;
	}

	if (!m_isPlaySound && (iFrame == 37 || iFrame == 38))
	{
		m_pMonster->Get_SoundCom()->Play3D(TEXT("Voice_Susanoo_Roar.wav"), g_fVolume + 0.2f);
		m_isPlaySound = true;
	}
}

void CState_Susanoo_Roar::End_State()
{
	dynamic_cast<CBoss_Susanoo*>(m_pMonster)->Appear_TargetUI();
}

CState_Susanoo_Roar* CState_Susanoo_Roar::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum)
{
	CState_Susanoo_Roar* pInstance = new CState_Susanoo_Roar(pFsm, pMonster);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Susanoo_Roar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Susanoo_Roar::Free()
{
	__super::Free();
}
