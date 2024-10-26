#include "stdafx.h"
#include "TenTail.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"

#include "State_TenTail_Appear.h"
#include "State_TenTail_Idle.h"
#include "State_TenTail_Attack.h"

CTenTail::CTenTail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster{ pDevice, pContext }
{
	m_eMonsterID = TENTAIL;
}

CTenTail::CTenTail(const CTenTail& Prototype)
	: CMonster{ Prototype }
{
	m_eMonsterID = TENTAIL;
}

HRESULT CTenTail::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTenTail::Initialize(void* pArg)
{
	/* 직교투영을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (FAILED(Ready_State()))
		return E_FAIL;

	m_pRigidBodyCom->Set_IsGravity(false);

	m_eMonsterID = TENTAIL;

	return S_OK;
}

void CTenTail::Priority_Update(_float fTimeDelta)
{
	if (KEY_TAP(KEY::X))
		m_pRigidBodyCom->Set_IsGravity(true);

	//m_pRigidBodyCom->Update(fTimeDelta);
}

void CTenTail::Update(_float fTimeDelta)
{
	m_pFsmCom->Update(fTimeDelta);
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());

	m_pModelCom->Play_Animation(fTimeDelta);
}

void CTenTail::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_ColliderList(m_pColliderCom);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CTenTail::Render()
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

HRESULT CTenTail::Ready_Components(void* pArg)
{
	__super::Ready_Components(pArg);

	/* FOR.Com_RigidBody */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBodyCom))))
		return E_FAIL;
	m_pRigidBodyCom->SetOwner(this);

	/* For.Com_Collider_Body */
	CBounding_OBB::BOUNDING_OBB_DESC			ColliderDesc_OBB{};
	ColliderDesc_OBB.vExtents = _float3(0.3f, 0.5f, 0.25f);
	ColliderDesc_OBB.vCenter = _float3(0.f, 0.7f, 0.1f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_Body"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc_OBB)))
		return E_FAIL;
	m_pColliderCom->Set_Owner(this);

	return S_OK;
}

HRESULT CTenTail::Ready_State()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Fsm"),
		TEXT("Com_Fsm"), reinterpret_cast<CComponent**>(&m_pFsmCom), this)))
		return E_FAIL;

	m_pFsmCom->Add_State(CState_TenTail_Appear::Create(m_pFsmCom, this, APPEAR));
	m_pFsmCom->Add_State(CState_TenTail_Idle::Create(m_pFsmCom, this, IDLE));
	m_pFsmCom->Add_State(CState_TenTail_Attack::Create(m_pFsmCom, this, ATTACK1));

	m_pFsmCom->Set_State(APPEAR);

	return S_OK;
}

CTenTail* CTenTail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTenTail* pInstance = new CTenTail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CTenTail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CTenTail::Clone(void* pArg)
{
	CTenTail* pInstance = new CTenTail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CTenTail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTenTail::Free()
{
	__super::Free();
}
