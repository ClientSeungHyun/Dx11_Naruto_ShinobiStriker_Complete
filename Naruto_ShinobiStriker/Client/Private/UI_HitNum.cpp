#include "stdafx.h"
#include "UI_HitNum.h"

#include "GameInstance.h"

CUI_HitNum::CUI_HitNum(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CUI_HitNum::CUI_HitNum(const CUI_HitNum& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CUI_HitNum::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_HitNum::Initialize(void* pArg)
{
	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	// 45도 돌려두기
	m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, -1.f, 0.f), XMConvertToRadians(-30.f));

	HitNumMatrix[HITNUM] = XMMatrixScaling(0.35f, 0.8f, 1.f);
	HitNumMatrix[BIGTEXT] = XMMatrixScaling(1.f, 1.f, 1.f);
	HitNumMatrix[SMALLTEXT] = XMMatrixScaling(0.4f, 0.5f, 1.f);

	m_iDepth = 0;
	m_fAlpha = 1.f;

	m_isActive = false;

	return S_OK;
}

void CUI_HitNum::Priority_Update(_float fTimeDelta)
{

	for (auto& Child : m_UIChilds)
	{
		Child->Priority_Update(fTimeDelta);
	}
}

void CUI_HitNum::Update(_float fTimeDelta)
{
	m_fLifeTime -= fTimeDelta;

	if (m_isAppear)
		Appear(fTimeDelta);
	else if (m_fLifeTime <= fTimeDelta)
		Disappear(fTimeDelta);
	else
	{
	}
}

void CUI_HitNum::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
	
	if (m_isAppear)
	{
		m_pTransformCom->Set_Scaled(m_fSizeX + m_fScale, m_fSizeY + m_fScale, 1.f);
	}


	Convert_NumToStr();
	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_HitNum::Render()
{
	_float4x4 World4x4 = {};

	// 리스트가 비어있으면 큰 글씨만 렌더
	if (m_HitNumList.empty())
	{
		_matrix WorldMatrix = HitNumMatrix[BIGTEXT] * m_pTransformCom->Get_WorldMatrix();

		XMStoreFloat4x4(&World4x4, WorldMatrix);
		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &World4x4)))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pHitTextTexture->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
			return E_FAIL;


		if (FAILED(m_pShaderCom->Begin(1)))
			return E_FAIL;
		if (FAILED(m_pVIBufferCom->Bind_Buffers()))
			return E_FAIL;
		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;

		return S_OK;
	}
	
	_vector vOffsetDir = XMVectorSet(1.f, 0.f, 0.f, 0.f);
	XMStoreFloat3(&m_vOffset, vOffsetDir * -0.5f);

	for (auto& HitNum : m_HitNumList)
	{
		_matrix WorldMatrix = XMMatrixTranslation(m_vOffset.x, m_vOffset.y, 1.f) * HitNumMatrix[HITNUM];
		WorldMatrix *= m_pTransformCom->Get_WorldMatrix();

		XMStoreFloat4x4(&World4x4, WorldMatrix);
		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &World4x4)))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", HitNum)))
			return E_FAIL;


		if (FAILED(m_pShaderCom->Begin(1)))
			return E_FAIL;
		if (FAILED(m_pVIBufferCom->Bind_Buffers()))
			return E_FAIL;
		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;

		XMStoreFloat3(&m_vOffset, XMLoadFloat3(&m_vOffset) + vOffsetDir * 0.7f);
	}

	XMStoreFloat3(&m_vOffset, XMLoadFloat3(&m_vOffset) + vOffsetDir * 0.25f + XMVectorSet(0.f, 1.f, 0.f, 0.f) * -0.3f);

	_matrix WorldMatrix = XMMatrixTranslation(m_vOffset.x, m_vOffset.y, m_vOffset.z) * HitNumMatrix[SMALLTEXT];
	WorldMatrix *= m_pTransformCom->Get_WorldMatrix();

	XMStoreFloat4x4(&World4x4, WorldMatrix);
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &World4x4)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pHitTextTexture->Bind_ShadeResource(m_pShaderCom, "g_Texture", 1)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(1)))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CUI_HitNum::Appear(_float fTimeDelta)
{
	m_fScale += 5.f * fTimeDelta;

	if (m_fScale > 1.5f)
	{
		m_fScale = 1.0f;
		m_isAppear = false;
	}

	m_pTransformCom->Set_Scaled(m_fScale, m_fScale, 1.f);
}

void CUI_HitNum::Disappear(_float fTimeDelta)
{
	m_fAlpha -= fTimeDelta;

	if (m_fAlpha <= 0.f)
	{
		m_fAlpha = 0.f;
		m_iHitCount = 0;
		m_isDisappear = false;
		m_isActive = false;
	}
}

void CUI_HitNum::Start_Appear()
{
		m_fScale = 0.1f;
		m_fAlpha = 1.f;
		m_fLifeTime = 2.0f;

		m_isAppear = true;
		m_isActive = true;
}

void CUI_HitNum::Start_Disappear()
{
	if (!m_isDisappear && !m_isAppear)
	{

		m_isDisappear = true;
	}
}

void CUI_HitNum::Add_HitCount()
{
	++m_iHitCount;
}

HRESULT CUI_HitNum::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HitNum"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HitText"),
		TEXT("Com_TextTexture"), reinterpret_cast<CComponent**>(&m_pHitTextTexture))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

void CUI_HitNum::Convert_NumToStr()
{
	m_HitNumList.clear();

	if (1 >= m_iHitCount)
		return;

	string strHitCount = to_string(m_iHitCount);
	_uint strLength = (_uint)strHitCount.length();

	for (_uint i = 0; i < strLength; ++i)
	{ 
		string strNumber(1, strHitCount[i]);
		m_HitNumList.push_back(stoi(strNumber));
	}
}

CUI_HitNum* CUI_HitNum::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_HitNum* pInstance = new CUI_HitNum(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_HitNum"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CUI_HitNum::Clone(void* pArg)
{
	CUI_HitNum* pInstance = new CUI_HitNum(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUI_HitNum"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_HitNum::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pHitTextTexture);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
