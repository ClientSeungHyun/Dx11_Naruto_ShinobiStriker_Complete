#include "stdafx.h"
#include "ChidoriTrail.h"

#include "GameInstance.h"

#include "Camera.h"
#include "Model.h"
#include "Player.h"

CChidoriTrail::CChidoriTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect{ pDevice, pContext }
{
}

CChidoriTrail::CChidoriTrail(const CChidoriTrail& Prototype)
	: CEffect{ Prototype }
{
}

HRESULT CChidoriTrail::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CChidoriTrail::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_isActive = false;
	m_vEffectColor = _float4(0.07f, 0.36f, 0.91f, 1.f);

	return S_OK;
}

void CChidoriTrail::Priority_Update(_float fTimeDelta)
{
}

void CChidoriTrail::Update(_float fTimeDelta)
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

	m_pVIBufferCom->Set_TrailSize(m_fTrailSize);
	m_pVIBufferCom->Add_TrailPoint(XMLoadFloat3(&m_vTrailPos), m_iNumTrail);
	m_pVIBufferCom->Update_Trail();

}

void CChidoriTrail::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_EFFECT, this);
}

HRESULT CChidoriTrail::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vEffectColor, sizeof(_float4))))
		return E_FAIL;

	_float3 vWorldPos;
	XMStoreFloat3(&vWorldPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vWorldPos", &m_vTrailPos, sizeof(_float3))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CChidoriTrail::Appear(_float4 vColor, _uint iNumTrail)
{
	if (m_isActive)
		return;

	m_isDisappear = false;
	m_vEffectColor = vColor;
	m_iNumTrail = iNumTrail;

	m_isActive = true;
}

void CChidoriTrail::DisAppear()
{
	m_isDisappear = true;
	m_pVIBufferCom->Clear_Trail();
}

void CChidoriTrail::Clear()
{
	m_pVIBufferCom->Clear_Trail();
}

HRESULT CChidoriTrail::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTrailInstance"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Trail"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;
	m_pVIBufferCom->Set_TrailSize(m_fTrailSize);

	return S_OK;
}

CChidoriTrail* CChidoriTrail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CChidoriTrail* pInstance = new CChidoriTrail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CChidoriTrail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CChidoriTrail::Clone(void* pArg)
{
	CChidoriTrail* pInstance = new CChidoriTrail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CFootTrail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChidoriTrail::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
}
