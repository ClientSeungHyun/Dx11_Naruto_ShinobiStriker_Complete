#include "stdafx.h"
#include "JumpGuide.h"

#include "GameInstance.h"
#include "Camera.h"

CJumpGuide::CJumpGuide(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CJumpGuide::CJumpGuide(const CJumpGuide& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CJumpGuide::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CJumpGuide::Initialize(void* pArg)
{
	/* 직교투영을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_isActive = false;

	m_vColor = _float4(0.f, 1.f, 1.f, 0.8f);

	return S_OK;
}

void CJumpGuide::Priority_Update(_float fTimeDelta)
{

}

void CJumpGuide::Update(_float fTimeDelta)
{
	m_fTexCoordY += 0.8f * fTimeDelta;

	_vector vCameraLook = m_pGameInstance->Find_Camera(m_iLevelIndex)->Get_Transform()->Get_State(CTransform::STATE_LOOK) * -1.f;
	_vector vPos = m_pOwner->Get_Position();
	_vector vUp = XMVectorSet(0.f, 2.f, 0.f, 0.f);
	m_pTransformCom->LookAt(vPos + vCameraLook - vUp);

	//m_pTransformCom->LookAt(vPos + vCameraLook);
}

void CJumpGuide::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_EFFECT, this);
}

HRESULT CJumpGuide::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTexCoordY", &m_fTexCoordY, sizeof(_float))))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, "g_MaskTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i);

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render((_uint)i)))
			return E_FAIL;
	}

	return S_OK;
}

void CJumpGuide::Appear()
{
	_matrix OwnerMatrix = m_pOwner->Get_Transform()->Get_WorldMatrix();

	_vector vPos = OwnerMatrix.r[3];
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(vPos, XMVectorGetY(vPos) + 0.1f));

	_vector vCameraLook = m_pGameInstance->Find_Camera(m_iLevelIndex)->Get_Transform()->Get_State(CTransform::STATE_LOOK) * -1.f;
	_vector vUp = XMVectorSet(0.f, 2.f, 0.f, 0.f);
	m_pTransformCom->LookAt(vPos + vCameraLook - vUp);

	m_isActive = true;
}

void CJumpGuide::Disappear()
{
	m_isActive = false;
}

HRESULT CJumpGuide::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Effect"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(m_iLevelIndex, TEXT("Prototype_Component_Model_JumpGuide"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

CJumpGuide* CJumpGuide::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CJumpGuide* pInstance = new CJumpGuide(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CJumpGuide"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CJumpGuide::Clone(void* pArg)
{
	CJumpGuide* pInstance = new CJumpGuide(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CJumpGuide"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CJumpGuide::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
