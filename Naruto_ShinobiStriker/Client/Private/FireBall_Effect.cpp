#include "stdafx.h"
#include "FireBall_Effect.h"

#include "GameInstance.h"

#include "Player.h"
#include "Model.h"

CFireBall_Effect::CFireBall_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect{ pDevice, pContext }
{
}

CFireBall_Effect::CFireBall_Effect(const CFireBall_Effect& Prototype)
	: CEffect{ Prototype }
{
}

HRESULT CFireBall_Effect::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CFireBall_Effect::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_isActive = false;
	m_vEffectColor = _float4(1.f, 0.f, 0.f, 0.7f);


	return S_OK;
}

void CFireBall_Effect::Priority_Update(_float fTimeDelta)
{
}

void CFireBall_Effect::Update(_float fTimeDelta)
{
	m_fTexCoordX += 2.f * fTimeDelta;

	//if (m_fTexCoordX >= 1.f)
		//DisAppear();

	if (m_fCurrentScale != m_fTargetScale)
	{
		m_fCurrentScale += 3.f * fTimeDelta;

		if (m_fCurrentScale > m_fTargetScale)
			m_fCurrentScale = m_fTargetScale;
	}
	m_pTransformCom->Set_Scaled(m_fCurrentScale, m_fCurrentScale, m_fCurrentScale);

	_vector vPos = m_pOwner->Get_Position();
	_vector vLook = XMVector3Normalize(m_pOwner->Get_Transform()->Get_State(CTransform::STATE_LOOK));

	m_pTransformCom->LookAt(vPos + vLook);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
}

void CFireBall_Effect::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONLIGHT, this);
}

HRESULT CFireBall_Effect::Render()
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

void CFireBall_Effect::Appear()
{
	m_fTexCoordX = 0.f;
	m_fCurrentScale = 0.01f;

	_vector vPos = m_pOwner->Get_Position();
	_vector vLook = XMVector3Normalize(m_pOwner->Get_Transform()->Get_State(CTransform::STATE_LOOK));

	m_pTransformCom->LookAt_Forward(vPos + vLook);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	m_pTransformCom->Set_Scaled(0.1f, 0.1f, 0.1f);
	m_isActive = true;
}

void CFireBall_Effect::DisAppear()
{
	m_isActive = false;
}

HRESULT CFireBall_Effect::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Effect"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_FireBallEffect"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

CFireBall_Effect* CFireBall_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFireBall_Effect* pInstance = new CFireBall_Effect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CFireBall_Effect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CFireBall_Effect::Clone(void* pArg)
{
	CFireBall_Effect* pInstance = new CFireBall_Effect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CFireBall_Effect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFireBall_Effect::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

}
