#include "stdafx.h"
#include "Shadow_Naruto.h"
#include "Player_Naruto.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"

CShadow_Naruto::CShadow_Naruto(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayer{ pDevice, pContext }
{
}

CShadow_Naruto::CShadow_Naruto(const CShadow_Naruto& Prototype)
	: CPlayer{ Prototype }
{
}

HRESULT CShadow_Naruto::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CShadow_Naruto::Initialize(void* pArg)
{
	PLAYER_DESC* pDesc = static_cast<PLAYER_DESC*>(pArg);
	m_isShadow = true;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_isActive = false;

	return S_OK;
}

void CShadow_Naruto::Priority_Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;
}

void CShadow_Naruto::Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	if (m_pModelCom->Get_IsEnd_Animation(m_iAnimationIndex))
		m_isActive = false;

	m_pModelCom->Play_Animation(fTimeDelta);
}

void CShadow_Naruto::Late_Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CShadow_Naruto::Render()
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

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i)))
			return E_FAIL;
		/*if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", aiTextureType_NORMALS, i)))
			return E_FAIL;*/

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render((_uint)i)))
			return E_FAIL;
	}

	return S_OK;
}

void CShadow_Naruto::Create_ShadowNaruto(SHADOW_NARUTO_DESC tDesc)
{
	m_eSkill = tDesc.eSkill;
	m_eDir = tDesc.eDir;

	_matrix		OwnerWorldMatirx = m_pOwner->Get_Transform()->Get_WorldMatrix();

	_vector		vRight = OwnerWorldMatirx.r[0];
	_vector		vLook = OwnerWorldMatirx.r[2];
	_vector		vPosition = OwnerWorldMatirx.r[3];

	if (RASENSHURIKEN == m_eSkill)
	{
		if (LEFT == m_eDir)
		{
			m_iAnimationIndex = m_pModelCom->Get_AnimationIndex("NarutoShadowClone_Ninjutsu_TrueRasenShuriken_Left", 1.5);
		}
		else
		{
			m_iAnimationIndex = m_pModelCom->Get_AnimationIndex("NarutoShadowClone_Ninjutsu_TrueRasenShuriken_Right", 1.5);
		}
	}
	else if (RASENSHURIKEN_AERIAL == m_eSkill)
	{
		if (LEFT == m_eDir)
		{
			m_iAnimationIndex = m_pModelCom->Get_AnimationIndex("NarutoShadowClone_Ninjutsu_Aerial_TrueRasenShuriken_Left", 1.5);
		}
		else
		{
			m_iAnimationIndex = m_pModelCom->Get_AnimationIndex("NarutoShadowClone_Ninjutsu_Aerial_TrueRasenShuriken_Right", 1.5);
		}
	}
	else if(MASSIVERASENGAN == m_eSkill)
	{
		if (LEFT == m_eDir)
		{
			m_iAnimationIndex = m_pModelCom->Get_AnimationIndex("NarutoShadowClone_Ninjutsu_MassiveRasengan_Left_Loop", 1.5);
		}
		else
		{
			m_iAnimationIndex = m_pModelCom->Get_AnimationIndex("NarutoShadowClone_Ninjutsu_MassiveRasengan_Right_Loop", 1.5);
		}
	}

	if (LEFT == m_eDir)
		vPosition -= XMVector3Normalize(vRight) * 0.01f;
	else if (RIGHT == m_eDir)
		vPosition += XMVector3Normalize(vRight) * 0.01f;

	vPosition -= XMVector3Normalize(vLook) * 0.3f;

	OwnerWorldMatirx.r[3] = vPosition;
	m_pTransformCom->Set_WorldMatrix(OwnerWorldMatirx);

	m_pModelCom->SetUp_NextAnimation(m_iAnimationIndex, false, 0.01f);
	m_isActive = true;
}

HRESULT CShadow_Naruto::Ready_Components()
{
	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(m_iLevelIndex, TEXT("Prototype_Component_Model_Naruto"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

CShadow_Naruto* CShadow_Naruto::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CShadow_Naruto* pInstance = new CShadow_Naruto(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CShadow_Naruto"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CShadow_Naruto::Clone(void* pArg)
{
	CShadow_Naruto* pInstance = new CShadow_Naruto(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CShadow_Naruto"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShadow_Naruto::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
}
