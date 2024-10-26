#include "stdafx.h"
#include "State_Kurama_JumpCrush.h"
#include "GameInstance.h"
#include "Boss_Kurama.h"
#include "Model.h"
#include "ColliderObject.h"
#include "ObjectPool.h"
#include "Smoke11.h"

CState_Kurama_JumpCrush::CState_Kurama_JumpCrush(CFsm* pFsm, CMonster* pMonster)
	:CState{ pFsm }
	, m_pMonster{ pMonster }
{
}

HRESULT CState_Kurama_JumpCrush::Initialize(_uint iStateNum)
{
	m_iAnimation_JumpCrush = m_pMonster->Get_Model()->Get_AnimationIndex("Kurama_Attack_JumpCrush", 2.0);
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Kurama_JumpCrush::Start_State(void* pArg)
{
	m_pMonster->SetUp_NextAnimation(m_iAnimation_JumpCrush, false);
	m_isAppearSmoke = false;

	return S_OK;
}

void CState_Kurama_JumpCrush::Update(_float fTimeDelta)
{
	if(m_pMonster->Get_IsEndAnimation(m_iAnimation_JumpCrush))
		m_pMonster->Change_State(CBoss_Kurama::IDLE);

	Attack_Collider();
}

void CState_Kurama_JumpCrush::End_State()
{
	m_pMonster->Set_IsStrongAttack(false);
}

void CState_Kurama_JumpCrush::Attack_Collider()
{
	_uint iFrame = m_pMonster->Get_Frame(m_iAnimation_JumpCrush);

	if (iFrame == m_iAttackFrame || iFrame == m_iAttackFrame + 1 || iFrame == m_iAttackFrame + 2)
	{
		if (!m_isAppearSmoke)
		{
			_float3 vPos;
			XMStoreFloat3(&vPos, m_pMonster->Get_Position());

			for (_uint i = 0; i < 40; ++i)
			{
				CObjectPool<CSmoke11>::Get_GameObject()->Appear(vPos, _float3(3.f, 1.5f, 3.f), 1.5f);
			}

			m_pMonster->Get_SoundCom()->Play3D(TEXT("Effect_BossLand.wav"), g_fVolume + 0.2f);
			m_isAppearSmoke = true;
		}
		m_pMonster->Set_IsStrongAttack(true);
		m_pMonster->Get_AttackCollider()->IsActive(true);
	}
	else
	{
		m_pMonster->Set_IsStrongAttack(false);
		m_pMonster->Get_AttackCollider()->IsActive(false);
	}

}

CState_Kurama_JumpCrush* CState_Kurama_JumpCrush::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum)
{
	CState_Kurama_JumpCrush* pInstance = new CState_Kurama_JumpCrush(pFsm, pMonster);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Kurama_Idle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Kurama_JumpCrush::Free()
{
	__super::Free();
}
