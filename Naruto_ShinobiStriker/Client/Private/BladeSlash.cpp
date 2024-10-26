#include "stdafx.h"
#include "BladeSlash.h"

#include "GameInstance.h"

#include "Player.h"
#include "Model.h"
#include "ObjectPool.h"

CBladeSlash::CBladeSlash(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect{ pDevice, pContext }
{
}

CBladeSlash::CBladeSlash(const CBladeSlash& Prototype)
	: CEffect{ Prototype }
{
}

HRESULT CBladeSlash::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CBladeSlash::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_isActive = false;
	m_vEffectColor = _float4(0.68f, 0.23f, 0.89f, 0.6f);

	m_pTransformCom->Set_Scaled(13.f, 1.f, 7.f);

	return S_OK;
}

void CBladeSlash::Priority_Update(_float fTimeDelta)
{
}

void CBladeSlash::Update(_float fTimeDelta)
{
	m_fCurrentTime += fTimeDelta;
	
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPos = vPos + XMLoadFloat3(&m_vMoeDir) * 50.f * fTimeDelta;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
}

void CBladeSlash::Late_Update(_float fTimeDelta)
{
	if (m_fCurrentTime > 4.f)
	{
		m_vEffectColor.w -= fTimeDelta;
		if (m_vEffectColor.w <= 0.f)
		{
			CObjectPool<CBladeSlash>::Return_GameObject(this);
			m_isActive = false;
		}
	}

	m_pGameInstance->Add_ColliderList(m_pColliderCom);

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONLIGHT, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_EFFECT, this);
}

HRESULT CBladeSlash::Render()
{
	m_pTransformCom->Set_Scaled(16.f, 1.f, 9.f);

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vEffectColor, sizeof(_float4))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(9)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render((_uint)i)))
			return E_FAIL;
	}

#ifdef _DEBUG
	m_pColliderCom->Render();
#endif

	return S_OK;
}

void CBladeSlash::OnCollisionEnter(CGameObject* pOther)
{
	if (pOther->Get_LayerTag() == TEXT("Layer_Player"))
	{
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(pOther);
		if (pPlayer->Get_IsInvincible())
			return;

		_float2 vForce = _float2(15.f, 5.f);
		pPlayer->Damaged(10.f);
		pPlayer->Change_State(CPlayer::HIT_SPIN, &vForce);
	}
}

void CBladeSlash::OnCollisionStay(CGameObject* pOther)
{
}

void CBladeSlash::OnCollisionExit(CGameObject* pOther)
{
}

void CBladeSlash::Appear(_fvector vInitPos, _fvector vTargetPos)
{
	m_fCurrentTime = 0.f;
	m_vEffectColor.w = 1.0f;
	m_isActive = true;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vInitPos);
	m_pTransformCom->LookAt(vTargetPos);

	_vector vMoveDir = XMVector3Normalize(vTargetPos - vInitPos);
	XMStoreFloat3(&m_vMoeDir, vMoveDir);
}

void CBladeSlash::DisAppear()
{
	m_isActive = false;
}

HRESULT CBladeSlash::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Effect"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_SusanooSlash"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	_float3 vInitPos = {};
	XMStoreFloat3(&vInitPos, Get_Position());

	/* For.Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC			ColliderDesc_OBB{};
	ColliderDesc_OBB.vExtents = _float3(0.3f, 0.2f, 0.25f);
	ColliderDesc_OBB.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_Body"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc_OBB)))
		return E_FAIL;
	m_pColliderCom->Set_Owner(this);

	return S_OK;
}

CBladeSlash* CBladeSlash::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBladeSlash* pInstance = new CBladeSlash(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBladeSlash"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CBladeSlash::Clone(void* pArg)
{
	CBladeSlash* pInstance = new CBladeSlash(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CBladeSlash"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBladeSlash::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

}
