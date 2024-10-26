#include "stdafx.h"
#include "UI_TitleButton.h"

#include "GameInstance.h"

CUI_TitleButton::CUI_TitleButton(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CUI_TitleButton::CUI_TitleButton(const CUI_TitleButton& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CUI_TitleButton::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_TitleButton::Initialize(void* pArg)
{
	UI_DESC			Desc{};

	Desc.fX = g_iWinSizeX >> 1;
	Desc.fY = g_iWinSizeY * 0.8f;
	Desc.fSizeX = 650.f;
	Desc.fSizeY = 170.f;

	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(90.0f);

	m_iDepth = -1;
	m_fAlpha = 1.f;
	m_fTime = 0.f;

	m_vColor = { 0.f,0.f,0.f,1.f };

	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CUI_TitleButton::Priority_Update(_float fTimeDelta)
{

}

void CUI_TitleButton::Update(_float fTimeDelta)
{
	m_fTime += fTimeDelta;
}

void CUI_TitleButton::Late_Update(_float fTimeDelta)
{
	/* 직교투영을 위한 월드행렬까지 셋팅하게 된다. */
	__super::Late_Update(fTimeDelta);


	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_TitleButton::Render()
{

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;


	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	m_vColor = { 1.f ,1.f,1.f, m_fTime - 2.3f };
	if (m_vColor.w >= 1.f)
		m_vColor.w = 1.f;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 1)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(8)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;


	m_vColor = { 1.f ,1.f,1.f, m_fTime - 1.3f };
	if (m_vColor.w >= 1.f)
		m_vColor.w = 1.f;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(8)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_TitleButton::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_LOGO, TEXT("Prototype_Component_Texture_TitleButton"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CUI_TitleButton* CUI_TitleButton::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_TitleButton* pInstance = new CUI_TitleButton(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_TitleButton"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CUI_TitleButton::Clone(void* pArg)
{
	CUI_TitleButton* pInstance = new CUI_TitleButton(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUI_TitleButton"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_TitleButton::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
