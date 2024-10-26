#include "stdafx.h"
#include "UI_PlayerSkill.h"

#include "GameInstance.h"

CUI_PlayerSkill::CUI_PlayerSkill(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CUI_PlayerSkill::CUI_PlayerSkill(const CUI_PlayerSkill& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CUI_PlayerSkill::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_PlayerSkill::Initialize(void* pArg)
{
	SKILL_DESC* pDesc = static_cast<SKILL_DESC*>(pArg);
	m_isBaseSkill = pDesc->isBaseSkill;

	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_iDepth = 2;

	m_isActive = false;

	return S_OK;
}

void CUI_PlayerSkill::Priority_Update(_float fTimeDelta)
{

	for (auto& Child : m_UIChilds)
	{
		Child->Priority_Update(fTimeDelta);
	}
}

void CUI_PlayerSkill::Update(_float fTimeDelta)
{
	if (*m_pCoolTime <= 0.f)
	{
		m_fTime += 1.5f * fTimeDelta;
		if (sinf(m_fTime) >= 0.99f)
			m_fTime = -1.6f;
	}
	else
	{
		m_fTime = -1.6f;
	}

	_float fSin = sinf(m_fTime);
	//if ((*m_pCoolTime) > 0.f || m_fTime > 4.0f)
	//	m_fTime = 0.f;

	if (m_isAppear)
		Appear(fTimeDelta);
	else if (m_isDisappear)
		Disappear(fTimeDelta);

	m_TextCoord.y = (*m_pCoolTime) / 10.f;

}

void CUI_PlayerSkill::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	for (auto& Child : m_UIChilds)
	{
		Child->Late_Update(fTimeDelta);
	}

	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_PlayerSkill::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (m_isBaseSkill)
	{
		if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_MaskTexture", 1)))
			return E_FAIL;
	}

	if (FAILED(m_pTexture_SkillIcon->Bind_ShadeResource(m_pShaderCom, "g_MaskTexture2", m_iSkillIcon)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTime", &m_fTime, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_TextCoord", &m_TextCoord, sizeof(_float2))))
		return E_FAIL;

	if (m_isBaseSkill)
	{
		if (FAILED(m_pShaderCom->Begin(3)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pShaderCom->Begin(4)))
			return E_FAIL;
	}

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

void CUI_PlayerSkill::Appear(_float fTimeDelta)
{
	m_fAlpha += fTimeDelta;

	if (m_fAlpha >= 1.f)
	{
		m_fAlpha = 1.f;
		m_isAppear = false;
	}
}

void CUI_PlayerSkill::Disappear(_float fTimeDelta)
{
	m_fAlpha -= 15.f * fTimeDelta;

	if (m_fAlpha <= 0.f)
	{
		m_fAlpha = 0.f;
		m_isDisappear = false;
		m_isActive = false;
	}
}

void CUI_PlayerSkill::Start_Appear()
{
	if (!m_isAppear && !m_isDisappear)
	{
		m_isAppear = true;
		m_isActive = true;
	}
}

void CUI_PlayerSkill::Start_Disappear()
{
	if (!m_isDisappear && !m_isAppear)
	{
		m_isDisappear = true;
	}
}

HRESULT CUI_PlayerSkill::Change_Skill(_uint iIconNum, _float* pSkillCollTime)
{
	if (iIconNum >= SKILL_END || nullptr == pSkillCollTime)
		return E_FAIL;

	m_iSkillIcon = iIconNum;

	m_pCoolTime = pSkillCollTime;

	return E_NOTIMPL;
}

HRESULT CUI_PlayerSkill::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SkillSlotBase"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	if (m_isBaseSkill)
	{
		/* FOR.Com_TextureSkillIcon */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SkillIcon"),
			TEXT("Com_TextureSkillIcon"), reinterpret_cast<CComponent**>(&m_pTexture_SkillIcon))))
			return E_FAIL;
	}
	else
	{
		/* FOR.Com_TextureSkillIcon */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SPSkillIcon"),
			TEXT("Com_TextureSkillIcon"), reinterpret_cast<CComponent**>(&m_pTexture_SkillIcon))))
			return E_FAIL;
	}
	
	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CUI_PlayerSkill* CUI_PlayerSkill::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_PlayerSkill* pInstance = new CUI_PlayerSkill(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_PlayerSkill"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CUI_PlayerSkill::Clone(void* pArg)
{
	CUI_PlayerSkill* pInstance = new CUI_PlayerSkill(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUI_PlayerSkill"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_PlayerSkill::Free()
{
	__super::Free();

	Safe_Release(m_pTexture_SkillIcon);

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
