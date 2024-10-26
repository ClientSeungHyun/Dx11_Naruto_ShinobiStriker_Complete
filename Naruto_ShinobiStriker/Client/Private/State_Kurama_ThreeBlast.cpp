#include "stdafx.h"
#include "State_Kurama_ThreeBlast.h"
#include "GameInstance.h"
#include "Boss_Kurama.h"
#include "Model.h"


CState_Kurama_ThreeBlast::CState_Kurama_ThreeBlast(CFsm* pFsm, CMonster* pMonster)
	:CState{ pFsm }
	, m_pMonster{ pMonster }
{
}

HRESULT CState_Kurama_ThreeBlast::Initialize(_uint iStateNum)
{
	m_iAnimation_ThreeBlast = m_pMonster->Get_Model()->Get_AnimationIndex("Kurama_Ninjutsu_ConsecutiveTailedBeastBomb", 1.0);

	m_iStateNum = iStateNum;

	m_iAttackFrame[0] = 15;
	m_iAttackFrame[1] = 24;
	m_iAttackFrame[2] = 34;

	return S_OK;
}

HRESULT CState_Kurama_ThreeBlast::Start_State(void* pArg)
{
	m_iAnimation_ThreeBlast = m_pMonster->Get_Model()->Get_AnimationIndex("Kurama_Ninjutsu_ConsecutiveTailedBeastBomb", 0.5);
	m_pMonster->SetUp_NextAnimation(m_iAnimation_ThreeBlast, false);

	for (_uint i = 0; i < 3; ++i)
	{
		m_isShoot[i] = false;
	}

	m_pMonster->Get_Transform()->LookAt_Forward(m_pMonster->Get_PlayerTarget()->Get_Position());

	m_pMonster->Get_SoundCom()->Play3D(TEXT("Voice_Kurama_FoxSphere.wav"), g_fVolume + 0.2f);

	return S_OK;
}

void CState_Kurama_ThreeBlast::Update(_float fTimeDelta)
{
	_uint iFrame = m_pMonster->Get_Frame(m_iAnimation_ThreeBlast);

	if (iFrame == 26 || iFrame == 27)
		XMStoreFloat3(&m_vTargetPos, m_pMonster->Get_PlayerTarget()->Get_Position());

	if (!m_isShoot[0] && (iFrame == m_iAttackFrame[0] || iFrame == m_iAttackFrame[0] + 1))
	{
		dynamic_cast<CBoss_Kurama*>(m_pMonster)->Shoot_FoxSphere();
		m_isShoot[0] = true;
	}
	else if (!m_isShoot[1] && (iFrame == m_iAttackFrame[1] || iFrame == m_iAttackFrame[1] + 1))
	{
		dynamic_cast<CBoss_Kurama*>(m_pMonster)->Shoot_FoxSphere();
		Look_Player();
		m_isShoot[1] = true;
	}
	else if (!m_isShoot[2] && (iFrame == m_iAttackFrame[2] || iFrame == m_iAttackFrame[2] + 1))
	{
		dynamic_cast<CBoss_Kurama*>(m_pMonster)->Shoot_FoxSphere();
		Look_Player();
		m_isShoot[2] = true;
	}

	if (m_pMonster->Get_IsEndAnimation(m_iAnimation_ThreeBlast))
	{
		m_pMonster->Change_State(CBoss_Kurama::IDLE);
	}

}

void CState_Kurama_ThreeBlast::End_State()
{
}

void CState_Kurama_ThreeBlast::Look_Player()
{
	m_pMonster->Find_PlayerTarget();

	CGameObject* pTarget = m_pMonster->Get_PlayerTarget();
	if (nullptr != pTarget)
	{
		m_pMonster->Get_Transform()->LookAt_Forward(pTarget->Get_Position());
	}
}

CState_Kurama_ThreeBlast* CState_Kurama_ThreeBlast::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum)
{
	CState_Kurama_ThreeBlast* pInstance = new CState_Kurama_ThreeBlast(pFsm, pMonster);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Kurama_ThreeBlast"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Kurama_ThreeBlast::Free()
{
	__super::Free();
}
