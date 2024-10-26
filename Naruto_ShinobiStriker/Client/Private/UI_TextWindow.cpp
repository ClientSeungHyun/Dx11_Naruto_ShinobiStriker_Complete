#include "stdafx.h"
#include "UI_TextWindow.h"

#include "GameInstance.h"

CUI_TextWindow::CUI_TextWindow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CUI_TextWindow::CUI_TextWindow(const CUI_TextWindow& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CUI_TextWindow::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_TextWindow::Initialize(void* pArg)
{
	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fInitX = 0.f;
	m_fTargetX = 250.f;
	m_fX = 250.f;
	m_fY = 500.f;
	m_fSizeX = 500.f;
	m_fSizeY = 150;

	m_iDepth = 2;

	m_isActive = false;

	return S_OK;
}

void CUI_TextWindow::Priority_Update(_float fTimeDelta)
{

}

void CUI_TextWindow::Update(_float fTimeDelta)
{
	if (m_isAppear)
		Appear(fTimeDelta);
	else if (m_isDisappear)
		Disappear(fTimeDelta);
	else if (m_isChaninge)
		ReAppear(fTimeDelta);

}

void CUI_TextWindow::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_TextWindow::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", m_eType)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(1)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	if (!m_isAppear && !m_isDisappear && !m_isChaninge)
	{
		m_pGameInstance->Render_Text(TEXT("Font_145"), m_strText.c_str(), XMVectorSet(m_fX - m_fSizeX * 0.4f, m_fY - m_fSizeY * 0.15f, 0.f, 1.f),
			XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.5f);
	}

	return S_OK;
}

void CUI_TextWindow::Appear(_float fTimeDelta)
{
	m_fX += 3000.f * fTimeDelta;
	m_fAlpha += fTimeDelta;
	if (m_fX >= m_fTargetX)
	{
		m_fX = m_fTargetX;
		m_fAlpha = 1.f;
		m_isAppear = false;
	}

}

void CUI_TextWindow::Disappear(_float fTimeDelta)
{
	m_fX -= 3000.f * fTimeDelta;
	m_fAlpha -= fTimeDelta;

	if (m_fX <= m_fInitX)
	{
		m_fX = m_fInitX;
		m_fAlpha = 0.f;
		m_isDisappear = false;
		m_isActive = false;
	}

}

void CUI_TextWindow::Start_Appear()
{
	if (m_eType == LEFT)
	{
		if (!m_isAppear && !m_isDisappear)
		{
			m_fAlpha = 0.f;
			m_fX = m_fInitX;
			m_fX = 0.f;
			m_isAppear = true;
			m_isActive = true;
		}
	}
	else
	{
		m_fAlpha = 1.f;
		m_isActive = true;
	}
}

void CUI_TextWindow::Start_Disappear()
{
	if (m_eType == LEFT)
	{
		if (!m_isDisappear && !m_isAppear)
		{
			m_isDisappear = true;
		}
	}
	else
	{
		m_fAlpha = 0.f;
		m_isDisappear = false;
		m_isActive = false;
	}
}

void CUI_TextWindow::Change_Text(_wstring strText)
{
	if (m_eType == LEFT)
	{
		if (m_isChaninge)
			return;

		if (m_isActive)
		{
			m_isChaninge = true;
			m_fChnageTime = 0.f;
		}
	}

	m_strText = strText;
}

void CUI_TextWindow::ReAppear(_float fTimeDelta)
{
	if (m_eType == LEFT)
	{
		m_fChnageTime += fTimeDelta;

		if (m_fChnageTime < 0.2f)
		{
			Disappear(fTimeDelta);
			m_isActive = true;
		}
		else if (m_fChnageTime < 0.4f)
		{
			Appear(fTimeDelta);
		}
		else
		{
			m_isChaninge = false;
		}
	}
}

void CUI_TextWindow::Change_WindowType(TEXTWINDOW_TYPE eType)
{
	if (m_eType == eType)
		return;

	if (eType == CENTER)
	{
		m_fX = 640.f;
		m_fY = 600.f;
		m_fSizeX = 1000.f;
		m_fSizeY = 200;
	}
	else
	{
		m_fInitX = 0.f;
		m_fTargetX = 250.f;
		m_fX = m_fInitX;
		m_fY = 500.f;
		m_fSizeX = 500.f;
		m_fSizeY = 150;
	}

	m_eType = eType;
}

HRESULT CUI_TextWindow::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_TextWindow"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CUI_TextWindow* CUI_TextWindow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_TextWindow* pInstance = new CUI_TextWindow(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_TextWindow"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CUI_TextWindow::Clone(void* pArg)
{
	CUI_TextWindow* pInstance = new CUI_TextWindow(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUI_TextWindow"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_TextWindow::Free()
{
	__super::Free();


	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
