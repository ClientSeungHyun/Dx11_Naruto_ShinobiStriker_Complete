#include "stdafx.h"
#include "Wood.h"

#include "GameInstance.h"

CWood::CWood(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CWood::CWood(const CWood& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CWood::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWood::Initialize(void* pArg)
{
	/* 직교투영을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_isActive = false;

	return S_OK;
}

void CWood::Priority_Update(_float fTimeDelta)
{

}

void CWood::Update(_float fTimeDelta)
{
	m_fAliveTime += fTimeDelta;

	_vector vPos = Get_Position();
	if (m_fAliveDuration < 1.f)
	{
		vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + (1.f - m_fAliveDuration) * fTimeDelta);
	}
	else
	{
		vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) - (2.f - m_fAliveDuration) * fTimeDelta);
	}

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	if (m_fAliveTime >= m_fAliveDuration)
	{
		m_isEnd = true;
	}

	if (m_isEnd)
	{
		m_fAlpha -= fTimeDelta;
		if (m_fAlpha <= 0.01f)
			m_isActive = false;
	}
}

void CWood::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CWood::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i);

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render((_uint)i)))
			return E_FAIL;
	}

	return S_OK;
}

void CWood::Appear(_fmatrix WorldMatrix)
{ 
	_float3 vScale = m_pTransformCom->Get_Scaled();

	_vector vRight = XMVector3Normalize(WorldMatrix.r[0]) * vScale.x;
	_vector vUp = XMVector3Normalize(WorldMatrix.r[1]) * vScale.y;
	_vector vLook = XMVector3Normalize(WorldMatrix.r[2]) * vScale.z;
	_vector vPos = WorldMatrix.r[3];

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
	m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(vPos, XMVectorGetY(vPos) + 0.5f));

	m_fAliveTime = 0.f;
	m_fAlpha = 1.f;

	m_isActive = true;
}

HRESULT CWood::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Model"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Wood"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

CWood* CWood::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWood* pInstance = new CWood(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CWood"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CWood::Clone(void* pArg)
{
	CWood* pInstance = new CWood(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CWood"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWood::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
