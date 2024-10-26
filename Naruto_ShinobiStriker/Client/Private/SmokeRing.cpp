#include "stdafx.h"
#include "Effect_GroundBreak.h"

#include "GameInstance.h"
#include "Model.h"

#include "ObjectPool.h"
#include "SmokeRing.h"

CSmokeRing::CSmokeRing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect{ pDevice, pContext }
{
}

CSmokeRing::CSmokeRing(const CSmokeRing& Prototype)
	: CEffect{ Prototype }
{
}

HRESULT CSmokeRing::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CSmokeRing::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_isActive = false;
	m_vEffectColor = _float4(1.f, 1.f, 1.f, 1.f);

	m_pTransformCom->Set_Scaled(1.f, 1.f, 1.f);

	return S_OK;
}

void CSmokeRing::Priority_Update(_float fTimeDelta)
{
}

void CSmokeRing::Update(_float fTimeDelta)
{
	m_fCurrentTime += fTimeDelta;
	m_fCurrentScale += m_fScaleSpeed * fTimeDelta;
	if (m_fCurrentScale >= m_fTargetScale)
		m_fCurrentScale = m_fTargetScale;

	if (m_fCurrentTime >= 0.2f)
	{
		m_fDissloveAmount += 1.f * fTimeDelta;
		if (m_fDissloveAmount >= 1.f)
		{
			CObjectPool<CSmokeRing>::Return_GameObject(this);
			m_isActive = false;
		}
	}

	m_pTransformCom->Set_Scaled(m_fCurrentScale, 2.f, m_fCurrentScale);
}

void CSmokeRing::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONLIGHT, this);
}

HRESULT CSmokeRing::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vEffectColor, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolve", &m_fDissloveAmount, sizeof(_float))))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i);

		if (FAILED(m_pShaderCom->Begin(13)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render((_uint)i)))
			return E_FAIL;
	}

	return S_OK;
}

void CSmokeRing::Appear(_fvector vPos, _float fScaleSpeed, _float fInitScale, _float fTargetScale)
{
	m_vEffectColor = _float4(1.f, 1.f, 1.f, 1.f); //vColor;

	m_fScaleSpeed = fScaleSpeed;
	m_fCurrentScale = fInitScale;
	m_fTargetScale = fTargetScale;
	m_pTransformCom->Set_Scaled(fInitScale, 2.f, fInitScale);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	m_fDissloveAmount = 0.1f;

	m_isActive = true;
}

void CSmokeRing::DisAppear()
{
	m_isActive = false;
}

HRESULT CSmokeRing::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Effect"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_SmokeRing"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

CSmokeRing* CSmokeRing::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSmokeRing* pInstance = new CSmokeRing(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSmokeRing"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CSmokeRing::Clone(void* pArg)
{
	CSmokeRing* pInstance = new CSmokeRing(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CSmokeRing"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSmokeRing::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

}
