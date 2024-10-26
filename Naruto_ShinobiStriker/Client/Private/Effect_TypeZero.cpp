#include "stdafx.h"
#include "Effect_TypeZero.h"

#include "GameInstance.h"
#include "Model.h"

CEffect_TypeZero::CEffect_TypeZero(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect{ pDevice, pContext }
{
}

CEffect_TypeZero::CEffect_TypeZero(const CEffect_TypeZero& Prototype)
	: CEffect{ Prototype }
{
}

HRESULT CEffect_TypeZero::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CEffect_TypeZero::Initialize(void* pArg)
{
	TYPEZERO_DESC* pDesc = static_cast<TYPEZERO_DESC*>(pArg);
	m_eType = pDesc->eType;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_isActive = false;
	m_vEffectColor = _float4(1.f, 0.9f, 0.f, 0.9f);


	return S_OK;
}

void CEffect_TypeZero::Priority_Update(_float fTimeDelta)
{
}

void CEffect_TypeZero::Update(_float fTimeDelta)
{
	m_fCurrentTime += fTimeDelta;

	if(m_fTexCoordX < -0.1f)
		m_fTexCoordX -= 1.f * fTimeDelta;
	else
		m_fTexCoordX -=  6.f * fTimeDelta;

	if (m_fTexCoordX <= -1.f)
	{
		m_isActive = false;
	}
}

void CEffect_TypeZero::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_EFFECT, this);
}

HRESULT CEffect_TypeZero::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vEffectColor, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTexCoordX", &m_fTexCoordX, sizeof(_float))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(11)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render((_uint)i)))
			return E_FAIL;
	}

	return S_OK;
}

void CEffect_TypeZero::Appear(_fmatrix PlayerWorldMatrix)
{
	m_fTexCoordX = 1.f;
	m_fCurrentTime = 0.f;

	_matrix WorldMatrix = PlayerWorldMatrix;

	_vector vRight = XMVector3Normalize(PlayerWorldMatrix.r[0]);
	_vector vUp = XMVector3Normalize(PlayerWorldMatrix.r[1]);
	_vector vLook = XMVector3Normalize(PlayerWorldMatrix.r[2]);
	_vector vPos = PlayerWorldMatrix.r[3] + vUp * 0.35f;
	WorldMatrix.r[3] = vPos;

	m_pTransformCom->Set_WorldMatrix(WorldMatrix);

	m_pTransformCom->Set_Scaled(1.f, 1.5f, 1.f);

	m_isActive = true;
}

void CEffect_TypeZero::DisAppear()
{
	m_isActive = false;
}

HRESULT CEffect_TypeZero::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Effect"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	_wstring strModelTag = TEXT("Prototype_Component_Model_TypeZero");
	_wstring strModelNum = to_wstring((_uint)m_eType);
	strModelTag += strModelNum;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, strModelTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

CEffect_TypeZero* CEffect_TypeZero::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffect_TypeZero* pInstance = new CEffect_TypeZero(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CEffect_TypeZero"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CEffect_TypeZero::Clone(void* pArg)
{
	CEffect_TypeZero* pInstance = new CEffect_TypeZero(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CEffect_TypeZero"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_TypeZero::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

}
