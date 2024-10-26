#include "stdafx.h"
#include "Rasengan_RunSmoke.h"

#include "GameInstance.h"

#include "Model.h"

CRasengan_RunSmoke::CRasengan_RunSmoke(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect{ pDevice, pContext }
{
}

CRasengan_RunSmoke::CRasengan_RunSmoke(const CRasengan_RunSmoke& Prototype)
	: CEffect{ Prototype }
{
}

HRESULT CRasengan_RunSmoke::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CRasengan_RunSmoke::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_isActive = false;
	m_vEffectColor = _float4(0.8f, 0.8f, 0.8f, 0.7f);

	m_pTransformCom->Set_Scaled(0.01f, 0.01f, 0.01f);

	m_fAliveTime = 1.0f;

	return S_OK;
}

void CRasengan_RunSmoke::Priority_Update(_float fTimeDelta)
{
}

void CRasengan_RunSmoke::Update(_float fTimeDelta)
{
	m_fTexCoord -= fTimeDelta;

	if (m_isEnd)
	{
		m_vEffectColor.w -= 2.f * fTimeDelta;
		if (m_vEffectColor.w <= 0.0f)
			m_isActive = false;
	}

}

void CRasengan_RunSmoke::Late_Update(_float fTimeDelta)
{
	_vector vPos = m_pOwner->Get_Position();
	_vector vLook = XMVector3Normalize(m_pGameInstance->Find_Player(m_iLevelIndex)->Get_Transform()->Get_State(CTransform::STATE_LOOK));

	m_pTransformCom->LookAt(vPos + vLook);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONLIGHT, this);
}

HRESULT CRasengan_RunSmoke::Render()
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

void CRasengan_RunSmoke::Appear()
{
	if (m_isActive)
		return;

	m_fTexCoord = 3.f;
	m_fCurrentTime = 0.f;
	m_vEffectColor.w = 0.7f;

	m_isEnd = false;
	m_isActive = true;
}

void CRasengan_RunSmoke::DisAppear()
{
	m_isEnd = true;
}

HRESULT CRasengan_RunSmoke::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Effect"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Rasengan_RunSmoke"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

CRasengan_RunSmoke* CRasengan_RunSmoke::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRasengan_RunSmoke* pInstance = new CRasengan_RunSmoke(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CRasengan_CreateSmoke"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CRasengan_RunSmoke::Clone(void* pArg)
{
	CRasengan_RunSmoke* pInstance = new CRasengan_RunSmoke(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CRasengan_RunSmoke"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRasengan_RunSmoke::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

}
