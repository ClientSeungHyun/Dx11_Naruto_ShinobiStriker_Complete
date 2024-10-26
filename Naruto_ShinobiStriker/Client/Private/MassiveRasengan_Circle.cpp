#include "stdafx.h"
#include "MassiveRasengan_Circle.h"

#include "GameInstance.h"

#include "Camera.h"
#include "Model.h"

CMassiveRasengan_Circle::CMassiveRasengan_Circle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect{ pDevice, pContext }
{
}

CMassiveRasengan_Circle::CMassiveRasengan_Circle(const CMassiveRasengan_Circle& Prototype)
	: CEffect{ Prototype }
{
}

HRESULT CMassiveRasengan_Circle::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CMassiveRasengan_Circle::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_isActive = false;
	m_vEffectColor = _float4(0.f, 0.f, 1.f, 0.1f);

	m_pTransformCom->Set_Scaled(0.1f, 0.1f, 0.1f);
	m_fScaledSpeed = 10.f;
	m_fTargetScale = 7.2f;

	return S_OK;
}

void CMassiveRasengan_Circle::Priority_Update(_float fTimeDelta)
{
}

void CMassiveRasengan_Circle::Update(_float fTimeDelta)
{
	m_fRadian += 7.f * fTimeDelta;

	if (m_fCurrentScale != m_fTargetScale)
	{
		m_fCurrentScale += m_fScaledSpeed * fTimeDelta;

		if (m_fCurrentScale > m_fTargetScale)
			m_fCurrentScale = m_fTargetScale;
	}

	m_pTransformCom->Set_Scaled(m_fCurrentScale, m_fCurrentScale, m_fCurrentScale);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pOwner->Get_Position());

	_vector vCameraPos = m_pGameInstance->Find_Camera(m_iLevelIndex)->Get_Position();
	m_pTransformCom->BillBoard();
}

void CMassiveRasengan_Circle::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_EFFECT, this);
}

HRESULT CMassiveRasengan_Circle::Render()
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

void CMassiveRasengan_Circle::Appear(_float fScaleSpeed, _float fTargetScale)
{
	m_fTexCoord = 3.f;
	m_fCurrentScale = 0.1f;
	m_vEffectColor.w = 1.0f;
	m_fRadian = 0.f;
	m_fScaledSpeed = fScaleSpeed;
	m_fTargetScale = fTargetScale;

	m_pTransformCom->Set_Scaled(0.01f, 0.01f, 0.01f);
	m_isActive = true;
}

void CMassiveRasengan_Circle::DisAppear()
{
	m_isActive = false;
}

HRESULT CMassiveRasengan_Circle::Ready_Components()
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

CMassiveRasengan_Circle* CMassiveRasengan_Circle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMassiveRasengan_Circle* pInstance = new CMassiveRasengan_Circle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CRasenShuriken_Sphere"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CMassiveRasengan_Circle::Clone(void* pArg)
{
	CMassiveRasengan_Circle* pInstance = new CMassiveRasengan_Circle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CRasenShuriken_Sphere"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMassiveRasengan_Circle::Free()
{
	__super::Free();

	Safe_Release(m_pTexture_Ring);
	Safe_Release(m_pTexture_Sphere);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pVIBufferCom);
}
