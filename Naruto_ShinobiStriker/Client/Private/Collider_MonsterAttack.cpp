#include "stdafx.h"
#include "ColliderObject.h"

#include "GameInstance.h"
#include "Collider_PlayerAttack.h"
#include "Collider_MonsterAttack.h"
#include "Player.h"

#include "Boss_Kurama.h"
#include "Boss_Susanoo.h"
#include "WhiteZetsu.h"

CCollider_MonsterAttack::CCollider_MonsterAttack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CColliderObject{ pDevice, pContext }
{
}

HRESULT CCollider_MonsterAttack::Initialize(void* pArg)
{
	/* 직교투영을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_strLayerTag = TEXT("Layer_MonsterAttack");
	m_isActive = false;

	m_eMonsterID = dynamic_cast<CMonster*>(m_pOwner)->Get_MonsterID();

	return S_OK;
}

void CCollider_MonsterAttack::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CCollider_MonsterAttack::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	m_pTransformCom->Set_WorldMatrix(m_pOwner->Get_Transform()->Get_WorldMatrix());
}

void CCollider_MonsterAttack::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CCollider_MonsterAttack::Render()
{
	__super::Render();

	return S_OK;
}

void CCollider_MonsterAttack::OnCollisionEnter(CGameObject* pOther)
{
	if (pOther->Get_LayerTag() == TEXT("Layer_Player"))
	{
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(pOther);
		if (nullptr == pPlayer || pPlayer->Get_IsInvincible())
			return;


		if (m_eMonsterID == KURAMA)
		{
			CBoss_Kurama* pKurama = dynamic_cast<CBoss_Kurama*>(m_pOwner);

			_float2 vForce = _float2(10.f, 8.f);
			if (pKurama->Get_IsStrongAttack())
			{
				vForce = _float2(30.f, 15.f);
			}

			pPlayer->Change_State(CPlayer::HIT_SPIN, &vForce);

		}
		else if (m_eMonsterID == SUSANOO)
		{
			CBoss_Susanoo* pSusanoo = dynamic_cast<CBoss_Susanoo*>(m_pOwner);

			_float2 vForce = _float2(25.f, 13.f);

			pPlayer->Change_State(CPlayer::HIT_SPIN, &vForce);
		}
		else if (m_eMonsterID == WHITEZETSU)
		{
			CWhiteZetsu* pWhiteZetsu = dynamic_cast<CWhiteZetsu*>(m_pOwner);

			pPlayer->Damaged(3.f);
			pPlayer->Change_State(CPlayer::HIT1);
		}
	}
}

void CCollider_MonsterAttack::OnCollisionStay(CGameObject* pOther)
{
}

void CCollider_MonsterAttack::OnCollisionExit(CGameObject* pOther)
{
}

CCollider_MonsterAttack* CCollider_MonsterAttack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CCollider_MonsterAttack* pInstance = new CCollider_MonsterAttack(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CCollider_PlayerAttack"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollider_MonsterAttack::Free()
{
	__super::Free();
}
