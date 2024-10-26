#include "stdafx.h"
#include "State_Kurama_AerialBlast.h"
#include "GameInstance.h"
#include "Boss_Kurama.h"
#include "Model.h"


CState_Kurama_AerialBlast::CState_Kurama_AerialBlast(CFsm* pFsm, CMonster* pMonster)
	:CState{ pFsm }
	, m_pMonster{ pMonster }
{
}

HRESULT CState_Kurama_AerialBlast::Initialize(_uint iStateNum)
{
	m_iAnimation_Blast = m_pMonster->Get_Model()->Get_AnimationIndex("Kurama_Ninjutsu_Aerial_FakeFoxBlast", 1.3);

	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Kurama_AerialBlast::Start_State(void* pArg)
{
	m_pMonster->SetUp_NextAnimation(m_iAnimation_Blast, false);

	m_isShoot = false;

	m_pMonster->Get_Transform()->LookAt_Forward(m_pMonster->Get_PlayerTarget()->Get_Position());


	m_pMonster->Get_SoundCom()->Play3D(TEXT("Voice_Kurama_FoxSphere.wav"), g_fVolume + 0.2f);

	return S_OK;
}

void CState_Kurama_AerialBlast::Update(_float fTimeDelta)
{
	_uint iFrame = m_pMonster->Get_Frame(m_iAnimation_Blast);

	if (iFrame == 26 || iFrame == 27)
		XMStoreFloat3(&m_vTargetPos, m_pMonster->Get_PlayerTarget()->Get_Position());

	if (!m_isShoot && iFrame >= 43 && iFrame <= 44)
	{
		_uint iRand = rand() % 10;
		dynamic_cast<CBoss_Kurama*>(m_pMonster)->Shoot_FoxBlast(XMLoadFloat3(&m_vTargetPos));

		m_isShoot = true;
	}

	if (m_pMonster->Get_IsEndAnimation(m_iAnimation_Blast))
	{
		m_pMonster->Change_State(CBoss_Kurama::IDLE);
	}

}

void CState_Kurama_AerialBlast::End_State()
{
}

void CState_Kurama_AerialBlast::Look_Player()
{
	m_pMonster->Find_PlayerTarget();

	CGameObject* pTarget = m_pMonster->Get_PlayerTarget();
	if (nullptr != pTarget)
	{
		m_pMonster->Get_Transform()->LookAt_Forward(pTarget->Get_Position());
	}
}

CState_Kurama_AerialBlast* CState_Kurama_AerialBlast::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum)
{
	CState_Kurama_AerialBlast* pInstance = new CState_Kurama_AerialBlast(pFsm, pMonster);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Kurama_Roar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Kurama_AerialBlast::Free()
{
	__super::Free();
}
