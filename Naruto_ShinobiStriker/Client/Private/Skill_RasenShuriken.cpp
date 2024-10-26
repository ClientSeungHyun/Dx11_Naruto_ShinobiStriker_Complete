#include "stdafx.h"
#include "Skill_RasenShuriken.h"

#include "GameInstance.h"
#include "Model.h"
#include "Collider.h"

#include "ObjectPool.h"
#include "RasenganRing.h"
#include "PlayerCamera.h"

CSkill_RasenShuriken::CSkill_RasenShuriken(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSkill{ pDevice, pContext }
{
}

CSkill_RasenShuriken::CSkill_RasenShuriken(const CSkill_RasenShuriken& Prototype)
	: CSkill{ Prototype }
{
}

HRESULT CSkill_RasenShuriken::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSkill_RasenShuriken::Initialize(void* pArg)
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

	m_pRasenShuriken_Sphere = dynamic_cast<CRasenShuriken_Sphere*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_RasenShuriken_Sphere"), &Desc));
	if (nullptr == m_pRasenShuriken_Sphere)
		return E_FAIL;
	Safe_AddRef(m_pRasenShuriken_Sphere);
	m_pRasenShuriken_Sphere->Set_Owner(this);
	m_pGameInstance->Add_Object_ToLayer(m_iLevelIndex, TEXT("Layer_SkillEffect"), m_pRasenShuriken_Sphere);

	m_pRasenShuriken_Blade = dynamic_cast<CRasenShuriken_Blade*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_RasenShuriken_Blade"), &Desc));
	if (nullptr == m_pRasenShuriken_Blade)
		return E_FAIL;
	Safe_AddRef(m_pRasenShuriken_Blade);
	m_pRasenShuriken_Blade->Set_Owner(this);
	m_pGameInstance->Add_Object_ToLayer(m_iLevelIndex, TEXT("Layer_SkillEffect"), m_pRasenShuriken_Blade);

	m_pRasenShuriken_BladeFly = dynamic_cast<CRasenShuriken_BladeFly*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_RasenShuriken_BladeFly"), &Desc));
	if (nullptr == m_pRasenShuriken_BladeFly)
		return E_FAIL;
	Safe_AddRef(m_pRasenShuriken_BladeFly);
	m_pRasenShuriken_BladeFly->Set_Owner(this);
	m_pGameInstance->Add_Object_ToLayer(m_iLevelIndex, TEXT("Layer_SkillEffect"), m_pRasenShuriken_BladeFly);

	m_pRasenShuriken_BladeLine = dynamic_cast<CRasenShuriken_BladeLine*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_RasenShuriken_BladeLine"), &Desc));
	if (nullptr == m_pRasenShuriken_BladeLine)
		return E_FAIL;
	Safe_AddRef(m_pRasenShuriken_BladeLine);
	m_pRasenShuriken_BladeLine->Set_Owner(this);
	m_pGameInstance->Add_Object_ToLayer(m_iLevelIndex, TEXT("Layer_SkillEffect"), m_pRasenShuriken_BladeLine);

	m_pRasenShuriken_Circle = dynamic_cast<CRasenShuriken_Circle*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_RasenShuriken_Circle"), &Desc));
	if (nullptr == m_pRasenShuriken_Circle)
		return E_FAIL;
	Safe_AddRef(m_pRasenShuriken_Circle);
	m_pRasenShuriken_Circle->Set_Owner(this);
	m_pGameInstance->Add_Object_ToLayer(m_iLevelIndex, TEXT("Layer_SkillEffect"), m_pRasenShuriken_Circle);

	m_pExplosionSphere = dynamic_cast<CExplosionSphere*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_ExplosionSphere"), &Desc));
	if (nullptr == m_pExplosionSphere)
		return E_FAIL;
	Safe_AddRef(m_pExplosionSphere);
	m_pExplosionSphere->Set_Owner(this);
	m_pGameInstance->Add_Object_ToLayer(m_iLevelIndex, TEXT("Layer_SkillEffect"), m_pExplosionSphere);

	strcpy_s(m_szPositionBoneName, "RightHandMiddle1");
	m_pSocketMatrix = dynamic_cast<CPlayer*>(m_pOwner)->Get_Model()->Get_BoneCombindTransformationMatrix_Ptr(m_szPositionBoneName);

	m_isActive = false;
	m_pColliderCom->IsActive(false);

	m_pPlayer = dynamic_cast<CPlayer*>(m_pOwner);

	return S_OK;
}

void CSkill_RasenShuriken::Priority_Update(_float fTimeDelta)
{
}

void CSkill_RasenShuriken::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
	m_fExplosionRingTime += fTimeDelta;

	_matrix		WorldMatrix = XMMatrixIdentity();
	_matrix		PlayerWorldMatrix = m_pOwner->Get_Transform()->Get_WorldMatrix();

	if (!m_isShoot)
	{
		_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

		for (size_t i = 0; i < 3; i++)
		{
			SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
		}

		WorldMatrix = SocketMatrix * PlayerWorldMatrix;

		_vector vPosition = WorldMatrix.r[3] + XMVector3Normalize(WorldMatrix.r[1]) * 0.35f;
		WorldMatrix.r[3] = vPosition;

		m_pTransformCom->Set_WorldMatrix(WorldMatrix);

		if (m_pNearTarget)
		{
			_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_vector vTargetPos = m_pNearTarget->Get_Transform()->Get_State(CTransform::STATE_POSITION);
			vTargetPos = XMVectorSetY(vTargetPos, XMVectorGetY(vTargetPos) + 0.5f);

			m_pTransformCom->LookAt(vTargetPos);
		}
		else
		{
			_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			vPos += m_pOwner->Get_Transform()->Get_State(CTransform::STATE_LOOK);
			m_pTransformCom->LookAt(vPos);
		}

		return;
	}
	
	if (m_isExplosion)
	{
		if (m_pExplosionSphere->Get_CurrentTime() < 2.8f)
		{
			if (m_fSacle <= m_fExplosionTargetScale)
				m_fSacle += 13.f * fTimeDelta;
			else
				m_fSacle = m_fExplosionTargetScale;

			m_pTransformCom->Set_Scaled(m_fSacle, m_fSacle, m_fSacle);

			_float4 vColor = { 1.f,1.f,1.f,1.f };
			if (m_fExplosionRingTime >= 0.2f)
			{
				for (_uint i = 0; i < 3; ++i)
				{
					CObjectPool<CRasenganRing>::Get_GameObject()->Appear(this, 1.5f, 26.f, 25.f, 0.25f, vColor);
				}
				m_fExplosionRingTime = 0.f;
			}
		}
	}
	else
	{
		if (m_fCurrentTime >= m_fFlyTime)
			Explosion();

		m_pTransformCom->Go_Straight(13.0f, fTimeDelta);
	}

}

void CSkill_RasenShuriken::Late_Update(_float fTimeDelta)
{
	if (m_isExplosion)
	{
		if (m_pExplosionSphere->Get_CurrentTime() >= 2.9f)
		{
			m_pColliderCom->IsActive(false);
			m_isActive = false;
		}
	}

	m_pSoundCom->Update(fTimeDelta);
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());

	m_pGameInstance->Add_ColliderList(m_pColliderCom);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CSkill_RasenShuriken::Render()
{
#ifdef _DEBUG
	m_pColliderCom->Render();
#endif

	return S_OK;
}

void CSkill_RasenShuriken::OnCollisionEnter(CGameObject* pOther)
{
	if (pOther->Get_LayerTag() == TEXT("Layer_Monster"))
	{
		CMonster* pMonster = dynamic_cast<CMonster*>(pOther);
		if (nullptr == pMonster)
			return;

		pMonster->Set_AnimationSpeed(0.01f);

		pMonster->Damaged_To_Player(m_pPlayer->Get_Fsm()->Get_CurrentState(), 5.f, false);

		m_pPlayer->Add_HitEffect();

		Explosion();
	}
}

void CSkill_RasenShuriken::OnCollisionStay(CGameObject* pOther)
{
}

void CSkill_RasenShuriken::OnCollisionExit(CGameObject* pOther)
{
	if (pOther->Get_LayerTag() == TEXT("Layer_Monster"))
	{
		CMonster* pMonster = dynamic_cast<CMonster*>(pOther);
		if (nullptr == pMonster)
			return;

		pMonster->Damaged_To_Player(m_pPlayer->Get_Fsm()->Get_CurrentState(), 30.f, false);
		pMonster->Set_AnimationSpeed(1.f);

		m_pPlayer->Add_HitEffect();
	}
}

void CSkill_RasenShuriken::Appear()
{
	if (m_isActive)
		return;

	m_fCurrentTime = 0.f;
	m_fSacle = 1.f;
	m_fExplosionRingTime = 0.f;

	m_pTransformCom->Set_Scaled(m_fSacle, m_fSacle, m_fSacle);

	m_pRasenShuriken_Blade->Appear();
	m_pRasenShuriken_BladeFly->Appear();
	m_pRasenShuriken_BladeLine->Appear();
	m_pRasenShuriken_Circle->Appear();
	m_pRasenShuriken_Sphere->Appear();

	for (_uint i = 0; i < 5; ++i)
	{
		CObjectPool<CRasenganRing>::Get_GameObject()->Appear(this, 0.5f, 1.5f, 1.5f, 0.3f);
	}
	for (_uint i = 0; i < 5; ++i)
	{
		CObjectPool<CRasenganRing>::Get_GameObject()->Appear(this, 0.3f, 1.3f, 1.5f, 0.3f);
	}

	m_pColliderCom->IsActive(false);

	m_isShoot = false;
	m_isExplosion = false;
	m_isActive = true;

	Find_NearTarget();

	m_pSoundCom->Stop();
	m_pSoundCom->Play3D(TEXT("RaseShuriken_Create.wav"), g_fVolume - 0.1f);
}

void CSkill_RasenShuriken::DisAppear()
{
	m_isActive = false;
}

void CSkill_RasenShuriken::Shoot()
{
	if (m_isShoot)
		return;

	m_fCurrentTime = 0.f;

	XMStoreFloat3(&m_vMoveDir, XMVector3Normalize(m_pOwner->Get_Transform()->Get_State(CTransform::STATE_LOOK)));
	m_pColliderCom->IsActive(true);

	m_isShoot = true;

	m_pSoundCom->Stop();
	m_pSoundCom->Play3D(TEXT("RasenShuriken_Start.wav"), g_fVolume - 0.1f);
}

void CSkill_RasenShuriken::Explosion()
{
	if (m_isExplosion)
		return;

	m_fCurrentTime = 0.f;

	m_pRasenShuriken_Blade->DisAppear();
	m_pRasenShuriken_BladeFly->DisAppear();
	m_pRasenShuriken_BladeLine->DisAppear();
	m_pRasenShuriken_Circle->DisAppear();
	m_pRasenShuriken_Sphere->DisAppear();

	m_pExplosionSphere->Appear();

	m_isExplosion = true;

	m_pSoundCom->Play3D(TEXT("RasenShuriken_Explore.wav"), g_fVolume);
}

HRESULT CSkill_RasenShuriken::Ready_Components()
{
	__super::Ready_Components();

	_float3 vInitPos = {};
	XMStoreFloat3(&vInitPos, Get_Position());

	/* For.Com_Collider */
	CBounding_Sphere::BOUNDING_SPHERE_DESC			ColliderSphereDesc{};
	ColliderSphereDesc.fRadius = 0.7f;
	ColliderSphereDesc.vCenter = vInitPos;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderSphereDesc)))
		return E_FAIL;
	m_pColliderCom->Set_Owner(this);

	return S_OK;
}

CSkill_RasenShuriken* CSkill_RasenShuriken::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkill_RasenShuriken* pInstance = new CSkill_RasenShuriken(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSkill_RasenShuriken"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CSkill_RasenShuriken::Clone(void* pArg)
{
	CSkill_RasenShuriken* pInstance = new CSkill_RasenShuriken(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CSkill_RasenShuriken"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkill_RasenShuriken::Free()
{
	__super::Free();

	Safe_Release(m_pExplosionSphere);
	Safe_Release(m_pRasenShuriken_Circle);
	Safe_Release(m_pRasenShuriken_Blade);
	Safe_Release(m_pRasenShuriken_BladeFly);
	Safe_Release(m_pRasenShuriken_BladeLine);
	Safe_Release(m_pRasenShuriken_Sphere);
}
