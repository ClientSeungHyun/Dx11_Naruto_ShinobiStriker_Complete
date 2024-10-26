#include "stdafx.h"
#include "UI_BossTarget.h"

#include "Monster.h"
#include "GameInstance.h"

CUI_BossTarget::CUI_BossTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CUI_BossTarget::CUI_BossTarget(const CUI_BossTarget& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CUI_BossTarget::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_BossTarget::Initialize(void* pArg)
{
	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_iDepth = 2;
	m_fAlpha = 1.f;

	m_pTransformCom->Set_Scaled(2.f, 2.f, 1.f);

	m_fSizeX = 100.f;
	m_fSizeY = 100.f;

	m_isActive = false;

	return S_OK;
}

void CUI_BossTarget::Priority_Update(_float fTimeDelta)
{

}

void CUI_BossTarget::Update(_float fTimeDelta)
{

}

void CUI_BossTarget::Late_Update(_float fTimeDelta)
{
	if (m_pMonster == nullptr || m_pMonster->Get_Fsm()->Get_CurrentState() == CMonster::DIE)
	{
		m_pMonster = nullptr;
		return;
	}

	_float2 vPos = m_pMonster->Calculate_ScreenPosition_ClampSide(6.5f);
	m_fX = vPos.x;
	m_fY = vPos.y;

	__super::Late_Update(fTimeDelta);

	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_BossTarget::Render()
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

	if (FAILED(m_pShaderCom->Begin(9)))
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

void CUI_BossTarget::Appear(_float fTimeDelta)
{
	m_isAppear = false;
}

void CUI_BossTarget::Disappear(_float fTimeDelta)
{
	m_isActive = false;
}

void CUI_BossTarget::Start_Appear()
{

}

void CUI_BossTarget::Start_Disappear()
{

}

void CUI_BossTarget::Damaged()
{

}

HRESULT CUI_BossTarget::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BossTarget"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CUI_BossTarget* CUI_BossTarget::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_BossTarget* pInstance = new CUI_BossTarget(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_BossTarget"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CUI_BossTarget::Clone(void* pArg)
{
	CUI_BossTarget* pInstance = new CUI_BossTarget(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUI_BossTarget"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_BossTarget::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
