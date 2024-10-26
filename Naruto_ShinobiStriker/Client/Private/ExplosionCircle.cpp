#include "stdafx.h"
#include "ExplosionCircle.h"

#include "GameInstance.h"

#include "Camera.h"
#include "Model.h"
#include "ObjectPool.h"

CExplosionCircle::CExplosionCircle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect{ pDevice, pContext }
{
}

CExplosionCircle::CExplosionCircle(const CExplosionCircle& Prototype)
	: CEffect{ Prototype }
{
}

HRESULT CExplosionCircle::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CExplosionCircle::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_isActive = false;
	m_vEffectColor = _float4(0.f, 0.f, 1.f, 1.f);

	m_pTransformCom->Set_Scaled(0.01f, 0.01f, 0.01f);
	m_fTargetScale = 0.25f;

	return S_OK;
}

void CExplosionCircle::Priority_Update(_float fTimeDelta)
{
}

void CExplosionCircle::Update(_float fTimeDelta)
{
	m_fCurrentTime += fTimeDelta;

	if (m_fCurrentScale <= m_fTargetScale)
	{
		m_fCurrentScale += m_fScaleSpeed * fTimeDelta;

		if (m_fCurrentScale >= m_fTargetScale)
			m_fCurrentScale = m_fTargetScale;
	}

	if (m_fCurrentTime >= m_fLifeTime)
	{
		m_vEffectColor.w -= fTimeDelta;
		if (m_vEffectColor.w <= 0.f)
		{
			CObjectPool<CExplosionCircle>::Return_GameObject(this);
			m_isActive = false;
			return;
		}
	}

	m_pTransformCom->Set_Scaled(m_fCurrentScale, m_fCurrentScale, 1.f);

	m_pTransformCom->BillBoard();
}

void CExplosionCircle::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_EFFECT, this);
}

HRESULT CExplosionCircle::Render()
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

	if (FAILED(m_pShaderCom->Begin(5)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CExplosionCircle::Appear(_float3 vPos, _float4 vColor, _float fInitScale, _float fTargetScale, _float fScaleSpeed, _float fLifeTime)
{
	m_fCurrentScale = fInitScale;
	m_fTargetScale = fTargetScale;
	m_fScaleSpeed = fScaleSpeed;
	m_vEffectColor = vColor;
	m_fLifeTime = fLifeTime;
	m_fCurrentTime = 0.f;

	m_pTransformCom->Set_Scaled(m_fCurrentScale, m_fCurrentScale, m_fCurrentScale);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&vPos));

	m_isActive = true;
}

void CExplosionCircle::DisAppear()
{
	m_isActive = false;
}

HRESULT CExplosionCircle::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Effect_Texture"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Ring"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CExplosionCircle* CExplosionCircle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CExplosionCircle* pInstance = new CExplosionCircle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CExplosionCircel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CExplosionCircle::Clone(void* pArg)
{
	CExplosionCircle* pInstance = new CExplosionCircle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CExplosionCircel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CExplosionCircle::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
}
