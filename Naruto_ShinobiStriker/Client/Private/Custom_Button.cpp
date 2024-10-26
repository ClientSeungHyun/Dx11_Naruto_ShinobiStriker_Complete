#include "stdafx.h"
#include "Custom_Button.h"
#include "CustomPannel.h"

#include "GameInstance.h"

CCustom_Button::CCustom_Button(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CCustom_Button::CCustom_Button(const CCustom_Button& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CCustom_Button::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCustom_Button::Initialize(void* pArg)
{
	BUTTON_DESC* ButtonDesc = static_cast<BUTTON_DESC*>(pArg);
	m_iButtonIndex = ButtonDesc->iButtonIndex;
	m_isSelect = ButtonDesc->isSelect;
	m_iDepth = 1;

	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(ButtonDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fInitX = m_fX;
	m_fAlpha = 1.f;

	Choice_PartName();

	return S_OK;
}

void CCustom_Button::Priority_Update(_float fTimeDelta)
{
}

void CCustom_Button::Update(_float fTimeDelta)
{
	if (m_isAppear)
		Appear(fTimeDelta);
	else if (m_isDisappear)
		Disappear(fTimeDelta);
}

void CCustom_Button::Late_Update(_float fTimeDelta)
{
	/* 직교투영을 위한 월드행렬까지 셋팅하게 된다. */
	__super::Late_Update(fTimeDelta);
}

HRESULT CCustom_Button::Render()
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
		m_pGameInstance->Render_TextCenter(TEXT("Font_145"), m_strPartName.c_str(), XMVectorSet(m_fX, m_fY, 0.f, 1.f),
			XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.5f);
	}


	return S_OK;
}

void CCustom_Button::Appear(_float fTimeDleta)
{
	m_fX += 1400 * fTimeDleta;
	m_fAlpha += 15.f * fTimeDleta;
	if (m_fX >= m_fInitX )
	{
		m_fX = m_fInitX;

		if (m_fAlpha >= 1.f)
		{
			m_fAlpha = 1.f;
			m_isAppear = false;
		}
	}
}

void CCustom_Button::Disappear(_float fTimeDleta)
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

void CCustom_Button::Start_Appear()
{
	m_isActive = true;
	m_isAppear = true;
}

void CCustom_Button::Start_Disappear()
{
	m_isDisappear = true;
}

HRESULT CCustom_Button::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_CUSTOM, TEXT("Prototype_Component_Texture_Custom_Button"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

void CCustom_Button::Choice_PartName()
{
	CCustomPannel* pParent = dynamic_cast<CCustomPannel*>(m_pParentUI);

	if (pParent->Get_PannelType() == CCustomPannel::PANNEL_MAIN)
	{
		Choice_MainName();
	}
	else if (pParent->Get_PannelType() == CCustomPannel::PANNEL_HEAD)
	{
		Choice_HeadName();
	}
	else if (pParent->Get_PannelType() == CCustomPannel::PANNEL_ONCE)
	{
		Choice_OnceName();
	}
	else if (pParent->Get_PannelType() == CCustomPannel::PANNEL_UPPER)
	{
		Choice_UpperName();
	}
	else if (pParent->Get_PannelType() == CCustomPannel::PANNEL_LOWER)
	{
		Choice_LowerName();
	}
	else if (pParent->Get_PannelType() == CCustomPannel::PANNEL_FACE)
	{
		Choice_FaceName();
	}
	else if (pParent->Get_PannelType() == CCustomPannel::PANNEL_EYE)
	{
		Choice_EyeName();
	}
	else
	{
		m_strPartName = TEXT("아직 안 넣음");
	}
}

void CCustom_Button::Choice_MainName()
{
	switch (m_iButtonIndex)
	{
	case 0:
		m_strPartName = TEXT("머리");
		break;
	case 1:
		m_strPartName = TEXT("눈");
		break;
	case 2:
		m_strPartName = TEXT("얼굴");
		break;
	case 3:
		m_strPartName = TEXT("한벌옷");
		break;
	case 4:
		m_strPartName = TEXT("상의");
		break;
	case 5:
		m_strPartName = TEXT("하의");
		break;
	}
}

void CCustom_Button::Choice_HeadName()
{
	switch (m_iButtonIndex)
	{
	case 0:
		m_strPartName = TEXT("머리1");
		break;
	case 1:
		m_strPartName = TEXT("머리2");
		break;
	case 2:
		m_strPartName = TEXT("모자");
		break;
	case 3:
		m_strPartName = TEXT("개구리");
		break;
	case 4:
		m_strPartName = TEXT("헬멧");
		break;
	case 5:
		m_strPartName = TEXT("호카게");
		break;
	case 6:
		m_strPartName = TEXT("후드");
		break;
	case 7:
		m_strPartName = TEXT("파자마");
		break;
	}
}
void CCustom_Button::Choice_FaceName()
{
	switch (m_iButtonIndex)
	{
	case 0:
		m_strPartName = TEXT("기본");
		break;
	case 1:
		m_strPartName = TEXT("밴드");
		break;
	case 2:
		m_strPartName = TEXT("마스크1");
		break;
	case 3:
		m_strPartName = TEXT("마스크2");
		break;
	}
}

void CCustom_Button::Choice_EyeName()
{
	switch (m_iButtonIndex)
	{
	case 0:
		m_strPartName = TEXT("기본");
		break;
	case 1:
		m_strPartName = TEXT("사륜안");
		break;
	case 2:
		m_strPartName = TEXT("백안");
		break;
	case 3:
		m_strPartName = TEXT("선인");
		break;
	case 4:
		m_strPartName = TEXT("구미");
		break;
	case 5:
		m_strPartName = TEXT("만화경1");
		break;
	case 6:
		m_strPartName = TEXT("만화경2");
		break;
	case 7:
		m_strPartName = TEXT("윤회안");
		break;
	}
}

void CCustom_Button::Choice_OnceName()
{
	switch (m_iButtonIndex)
	{
	case 0:
		m_strPartName = TEXT("호카게코트");
		break;
	case 1:
		m_strPartName = TEXT("갑옷");
		break;
	case 2:
		m_strPartName = TEXT("멋진코트");
		break;
	case 3:
		m_strPartName = TEXT("재킷");
		break;
	case 4:
		m_strPartName = TEXT("산타");
		break;
	case 5:
		m_strPartName = TEXT("선인모드");
		break;
	case 6:
		m_strPartName = TEXT("트랙");
		break;
	case 7:
		m_strPartName = TEXT("육도");
		break;
	}
}

void CCustom_Button::Choice_UpperName()
{
	switch (m_iButtonIndex)
	{
	case 0:
		m_strPartName = TEXT("트레이닝복1");
		break;
	case 1:
		m_strPartName = TEXT("트레이닝복2");
		break;
	case 2:
		m_strPartName = TEXT("암부");
		break;
	case 3:
		m_strPartName = TEXT("갑옷");
		break;
	case 4:
		m_strPartName = TEXT("하오리");
		break;
	case 5:
		m_strPartName = TEXT("후드");
		break;
	case 6:
		m_strPartName = TEXT("셔츠");
		break;
	case 7:
		m_strPartName = TEXT("조끼");
		break;
	}
}

void CCustom_Button::Choice_LowerName()
{
	switch (m_iButtonIndex)
	{
	case 0:
		m_strPartName = TEXT("바지1");
		break;
	case 1:
		m_strPartName = TEXT("바지2");
		break;
	case 2:
		m_strPartName = TEXT("암부");
		break;
	case 3:
		m_strPartName = TEXT("갑옷");
		break;
	case 4:
		m_strPartName = TEXT("반바지");
		break;
	case 5:
		m_strPartName = TEXT("짧은바지");
		break;
	case 6:
		m_strPartName = TEXT("스커트???");
		break;
	case 7:
		m_strPartName = TEXT("조끼바지");
		break;
	}
}

CCustom_Button* CCustom_Button::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCustom_Button* pInstance = new CCustom_Button(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBackGround"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CCustom_Button::Clone(void* pArg)
{
	CCustom_Button* pInstance = new CCustom_Button(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CBackGround"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCustom_Button::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
