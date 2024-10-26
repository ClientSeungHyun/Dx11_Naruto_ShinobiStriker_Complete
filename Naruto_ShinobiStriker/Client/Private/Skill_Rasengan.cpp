#include "stdafx.h"
#include "Skill_Rasengan.h"

#include "GameInstance.h"
#include "Model.h"
#include "Collider.h"

#include "ObjectPool.h"
#include "Particle_Hit.h"
#include "RasenganRing.h"
#include "PlayerCamera.h"

CSkill_Rasengan::CSkill_Rasengan(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSkill{ pDevice, pContext }
{
}

CSkill_Rasengan::CSkill_Rasengan(const CSkill_Rasengan& Prototype)
	: CSkill{ Prototype }
{
}

HRESULT CSkill_Rasengan::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSkill_Rasengan::Initialize(void* pArg)
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

	m_pRasengan_Center = dynamic_cast<CRasengan_Center*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Rasengan_Center"), &Desc));
	if (nullptr == m_pRasengan_Center)
		return E_FAIL;
	m_pRasengan_Center->Set_Owner(this);
	m_pGameInstance->Add_Object_ToLayer(m_iLevelIndex, TEXT("Layer_SkillEffect"), m_pRasengan_Center);

	m_pRasengan_CreateSmoke = dynamic_cast<CRasengan_CreateSmoke*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Rasengan_CreateSmoke"), &Desc));
	if (nullptr == m_pRasengan_CreateSmoke)
		return E_FAIL;
	m_pRasengan_CreateSmoke->Set_Owner(this);
	m_pGameInstance->Add_Object_ToLayer(m_iLevelIndex, TEXT("Layer_SkillEffect"), m_pRasengan_CreateSmoke);

	m_pRasengan_RunSmoke = dynamic_cast<CRasengan_RunSmoke*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Rasengan_RunSmoke"), &Desc));
	if (nullptr == m_pRasengan_RunSmoke)
		return E_FAIL;
	m_pRasengan_RunSmoke->Set_Owner(this);
	m_pGameInstance->Add_Object_ToLayer(m_iLevelIndex, TEXT("Layer_SkillEffect"), m_pRasengan_RunSmoke);

	strcpy_s(m_szPositionBoneName, "RightHandMiddle1");
	m_pSocketMatrix = dynamic_cast<CPlayer*>(m_pOwner)->Get_Model()->Get_BoneCombindTransformationMatrix_Ptr(m_szPositionBoneName);

	m_fExplosionTime = 1.5f;

	m_isActive = false;
	m_pColliderCom->IsActive(false);

	m_pPlayer = dynamic_cast<CPlayer*>(m_pOwner);

	return S_OK;
}

void CSkill_Rasengan::Priority_Update(_float fTimeDelta)
{
}

void CSkill_Rasengan::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
	m_fParticleTime += fTimeDelta;
	m_fRingTime += fTimeDelta;

	if (!m_isExplosion)
	{
		_matrix		WorldMatrix = XMMatrixIdentity();
		_matrix		PlayerWorldMatrix = m_pOwner->Get_Transform()->Get_WorldMatrix();

		_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);
		WorldMatrix = SocketMatrix * PlayerWorldMatrix;

		_vector vPosition = WorldMatrix.r[3] + XMVector3Normalize(WorldMatrix.r[1]) * 0.2f;
		WorldMatrix.r[3] = vPosition;

		m_pTransformCom->Set_WorldMatrix(WorldMatrix);
	}

	m_pSoundCom->Update(fTimeDelta);
}

void CSkill_Rasengan::Late_Update(_float fTimeDelta)
{
	if (m_isExplosion)
	{
		if (m_fCurrentTime >= m_fExplosionTime)
		{
			m_pSoundCom->Stop();
			m_pColliderCom->IsActive(false);
			m_isActive = false;
		}

		if (m_fRingTime > 0.1f)
		{
			if (!m_isRun)
			{
				_float4 vColor = { 0.f,0.2f,1.f,1.f };
				for (_uint i = 0; i < 3; ++i)
				{
					CObjectPool<CRasenganRing>::Get_GameObject()->Appear(this, 1.5f, 6.f, 10.f, 0.07f, vColor);
				}
			}

			m_fRingTime = 0.f;
		}
	}

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());

	m_pGameInstance->Add_ColliderList(m_pColliderCom);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CSkill_Rasengan::Render()
{
#ifdef _DEBUG
	m_pColliderCom->Render();
#endif

	return S_OK;
}

void CSkill_Rasengan::OnCollisionEnter(CGameObject* pOther)
{
	if (pOther->Get_LayerTag() == TEXT("Layer_Monster"))
	{
		if (!m_isLack)
		{
			m_pGameInstance->Start_Lack(TEXT("Timer_60"), 5);
			m_isLack = true;
		}

		CMonster* pMonster = dynamic_cast<CMonster*>(pOther);
		if (nullptr == pMonster || pMonster->Get_IsInvincible())
			return;

			pMonster->Set_AnimationSpeed(0.01f);

		m_pPlayer->Set_AnimationSpeed(0.01f);

		m_fParticleTime = 0.f;

		m_pGameInstance->Find_Camera(m_iLevelIndex)->Start_Shake(0.1f, 1.f);
	}
}

void CSkill_Rasengan::OnCollisionStay(CGameObject* pOther)
{
	if (pOther->Get_LayerTag() == TEXT("Layer_Monster"))
	{
		CMonster* pMonster = dynamic_cast<CMonster*>(pOther);
		if (nullptr == pMonster)
			return;

		if (m_fParticleTime >= 0.2f)
		{
			_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			CParticle_Hit* pParticleHit = CObjectPool<CParticle_Hit>::Get_GameObject();
			pParticleHit->Appear(vPos, XMVectorSet(0.05f, 0.75f, 0.9f, 1.f), 30, 2.f);

			if (pMonster->Get_MonsterID() != KURAMA && pMonster->Get_MonsterID() != SUSANOO)
				pMonster->Damaged(0.2f);
			else
				pMonster->Damaged_To_Player(m_pPlayer->Get_Fsm()->Get_CurrentState(), 0.2f, true);

			m_pPlayer->Add_HitEffect();

			m_fParticleTime = 0.f;
		}
	}
}

void CSkill_Rasengan::OnCollisionExit(CGameObject* pOther)
{
	if (pOther->Get_LayerTag() == TEXT("Layer_Monster"))
	{
		CMonster* pMonster = dynamic_cast<CMonster*>(pOther);
		if (nullptr == pMonster)
			return;

		pMonster->Damaged_To_Player(m_pPlayer->Get_Fsm()->Get_CurrentState(), 20.f, true);
		pMonster->Set_AnimationSpeed(1.f);
		m_pPlayer->Set_AnimationSpeed(1.f);
	}
}

void CSkill_Rasengan::Appear()
{
	m_fCurrentTime = 0.f;

	m_pRasengan_Center->Appear();
	m_pRasengan_CreateSmoke->Appear();

	m_pColliderCom->IsActive(false);

	for (_uint i = 0; i < 5; ++i)
	{
		CObjectPool<CRasenganRing>::Get_GameObject()->Appear(this, 0.1f, 0.7f, 1.5f, 0.5f);
	}
	for (_uint i = 0; i < 5; ++i)
	{
		CObjectPool<CRasenganRing>::Get_GameObject()->Appear(this, 0.1f, 0.5f, 1.5f, 0.5f);
	}

	m_isLack = false;
	m_isRun = false;
	m_isExplosion = false;
	m_isActive = true;

	m_pSoundCom->Play3D(TEXT("Rasengan_Create.wav"), g_fVolume - 0.1f);
}

void CSkill_Rasengan::DisAppear()
{
	m_pRasengan_Center->DisAppear();
	m_pRasengan_CreateSmoke->DisAppear();
	m_pSoundCom->Stop();
	m_isActive = false;
}

void CSkill_Rasengan::Run()
{
	m_pRasengan_CreateSmoke->DisAppear();
	m_pRasengan_RunSmoke->Appear();

	m_isRun = true;

	m_pSoundCom->Play3D(TEXT("Rasengan_Run.wav"), g_fVolume - 0.1f);
}

void CSkill_Rasengan::Explosion()
{
	if (m_isExplosion)
		return;

	m_fCurrentTime = 0.f;

	m_pColliderCom->IsActive(true);

	m_pRasengan_Center->DisAppear();
	m_pRasengan_RunSmoke->DisAppear();

	_vector vPos = Get_Position();
	for (_uint i = 0; i < 10; ++i)
	{
		CObjectPool<CRasenganRing>::Get_GameObject()->Appear(this, 0.5f, 2.f, 2.f, 1.f);
	}
	for (_uint i = 0; i < 10; ++i)
	{
		CObjectPool<CRasenganRing>::Get_GameObject()->Appear(this, 0.5f, 1.5f, 2.f, 1.f);
	}

	dynamic_cast<CPlayerCamera*>(m_pGameInstance->Find_Camera(m_iLevelIndex))->Start_Shake(0.2f, 0.8f);

	m_isExplosion = true;

	m_pSoundCom->Play3D(TEXT("Rasengan_Explore.wav"), g_fVolume - 0.1f);
}

HRESULT CSkill_Rasengan::Ready_Components()
{
	__super::Ready_Components();

	_float3 vInitPos = {};
	XMStoreFloat3(&vInitPos, Get_Position());

	/* For.Com_Collider */
	CBounding_Sphere::BOUNDING_SPHERE_DESC			ColliderSphereDesc{};
	ColliderSphereDesc.fRadius = 0.45f;
	ColliderSphereDesc.vCenter = vInitPos;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderSphereDesc)))
		return E_FAIL;
	m_pColliderCom->Set_Owner(this);

	return S_OK;
}

CSkill_Rasengan* CSkill_Rasengan::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkill_Rasengan* pInstance = new CSkill_Rasengan(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSkill_Rasengan"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CSkill_Rasengan::Clone(void* pArg)
{
	CSkill_Rasengan* pInstance = new CSkill_Rasengan(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CSkill_RasenShuriken"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkill_Rasengan::Free()
{
	__super::Free();
}
