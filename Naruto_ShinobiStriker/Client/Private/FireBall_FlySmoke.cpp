#include "stdafx.h"
#include "FireBall_FlySmoke.h"

#include "GameInstance.h"

#include "Player.h"
#include "Model.h"

CFireBall_FlySmoke::CFireBall_FlySmoke(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect{ pDevice, pContext }
{
}

CFireBall_FlySmoke::CFireBall_FlySmoke(const CFireBall_FlySmoke& Prototype)
	: CEffect{ Prototype }
{
}

HRESULT CFireBall_FlySmoke::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CFireBall_FlySmoke::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_isActive = false;
	m_vEffectColor = _float4(1.f, 0.f, 0.f, 0.7f);

	//m_pTransformCom->Set_Scaled(0.01f, 0.01f, 0.01f);

	return S_OK;
}

void CFireBall_FlySmoke::Priority_Update(_float fTimeDelta)
{
}

void CFireBall_FlySmoke::Update(_float fTimeDelta)
{
	m_fTexCoordX += fTimeDelta;

	if (m_fCurrentScale != m_fTargetScale)
	{
		m_fCurrentScale += m_fScaledSpeed * fTimeDelta;

		if (m_fCurrentScale > m_fTargetScale)
			m_fCurrentScale = m_fTargetScale;
	}
	m_pTransformCom->Set_Scaled(m_fCurrentScale, m_fCurrentScale, m_fCurrentScale);

	_vector vPos = m_pOwner->Get_Position();
	_vector vLook = XMVector3Normalize(m_pOwner->Get_Transform()->Get_State(CTransform::STATE_LOOK));

	m_pTransformCom->LookAt(vPos + vLook * 0.1f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
}

void CFireBall_FlySmoke::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_EFFECT, this);
}

HRESULT CFireBall_FlySmoke::Render()
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

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render((_uint)i)))
			return E_FAIL;
	}

	return S_OK;
}

void CFireBall_FlySmoke::Appear()
{
	m_fTexCoordX = 0.f;
	m_fCurrentScale = 0.01f;

	_vector vPos = m_pOwner->Get_Position();
	_vector vLook = XMVector3Normalize(m_pOwner->Get_Transform()->Get_State(CTransform::STATE_LOOK));

	m_pTransformCom->LookAt_Forward(vPos + vLook);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos + vLook);

	m_pTransformCom->Set_Scaled(0.1f, 0.1f, 0.1f);
	m_isActive = true;
}

void CFireBall_FlySmoke::DisAppear()
{
	m_isActive = false;
}

HRESULT CFireBall_FlySmoke::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Effect"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_FireFlySmoke"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

CFireBall_FlySmoke* CFireBall_FlySmoke::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFireBall_FlySmoke* pInstance = new CFireBall_FlySmoke(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CFireBall_FlySmoke"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CFireBall_FlySmoke::Clone(void* pArg)
{
	CFireBall_FlySmoke* pInstance = new CFireBall_FlySmoke(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CFireBall_FlySmoke"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFireBall_FlySmoke::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

}
