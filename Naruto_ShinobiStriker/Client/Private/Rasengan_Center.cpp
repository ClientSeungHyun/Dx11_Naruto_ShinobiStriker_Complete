#include "stdafx.h"
#include "Rasengan_Center.h"

#include "GameInstance.h"

#include "Camera.h"
#include "Model.h"

CRasengan_Center::CRasengan_Center(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect{ pDevice, pContext }
{
}

CRasengan_Center::CRasengan_Center(const CRasengan_Center& Prototype)
	: CEffect{ Prototype }
{
}

HRESULT CRasengan_Center::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CRasengan_Center::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_isActive = false;
	m_vEffectColor = _float4(0.f, 0.f, 1.f, 1.f);

	m_pTransformCom->Set_Scaled(0.01f, 0.01f, 0.01f);
	m_fScaledSpeed = 2.f;
	m_fTargetScale = 0.25f;

	return S_OK;
}

void CRasengan_Center::Priority_Update(_float fTimeDelta)
{
}

void CRasengan_Center::Update(_float fTimeDelta)
{
	m_fRadian += 8.f * fTimeDelta;

	if (m_fCurrentScale <= m_fTargetScale)
	{
		m_fCurrentScale += fTimeDelta;

		if (m_fCurrentScale >= m_fTargetScale)
			m_fCurrentScale = m_fTargetScale;
	}

	m_pTransformCom->Set_Scaled(m_fCurrentScale, m_fCurrentScale, m_fCurrentScale);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pOwner->Get_Position());

	_vector vCameraPos = m_pGameInstance->Find_Camera(m_iLevelIndex)->Get_Position();
	m_pTransformCom->BillBoard();
}

void CRasengan_Center::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_EFFECT, this);
}

HRESULT CRasengan_Center::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vEffectColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fRadian", &m_fRadian, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pTexture_Ring->Bind_ShadeResource(m_pShaderCom, "g_MaskTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pTexture_Sphere->Bind_ShadeResource(m_pShaderCom, "g_MaskTexture2", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CRasengan_Center::Appear()
{
	m_fTexCoord = 3.f;
	m_fCurrentScale = 0.1f;
	m_vEffectColor.w = 1.0f;
	m_fRadian = 0.f;
	m_isRing = false;
	m_pTransformCom->Set_Scaled(0.01f, 0.01f, 0.01f);
	m_isActive = true;
}

void CRasengan_Center::DisAppear()
{
	m_isActive = false;
}

HRESULT CRasengan_Center::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Effect_Texture"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Rasengan"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Ring"),
		TEXT("Com_Texture_Ring"), reinterpret_cast<CComponent**>(&m_pTexture_Ring))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Sphere"),
		TEXT("Com_Texture_Sphere"), reinterpret_cast<CComponent**>(&m_pTexture_Sphere))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CRasengan_Center* CRasengan_Center::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRasengan_Center* pInstance = new CRasengan_Center(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CRasengan_Center"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CRasengan_Center::Clone(void* pArg)
{
	CRasengan_Center* pInstance = new CRasengan_Center(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CRasengan_Center"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRasengan_Center::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTexture_Ring);
	Safe_Release(m_pTexture_Sphere);
}
