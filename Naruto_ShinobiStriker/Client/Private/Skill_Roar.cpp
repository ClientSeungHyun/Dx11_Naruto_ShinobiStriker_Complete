#include "stdafx.h"
#include "Skill_Roar.h"

#include "GameInstance.h"
#include "Model.h"
#include "Collider.h"

CSkill_Roar::CSkill_Roar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSkill{ pDevice, pContext }
{
}

CSkill_Roar::CSkill_Roar(const CSkill_Roar& Prototype)
	: CSkill{ Prototype }
{
}

HRESULT CSkill_Roar::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSkill_Roar::Initialize(void* pArg)
{
	/* 직교투영을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	CEffect_TextureScale::TEXTURE_SCALE_DESC	Desc;
	Desc.fRotationPerSec = 0.f;
	Desc.fSpeedPerSec = 0.;
	Desc.iLevelIndex = m_iLevelIndex;
	Desc.vColor = _float4(1.f, 1.f, 1.f, 0.8f);
	Desc.fDuration = 0.2f;

	CEffect_TextureScale* m_pRoarTexture = nullptr;
	for (_uint i = 0; i < 6; ++i)
	{
		Desc.strTextureTag = TEXT("Prototype_Component_Texture_RoarEffect1");
		m_pRoarTexture = dynamic_cast<CEffect_TextureScale*>(m_pGameInstance->Get_CloneObject_AddLayer(LEVEL_STATIC, TEXT("Layer_SkillEffect"), TEXT("Prototype_GameObject_TextureScale"), &Desc));
		if (nullptr == m_pRoarTexture)
			return E_FAIL;
		m_pRoarTexture->Set_Owner(this);
		m_EffectTextures.emplace_back(m_pRoarTexture);

		Desc.strTextureTag = TEXT("Prototype_Component_Texture_RoarEffect2");
		m_pRoarTexture = dynamic_cast<CEffect_TextureScale*>(m_pGameInstance->Get_CloneObject_AddLayer(LEVEL_STATIC, TEXT("Layer_SkillEffect"), TEXT("Prototype_GameObject_TextureScale"), &Desc));
		if (nullptr == m_pRoarTexture)
			return E_FAIL;
		m_pRoarTexture->Set_Owner(this);
		m_EffectTextures.emplace_back(m_pRoarTexture);
	}

	CMonster* pMonster = dynamic_cast<CMonster*>(m_pOwner);
	if(pMonster->Get_MonsterID() == KURAMA)
		strcpy_s(m_szPositionBoneName, "Tongue4");

	m_pSocketMatrix = pMonster->Get_Model()->Get_BoneCombindTransformationMatrix_Ptr(m_szPositionBoneName);

	m_vEffectColor = _float4(1.f, 1.f, 1.f, 0.9f);
	m_pTransformCom->Set_Scaled(30.f, 30.f, 30.f);
	m_isActive = false;

	return S_OK;
}

void CSkill_Roar::Priority_Update(_float fTimeDelta)
{
}

void CSkill_Roar::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if (m_isDisappear)
	{
		for (auto& EffectTexture : m_EffectTextures)
		{
			if (EffectTexture->IsActive())
				return;
		}

		m_isActive = false;
		return;
	}

	if (m_fCurrentTime >= 0.1f)
	{
		if (m_iAppearIndex % 2 == 0)
		{
			m_EffectTextures[m_iAppearIndex]->Appear(0.1f, 1.f, true);
		}
		else
		{
			m_EffectTextures[m_iAppearIndex]->Appear(0.1f, 1.f, false);
		}
		m_iAppearIndex = (m_iAppearIndex + 1) % m_EffectTextures.size();
		m_fCurrentTime = 0.f;
	}

	_matrix		WorldMatrix = XMMatrixIdentity();
	_matrix		MonsterWorldMatrix = m_pOwner->Get_Transform()->Get_WorldMatrix();

	_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);
	WorldMatrix = SocketMatrix * MonsterWorldMatrix;

	_vector vPosition = WorldMatrix.r[3];

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	m_pTransformCom->BillBoard();
}

void CSkill_Roar::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_ColliderList(m_pColliderCom);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONLIGHT, this);
}

HRESULT CSkill_Roar::Render()
{
#ifdef _DEBUG
	m_pColliderCom->Render();
#endif

	return S_OK;
}

void CSkill_Roar::OnCollisionEnter(CGameObject* pOther)
{
	if (pOther->Get_LayerTag() == TEXT("Layer_Player"))
	{
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(pOther);
		if (pPlayer->Get_IsInvincible())
			return;

		_float2 vForce = _float2(30.f, 15.f);
		pPlayer->Change_State(CPlayer::HIT_SPIN, &vForce);
	}
}

void CSkill_Roar::OnCollisionStay(CGameObject* pOther)
{
}

void CSkill_Roar::OnCollisionExit(CGameObject* pOther)
{
}

void CSkill_Roar::Appear()
{
	if (m_isActive)
		return;

	m_fCurrentTime = 0.8f;
	m_iAppearIndex = 0;

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pOwner->Get_Position());
	m_pColliderCom->IsActive(true);

	m_isDisappear = false;
	m_isActive = true;
}

void CSkill_Roar::Disappear()
{
	m_pColliderCom->IsActive(false);
	m_isDisappear = true;
}

HRESULT CSkill_Roar::Ready_Components()
{
	__super::Ready_Components();

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

CSkill_Roar* CSkill_Roar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkill_Roar* pInstance = new CSkill_Roar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSkill_Roar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CSkill_Roar::Clone(void* pArg)
{
	CSkill_Roar* pInstance = new CSkill_Roar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CSkill_Roar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkill_Roar::Free()
{
	__super::Free();
}
