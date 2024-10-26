#include "stdafx.h"
#include "Effect_RasenkoStar.h"

#include "GameInstance.h"
#include "Model.h"

CEffect_RasenkoStar::CEffect_RasenkoStar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect{ pDevice, pContext }
{
}

CEffect_RasenkoStar::CEffect_RasenkoStar(const CEffect_RasenkoStar& Prototype)
	: CEffect{ Prototype }
{
}

HRESULT CEffect_RasenkoStar::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CEffect_RasenkoStar::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_isActive = false;
	m_vEffectColor = _float4(0.f, 0.6f, 1.f, 0.9f);


	return S_OK;
}

void CEffect_RasenkoStar::Priority_Update(_float fTimeDelta)
{
}

void CEffect_RasenkoStar::Update(_float fTimeDelta)
{
	m_fCurrentTime += fTimeDelta;
	m_fTexCoordY += fTimeDelta;

	_matrix WorldMatrix = m_pOwner->Get_Transform()->Get_WorldMatrix();

	_vector vRight = XMVector3Normalize(WorldMatrix.r[0]);
	_vector vUp = XMVector3Normalize(WorldMatrix.r[1]);
	_vector vLook = XMVector3Normalize(WorldMatrix.r[2]);
	_vector vPos = WorldMatrix.r[3] + vUp * 2.3f + vLook * 1.f - vRight * 0.3f;
	WorldMatrix.r[3] = vPos;

	m_pTransformCom->Set_WorldMatrix(WorldMatrix);

	m_pTransformCom->Set_Scaled(3.5f, 2.5f, 2.5f);
}

void CEffect_RasenkoStar::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_EFFECT, this);
}

HRESULT CEffect_RasenkoStar::Render()
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

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(12)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render((_uint)i)))
			return E_FAIL;
	}

	return S_OK;
}

void CEffect_RasenkoStar::Appear(CGameObject* pOwner)
{
	m_fTexCoordY = 0.f;
	m_fCurrentTime = 0.f;

	m_pOwner = pOwner;

	m_pTransformCom->Set_Scaled(4.f, 4.f, 4.f);

	m_isActive = true;
}

void CEffect_RasenkoStar::DisAppear()
{
	m_isActive = false;
}

HRESULT CEffect_RasenkoStar::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Effect"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_RasenkoStar"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

CEffect_RasenkoStar* CEffect_RasenkoStar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffect_RasenkoStar* pInstance = new CEffect_RasenkoStar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CEffect_RasenkoStar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CEffect_RasenkoStar::Clone(void* pArg)
{
	CEffect_RasenkoStar* pInstance = new CEffect_RasenkoStar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CEffect_RasenkoStar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_RasenkoStar::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

}
