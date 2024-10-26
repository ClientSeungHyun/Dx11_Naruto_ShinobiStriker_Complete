#include "stdafx.h"
#include "UI_MonsterHP.h"

#include "Monster.h"
#include "GameInstance.h"

CUI_MonsterHP::CUI_MonsterHP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CUI_MonsterHP::CUI_MonsterHP(const CUI_MonsterHP& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CUI_MonsterHP::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_MonsterHP::Initialize(void* pArg)
{
	HPUI_DESC* pDesc = static_cast<HPUI_DESC*>(pArg);

	m_pMonsterHP = pDesc->pMonsterHP;
	m_fMaxHP = *m_pMonsterHP;
	m_pMonster = pDesc->pMonster;

	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_iDepth = 2;
	m_TextCoord = _float2(0.f, 1.f);
	m_fAlpha = 0.f;

	m_pTransformCom->Set_Scaled(0.7f, 0.4f, 1.f);

	m_isActive = false;

	return S_OK;
}

void CUI_MonsterHP::Priority_Update(_float fTimeDelta)
{
	if (!m_pMonster->IsActive())
		m_isActive = false;

	for (auto& Child : m_UIChilds)
	{
		Child->Priority_Update(fTimeDelta);
	}
}

void CUI_MonsterHP::Update(_float fTimeDelta)
{
	if (!m_pMonster->IsActive())
		m_isActive = false;

	if (m_isAppear)
		Appear(fTimeDelta);
	else if (m_isDisappear)
		Disappear(fTimeDelta);
	else
	{
		m_TextCoord.x = (*m_pMonsterHP) / m_fMaxHP;
		if (m_TextCoord.x <= 0.f)
			m_TextCoord.x = 0.f;
		else if (m_TextCoord.x >= 1.f)
			m_TextCoord.x = 1.f;
	}

	if (m_isDamaged)
	{
		m_fDamagedTime += fTimeDelta;
		if (m_fDamagedTime > 0.8f)
		{
			m_fAlpha = 0.f;
			m_isDamaged = false;
		}
		else if (m_fDamagedTime < 0.3f)
			m_fAlpha += 10.f * fTimeDelta;
		else if (m_fDamagedTime >= 0.3f)
			m_fAlpha -= 10.f * fTimeDelta;

		if (m_fAlpha <= 0.f)
			m_fAlpha = 0.f;
		else if (m_fAlpha >= 1.f)
			m_fAlpha = 1.f;
	}
}

void CUI_MonsterHP::Late_Update(_float fTimeDelta)
{
	if (m_pMonster == nullptr || m_pMonster->Get_Dead())
	{
		m_pGameInstance->Event_DestoryObject(this);
		return;
	}

	if (!m_pMonster->IsActive())
		m_isActive = false;

	_vector vCurrentPos = m_pMonster->Get_Position();
	vCurrentPos = XMVectorSetY(vCurrentPos, XMVectorGetY(vCurrentPos) + 1.8f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCurrentPos);
	m_pTransformCom->BillBoard();

	_vector vCameraPos = m_pGameInstance->Get_CamPosition_Vector();
	_float fLength = XMVectorGetX(XMVector3Length(vCameraPos - vCurrentPos));

	for (auto& Child : m_UIChilds)
	{
		Child->Late_Update(fTimeDelta);
	}

	if (fLength < 20.0f)
		m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_MonsterHP::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_TextCoord", &m_TextCoord, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pTexture_Guage->Bind_ShadeResource(m_pShaderCom, "g_MaskTexture", 0)))
		return E_FAIL;

	//if (FAILED(m_pTexture_DamageEffect->Bind_ShadeResource(m_pShaderCom, "g_MaskTexture2", 0)))
	//	return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(2)))
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

void CUI_MonsterHP::Appear(_float fTimeDelta)
{
	m_TextCoord.x += 10.f * fTimeDelta;

	if (m_TextCoord.x >= 1.f)
	{
		m_TextCoord.x = 1.f;
		m_isAppear = false;
	}
}

void CUI_MonsterHP::Disappear(_float fTimeDelta)
{
	m_fAlpha -= 15.f * fTimeDelta;

	if (m_fAlpha <= 0.f)
	{
		m_fAlpha = 0.f;
		m_isDisappear = false;
		m_isActive = false;
	}
}

void CUI_MonsterHP::Start_Appear()
{
	if (!m_isAppear && !m_isDisappear)
	{
		m_TextCoord.x = 0.f;
		m_isAppear = true;
		m_isActive = true;
	}
}

void CUI_MonsterHP::Start_Disappear()
{
	if (!m_isDisappear && !m_isAppear)
	{

		m_isDisappear = true;
	}
}

void CUI_MonsterHP::Damaged()
{
	m_fAlpha = 0.f;
	m_fDamagedTime = 0.f;
	m_isDamaged = true;
}

HRESULT CUI_MonsterHP::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HPBase"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* FOR.Com_MaskTexture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HPMask"),
		TEXT("Com_MaksTexture"), reinterpret_cast<CComponent**>(&m_pTexture_Guage))))
		return E_FAIL;

	/* FOR.Com_DamageTexture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HPDamageEffect"),
		TEXT("Com_DamageTexture"), reinterpret_cast<CComponent**>(&m_pTexture_DamageEffect))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CUI_MonsterHP* CUI_MonsterHP::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_MonsterHP* pInstance = new CUI_MonsterHP(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_MonsterHP"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CUI_MonsterHP::Clone(void* pArg)
{
	CUI_MonsterHP* pInstance = new CUI_MonsterHP(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUI_MonsterHP"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_MonsterHP::Free()
{
	__super::Free();

	Safe_Release(m_pTexture_Guage);
	Safe_Release(m_pTexture_DamageEffect);

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
