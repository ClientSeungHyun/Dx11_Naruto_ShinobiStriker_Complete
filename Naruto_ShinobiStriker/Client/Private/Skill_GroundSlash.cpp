#include "stdafx.h"
#include "Skill_GroundSlash.h"

#include "GameInstance.h"
#include "Model.h"
#include "Collider.h"

#include "ObjectPool.h"
#include "ExplosionCircle.h"

CSkill_GroundSlash::CSkill_GroundSlash(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSkill{ pDevice, pContext }
{
}

CSkill_GroundSlash::CSkill_GroundSlash(const CSkill_GroundSlash& Prototype)
	: CSkill{ Prototype }
{
}

HRESULT CSkill_GroundSlash::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSkill_GroundSlash::Initialize(void* pArg)
{
	/* 직교투영을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	CGameObject::GAMEOBJECT_DESC	Desc;
	Desc.fRotationPerSec = 0.f;
	Desc.fSpeedPerSec = 0.;
	Desc.iLevelIndex = m_iLevelIndex;

	CMonster* pMonster = dynamic_cast<CMonster*>(m_pOwner);
	if (pMonster->Get_MonsterID() == SUSANOO)
		strcpy_s(m_szPositionBoneName, "L_Hand_Weapon_cnt_tr");

	m_pSocketMatrix = pMonster->Get_Model()->Get_BoneCombindTransformationMatrix_Ptr(m_szPositionBoneName);

	m_vEffectColor = _float4(0.68f, 0.23f, 0.89f, 0.2f);
	m_isActive = false;

	return S_OK;
}

void CSkill_GroundSlash::Priority_Update(_float fTimeDelta)
{
}

void CSkill_GroundSlash::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	m_vTexCoord.x -= fTimeDelta;
	if (m_fCurrentTime < 0.3f)
	{
		m_fCurrentScale += 50.f * fTimeDelta;
		if (m_fCurrentScale >= 25.f)
			m_fCurrentScale = 25.f;

;		m_pTransformCom->Set_Scaled(m_fCurrentScale, m_fCurrentScale, m_fCurrentScale);
	}

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
	m_pTransformCom->BillBoard();
}

void CSkill_GroundSlash::Late_Update(_float fTimeDelta)
{
	if (m_fCurrentTime >= 0.3f)
	{
		m_pColliderCom->IsActive(false);

		if(m_vTexCoord.x <= -2.2f)
			m_isActive = false;
	}

	m_pGameInstance->Add_ColliderList(m_pColliderCom);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONLIGHT, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_EFFECT, this);
}

HRESULT CSkill_GroundSlash::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vEffectColor, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTexCoordX", &m_vTexCoord.x, sizeof(_float))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(8)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render((_uint)i)))
			return E_FAIL;
	}

#ifdef _DEBUG
	m_pColliderCom->Render();
#endif

	return S_OK;
}

void CSkill_GroundSlash::OnCollisionEnter(CGameObject* pOther)
{
	if (pOther->Get_LayerTag() == TEXT("Layer_Player"))
	{
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(pOther);
		if (pPlayer->Get_IsInvincible())
			return;

		_float2 vForce = _float2(30.f, 15.f);
		pPlayer->Damaged(20.f);
		m_pGameInstance->Start_Lack(TEXT("Timer_60"), 15);
		pPlayer->Change_State(CPlayer::HIT_SPIN, &vForce);
	}
}

void CSkill_GroundSlash::OnCollisionStay(CGameObject* pOther)
{
}

void CSkill_GroundSlash::OnCollisionExit(CGameObject* pOther)
{
}

void CSkill_GroundSlash::Appear()
{
	if (m_isActive)
		return;

	m_fCurrentTime = 0.f;
	m_iAppearIndex = 0;
	m_vTexCoord.x = 0.f;
	m_vEffectColor.w = 0.9f;

	_matrix		WorldMatrix = XMMatrixIdentity();
	_matrix		MonsterWorldMatrix = m_pOwner->Get_Transform()->Get_WorldMatrix();

	_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);
	WorldMatrix = SocketMatrix * MonsterWorldMatrix;

	_vector vPosition = WorldMatrix.r[3] + XMVector3Normalize(WorldMatrix.r[1]) * 4.f;
	_float3 vPos;
	XMStoreFloat3(&vPos, vPosition);

	CObjectPool<CExplosionCircle>::Get_GameObject()->Appear(vPos, _float4(0.f, 0.f, 0.f, 0.8f), 18.f, 27.f, 5.f, 0.6f);
	m_fCurrentScale = 5.f;
	m_pTransformCom->Set_Scaled(m_fCurrentScale, m_fCurrentScale, m_fCurrentScale);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	m_pColliderCom->IsActive(true);

	m_isDisappear = false;
	m_isActive = true;
}

void CSkill_GroundSlash::Disappear()
{
	m_pColliderCom->IsActive(false);
	m_isDisappear = true;
}

HRESULT CSkill_GroundSlash::Ready_Components()
{
	__super::Ready_Components();

	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Effect"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Noise09"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_ExplosionSphere"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	_float3 vInitPos = {};
	XMStoreFloat3(&vInitPos, Get_Position());

	/* For.Com_Collider */
	CBounding_Sphere::BOUNDING_SPHERE_DESC			ColliderSphereDesc{};
	ColliderSphereDesc.fRadius = 0.4f;
	ColliderSphereDesc.vCenter = vInitPos;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderSphereDesc)))
		return E_FAIL;
	m_pColliderCom->Set_Owner(this);

	return S_OK;
}

CSkill_GroundSlash* CSkill_GroundSlash::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkill_GroundSlash* pInstance = new CSkill_GroundSlash(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSkill_GroundSlash"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CSkill_GroundSlash::Clone(void* pArg)
{
	CSkill_GroundSlash* pInstance = new CSkill_GroundSlash(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CSkill_GroundSlash"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkill_GroundSlash::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pMaskTextureCom);
}
