#include "stdafx.h"
#include "RasenShuriken_BladeFly.h"

#include "GameInstance.h"

#include "Model.h"

CRasenShuriken_BladeFly::CRasenShuriken_BladeFly(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect{ pDevice, pContext }
{
}

CRasenShuriken_BladeFly::CRasenShuriken_BladeFly(const CRasenShuriken_BladeFly& Prototype)
	: CEffect{ Prototype }
{
}

HRESULT CRasenShuriken_BladeFly::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CRasenShuriken_BladeFly::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_isActive = false;
	m_vEffectColor = _float4(0.05f, 0.75f, 0.9f, 0.9f);

	m_pTransformCom->Set_Scaled(0.01f, 0.01f, 0.01f);
	m_fScaledSpeed = 5.f;
	m_fRotationsSpeed = 20.f;
	m_fTargetScale = 3.f;

	return S_OK;
}

void CRasenShuriken_BladeFly::Priority_Update(_float fTimeDelta)
{
}

void CRasenShuriken_BladeFly::Update(_float fTimeDelta)
{
	if (m_fCurrentScale != m_fTargetScale)
	{
		m_fCurrentScale += m_fScaledSpeed * fTimeDelta;

		if (m_fCurrentScale > m_fTargetScale)
			m_fCurrentScale = m_fTargetScale;
	}

	_matrix matOwnerWorld = m_pOwner->Get_Transform()->Get_WorldMatrix();
	m_pTransformCom->Set_WorldMatrix(matOwnerWorld);

	m_fRadian += m_fRotationsSpeed * fTimeDelta;
	_vector vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
	m_pTransformCom->Rotation(vUp, m_fRadian);

	m_pTransformCom->Set_Scaled(m_fCurrentScale, m_fCurrentScale, m_fCurrentScale);
}

void CRasenShuriken_BladeFly::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_EFFECT, this);
}

HRESULT CRasenShuriken_BladeFly::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vEffectColor, sizeof(_float4))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		/*if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_MaskTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i)))
			return E_FAIL;*/

		if (FAILED(m_pShaderCom->Begin(1)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render((_uint)i)))
			return E_FAIL;
	}

	return S_OK;
}

void CRasenShuriken_BladeFly::Appear()
{
	m_fRadian = 0.f;
	m_fCurrentScale = 0.1f;
	m_vEffectColor.w = 1.0f;

	m_pTransformCom->Set_Scaled(0.01f, 0.01f, 0.01f);
	m_isActive = true;
}

void CRasenShuriken_BladeFly::DisAppear()
{
	m_isActive = false;
}

HRESULT CRasenShuriken_BladeFly::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Effect"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_RasenShuriken_BladeFly"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

CRasenShuriken_BladeFly* CRasenShuriken_BladeFly::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRasenShuriken_BladeFly* pInstance = new CRasenShuriken_BladeFly(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CRasenShuriken_BladeFly"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CRasenShuriken_BladeFly::Clone(void* pArg)
{
	CRasenShuriken_BladeFly* pInstance = new CRasenShuriken_BladeFly(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CRasenShuriken_BladeFly"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRasenShuriken_BladeFly::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

}
