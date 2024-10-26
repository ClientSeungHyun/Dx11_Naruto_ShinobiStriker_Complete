#include "stdafx.h"
#include "State_Custom_HIt1.h"
#include "GameInstance.h"
#include "Player_Custom.h"
#include "Model.h"


CState_Custom_Hit1::CState_Custom_Hit1(CFsm* pFsm, CPlayer* pPlayer)
	:CState{ pFsm }
	, m_pPlayer{ pPlayer }
{
}

HRESULT CState_Custom_Hit1::Initialize(_uint iStateNum)
{
	m_iAnimation_Hit = m_pPlayer->Get_Model()->Get_AnimationIndex("CustomMan_Beaten_UpLeft_Shoulder");
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Custom_Hit1::Start_State(void* pArg)
{
	m_pPlayer->SetUp_NextAnimation(m_iAnimation_Hit, false, 0.05f);

	if (nullptr != pArg)
	{
 		_float2* vForceBackPower = static_cast<_float2*>(pArg);
		m_pPlayer->KnockBack(vForceBackPower->x, vForceBackPower->y);
	}
	else
	{
		m_pPlayer->KnockBack(5.f, 0.f);
	}

	m_pPlayer->Get_SoundCom()->Play3D(TEXT("Voice_Hit0.wav"), g_fVolume + 0.2f);

	return S_OK;
}

void CState_Custom_Hit1::Update(_float fTimeDelta)
{
	if(m_pPlayer->Get_IsEndAnimation(m_iAnimation_Hit))
		m_pPlayer->Change_State(CPlayer_Custom::IDLE);
}

void CState_Custom_Hit1::End_State()
{
}

CState_Custom_Hit1* CState_Custom_Hit1::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Custom_Hit1* pInstance = new CState_Custom_Hit1(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Custom_Hit1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Custom_Hit1::Free()
{
	__super::Free();
}
