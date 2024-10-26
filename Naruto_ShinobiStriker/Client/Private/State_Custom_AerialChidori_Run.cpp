#include "stdafx.h"
#include "State_Custom_AerialChidori_Run.h"
#include "GameInstance.h"
#include "Player_Custom.h"
#include "Model.h"
#include "Collider_PlayerAttack.h"
#include "ObjectPool.h"
#include "Particle_Up.h"

CState_Custom_AerialChidori_Run::CState_Custom_AerialChidori_Run(CFsm* pFsm, CPlayer* pPlayer)
	:CState{ pFsm }
	, m_pPlayer{ pPlayer }
{
}

HRESULT CState_Custom_AerialChidori_Run::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;

	m_iAnimation_AerialChidori_RunStart = m_pPlayer->Get_Model()->Get_AnimationIndex("CustomMan_Ninjutsu_Aerial_Chidori_Charge_Conect_toRun", 1.7);
	m_iAnimation_AerialChidori_RunLoop = m_pPlayer->Get_Model()->Get_AnimationIndex("CustomMan_Ninjutsu_Aerial_Chidori_Run_Loop", 1.7);

	return S_OK;
}

HRESULT CState_Custom_AerialChidori_Run::Start_State(void* pArg)
{
	m_pPlayer->SetUp_NextAnimation(m_iAnimation_AerialChidori_RunStart, false, 0.1f, 5);

	m_pPlayer->Find_NearTarget();

	dynamic_cast<CPlayer_Custom*>(m_pPlayer)->Run_Chidori();

	m_pPlayer->Get_AttackCollider()->IsActive(true);

	m_isRunStart = false;
	m_fRunTime = 0.f;

	return S_OK;
}

void CState_Custom_AerialChidori_Run::Update(_float fTimeDelta)
{
	m_fRunTime += fTimeDelta;

	Chase_Target(fTimeDelta);

	// 일정 시간 동안 충돌이 안 일어나면 바로 어택으로 가기
	if (m_fRunTime >= m_fRunDuration)
	{
		m_pPlayer->Change_State(CPlayer_Custom::AERIAL_CHIDORI_ATTACK);
	}
	else if (m_pPlayer->Get_CurrentAnimationIndex() == m_iAnimation_AerialChidori_RunStart)
	{
		if (m_pPlayer->Get_IsEndAnimation(m_iAnimation_AerialChidori_RunStart) && !m_isRunStart)
		{
			m_pPlayer->SetUp_NextAnimation(m_iAnimation_AerialChidori_RunLoop, true, 0.1f);

			m_isRunStart = true;
		}
	}

}

void CState_Custom_AerialChidori_Run::End_State()
{
}

void CState_Custom_AerialChidori_Run::Chase_Target(_float fTimeDelta)
{
	CGameObject* pTarget = m_pPlayer->Get_NearTarget();

	if (nullptr != pTarget)
	{
		_vector vTargetPos = pTarget->Get_Position();
		_vector vPlayerPos = m_pPlayer->Get_Position();
		_vector vChaseDir = vTargetPos - vPlayerPos;

		m_pPlayer->Get_Transform()->LookAt(vTargetPos);
	}

	m_pPlayer->Get_Transform()->Go_Straight(fTimeDelta, 18.f);
}

CState_Custom_AerialChidori_Run* CState_Custom_AerialChidori_Run::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Custom_AerialChidori_Run* pInstance = new CState_Custom_AerialChidori_Run(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Custom_AerialChidori_Run"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Custom_AerialChidori_Run::Free()
{
	__super::Free();
}
