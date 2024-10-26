#include "stdafx.h"
#include "CustomPannel.h"

#include "GameInstance.h"

#include "Custom_Button.h"
#include "Custom_Head.h"

#include "Head_Player.h"
#include "Face_Player.h"
#include "BodyOnce_Player.h"
#include "BodyUpper_Player.h"
#include "BodyLower_Player.h"

CCustomPannel::CCustomPannel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CCustomPannel::CCustomPannel(const CCustomPannel& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CCustomPannel::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCustomPannel::Initialize(void* pArg)
{
	PANNEL_DESC*		pDesc = static_cast<PANNEL_DESC*>(pArg);

	m_iNumButton = pDesc->iNumButton;
	m_ePannelType = pDesc->eType;

	/* Á÷±³Åü¿©À» À§ÇÑ µ¥ÀÌÅÍµéÀ» ¸ðµÎ ¼ÂÇÏ³®. */
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Child()))
		return E_FAIL;

	return S_OK;
}

void CCustomPannel::Priority_Update(_float fTimeDelta)
{
	if (m_isFirst)
	{
		Start_Disappear();
	}

	for (auto& Child : m_UIChilds)
	{
		Child->Priority_Update(fTimeDelta);
	}
}

void CCustomPannel::Update(_float fTimeDelta)
{
	m_fCurrentTime += fTimeDelta;

	if (m_isAppear)
		Appear(fTimeDelta);
	else if (m_isDisappear)
		Disappear(fTimeDelta);
	else
	{
		Control_UI();
	}

	for (auto& Child : m_UIChilds)
	{
		Child->Update(fTimeDelta);
	}
}

void CCustomPannel::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	for (auto& Child : m_UIChilds)
	{
		Child->Late_Update(fTimeDelta);
	}

	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CCustomPannel::Render()
{
	if (!m_isFirst)
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
	}
	return S_OK;
}

void CCustomPannel::Appear(_float fTimeDelta)
{
	m_fAlpha += 10.f * fTimeDelta;

	if (m_fCurrentTime >= 0.05f)
	{
		m_fCurrentTime = 0.f;

		if (!m_UIChilds[m_iChildIndex]->IsActive())
			m_UIChilds[m_iChildIndex]->Start_Appear();

		if (m_iChildIndex < m_UIChilds.size())
			++m_iChildIndex;
	}


	for (auto& Child : m_UIChilds)
	{
		if (!Child->IsActive())
			return;

		if (m_fAlpha < 1.f)
			return;

	}

	m_isAppear = false;
}

void CCustomPannel::Disappear(_float fTimeDelta)
{
	m_fAlpha -= 10.f * fTimeDelta;

	if (m_fCurrentTime >= 0.05f)
	{
		m_fCurrentTime = 0.f;

		if (m_iChildIndex < m_UIChilds.size())
			++m_iChildIndex;
	}

	for (_int i = 0; i < m_iChildIndex; ++i)
		m_UIChilds[i]->Start_Disappear();

	for (auto& Child : m_UIChilds)
	{
		if (Child->IsActive())
			return;

	}

	if (m_fAlpha <= 0.f)
	{
		m_fAlpha = 0.f;
		m_isDisappear = false;
		m_isActive = false;

		if (m_ePannelType == PANNEL_MAIN && m_isFirst)
		{
			Start_Appear();
		}
		m_isFirst = false;
	}
}

void CCustomPannel::Start_Appear()
{
	if (!m_isAppear && !m_isDisappear)
	{
		m_iChildIndex = 0;
		m_fCurrentTime = 0.f;

		m_isAppear = true;
		m_isActive = true;
	}
}

void CCustomPannel::Start_Disappear()
{
	if (!m_isDisappear && !m_isAppear)
	{
		m_iChildIndex = 0;
		m_fCurrentTime = 0.f;

		m_isDisappear = true;
	}
}

void CCustomPannel::Clear_Select()
{
	for (auto& Button : m_Buttons)
	{
		Button->Set_IsSelect(false);
	}
}

void CCustomPannel::Control_UI()
{
	if (KEY_TAP(KEY::S))
	{
		m_pSoundCom->Play2D(TEXT("Select2.wav"), g_fVolume);
		m_Buttons[m_iSelectIndex++]->Set_IsSelect(false);
		m_iSelectIndex = m_iSelectIndex % m_Buttons.size();
		m_Buttons[m_iSelectIndex]->Set_IsSelect(true);
	}
	else if (KEY_TAP(KEY::W))
	{
		m_pSoundCom->Play2D(TEXT("Select2.wav"), g_fVolume);
		m_Buttons[m_iSelectIndex]->Set_IsSelect(false);
		m_iSelectIndex = m_iSelectIndex - 1;
		if (m_iSelectIndex < 0)
			m_iSelectIndex = (_int)m_Buttons.size() - 1;
		m_Buttons[m_iSelectIndex]->Set_IsSelect(true);
	}
}

HRESULT CCustomPannel::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_CUSTOM, TEXT("Prototype_Component_Texture_CustomPannel"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* FOR.Com_Sound */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sound"),
		TEXT("Com_Sound"), reinterpret_cast<CComponent**>(&m_pSoundCom))))
		return E_FAIL;
	m_pSoundCom->Set_Owner(this);

	return S_OK;
}

HRESULT CCustomPannel::Ready_Child()
{
	CUIObject::UI_DESC UIDesc{};
	UIDesc.fParentfX = m_fX;
	UIDesc.fParentfY = m_fY;
	UIDesc.fSizeX = 400.f;
	UIDesc.fSizeY = 55.f;
	UIDesc.iLevelIndex = m_iLevelIndex;
	UIDesc.iDepth = m_iDepth + 1;
	UIDesc.fSpeedPerSec = 10.f;
	UIDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	UIDesc.isChild = true;
	UIDesc.pParentUI = this;

#pragma region ¹öÆ°
	CCustom_Button::BUTTON_DESC ButtonDesc{};
	memcpy(&ButtonDesc, &UIDesc, sizeof(CUIObject::UI_DESC));

	ButtonDesc.fX = 0.f;
	ButtonDesc.fY = -190.f;
	ButtonDesc.isSelect = true;
	ButtonDesc.iButtonIndex = 0;
	CCustom_Button* pButton = dynamic_cast<CCustom_Button*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_UI_CustomButton"), &ButtonDesc));

	m_UIChilds.emplace_back(dynamic_cast<CUIObject*>(pButton));
	m_Buttons.emplace_back(pButton);
	Safe_AddRef(pButton);

	ButtonDesc.isSelect = false;
	for (_uint i = 1; i < m_iNumButton; ++i)
	{
		ButtonDesc.iButtonIndex = i;
		ButtonDesc.fX = 0.f;
		ButtonDesc.fY = -190.f + i * 60.f;
		pButton = dynamic_cast<CCustom_Button*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_UI_CustomButton"), &ButtonDesc));
		m_UIChilds.emplace_back(dynamic_cast<CUIObject*>(pButton));
		m_Buttons.emplace_back(pButton);
		Safe_AddRef(pButton);
	}
#pragma endregion

	CCustom_Head::CUSTOMHEAD_DESC HeadDesc{};
	memcpy(&HeadDesc, &UIDesc, sizeof(CUIObject::UI_DESC));

	HeadDesc.fX = 0.f;
	HeadDesc.fY = -250.f;
	HeadDesc.fSizeX = 450.f;
	HeadDesc.fSizeY = 55.f;

	switch (m_ePannelType)
	{
	case Client::CCustomPannel::PANNEL_MAIN:
		HeadDesc.strHead = TEXT("ÆÄÃ÷ ¼±ÅÃ");
		break;
	case Client::CCustomPannel::PANNEL_HEAD:
		HeadDesc.strHead = TEXT("¸Ó¸®");
		break;
	case Client::CCustomPannel::PANNEL_EYE:
		HeadDesc.strHead = TEXT("´«");
		break;
	case Client::CCustomPannel::PANNEL_FACE:
		HeadDesc.strHead = TEXT("¾ó±¼");
		break;
	case Client::CCustomPannel::PANNEL_ONCE:
		HeadDesc.strHead = TEXT("ÇÑ¹ú¿Ê");
		break;
	case Client::CCustomPannel::PANNEL_UPPER:
		HeadDesc.strHead = TEXT("»óÀÇ");
		break;
	case Client::CCustomPannel::PANNEL_LOWER:
		HeadDesc.strHead = TEXT("ÇÏÀÇ");
		break;
	}

	CCustom_Head* pHead = dynamic_cast<CCustom_Head*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_UI_CustomHead"), &HeadDesc));
	m_UIChilds.emplace_back(dynamic_cast<CUIObject*>(pHead));
	
	return S_OK;
}

CCustomPannel* CCustomPannel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCustomPannel* pInstance = new CCustomPannel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBackGround"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CCustomPannel::Clone(void* pArg)
{
	CCustomPannel* pInstance = new CCustomPannel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CBackGround"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCustomPannel::Free()
{
	__super::Free();

	for (auto& Button : m_Buttons)
		Safe_Release(Button);
	m_Buttons.clear();

	Safe_Release(m_pSoundCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
