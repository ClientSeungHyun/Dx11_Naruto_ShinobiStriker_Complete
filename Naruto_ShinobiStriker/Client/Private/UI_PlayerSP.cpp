#include "stdafx.h"
#include "UI_PlayerSP.h"

#include "GameInstance.h"

CUI_PlayerSP::CUI_PlayerSP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CUI_PlayerSP::CUI_PlayerSP(const CUI_PlayerSP& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CUI_PlayerSP::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_PlayerSP::Initialize(void* pArg)
{

	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_iDepth = 1;
	m_fAlpha = 0.f;

	return S_OK;
}

void CUI_PlayerSP::Priority_Update(_float fTimeDelta)
{

	for (auto& Child : m_UIChilds)
	{
		Child->Priority_Update(fTimeDelta);
	}
}

void CUI_PlayerSP::Update(_float fTimeDelta)
{
	if (m_isAppear)
		Appear(fTimeDelta);
	else if (m_isDisappear)
		Disappear(fTimeDelta);
}

void CUI_PlayerSP::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	for (auto& Child : m_UIChilds)
	{
		Child->Late_Update(fTimeDelta);
	}

	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_PlayerSP::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(1)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	for (auto& Child : m_UIChilds)
	{
		Child->Render();
	}

	return S_OK;
}

void CUI_PlayerSP::Appear(_float fTimeDelta)
{
	m_fAlpha += fTimeDelta;

	if (m_fAlpha >= 1.f)
	{
		m_fAlpha = 1.f;
		m_isAppear = false;
	}
}

void CUI_PlayerSP::Disappear(_float fTimeDelta)
{
	m_fAlpha -= 5.f * fTimeDelta;

	if (m_fAlpha <= 0.f)
	{
		m_fAlpha = 0.f;
		m_isDisappear = false;
		m_isActive = false;
	}
}

void CUI_PlayerSP::Start_Appear()
{
	if (!m_isAppear && !m_isDisappear)
	{
		m_fAlpha = 0.f;
		m_isAppear = true;
		m_isActive = true;
	}
}

void CUI_PlayerSP::Start_Disappear()
{
	if (!m_isDisappear && !m_isAppear)
	{

		m_isDisappear = true;
	}
}

HRESULT CUI_PlayerSP::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SPBase"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CUI_PlayerSP* CUI_PlayerSP::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_PlayerSP* pInstance = new CUI_PlayerSP(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_PlayerSP"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CUI_PlayerSP::Clone(void* pArg)
{
	CUI_PlayerSP* pInstance = new CUI_PlayerSP(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUI_PlayerSP"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_PlayerSP::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
