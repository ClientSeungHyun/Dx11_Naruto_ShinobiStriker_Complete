#include "stdafx.h"
#include "RasenganRing.h"

#include "GameInstance.h"

#include "Player.h"
#include "Model.h"
#include "ObjectPool.h"

CRasenganRing::CRasenganRing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect{ pDevice, pContext }
{
}

CRasenganRing::CRasenganRing(const CRasenganRing& Prototype)
	: CEffect{ Prototype }
{
}

HRESULT CRasenganRing::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CRasenganRing::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_isActive = false;
	m_vEffectColor = _float4(0.05f, 0.75f, 0.9f, 1.f);

	m_pTransformCom->Set_Scaled(0.01f, 0.01f, 0.01f);

	return S_OK;
}

void CRasenganRing::Priority_Update(_float fTimeDelta)
{
}

void CRasenganRing::Update(_float fTimeDelta)
{
	m_fCurrentTime += fTimeDelta;
	if (m_fCurrentTime >= m_fLifeTime)
	{
		m_vEffectColor.w -= fTimeDelta;
		if (m_vEffectColor.w <= 0.f)
		{
			m_isActive = false;
			CObjectPool<CRasenganRing>::Return_GameObject(this);
		}
	}


	m_fTexCoordX += 3.f * fTimeDelta;

	if (m_fCurrentScale <= m_fTargetScale)
	{
		m_fCurrentScale += m_fScaledSpeed * fTimeDelta;

		if (m_fCurrentScale > m_fTargetScale)
			m_fCurrentScale = m_fTargetScale;
	}

	m_pTransformCom->Set_Scaled(m_fCurrentScale, m_fCurrentScale, m_fCurrentScale);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pOwner->Get_Position());
}

void CRasenganRing::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);
}

HRESULT CRasenganRing::Render()
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
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_MaskTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(7)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render((_uint)i)))
			return E_FAIL;
	}

	return S_OK;
}

void CRasenganRing::Appear(CGameObject* pOwner, _float fInitScale, _float fTargetScale, _float fScaleSpeed, _float fLifeTime, _float4 vColor)
{
	if (nullptr == pOwner)
	{
		CObjectPool<CRasenganRing>::Return_GameObject(this);
		return;
	}

	m_pOwner = pOwner;

	m_fCurrentScale = fInitScale;
	m_fTargetScale = fTargetScale;
	m_fScaledSpeed = fScaleSpeed;
	m_fLifeTime = fLifeTime;
	m_vEffectColor = vColor;

	m_fCurrentTime = 0.f;
	m_fTexCoordX = 0.f;
	m_vEffectColor.w = 1.f;

	_float fX = XMConvertToRadians((_float)(rand() % 360));
	_float fY = XMConvertToRadians((_float)(rand() % 360));
	_float fZ = XMConvertToRadians((_float)(rand() % 360));

	m_pTransformCom->Set_Scaled(m_fCurrentScale, m_fCurrentScale, m_fCurrentScale);
	m_pTransformCom->Rotation(fX, fY, fZ);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pOwner->Get_Position());

	m_isActive = true;
}

void CRasenganRing::DisAppear()
{
	m_isActive = false;
}

HRESULT CRasenganRing::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Effect"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_RasenganRing"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

CRasenganRing* CRasenganRing::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRasenganRing* pInstance = new CRasenganRing(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CFireBall"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CRasenganRing::Clone(void* pArg)
{
	CRasenganRing* pInstance = new CRasenganRing(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CRasenganRing"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRasenganRing::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

}
