#include "stdafx.h"
#include "Title_Logo.h"

#include "GameInstance.h"

CTitle_Logo::CTitle_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CTitle_Logo::CTitle_Logo(const CTitle_Logo& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CTitle_Logo::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTitle_Logo::Initialize(void* pArg)
{
	UI_DESC			Desc{};

	Desc.fX = (g_iWinSizeX >> 1) - 10.f;
	Desc.fY = (g_iWinSizeY >> 1) - 100.f;
	Desc.fSizeX = g_iWinSizeX * 1.8f;
	Desc.fSizeY = g_iWinSizeY * 1.2f;

	m_vInitSize.x = g_iWinSizeX * 0.9f;
	m_vInitSize.y = g_iWinSizeY * 0.6f;

	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(90.0f);

	m_iDepth = 0;
	m_fAlpha = 1.f;
	m_fRatio = m_vInitSize.x / m_vInitSize.y;

	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CTitle_Logo::Priority_Update(_float fTimeDelta)
{

}

void CTitle_Logo::Update(_float fTimeDelta)
{
	m_fSizeX -= 20000.f * fTimeDelta;
	m_fSizeY = m_fSizeX / m_fRatio;

	if (m_fSizeX <= m_vInitSize.x)
		m_fSizeX = m_vInitSize.x;

	if (m_fSizeY <= m_vInitSize.y)
		m_fSizeY = m_vInitSize.y;
}

void CTitle_Logo::Late_Update(_float fTimeDelta)
{
	/* 직교투영을 위한 월드행렬까지 셋팅하게 된다. */
	__super::Late_Update(fTimeDelta);


	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CTitle_Logo::Render()
{

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTitle_Logo::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_LOGO, TEXT("Prototype_Component_Texture_TitleLogo"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CTitle_Logo* CTitle_Logo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTitle_Logo* pInstance = new CTitle_Logo(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBackGround"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CTitle_Logo::Clone(void* pArg)
{
	CTitle_Logo* pInstance = new CTitle_Logo(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CBackGround"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTitle_Logo::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
