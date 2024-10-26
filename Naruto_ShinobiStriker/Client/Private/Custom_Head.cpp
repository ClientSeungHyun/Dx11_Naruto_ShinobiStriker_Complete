#include "stdafx.h"
#include "Custom_Head.h"

#include "GameInstance.h"

CCustom_Head::CCustom_Head(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CCustom_Head::CCustom_Head(const CCustom_Head& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CCustom_Head::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCustom_Head::Initialize(void* pArg)
{
	CUSTOMHEAD_DESC* pDesc = static_cast<CUSTOMHEAD_DESC*>(pArg);

	m_strHead = pDesc->strHead;

	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CCustom_Head::Priority_Update(_float fTimeDelta)
{

	for (auto& Child : m_UIChilds)
	{
		Child->Priority_Update(fTimeDelta);
	}
}

void CCustom_Head::Update(_float fTimeDelta)
{
	if (m_isAppear)
		Appear(fTimeDelta);
	else if (m_isDisappear)
		Disappear(fTimeDelta);
}

void CCustom_Head::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	for (auto& Child : m_UIChilds)
	{
		Child->Late_Update(fTimeDelta);
	}

}

HRESULT CCustom_Head::Render()
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

	if (m_fAlpha >= 1.f)
	{
		m_pGameInstance->Render_Text(TEXT("Font_145"), m_strHead.c_str(), XMVectorSet(m_fX - 40.f, m_fY - 10.f, 0.f, 1.f), 
			XMVectorSet(1.f, 0.6f, 0.f, 1.f),0.f, XMVectorSet(0.5f, 0.5f, 0.5f, 1.f), 0.5f);
	}

	return S_OK;
}

void CCustom_Head::Appear(_float fTimeDelta)
{
	m_fAlpha += 17.f * fTimeDelta;

	if (m_fAlpha >= 1.f)
	{
		m_fAlpha = 1.f;
		m_isAppear = false;
	}
}

void CCustom_Head::Disappear(_float fTimeDelta)
{
	m_fAlpha -= 17.f * fTimeDelta;

	if (m_fAlpha <= 0.f)
	{
		m_fAlpha = 0.f;
		m_isDisappear = false;
		m_isActive = false;
	}
}

void CCustom_Head::Start_Appear()
{
	if (!m_isAppear && !m_isDisappear)
	{
		m_fCurrentTime = 0.f;

		m_isAppear = true;
		m_isActive = true;
	}
}

void CCustom_Head::Start_Disappear()
{
	if (!m_isDisappear && !m_isAppear)
	{
		m_fCurrentTime = 0.f;

		m_isDisappear = true;
	}
}

HRESULT CCustom_Head::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_CUSTOM, TEXT("Prototype_Component_Texture_Custom_PannelHead"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CCustom_Head* CCustom_Head::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCustom_Head* pInstance = new CCustom_Head(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CCustom_Head"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CCustom_Head::Clone(void* pArg)
{
	CCustom_Head* pInstance = new CCustom_Head(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CCustom_Head"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCustom_Head::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
