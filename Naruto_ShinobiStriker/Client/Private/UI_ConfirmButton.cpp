#include "stdafx.h"
#include "UI_ConfirmButton.h"
#include "CustomPannel.h"

#include "GameInstance.h"

CUI_ConfirmButton::CUI_ConfirmButton(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CUI_ConfirmButton::CUI_ConfirmButton(const CUI_ConfirmButton& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CUI_ConfirmButton::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_ConfirmButton::Initialize(void* pArg)
{
	BUTTON_DESC* ButtonDesc = static_cast<BUTTON_DESC*>(pArg);
	m_iButtonIndex = ButtonDesc->iButtonIndex;
	m_isSelect = false;
	m_iDepth = 1;

	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(ButtonDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fInitX = m_fX;
	m_fAlpha = 1.f;

	return S_OK;
}

void CUI_ConfirmButton::Priority_Update(_float fTimeDelta)
{
}

void CUI_ConfirmButton::Update(_float fTimeDelta)
{
	if (m_isAppear)
		Appear(fTimeDelta);
	else if (m_isDisappear)
		Disappear(fTimeDelta);
}

void CUI_ConfirmButton::Late_Update(_float fTimeDelta)
{
	/* 직교투영을 위한 월드행렬까지 셋팅하게 된다. */
	__super::Late_Update(fTimeDelta);

	m_fX = 340.f;
	m_fY = 670.f;
	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_ConfirmButton::Render()
{

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", m_isSelect)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(1)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	if (m_fAlpha >= 1.f)
	{
		m_pGameInstance->Render_TextCenter(TEXT("Font_145"), TEXT("결정"), XMVectorSet(m_fX, m_fY, 0.f, 1.f),
			XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.5f);
	}


	return S_OK;
}

void CUI_ConfirmButton::Appear(_float fTimeDleta)
{
	m_fX += 1400 * fTimeDleta;
	m_fAlpha += 15.f * fTimeDleta;
	if (m_fX >= m_fInitX)
	{
		m_fX = m_fInitX;

		if (m_fAlpha >= 1.f)
		{
			m_fAlpha = 1.f;
			m_isAppear = false;
		}
	}
}

void CUI_ConfirmButton::Disappear(_float fTimeDleta)
{
	m_fX -= 1200 * fTimeDleta;
	m_fAlpha -= 15.f * fTimeDleta;

	if (m_fAlpha <= 0.f)
	{
		m_fAlpha = 0.f;
		m_fX = m_fInitX - 400.f;
		m_isActive = false;
		m_isDisappear = false;
	}
}

void CUI_ConfirmButton::Start_Appear()
{
	m_isActive = true;
	m_isAppear = true;
}

void CUI_ConfirmButton::Start_Disappear()
{
	m_isDisappear = true;
}

HRESULT CUI_ConfirmButton::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_CUSTOM, TEXT("Prototype_Component_Texture_Custom_ConfirmButton"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CUI_ConfirmButton* CUI_ConfirmButton::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_ConfirmButton* pInstance = new CUI_ConfirmButton(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_ConfirmButton"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CUI_ConfirmButton::Clone(void* pArg)
{
	CUI_ConfirmButton* pInstance = new CUI_ConfirmButton(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUI_ConfirmButton"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_ConfirmButton::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
