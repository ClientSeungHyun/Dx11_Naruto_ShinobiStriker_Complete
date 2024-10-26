#include "stdafx.h"
#include "Effect_GroundBreak.h"

#include "GameInstance.h"
#include "Model.h"

#include "ObjectPool.h"

CEffect_GroundBreak::CEffect_GroundBreak(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect{ pDevice, pContext }
{
}

CEffect_GroundBreak::CEffect_GroundBreak(const CEffect_GroundBreak& Prototype)
	: CEffect{ Prototype }
{
}

HRESULT CEffect_GroundBreak::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CEffect_GroundBreak::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_isActive = false;
	m_vEffectColor = _float4(0.8f, 0.5f, 0.f, 1.f);

	m_pTransformCom->Set_Scaled(1.f, 1.f, 1.f);

	return S_OK;
}

void CEffect_GroundBreak::Priority_Update(_float fTimeDelta)
{
}

void CEffect_GroundBreak::Update(_float fTimeDelta)
{
	m_fCurrentTime += fTimeDelta;
	if (m_fCurrentTime >= 1.f)
	{
		m_vEffectColor.w -= fTimeDelta;
		if (m_vEffectColor.w <= 0.f)
		{
			CObjectPool<CEffect_GroundBreak>::Return_GameObject(this);
			m_isActive = false;
		}
	}

}

void CEffect_GroundBreak::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONLIGHT, this);
}

HRESULT CEffect_GroundBreak::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vEffectColor, sizeof(_float4))))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i);

		m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", TEXTURE_TYPE::NORMALS, (_uint)i);

		if (FAILED(m_pShaderCom->Begin(5)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render((_uint)i)))
			return E_FAIL;
	}

	return S_OK;
}

void CEffect_GroundBreak::Appear(_fvector vPos, _float3 vSize, _float4 vColor)
{
	m_vEffectColor = vColor;
	m_pTransformCom->Set_Scaled(vSize.x, vSize.y, vSize.z);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	m_isActive = true;
}

void CEffect_GroundBreak::DisAppear()
{
	m_isActive = false;
}

HRESULT CEffect_GroundBreak::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Model"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_GroundBreak"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

CEffect_GroundBreak* CEffect_GroundBreak::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffect_GroundBreak* pInstance = new CEffect_GroundBreak(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CFireBall"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CEffect_GroundBreak::Clone(void* pArg)
{
	CEffect_GroundBreak* pInstance = new CEffect_GroundBreak(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CFireBall"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_GroundBreak::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

}
