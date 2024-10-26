#include "stdafx.h"
#include "Effect_Distortion.h"

#include "GameInstance.h"
#include "Camera.h"
#include "Model.h"
#include "ObjectPool.h"

CEffect_Distortion::CEffect_Distortion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect{ pDevice, pContext }
{
}

CEffect_Distortion::CEffect_Distortion(const CEffect_Distortion& Prototype)
	: CEffect{ Prototype }
{
}

HRESULT CEffect_Distortion::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CEffect_Distortion::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_isActive = false;
	m_vEffectColor = _float4(0.f, 0.f, 1.f, 1.f);

	m_pTransformCom->Set_Scaled(1.f, 1.f, 1.f);

	return S_OK;
}

void CEffect_Distortion::Priority_Update(_float fTimeDelta)
{
}

void CEffect_Distortion::Update(_float fTimeDelta)
{
	m_fCurrentTime += fTimeDelta;


	if (m_fCurrentTime >= m_fDuration)
	{
		CObjectPool<CEffect_Distortion>::Return_GameObject(this);
		m_isActive = false;
	}

	if (m_fInitScale > m_fTargetScale)
	{
		m_fCurrentScale -= m_fScaleSpeed * fTimeDelta;
		if (m_fCurrentScale <= m_fTargetScale)
		{
			m_fCurrentScale = m_fTargetScale;
		}
	}
	else
	{
		m_fCurrentScale += m_fScaleSpeed * fTimeDelta;
		if (m_fCurrentScale >= m_fTargetScale)
		{
			m_fCurrentScale = m_fTargetScale;
		}
	}

	m_pTransformCom->Set_Scaled(m_fCurrentScale, m_fCurrentScale, m_fCurrentScale);
	m_pTransformCom->BillBoardXZ();
}

void CEffect_Distortion::Late_Update(_float fTimeDelta)
{
	//m_pGameInstance->Add_RenderObject(CRenderer::RG_EFFECT, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_DISTORTION, this);
}

HRESULT CEffect_Distortion::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vEffectColor, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTime", &m_fCurrentTime, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_DiffuseTexture", _uint(m_eType))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(12)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CEffect_Distortion::Appear(_fvector vPos, _float fInitScale, _float fTargetScale, _float fScaleSpeed, _float fDuration, DISTORTION_TYPE eType)
{
	m_vEffectColor.w = 1.0f;
	m_fCurrentTime = 0.f;

	m_fInitScale = fInitScale;
	m_fCurrentScale = fInitScale;
	m_fTargetScale = fTargetScale;
	m_fScaleSpeed = fScaleSpeed;
	m_fDuration = fDuration;
	m_eType = eType;

	m_pTransformCom->Set_Scaled(m_fInitScale, m_fInitScale, m_fInitScale);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	_vector vCameraPos = m_pGameInstance->Find_Camera(m_iLevelIndex)->Get_Position();
	m_pTransformCom->BillBoardXZ();

	m_isActive = true;
}

void CEffect_Distortion::DisAppear()
{
	m_isActive = false;
}

HRESULT CEffect_Distortion::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Effect_Texture"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Distortion"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CEffect_Distortion* CEffect_Distortion::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffect_Distortion* pInstance = new CEffect_Distortion(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CEffect_Distortion"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CEffect_Distortion::Clone(void* pArg)
{
	CEffect_Distortion* pInstance = new CEffect_Distortion(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CEffect_Distortion"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Distortion::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
}
