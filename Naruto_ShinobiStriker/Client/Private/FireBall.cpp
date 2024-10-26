#include "stdafx.h"
#include "FireBall.h"

#include "GameInstance.h"

#include "Player.h"
#include "Model.h"

CFireBall::CFireBall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect{ pDevice, pContext }
{
}

CFireBall::CFireBall(const CFireBall& Prototype)
	: CEffect{ Prototype }
{
}

HRESULT CFireBall::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CFireBall::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_isActive = false;
	m_vEffectColor = _float4(1.f, 0.f, 0.f, 1.f);

	m_fCurrentScale = 0.01f;
	m_pTransformCom->Set_Scaled(0.01f, 0.01f, 0.01f);
	m_fScaledSpeed = 6.f;
	m_fTargetScale = 1.f;

	return S_OK;
}

void CFireBall::Priority_Update(_float fTimeDelta)
{
}

void CFireBall::Update(_float fTimeDelta)
{
	m_fTexCoordX += fTimeDelta;
	//m_fTexCoordY += 2.f * fTimeDelta;

	if (m_fCurrentScale != m_fTargetScale)
	{
		m_fCurrentScale += m_fScaledSpeed * fTimeDelta;

		if (m_fCurrentScale > m_fTargetScale)
			m_fCurrentScale = m_fTargetScale;
	}

	m_pTransformCom->Set_Scaled(m_fCurrentScale, m_fCurrentScale, m_fCurrentScale);

	_vector vPos = m_pOwner->Get_Position();
	_vector vLook = XMVector3Normalize(m_pOwner->Get_Transform()->Get_State(CTransform::STATE_LOOK));

	m_pTransformCom->LookAt(vPos + vLook);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
}

void CFireBall::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONLIGHT, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_EFFECT, this);
}

HRESULT CFireBall::Render()
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
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTexCoordY", &m_fTexCoordY, sizeof(_float))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_MaskTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(4)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render((_uint)i)))
			return E_FAIL;
	}

	return S_OK;
}

void CFireBall::Appear()
{
	m_fCurrentScale = 0.01f;
	m_fTexCoordX = 0.f;
	m_fTexCoordY = 0.f;

	_vector vPos = m_pOwner->Get_Position();
	_vector vLook = XMVector3Normalize(m_pOwner->Get_Transform()->Get_State(CTransform::STATE_LOOK));

	m_pTransformCom->Set_Scaled(0.1f, 0.1f, 0.1f);
	m_pTransformCom->LookAt(vLook);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	m_isActive = true;
}

void CFireBall::DisAppear()
{
	m_isActive = false;
}

HRESULT CFireBall::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Effect"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_FireBall"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

CFireBall* CFireBall::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFireBall* pInstance = new CFireBall(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CFireBall"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CFireBall::Clone(void* pArg)
{
	CFireBall* pInstance = new CFireBall(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CFireBall"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFireBall::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

}
