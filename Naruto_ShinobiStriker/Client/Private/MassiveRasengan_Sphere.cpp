#include "stdafx.h"
#include "MassiveRasengan_Sphere.h"

#include "GameInstance.h"

#include "Model.h"

CMassiveRasengan_Sphere::CMassiveRasengan_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect{ pDevice, pContext }
{
}

CMassiveRasengan_Sphere::CMassiveRasengan_Sphere(const CMassiveRasengan_Sphere& Prototype)
	: CEffect{ Prototype }
{
}

HRESULT CMassiveRasengan_Sphere::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CMassiveRasengan_Sphere::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_isActive = false;
	m_vEffectColor = _float4(1.f, 1.f, 1.f, 0.7f);

	m_pTransformCom->Set_Scaled(0.1f, 0.1f, 0.1f);
	m_fScaledSpeed = 8.f;
	m_fTargetScale = 5.7f;

	return S_OK;
}

void CMassiveRasengan_Sphere::Priority_Update(_float fTimeDelta)
{
}

void CMassiveRasengan_Sphere::Update(_float fTimeDelta)
{
	m_fTexCoord += 3.f * fTimeDelta;

	if (m_fCurrentScale != m_fTargetScale)
	{
		m_fCurrentScale += m_fScaledSpeed * fTimeDelta;

		if (m_fCurrentScale > m_fTargetScale)
			m_fCurrentScale = m_fTargetScale;
	}

	m_pTransformCom->Set_Scaled(m_fCurrentScale, m_fCurrentScale, m_fCurrentScale);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pOwner->Get_Position());

}

void CMassiveRasengan_Sphere::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);
}

HRESULT CMassiveRasengan_Sphere::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vEffectColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTexCoordX", &m_fTexCoord, sizeof(_float))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_MaskTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render((_uint)i)))
			return E_FAIL;
	}

	return S_OK;
}

void CMassiveRasengan_Sphere::Appear(_float fScaleSpeed, _float fTargetScale)
{
	m_fTexCoord = 3.f;
	m_fCurrentScale = 0.1f;
	m_vEffectColor.w = 1.0f;
	m_fScaledSpeed = fScaleSpeed;
	m_fTargetScale = fTargetScale;

	m_pTransformCom->Set_Scaled(0.01f, 0.01f, 0.01f);
	m_isActive = true;
}

void CMassiveRasengan_Sphere::DisAppear()
{
	m_isActive = false;
}

HRESULT CMassiveRasengan_Sphere::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Effect"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_MassiveRasengan"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

CMassiveRasengan_Sphere* CMassiveRasengan_Sphere::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMassiveRasengan_Sphere* pInstance = new CMassiveRasengan_Sphere(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMassiveRasengan_Sphere"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CMassiveRasengan_Sphere::Clone(void* pArg)
{
	CMassiveRasengan_Sphere* pInstance = new CMassiveRasengan_Sphere(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMassiveRasengan_Sphere"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMassiveRasengan_Sphere::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

}
