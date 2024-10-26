#include "stdafx.h"
#include "Effect_RasenkoHit.h"

#include "GameInstance.h"

#include "Camera.h"
#include "Model.h"
#include "ObjectPool.h"

CEffect_RasenkoHit::CEffect_RasenkoHit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect{ pDevice, pContext }
{
}

CEffect_RasenkoHit::CEffect_RasenkoHit(const CEffect_RasenkoHit& Prototype)
	: CEffect{ Prototype }
{
}

HRESULT CEffect_RasenkoHit::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CEffect_RasenkoHit::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_isActive = false;
	m_vEffectColor = _float4(1.f, 0.9f, 0.f, 1.f);


	return S_OK;
}

void CEffect_RasenkoHit::Priority_Update(_float fTimeDelta)
{
}

void CEffect_RasenkoHit::Update(_float fTimeDelta)
{
	m_fCurrentTime += fTimeDelta;
	if (m_fCurrentTime >= 0.07f)
	{
		m_fCurrentTime = 0.07f;
	}

	if (m_isScaleX)
	{
		XMStoreFloat3(&m_vCurrentScale, XMVectorLerp(XMLoadFloat3(&m_vInitScaleX), XMLoadFloat3(&m_vTargetScaleX), m_fCurrentTime / 0.07f));

		if (m_fCurrentTime >= 0.07f)
		{
			m_fCurrentTime = 0.f;
			m_isScaleX = false;
			m_isScaleY = true;
		}
	}
	else if (m_isScaleY)
	{
		XMStoreFloat3(&m_vCurrentScale, XMVectorLerp(XMLoadFloat3(&m_vInitScaleY), XMLoadFloat3(&m_vTargetScaleY), m_fCurrentTime / 0.07f));

		if (m_fCurrentTime >= 0.07f)
		{
			m_vEffectColor.w -= 4.f * fTimeDelta;
			if (m_vEffectColor.w <= 0.f)
			{
				CObjectPool<CEffect_RasenkoHit>::Return_GameObject(this);
				m_isActive = false;
			}
		}
	}

	m_pTransformCom->Set_Scaled(m_vCurrentScale.x, m_vCurrentScale.y, m_vCurrentScale.z);
	m_pTransformCom->BillBoardXZ();
}

void CEffect_RasenkoHit::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_EFFECT, this);
}

HRESULT CEffect_RasenkoHit::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vEffectColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(8)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CEffect_RasenkoHit::Appear(_fvector vPos)
{
	if (m_isActive)
		return;

	m_vInitScaleX = { 15.f, 1.f, 1.f };
	m_vTargetScaleX = { 0.5f, 1.f,1.f };

	m_vInitScaleY = { 0.5f, 1.f, 1.f };
	m_vTargetScaleY = { 0.5f, 15.f,1.f };

	m_pTransformCom->Set_Scaled(0.1f, 0.1f, 0.1f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	m_fCurrentTime = 0.f;
	m_vCurrentScale = m_vInitScaleX;
	m_vEffectColor = _float4(1.f, 0.9f, 0.f, 1.f);

	m_isScaleX = true;
	m_isScaleY = false;

	m_isActive = true;
}

void CEffect_RasenkoHit::DisAppear()
{
	m_isActive = false;
}

HRESULT CEffect_RasenkoHit::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Effect_Texture"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Hit09"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CEffect_RasenkoHit* CEffect_RasenkoHit::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffect_RasenkoHit* pInstance = new CEffect_RasenkoHit(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CEffect_RasenkoHit"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CEffect_RasenkoHit::Clone(void* pArg)
{
	CEffect_RasenkoHit* pInstance = new CEffect_RasenkoHit(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CEffect_RasenkoHit"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_RasenkoHit::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);

}
