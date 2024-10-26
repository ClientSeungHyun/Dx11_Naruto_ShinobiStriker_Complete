#include "stdafx.h"
#include "State_Kurama_Appear.h"
#include "GameInstance.h"
#include "Boss_Kurama.h"
#include "Model.h"
#include "ObjectPool.h"
#include "Smoke11.h"

CState_Kurama_Appear::CState_Kurama_Appear(CFsm* pFsm, CMonster* pMonster)
	:CState{ pFsm }
	, m_pMonster{ pMonster }
{
}

HRESULT CState_Kurama_Appear::Initialize(_uint iStateNum)
{
	m_iAnimation_Appear = m_pMonster->Get_Model()->Get_AnimationIndex("Kurama_etc_Appearance", 0.4);
	m_iStateNum = iStateNum;
	m_isFirst = true;

	return S_OK;
}

HRESULT CState_Kurama_Appear::Start_State(void* pArg)
{
	m_pMonster->SetUp_NextAnimation(m_iAnimation_Appear, false, 0.f);
	m_pMonster->Set_IsRender(true);

	if(m_isFirst)
		m_pGameInstance->Stop_BGM();

	m_isPlayLandSound = false;

	return S_OK;
}

void CState_Kurama_Appear::Update(_float fTimeDelta)
{
	_uint iFrame = m_pMonster->Get_Frame(m_iAnimation_Appear);

	if (m_pMonster->Get_IsEndAnimation(m_iAnimation_Appear))
	{
		if (m_isFirst)
		{
			m_pGameInstance->Play_BGM(TEXT("BGM_Kurama.wav"), 0.3f);
			m_pMonster->Change_State(CBoss_Kurama::ROAR);
		}
		else
			m_pMonster->Change_State(CBoss_Kurama::HEAL);

	}

	if (!m_isPlayLandSound && (iFrame == 4 || iFrame == 5))
	{
		_float3 vPos;
		XMStoreFloat3(&vPos, m_pMonster->Get_Position());

		for (_uint i = 0; i < 40; ++i)
		{
			CObjectPool<CSmoke11>::Get_GameObject()->Appear(vPos, _float3(3.f, 1.5f, 3.f), 1.5f);
		}

		m_pMonster->Get_SoundCom()->Play3D(TEXT("Effect_BossLand.wav"), g_fVolume + 0.2f);
		m_isPlayLandSound = true;
	}
}

void CState_Kurama_Appear::End_State()
{
	m_isFirst = false;
}

void CState_Kurama_Appear::Look_Player()
{
	m_pMonster->Find_PlayerTarget();

	CGameObject* pTarget = m_pMonster->Get_PlayerTarget();
	if (nullptr != pTarget)
	{
		m_pMonster->Get_Transform()->LookAt_Forward(pTarget->Get_Position());
	}
}

CState_Kurama_Appear* CState_Kurama_Appear::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum)
{
	CState_Kurama_Appear* pInstance = new CState_Kurama_Appear(pFsm, pMonster);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Kurama_Appear"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Kurama_Appear::Free()
{
	__super::Free();
}
