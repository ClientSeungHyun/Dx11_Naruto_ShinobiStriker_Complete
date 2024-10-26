#include "stdafx.h"
#include "State_Custom_GroundPunch.h"
#include "GameInstance.h"
#include "Player_Custom.h"
#include "Model.h"
#include "Collider_PlayerAttack.h"
#include "ObjectPool.h"
#include "Effect_GroundBreak.h"
#include "Smoke11.h"
#include "PlayerCamera.h"

CState_Custom_GroundPunch::CState_Custom_GroundPunch(CFsm* pFsm, CPlayer* pPlayer)
	:CState{ pFsm }
	, m_pPlayer{ pPlayer }
{
}

HRESULT CState_Custom_GroundPunch::Initialize(_uint iStateNum)
{
	m_iAnimation_GroundPunch = m_pPlayer->Get_Model()->Get_AnimationIndex("D34_Attack_GroundPunch", 1.7);
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Custom_GroundPunch::Start_State(void* pArg)
{
	m_pPlayer->SetUp_NextAnimation(m_iAnimation_GroundPunch, false, 0.f);
	m_pPlayer->Find_NearTarget();
	m_pPlayer->Set_IsInvincible(true);

	m_isChase = false;
	m_isSound = false;
	m_isGroundBreak = false;

	return S_OK;
}

void CState_Custom_GroundPunch::Update(_float fTimeDelta)
{
	_int iFrame = m_pPlayer->Get_Frame(m_iAnimation_GroundPunch);

	if (4 <= iFrame && iFrame <= 40)
	{
		if (!m_isSound)
		{
			m_pPlayer->Get_SoundCom()->Play3D(TEXT("Voice_Attack3.wav"), g_fVolume + 0.2f);
			m_isSound = true;
		}

		m_pPlayer->Chase_Target(fTimeDelta, 8.f);
		m_isChase = true;
	}

	if(iFrame == m_iAttackFrame + 2 || iFrame == m_iAttackFrame + 3)
		m_pGameInstance->Find_Camera(m_pPlayer->Get_LevelIndex())->ZoomOut(0.8f);

	if (m_pPlayer->Get_IsEndAnimation(m_iAnimation_GroundPunch))
		m_pPlayer->Change_State(CPlayer_Custom::IDLE);


	Attack_Collider();
}

void CState_Custom_GroundPunch::End_State()
{
	m_pPlayer->Set_IsInvincible(false);
	m_pPlayer->Get_AttackCollider()->IsActive(false);
}

void CState_Custom_GroundPunch::Attack_Collider()
{
	_uint iFrame = m_pPlayer->Get_Frame(m_iAnimation_GroundPunch);


	if (iFrame == m_iAttackFrame || iFrame == m_iAttackFrame + 1)
	{
		if (!m_isGroundBreak)
		{
			_vector vPos = m_pPlayer->Get_Position();
			_vector vLook = XMVector3Normalize(m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_LOOK));
			CObjectPool<CEffect_GroundBreak>::Get_GameObject()->Appear(vPos + vLook, _float3(0.5f,0.5f,0.5f));

			_float3 vSmokePos;
			XMStoreFloat3(&vSmokePos, vPos + vLook);
			for (_uint i = 0; i < 30; ++i)
			{
				CObjectPool<CSmoke11>::Get_GameObject()->Appear(vSmokePos, _float3(1.f, 1.1f, 1.f), 0.3f, CSmoke11::SMOKE_BROWN);
			}

			m_isGroundBreak = true;
		}

		m_pPlayer->Get_AttackCollider()->IsActive(true);
	}
	else
	{
		m_pPlayer->Get_AttackCollider()->IsActive(false);
	}

}

CState_Custom_GroundPunch* CState_Custom_GroundPunch::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Custom_GroundPunch* pInstance = new CState_Custom_GroundPunch(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Custom_GroundPunch"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Custom_GroundPunch::Free()
{
	__super::Free();
}
