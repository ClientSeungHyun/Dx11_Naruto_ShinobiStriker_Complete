#include "stdafx.h"
#include "State_WhiteZetsu_Chase.h"
#include "GameInstance.h"
#include "WhiteZetsu.h"
#include "Model.h"


CState_WhiteZetsu_Chase::CState_WhiteZetsu_Chase(CFsm* pFsm, CMonster* pMonster)
	:CState{ pFsm }
	, m_pMonster{ pMonster }
{
}

HRESULT CState_WhiteZetsu_Chase::Initialize(_uint iStateNum)
{
	m_iAnimation_RunLoop = m_pMonster->Get_Model()->Get_AnimationIndex("WhiteZetsuCrowdForm_Run_Loop");
	m_iStateNum = iStateNum;
	m_fMoveSpeed = 3.f;

	return S_OK;
}

HRESULT CState_WhiteZetsu_Chase::Start_State(void* pArg)
{
	m_pMonster->SetUp_NextAnimation(m_iAnimation_RunLoop, true);

	return S_OK;
}

void CState_WhiteZetsu_Chase::Update(_float fTimeDelta)
{
	CTransform* pTransform = m_pMonster->Get_Transform();
	_vector vTargetPos = XMVectorSetY(m_pMonster->Get_PlayerTarget()->Get_Position(), 0.f);
	_vector vPos = XMVectorSetY(pTransform->Get_State(CTransform::STATE_POSITION),0.f);

	_vector vChaserDir = vTargetPos - vPos;

	_float fDistance = -1.f;
	XMStoreFloat(&fDistance, XMVector3Length(vChaserDir));

	if (fDistance <= m_pGameInstance->Get_Random(0.8f, 1.8f))
	{
		m_pMonster->Change_State(CWhiteZetsu::ATTACK1);
		return;
	}

	vChaserDir = XMVector3Normalize(vChaserDir);

	m_pMonster->Move_Dir(vChaserDir, m_fMoveSpeed, fTimeDelta);
}

void CState_WhiteZetsu_Chase::End_State()
{
}

CState_WhiteZetsu_Chase* CState_WhiteZetsu_Chase::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum)
{
	CState_WhiteZetsu_Chase* pInstance = new CState_WhiteZetsu_Chase(pFsm, pMonster);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_WhiteZetsu_Chase"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_WhiteZetsu_Chase::Free()
{
	__super::Free();
}
