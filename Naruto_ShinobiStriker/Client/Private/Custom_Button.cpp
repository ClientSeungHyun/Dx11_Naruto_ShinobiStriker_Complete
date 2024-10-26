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

	/* ���������� ���� �����͵��� ��� ���ϳ�. */
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
	/* ���������� ���� ������ı��� �����ϰ� �ȴ�. */
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
		m_strPartName = TEXT("���� �� ����");
	}
}

void CCustom_Button::Choice_MainName()
{
	switch (m_iButtonIndex)
	{
	case 0:
		m_strPartName = TEXT("�Ӹ�");
		break;
	case 1:
		m_strPartName = TEXT("��");
		break;
	case 2:
		m_strPartName = TEXT("��");
		break;
	case 3:
		m_strPartName = TEXT("�ѹ���");
		break;
	case 4:
		m_strPartName = TEXT("����");
		break;
	case 5:
		m_strPartName = TEXT("����");
		break;
	}
}

void CCustom_Button::Choice_HeadName()
{
	switch (m_iButtonIndex)
	{
	case 0:
		m_strPartName = TEXT("�Ӹ�1");
		break;
	case 1:
		m_strPartName = TEXT("�Ӹ�2");
		break;
	case 2:
		m_strPartName = TEXT("����");
		break;
	case 3:
		m_strPartName = TEXT("������");
		break;
	case 4:
		m_strPartName = TEXT("���");
		break;
	case 5:
		m_strPartName = TEXT("ȣī��");
		break;
	case 6:
		m_strPartName = TEXT("�ĵ�");
		break;
	case 7:
		m_strPartName = TEXT("���ڸ�");
		break;
	}
}
void CCustom_Button::Choice_FaceName()
{
	switch (m_iButtonIndex)
	{
	case 0:
		m_strPartName = TEXT("�⺻");
		break;
	case 1:
		m_strPartName = TEXT("���");
		break;
	case 2:
		m_strPartName = TEXT("����ũ1");
		break;
	case 3:
		m_strPartName = TEXT("����ũ2");
		break;
	}
}

void CCustom_Button::Choice_EyeName()
{
	switch (m_iButtonIndex)
	{
	case 0:
		m_strPartName = TEXT("�⺻");
		break;
	case 1:
		m_strPartName = TEXT("�����");
		break;
	case 2:
		m_strPartName = TEXT("���");
		break;
	case 3:
		m_strPartName = TEXT("����");
		break;
	case 4:
		m_strPartName = TEXT("����");
		break;
	case 5:
		m_strPartName = TEXT("��ȭ��1");
		break;
	case 6:
		m_strPartName = TEXT("��ȭ��2");
		break;
	case 7:
		m_strPartName = TEXT("��ȸ��");
		break;
	}
}

void CCustom_Button::Choice_OnceName()
{
	switch (m_iButtonIndex)
	{
	case 0:
		m_strPartName = TEXT("ȣī����Ʈ");
		break;
	case 1:
		m_strPartName = TEXT("����");
		break;
	case 2:
		m_strPartName = TEXT("������Ʈ");
		break;
	case 3:
		m_strPartName = TEXT("��Ŷ");
		break;
	case 4:
		m_strPartName = TEXT("��Ÿ");
		break;
	case 5:
		m_strPartName = TEXT("���θ��");
		break;
	case 6:
		m_strPartName = TEXT("Ʈ��");
		break;
	case 7:
		m_strPartName = TEXT("����");
		break;
	}
}

void CCustom_Button::Choice_UpperName()
{
	switch (m_iButtonIndex)
	{
	case 0:
		m_strPartName = TEXT("Ʈ���̴׺�1");
		break;
	case 1:
		m_strPartName = TEXT("Ʈ���̴׺�2");
		break;
	case 2:
		m_strPartName = TEXT("�Ϻ�");
		break;
	case 3:
		m_strPartName = TEXT("����");
		break;
	case 4:
		m_strPartName = TEXT("�Ͽ���");
		break;
	case 5:
		m_strPartName = TEXT("�ĵ�");
		break;
	case 6:
		m_strPartName = TEXT("����");
		break;
	case 7:
		m_strPartName = TEXT("����");
		break;
	}
}

void CCustom_Button::Choice_LowerName()
{
	switch (m_iButtonIndex)
	{
	case 0:
		m_strPartName = TEXT("����1");
		break;
	case 1:
		m_strPartName = TEXT("����2");
		break;
	case 2:
		m_strPartName = TEXT("�Ϻ�");
		break;
	case 3:
		m_strPartName = TEXT("����");
		break;
	case 4:
		m_strPartName = TEXT("�ݹ���");
		break;
	case 5:
		m_strPartName = TEXT("ª������");
		break;
	case 6:
		m_strPartName = TEXT("��ĿƮ???");
		break;
	case 7:
		m_strPartName = TEXT("��������");
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
