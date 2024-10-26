#include "stdafx.h"
#include "FootTrail.h"

#include "GameInstance.h"

#include "Camera.h"
#include "Model.h"
#include "Player.h"

CFootTrail::CFootTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect{ pDevice, pContext }
{
}

CFootTrail::CFootTrail(const CFootTrail& Prototype)
	: CEffect{ Prototype }
{
}

HRESULT CFootTrail::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CFootTrail::Initialize(void* pArg)
{
	FOOTTRAIL_DESC*		Desc = static_cast<FOOTTRAIL_DESC*>(pArg);
	if (nullptr == Desc)
		return E_FAIL;

	m_pSocketMatrix = Desc->m_pSocketMatrix;
	m_PlayerWorldMatrix = Desc->m_pOwnerWorldMatrix;

	if (FAILED(__super::Initialize(Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_isActive = false;
	m_vEffectColor = _float4(0.03f, 0.7f, 1.f, 0.8f);

	return S_OK;
}

void CFootTrail::Priority_Update(_float fTimeDelta)
{
}

void CFootTrail::Update(_float fTimeDelta)
{
	if (m_isDisappear)
	{
		m_vEffectColor.w -= fTimeDelta;
		if (m_vEffectColor.w <= 0.01f)
		{
			m_vEffectColor.w = 0.f;
			m_pVIBufferCom->Clear_Trail();
			m_isActive = false;
			return;
		}
	}
	m_fAddPointTime += fTimeDelta;


	_matrix FootWroldMatirx = XMLoadFloat4x4(&(*m_pSocketMatrix)) * XMLoadFloat4x4(&(*m_PlayerWorldMatrix));

	if (m_fAddPointTime >= 0.01f)
	{
		m_pVIBufferCom->Add_TrailPoint(FootWroldMatirx);
		m_fAddPointTime = 0.f;
	}
	m_pVIBufferCom->Update_Trail();

}

void CFootTrail::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_EFFECT, this);
}

HRESULT CFootTrail::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vEffectColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CFootTrail::Appear()
{
	m_isDisappear = false;
	m_vEffectColor.w = 0.8f;
	m_isActive = true;
}

void CFootTrail::DisAppear()
{
	m_isDisappear = true;
}

HRESULT CFootTrail::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTrailInstance"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Trail"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;
	m_pVIBufferCom->Set_TrailSize(0.03f);

	return S_OK;
}

CFootTrail* CFootTrail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFootTrail* pInstance = new CFootTrail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CFootTrail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CFootTrail::Clone(void* pArg)
{
	CFootTrail* pInstance = new CFootTrail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CFootTrail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFootTrail::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
}
