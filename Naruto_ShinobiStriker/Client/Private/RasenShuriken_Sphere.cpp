#include "stdafx.h"
#include "RasenShuriken_Sphere.h"

#include "GameInstance.h"

#include "Camera.h"
#include "Model.h"

CRasenShuriken_Sphere::CRasenShuriken_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect{ pDevice, pContext }
{
}

CRasenShuriken_Sphere::CRasenShuriken_Sphere(const CRasenShuriken_Sphere& Prototype)
	: CEffect{ Prototype }
{
}

HRESULT CRasenShuriken_Sphere::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CRasenShuriken_Sphere::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_isActive = false;
	m_vEffectColor = _float4(0.f, 0.f, 1.f, 1.f);

	m_pTransformCom->Set_Scaled(0.01f, 0.01f, 0.01f);
	m_fScaledSpeed = 2.f;
	m_fTargetScale = 0.5f;

	return S_OK;
}

void CRasenShuriken_Sphere::Priority_Update(_float fTimeDelta)
{
}

void CRasenShuriken_Sphere::Update(_float fTimeDelta)
{
	m_fRadian += 10.f * fTimeDelta;

	if (m_fCurrentScale != m_fTargetScale)
	{
		m_fCurrentScale += m_fScaledSpeed * fTimeDelta;

		if (m_fCurrentScale > m_fTargetScale)
			m_fCurrentScale = m_fTargetScale;
	}

	_matrix matOwnerWorld = m_pOwner->Get_Transform()->Get_WorldMatrix();
	m_pTransformCom->Set_WorldMatrix(matOwnerWorld);

	m_pTransformCom->Set_Scaled(m_fCurrentScale, m_fCurrentScale, m_fCurrentScale);

	_vector vCameraPos = m_pGameInstance->Find_Camera(m_iLevelIndex)->Get_Position();
	m_pTransformCom->BillBoardXZ();
}

void CRasenShuriken_Sphere::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);
}

HRESULT CRasenShuriken_Sphere::Render()
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

	if (FAILED(m_pShaderCom->Begin(1)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CRasenShuriken_Sphere::Appear()
{
	m_fTexCoord = 3.f;
	m_fCurrentScale = 0.1f;
	m_vEffectColor.w = 1.0f;
	m_fRadian = 0.f;

	m_pTransformCom->Set_Scaled(0.01f, 0.01f, 0.01f);
	m_isActive = true;
}

void CRasenShuriken_Sphere::DisAppear()
{
	m_isActive = false;
}

HRESULT CRasenShuriken_Sphere::Ready_Components()
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

CRasenShuriken_Sphere* CRasenShuriken_Sphere::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRasenShuriken_Sphere* pInstance = new CRasenShuriken_Sphere(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CRasenShuriken_Sphere"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CRasenShuriken_Sphere::Clone(void* pArg)
{
	CRasenShuriken_Sphere* pInstance = new CRasenShuriken_Sphere(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CRasenShuriken_Sphere"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRasenShuriken_Sphere::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTexture_Ring);
	Safe_Release(m_pTexture_Sphere);

}
