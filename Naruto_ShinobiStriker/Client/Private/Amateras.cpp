#include "stdafx.h"
#include "Amateras.h"

#include "GameInstance.h"
#include "ObjectPool.h"
#include "Model.h"

CAmateras::CAmateras(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect{ pDevice, pContext }
{
}

CAmateras::CAmateras(const CAmateras& Prototype)
	: CEffect{ Prototype }
{
}

HRESULT CAmateras::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CAmateras::Initialize(void* pArg)
{
	AMATERAS_DESC* pDesc = static_cast<AMATERAS_DESC*>(pArg);
	m_eType = pDesc->eType;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_isActive = false;
	m_vEffectColor = _float4(0.f, 0.f, 1.f, 1.f);

	return S_OK;
}

void CAmateras::Priority_Update(_float fTimeDelta)
{
}

void CAmateras::Update(_float fTimeDelta)
{
	m_fTexCoord += 1.5f * fTimeDelta;

	if (m_fTexCoord >= 1.f)
	{
		CObjectPool<CAmateras>::Return_GameObject(this);
		m_isActive = false;
	}

}

void CAmateras::Late_Update(_float fTimeDelta)
{

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONLIGHT, this);
}

HRESULT CAmateras::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vEffectColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTexCoordY", &m_fTexCoord, sizeof(_float))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i)))
			return E_FAIL;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DissolveTexture", TEXTURE_TYPE::OPACITY, (_uint)i)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(5)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render((_uint)i)))
			return E_FAIL;
	}

	return S_OK;
}

void CAmateras::Appear(_fvector vPos, _float fScale)
{
	if (m_isActive)
		return;

	if (m_eType == AMATERAS0 || m_eType == AMATERAS5)
		fScale *= 0.5f;

	m_pTransformCom->Set_Scaled(fScale, fScale, fScale);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	m_pTransformCom->BillBoard();


	m_fTexCoord = -1.f;
	m_vEffectColor.w = 1.0f;

	m_isActive = true;
}

void CAmateras::DisAppear()
{
	m_isActive = false;
}

HRESULT CAmateras::Ready_Components()
{
	_wstring strPrototypeTag = TEXT("Prototype_Component_Model_");
	_wstring strModelName;


	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Effect"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	switch (m_eType)
	{
	case Client::CAmateras::AMATERAS0:
		strModelName = TEXT("Amateras0");
		break;
	case Client::CAmateras::AMATERAS1:
		strModelName = TEXT("Amateras1");
		break;
	case Client::CAmateras::AMATERAS2:
		strModelName = TEXT("Amateras2");
		break;
	case Client::CAmateras::AMATERAS3:
		strModelName = TEXT("Amateras3");
		break;
	case Client::CAmateras::AMATERAS4:
		strModelName = TEXT("Amateras4");
		break;
	case Client::CAmateras::AMATERAS5:
		strModelName = TEXT("Amateras5");
		break;
	}

	strPrototypeTag += strModelName;
	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, strPrototypeTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

CAmateras* CAmateras::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAmateras* pInstance = new CAmateras(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CAmateras"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CAmateras::Clone(void* pArg)
{
	CAmateras* pInstance = new CAmateras(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CAmateras"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAmateras::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

}
