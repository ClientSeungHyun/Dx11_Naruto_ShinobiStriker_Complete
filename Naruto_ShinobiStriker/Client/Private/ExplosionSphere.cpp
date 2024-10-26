#include "stdafx.h"
#include "ExplosionSphere.h"

#include "GameInstance.h"

#include "Model.h"

CExplosionSphere::CExplosionSphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect{ pDevice, pContext }
{
}

CExplosionSphere::CExplosionSphere(const CExplosionSphere& Prototype)
	: CEffect{ Prototype }
{
}

HRESULT CExplosionSphere::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CExplosionSphere::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_isActive = false;
	m_vEffectColor = _float4(0.05f, 0.75f, 1.f, 0.9f);

	m_pTransformCom->Set_Scaled(0.01f, 0.01f, 0.01f);
	m_fScaledSpeed = 15.f;
	m_fTargetScale = 11.f;
	m_fAlphaRatio = 1.f;

	return S_OK;
}

void CExplosionSphere::Priority_Update(_float fTimeDelta)
{
}

void CExplosionSphere::Update(_float fTimeDelta)
{
	m_fCurrentTime += fTimeDelta;
	if (m_fCurrentTime >= 2.5f)
	{
		if (m_fCurrentTime <= 2.7f)
		{
			m_fCurrentScale -= m_fScaledSpeed * 4.f * fTimeDelta;
		}
		else
		{
			m_fCurrentScale += m_fScaledSpeed * 4.f * fTimeDelta;
			m_fAlphaRatio -= 3.f * fTimeDelta;
			if (m_fAlphaRatio <= 0.f)
				m_isActive = false;
		}
	}

	if (m_fCurrentScale <= m_fTargetScale)
	{
		m_fCurrentScale += m_fScaledSpeed * fTimeDelta;

		if (m_fCurrentScale > m_fTargetScale)
			m_fCurrentScale = m_fTargetScale;
	}

	m_pTransformCom->Set_Scaled(m_fCurrentScale, m_fCurrentScale, m_fCurrentScale);

	m_fTexCoordY += 3.f * fTimeDelta;
}

void CExplosionSphere::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONLIGHT, this);
}

HRESULT CExplosionSphere::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vEffectColor, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTexCoordY", &m_fTexCoordY, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolve", &m_fAlphaRatio, sizeof(_float))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i)))
			return E_FAIL;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DissolveTexture", TEXTURE_TYPE::OPACITY, (_uint)i)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(6)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render((_uint)i)))
			return E_FAIL;
	}

	return S_OK;
}

void CExplosionSphere::Appear()
{
	m_fCurrentScale = 0.1f;
	m_fAlphaRatio = 1.f;
	m_fCurrentTime = 0.f;

	m_pTransformCom->Set_Scaled(0.1f, 0.1f, 0.1f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pOwner->Get_Position());

	m_isActive = true;
}

void CExplosionSphere::DisAppear()
{
	m_isActive = false;
}

HRESULT CExplosionSphere::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Effect"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_ExplosionSphere"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

CExplosionSphere* CExplosionSphere::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CExplosionSphere* pInstance = new CExplosionSphere(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CExplosionSphere"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CExplosionSphere::Clone(void* pArg)
{
	CExplosionSphere* pInstance = new CExplosionSphere(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CExplosionSphere"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CExplosionSphere::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

}
