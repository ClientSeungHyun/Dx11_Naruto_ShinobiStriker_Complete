#include "stdafx.h"
#include "State_WhiteZetsu_FlyGround.h"
#include "GameInstance.h"
#include "WhiteZetsu.h"
#include "Model.h"


CState_WhiteZetsu_FlyGround::CState_WhiteZetsu_FlyGround(CFsm* pFsm, CMonster* pMonster)
	:CState{ pFsm }
	, m_pMonster{ pMonster }
{
}

HRESULT CState_WhiteZetsu_FlyGround::Initialize(_uint iStateNum)
{
	m_iAnimation_SpinGround = m_pMonster->Get_Model()->Get_AnimationIndex("WhiteZetsuCrowdForm_Beaten_SpinBlowOff_Conect_toDown", 1.7);
	m_iStateNum = iStateNum;

	m_pMonster->Set_IsInvincible(true);

	return S_OK;
}

HRESULT CState_WhiteZetsu_FlyGround::Start_State(void* pArg)
{
	m_pMonster->SetUp_NextAnimation(m_iAnimation_SpinGround, false, 0.1f);

	m_pMonster->Set_IsCheckPhysX(true);
	m_pMonster->Set_IsInvincible(true);

	m_pMonster->Force_BackWard(m_pGameInstance->Get_Random(10.f, 15.f));

	return S_OK;
}

void CState_WhiteZetsu_FlyGround::Update(_float fTimeDelta)
{
	if (m_pMonster->Get_IsEndAnimation(m_iAnimation_SpinGround))
	{
		if (m_pMonster->Get_CurrentHP() <= 0.f)
		{
			m_pMonster->Change_State(CWhiteZetsu::DIE);
			return;
		}
		m_pMonster->Change_State(CWhiteZetsu::GETUP);
	}
}

void CState_WhiteZetsu_FlyGround::End_State()
{
	//m_pMonster->Set_IsCheckPhysX(false);
	m_pMonster->Set_IsInvincible(false);
}

CState_WhiteZetsu_FlyGround* CState_WhiteZetsu_FlyGround::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum)
{
	CState_WhiteZetsu_FlyGround* pInstance = new CState_WhiteZetsu_FlyGround(pFsm, pMonster);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Naruto_Idle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_WhiteZetsu_FlyGround::Free()
{
	__super::Free();
}
