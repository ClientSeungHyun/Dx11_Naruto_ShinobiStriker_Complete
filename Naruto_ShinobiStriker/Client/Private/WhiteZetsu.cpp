#include "stdafx.h"
#include "WhiteZetsu.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "Player_Custom.h"
#include "Collider_PlayerAttack.h"
#include "Collider_MonsterAttack.h"

#include "State_WhiteZetsu_Appear.h"
#include "State_WhiteZetsu_Idle.h"
#include "State_WhiteZetsu_Chase.h"
#include "State_WhiteZetsu_Attack1.h"
#include "State_WhiteZetsu_AttackFar.h"
#include "State_WhiteZetsu_FlyAway.h"
#include "State_WhiteZetsu_Fall.h"
#include "State_WhiteZetsu_Hit1.h"
#include "State_WhiteZetsu_Hit2.h"
#include "State_WhiteZetsu_FlyGround.h"
#include "State_WhiteZetsu_GetUp.h"
#include "State_WhiteZetsu_Die.h"
#include "State_WhiteZestu_BackDash.h"
#include "State_WhiteZetsu_Groggy.h"
#include "State_WhiteZetsu_HitDown.h"

CWhiteZetsu::CWhiteZetsu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster{ pDevice, pContext }
{
	m_eMonsterID = WHITEZETSU;
}

CWhiteZetsu::CWhiteZetsu(const CWhiteZetsu& Prototype)
	: CMonster{ Prototype }
{
	m_eMonsterID = WHITEZETSU;
}

HRESULT CWhiteZetsu::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWhiteZetsu::Initialize(void* pArg)
{
	/* 직교투영을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (FAILED(Ready_State()))
		return E_FAIL;

	if (FAILED(__super::Ready_UI()))
		return E_FAIL;

	m_pRigidBodyCom->Set_IsGravity(false);
	
	m_eMonsterID = WHITEZETSU;

	return S_OK;
}

void CWhiteZetsu::Priority_Update(_float fTimeDelta)
{
	if (KEY_TAP(KEY::X))
		m_pRigidBodyCom->Set_IsGravity(true);

	//m_pRigidBodyCom->Update(fTimeDelta);
}

void CWhiteZetsu::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
}

void CWhiteZetsu::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pGameInstance->Add_ColliderList(m_pColliderCom);

}

HRESULT CWhiteZetsu::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render((_uint)i)))
			return E_FAIL;
	}

#ifdef _DEBUG
	m_pColliderCom->Render();
#endif

	return S_OK;
}

void CWhiteZetsu::OnCollisionEnter(CGameObject* pOther)
{
	
}

void CWhiteZetsu::OnCollisionStay(CGameObject* pOther)
{
}

void CWhiteZetsu::OnCollisionExit(CGameObject* pOther)
{
}

void CWhiteZetsu::Damaged_To_Player(_uint iState, _float fDamage, _bool isStrongAttack)
{
	if (m_pFsmCom->Get_CurrentState() == DIE)
		return;

	if (m_fCurrentHP <= 0.f && !isStrongAttack && fDamage < 20.f)
	{
		m_pFsmCom->Change_State(DIE);
		return;
	}

	if (iState == CPlayer_Custom::CHIDORI_ATTACK || iState == CPlayer_Custom::AERIAL_CHIDORI_ATTACK || fDamage >= 20.f)
	{
		m_pFsmCom->Change_State(HIT_FLYAWAY);
	}
	else if (isStrongAttack)
	{
		m_pFsmCom->Change_State(HIT_FLYGROUND);
		m_fCurrentHP -= 15.f;
	}
	else
	{
		switch (m_pFsmCom->Get_CurrentState())
		{
		case HIT1:
			m_pFsmCom->Change_State(HIT2);
			break;
		case HIT2:
			m_pFsmCom->Change_State(HIT2);
			break;
		default:
			m_pFsmCom->Change_State(HIT1);
			break;
		}
	}

	m_fCurrentHP -= fDamage;
}

void CWhiteZetsu::Change_State_GeneralHit()
{
	_uint iRand = rand() % 2;
	switch (iRand)
	{
	case 0 :
		Change_State(HIT1);
		break;

	case 1:
		Change_State(HIT2);
		break;
	}
}

void CWhiteZetsu::Change_State_FlyAway()
{
	Change_State(HIT_FLYAWAY);
}

void CWhiteZetsu::Change_State_FlyGround()
{
	Change_State(HIT_FLYGROUND);
}

void CWhiteZetsu::Change_State_Groggy()
{
	m_pFsmCom->Change_State(GROGGY);
}

void CWhiteZetsu::Change_State_HitDown()
{
	m_pFsmCom->Change_State(HIT_DOWN);
}

HRESULT CWhiteZetsu::Ready_Components(void* pArg)
{
	__super::Ready_Components(pArg);

	/* For.Com_Collider_Body */
	CBounding_OBB::BOUNDING_OBB_DESC			ColliderDesc_OBB{};
	ColliderDesc_OBB.vExtents = _float3(0.3f, 0.5f, 0.25f);
	ColliderDesc_OBB.vCenter = _float3(0.f, 0.7f, 0.1f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_Body"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc_OBB)))
		return E_FAIL;
	m_pColliderCom->Set_Owner(this);

	CColliderObject::OBB_COLLIDEROBJ_DESC	ColliderObj_Desc{};
	ColliderObj_Desc.pOwner = this;
	ColliderObj_Desc.Desc_OBB.vExtents = _float3(0.4f, 0.6f, 0.5f);
	ColliderObj_Desc.Desc_OBB.vCenter = _float3(0.f, 0.7f, 0.6f);
	ColliderObj_Desc.fRotationPerSec = 0.f;
	ColliderObj_Desc.fSpeedPerSec = 0.f;
	m_pCollider_Attack = CCollider_MonsterAttack::Create(m_pDevice, m_pContext, &ColliderObj_Desc);

	return S_OK;
}

HRESULT CWhiteZetsu::Ready_State()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Fsm"),
		TEXT("Com_Fsm"), reinterpret_cast<CComponent**>(&m_pFsmCom), this)))
		return E_FAIL;

	m_pFsmCom->Add_State(CState_WhiteZetsu_Appear::Create(m_pFsmCom, this, APPEAR));
	m_pFsmCom->Add_State(CState_WhiteZetsu_Idle::Create(m_pFsmCom, this, IDLE));
	m_pFsmCom->Add_State(CState_WhiteZetsu_Chase::Create(m_pFsmCom, this, CHASE));
	m_pFsmCom->Add_State(CState_WhiteZetsu_Attack1::Create(m_pFsmCom, this, ATTACK1));
	m_pFsmCom->Add_State(CState_WhiteZetsu_AttackFar::Create(m_pFsmCom, this, ATTACK_FAR));
	m_pFsmCom->Add_State(CState_WhiteZetsu_FlyAway ::Create(m_pFsmCom, this, HIT_FLYAWAY));
	m_pFsmCom->Add_State(CState_WhiteZetsu_Fall::Create(m_pFsmCom, this, FALL));
	m_pFsmCom->Add_State(CState_WhiteZetsu_GetUp::Create(m_pFsmCom, this, GETUP));
	m_pFsmCom->Add_State(CState_WhiteZetsu_Hit1::Create(m_pFsmCom, this, HIT1));
	m_pFsmCom->Add_State(CState_WhiteZetsu_Hit2::Create(m_pFsmCom, this, HIT2));
	m_pFsmCom->Add_State(CState_WhiteZetsu_FlyGround::Create(m_pFsmCom, this, HIT_FLYGROUND));
	m_pFsmCom->Add_State(CState_WhiteZetsu_Die::Create(m_pFsmCom, this, DIE));
	m_pFsmCom->Add_State(CState_WhiteZetsu_BackDash::Create(m_pFsmCom, this, BACKDASH));
	m_pFsmCom->Add_State(CState_WhiteZetsu_Groggy::Create(m_pFsmCom, this, GROGGY));
	m_pFsmCom->Add_State(CState_WhiteZetsu_HitDown::Create(m_pFsmCom, this, HIT_DOWN));

	m_pFsmCom->Set_State(APPEAR);

	return S_OK;
}

CWhiteZetsu* CWhiteZetsu::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWhiteZetsu* pInstance = new CWhiteZetsu(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CWhiteZetsu"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CWhiteZetsu::Clone(void* pArg)
{
	CWhiteZetsu* pInstance = new CWhiteZetsu(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CWhiteZetsu"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWhiteZetsu::Free()
{
	__super::Free();
}
