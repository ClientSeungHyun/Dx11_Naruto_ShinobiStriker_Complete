#include "stdafx.h"
#include "Kirin.h"

#include "GameInstance.h"

CKirin::CKirin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CKirin::CKirin(const CKirin& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CKirin::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CKirin::Initialize(void* pArg)
{
	/* 직교투영을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_iAnimationIndex = m_pModelCom->Get_AnimationIndex("SK_WEP_Eff_Kirin_01_Anim", 1.0);

	m_isActive = false;
	m_vColor = _float4(0.7f, 0.9f, 1.f, 1.f);

	return S_OK;
}

void CKirin::Priority_Update(_float fTimeDelta)
{
}

void CKirin::Update(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);
}

void CKirin::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_EFFECT, this);
}

HRESULT CKirin::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);
		
		if (1 == i)
			m_vColor = _float4(1.f, 0.f, 0.f, 1.f);
		else
			m_vColor = _float4(0.7f, 0.9f, 1.f, 1.f);

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(1)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render((_uint)i)))
			return E_FAIL;
	}

	return S_OK;
}

void CKirin::Appear(_fvector vPos)
{
	m_pModelCom->SetUp_NextAnimation(m_iAnimationIndex, false, 0.1f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	m_isActive = true;
}

void CKirin::Disappear()
{
	m_isActive = false;
}

HRESULT CKirin::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Kirin"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}


CKirin* CKirin::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CKirin* pInstance = new CKirin(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CKirin::Clone(void* pArg)
{
	CKirin* pInstance = new CKirin(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CKirin::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
