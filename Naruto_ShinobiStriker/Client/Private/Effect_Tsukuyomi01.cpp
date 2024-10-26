#include "stdafx.h"
#include "Effect_Tsukuyomi01.h"

#include "GameInstance.h"
#include "ObjectPool.h"
#include "Model.h"

CEffect_Tsukuyomi01::CEffect_Tsukuyomi01(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect{ pDevice, pContext }
{
}

CEffect_Tsukuyomi01::CEffect_Tsukuyomi01(const CEffect_Tsukuyomi01& Prototype)
	: CEffect{ Prototype }
{
}

HRESULT CEffect_Tsukuyomi01::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CEffect_Tsukuyomi01::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_isActive = false;
	m_vEffectColor = _float4(0.f, 0.f, 0.f, 1.f);

	return S_OK;
}

void CEffect_Tsukuyomi01::Priority_Update(_float fTimeDelta)
{
}

void CEffect_Tsukuyomi01::Update(_float fTimeDelta)
{
	if (m_eTsukuyomiType == ACTION)
	{
		if (!m_isReverse && m_isRender)
		{
			m_fCurrentScale += 15.f * fTimeDelta;
			m_fAngle -= 480.f * fTimeDelta;

			if (m_fCurrentScale >= m_fTargetScale)
			{
				m_fCurrentScale = m_fTargetScale;
			}

			if (m_fAngle <= -150.f)
			{
				m_vEffectColor.w -= 2.f * fTimeDelta;
				if (m_vEffectColor.w <= 0.f)
				{
					m_fTargetScale = 0.1f;
					m_isRender = false;
				}
			}
		}
		else if (m_isReverse && m_isRender)
		{
			m_fAngle += 500.f * fTimeDelta;

			if (m_fAngle >= -250.f)
			{
				m_fCurrentScale -= 20.f * fTimeDelta;
			}

			if (m_fCurrentScale <= 0.1f)
			{
				m_fCurrentScale = m_fTargetScale;
				m_isActive = false;
			}
		}
	}
	else if (m_eTsukuyomiType == BASE)
	{
		if (!m_isReverse && m_isRender)
		{
			m_fCurrentScale += 15.f * fTimeDelta;
			m_fAngle -= 480.f * fTimeDelta;

			if (m_fCurrentScale >= m_fTargetScale)
			{
				m_fCurrentScale = m_fTargetScale;
			}

			if (m_fAngle <= -250.f)
			{
				m_fTargetScale = 0.1f;
				m_isRender = false;
			}
		}
		else if (m_isReverse && m_isRender)
		{

			m_fAngle += 500.f * fTimeDelta;

			if (m_fAngle >= -180.f)
			{
				m_fCurrentScale -= 20.f * fTimeDelta;
			}

			if (m_fCurrentScale <= m_fTargetScale)
			{
				m_fCurrentScale = m_fTargetScale;
				m_isActive = false;
			}
		}
	}

	m_pTransformCom->Set_Scaled(m_fCurrentScale, m_fCurrentScale, m_fCurrentScale);
	m_pTransformCom->BillBoardXZ();
}

void CEffect_Tsukuyomi01::Late_Update(_float fTimeDelta)
{
	if(m_isRender)
		m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);
}

HRESULT CEffect_Tsukuyomi01::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_float fRadian = XMConvertToRadians(m_fAngle);
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fRadian", &fRadian, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vEffectColor, sizeof(_float4))))
		return E_FAIL;	
	
	if (m_eTsukuyomiType == ACTION)
	{
		if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_DiffuseTexture", 1)))
			return E_FAIL;
		if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_MaskTexture", 2)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(10)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_DiffuseTexture", 0)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(9)))
			return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CEffect_Tsukuyomi01::Appear(_fvector vPos, _float fScale, _float fTargetScale, TSUKUYOMI eType)
{
	if (m_isActive)
		return;

	m_fCurrentScale = fScale;
	m_fTargetScale = fTargetScale;
	m_fAngle = 0.f;
	m_vEffectColor.w = 1.f;

	m_pTransformCom->Set_Scaled(fScale, fScale, fScale);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	m_pTransformCom->BillBoardXZ();


	m_eTsukuyomiType = eType;

	m_isRender = true;
	m_isReverse = false;
	m_isActive = true;
}

void CEffect_Tsukuyomi01::DisAppear()
{
	m_isActive = false;
}

void CEffect_Tsukuyomi01::Start_Reverse()
{
	m_vEffectColor.w = 1.f;
	m_isRender = true;
	m_isReverse = true;
}

HRESULT CEffect_Tsukuyomi01::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Effect_Texture"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Tsukuyomi"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;
	return S_OK;
}

CEffect_Tsukuyomi01* CEffect_Tsukuyomi01::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffect_Tsukuyomi01* pInstance = new CEffect_Tsukuyomi01(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CEffect_Tsukuyomi01"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CEffect_Tsukuyomi01::Clone(void* pArg)
{
	CEffect_Tsukuyomi01* pInstance = new CEffect_Tsukuyomi01(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CEffect_Tsukuyomi01"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Tsukuyomi01::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);

}
