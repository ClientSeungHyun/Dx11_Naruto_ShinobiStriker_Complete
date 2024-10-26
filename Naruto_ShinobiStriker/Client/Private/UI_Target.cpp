#include "stdafx.h"
#include "UI_Target.h"

#include "Monster.h"
#include "GameInstance.h"

CUI_Target::CUI_Target(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CUI_Target::CUI_Target(const CUI_Target& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CUI_Target::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Target::Initialize(void* pArg)
{
	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_iDepth = 2;
	m_fAlpha = 1.f;

	m_pTransformCom->Set_Scaled(2.f, 2.f, 1.f);

	m_fSizeX = 160.f;
	m_fSizeY = 160.f;

	m_isActive = true;

	return S_OK;
}

void CUI_Target::Priority_Update(_float fTimeDelta)
{
	if (m_pMonster == nullptr || m_pMonster->Get_Fsm()->Get_CurrentState() == CMonster::DIE
		|| m_pMonster->Get_MonsterID() == KURAMA || m_pMonster->Get_MonsterID() == SUSANOO)
	{
		m_pMonster = nullptr;
		return;
	}

}

void CUI_Target::Update(_float fTimeDelta)
{
	if (m_pMonster == nullptr || m_pMonster->Get_Fsm()->Get_CurrentState() == CMonster::DIE
		|| m_pMonster->Get_MonsterID() == KURAMA || m_pMonster->Get_MonsterID() == SUSANOO)
	{
		m_pMonster = nullptr;
		return;
	}

}

void CUI_Target::Late_Update(_float fTimeDelta)
{
	if (m_pMonster == nullptr || m_pMonster->Get_Fsm()->Get_CurrentState() == CMonster::DIE
		|| m_pMonster->Get_MonsterID() == KURAMA || m_pMonster->Get_MonsterID() == SUSANOO)
	{
		m_pMonster = nullptr;
		return;
	}

	_float2 vPos = m_pMonster->Calculate_ProjPosition(1.f);
	m_fX = vPos.x;
	m_fY = vPos.y;

	__super::Late_Update(fTimeDelta);

	if (m_fX == -1.f || m_fY == -1.f)
		return;

	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_Target::Render()
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

	if (FAILED(m_pShaderCom->Begin(6)))
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

void CUI_Target::Appear(_float fTimeDelta)
{
	m_isAppear = false;
}

void CUI_Target::Disappear(_float fTimeDelta)
{
	m_isActive = false;
}

void CUI_Target::Start_Appear()
{

}

void CUI_Target::Start_Disappear()
{

}

void CUI_Target::Damaged()
{

}

HRESULT CUI_Target::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_TargetCursol"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CUI_Target* CUI_Target::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Target* pInstance = new CUI_Target(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_Target"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CUI_Target::Clone(void* pArg)
{
	CUI_Target* pInstance = new CUI_Target(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUI_Target"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Target::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
