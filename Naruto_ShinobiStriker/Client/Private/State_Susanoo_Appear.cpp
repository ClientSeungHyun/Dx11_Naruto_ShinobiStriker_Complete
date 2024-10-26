#include "stdafx.h"
#include "State_Susanoo_Appear.h"
#include "GameInstance.h"
#include "Boss_Susanoo.h"
#include "Model.h"

#include "ObjectPool.h"
#include "Smoke11.h"

CState_Susanoo_Appear::CState_Susanoo_Appear(CFsm* pFsm, CMonster* pMonster)
	:CState{ pFsm }
	, m_pMonster{ pMonster }
{
}

HRESULT CState_Susanoo_Appear::Initialize(_uint iStateNum)
{
	m_iAnimation_Appear = m_pMonster->Get_Model()->Get_AnimationIndex("RineSusanoPS_etc_Appearance_Type01", 0.4);
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Susanoo_Appear::Start_State(void* pArg)
{
	m_pMonster->SetUp_NextAnimation(m_iAnimation_Appear, false, 0.f);

	m_pMonster->Find_PlayerTarget();
	m_pGameInstance->Stop_BGM();
	m_isSmoke = false;

	return S_OK;
}

void CState_Susanoo_Appear::Update(_float fTimeDelta)
{
	_uint iFrame = m_pMonster->Get_Frame(m_iAnimation_Appear);

	if ((iFrame == 3 || iFrame == 4) && !m_isSmoke)
	{
		_float3 vPos;
		XMStoreFloat3(&vPos, m_pMonster->Get_Position());

		for (_uint i = 0; i < 40; ++i)
		{
			CObjectPool<CSmoke11>::Get_GameObject()->Appear(vPos, _float3(3.f, 1.5f, 3.f), 1.5f);
		}

		m_isSmoke = true;
	}

	if (m_pMonster->Get_IsEndAnimation(m_iAnimation_Appear))
	{
		m_pGameInstance->Play_BGM(TEXT("BGM_Susanoo.wav"), 0.3f);
		m_pMonster->Change_State(CBoss_Susanoo::ROAR);
	}
}

void CState_Susanoo_Appear::End_State()
{
}

CState_Susanoo_Appear* CState_Susanoo_Appear::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum)
{
	CState_Susanoo_Appear* pInstance = new CState_Susanoo_Appear(pFsm, pMonster);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Susanoo_Appear"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Susanoo_Appear::Free()
{
	__super::Free();
}
