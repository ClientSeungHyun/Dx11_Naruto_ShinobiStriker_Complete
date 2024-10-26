#include "stdafx.h"
#include "Rasengan_CreateSmoke.h"

#include "GameInstance.h"

#include "Model.h"

CRasengan_CreateSmoke::CRasengan_CreateSmoke(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect{ pDevice, pContext }
{
}

CRasengan_CreateSmoke::CRasengan_CreateSmoke(const CRasengan_CreateSmoke& Prototype)
	: CEffect{ Prototype }
{
}

HRESULT CRasengan_CreateSmoke::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CRasengan_CreateSmoke::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_isActive = false;
	m_vEffectColor = _float4(0.8f, 0.8f, 0.8f, 0.9f);

	m_pTransformCom->Set_Scaled(1.5f, 1.5f, 1.5f);

	m_fAliveTime = 0.5f;

	return S_OK;
}

void CRasengan_CreateSmoke::Priority_Update(_float fTimeDelta)
{
}

void CRasengan_CreateSmoke::Update(_float fTimeDelta)
{
	m_fCurrentTime += fTimeDelta;

	if (m_fCurrentTime >= m_fAliveTime)
		m_isEnd = true;

	m_fTexCoord -= fTimeDelta;


	if (m_isEnd)
	{
		m_vEffectColor.w -= 3.f * fTimeDelta;
		if (m_vEffectColor.w <= 0.0f)
			m_isActive = false;
	}

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pOwner->Get_Position());
	m_pTransformCom->BillBoard();
}

void CRasengan_CreateSmoke::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONLIGHT, this);
}

HRESULT CRasengan_CreateSmoke::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vEffectColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTexCoordX", &m_fTexCoord, sizeof(_float))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_MaskTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render((_uint)i)))
			return E_FAIL;
	}

	return S_OK;
}

void CRasengan_CreateSmoke::Appear()
{
	if (m_isActive)
		return;

	m_fTexCoord = 3.f;
	m_fCurrentTime = 0.f;
	m_vEffectColor.w = 1.0f;

	m_isEnd = false;
	m_isActive = true;
}

void CRasengan_CreateSmoke::DisAppear()
{
	m_isEnd = true;
}

HRESULT CRasengan_CreateSmoke::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Effect"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Rasengan_CreateSmoke"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

CRasengan_CreateSmoke* CRasengan_CreateSmoke::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRasengan_CreateSmoke* pInstance = new CRasengan_CreateSmoke(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CRasengan_CreateSmoke"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CRasengan_CreateSmoke::Clone(void* pArg)
{
	CRasengan_CreateSmoke* pInstance = new CRasengan_CreateSmoke(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CRasengan_CreateSmoke"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRasengan_CreateSmoke::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

}
