#include "stdafx.h"
#include "Skill_Chidori.h"
#include "Player_Custom.h"
#include "Camera.h"

#include "GameInstance.h"
#include "Model.h"
#include "Collider.h"
#include "Lightning.h"
#include "Particle_Hit.h"
#include "ObjectPool.h"

CSkill_Chidori::CSkill_Chidori(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSkill{ pDevice, pContext }
{
}

CSkill_Chidori::CSkill_Chidori(const CSkill_Chidori& Prototype)
	: CSkill{ Prototype }
{
}

HRESULT CSkill_Chidori::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSkill_Chidori::Initialize(void* pArg)
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

	m_pChidoriCenter = dynamic_cast<CChidori_Center*>(m_pGameInstance->Get_CloneObject_AddLayer(LEVEL_CHUNIN, TEXT("Layer_Skill_Effect"), TEXT("Prototype_GameObject_ChidoriCenter"), &Desc));
	if (nullptr == m_pChidoriCenter)
		return E_FAIL;
	m_pChidoriCenter->Set_Owner(this);


	for (_uint i = 0; i < 3; ++i)
	{
		m_pChidoriTrail[i] = dynamic_cast<CChidoriTrail*>(m_pGameInstance->Get_CloneObject_AddLayer(LEVEL_CHUNIN, TEXT("Layer_Skill_Effect"), TEXT("Prototype_GameObject_ChidoriTrail"), &Desc));
		if (nullptr == m_pChidoriTrail[i])
			return E_FAIL;
		m_pChidoriTrail[i]->Set_TrailSize(0.04f);
	}

	strcpy_s(m_szPositionBoneName, "LeftHandMiddle1");
	m_pSocketMatrix = dynamic_cast<CPlayer*>(m_pOwner)->Get_Model()->Get_BoneCombindTransformationMatrix_Ptr(m_szPositionBoneName);

	m_isActive = false;
	m_pColliderCom->IsActive(false);

	m_fExplosionTime = 0.7f;

	m_iNumEffect = 80;
	m_fEffectTime = 1.5f;

	m_pPlayer = dynamic_cast<CPlayer*>(m_pOwner);

	return S_OK;
}

void CSkill_Chidori::Priority_Update(_float fTimeDelta)
{


}

void CSkill_Chidori::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
	m_fEffectTime += fTimeDelta;

	_matrix		WorldMatrix = XMMatrixIdentity();
	_matrix		PlayerWorldMatrix = m_pOwner->Get_Transform()->Get_WorldMatrix();

	_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (size_t i = 0; i < 3; i++)
	{
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
	}

	WorldMatrix = SocketMatrix * PlayerWorldMatrix;

	_vector vPosition = WorldMatrix.r[3];
	vPosition = XMVectorSetY(vPosition, XMVectorGetY(vPosition));
	WorldMatrix.r[3] = vPosition;

	m_pTransformCom->Set_WorldMatrix(WorldMatrix);
	
	if (m_fEffectTime >= 0.5f && !m_isExplosion && !m_isRun)
	{
		for (_uint i = 0; i <= m_iNumEffect; ++i)
		{
			CObjectPool<CLightning>::Get_GameObject()->Appear(Get_Position());
		}
		m_fEffectTime = 0.f;
	}

	if (!m_isPlayLoopSound && !m_isExplosion && 
		!m_pSoundCom->Get_IsPlaying())
	{
		m_pSoundCom->Play3D(TEXT("Chidori_Loop.wav"), g_fVolume - 0.1f);
		m_isPlayLoopSound = true;
	}
}

void CSkill_Chidori::Late_Update(_float fTimeDelta)
{
	_matrix		WorldMatrix = m_pTransformCom->Get_WorldMatrix();

	if (m_isExplosion)
	{
		if (m_fCurrentTime >= m_fExplosionTime)
		{
			for (_uint i = 0; i < 3; ++i)
			{
				m_pChidoriTrail[i]->DisAppear();
			}

			m_pColliderCom->IsActive(false);
			m_isActive = false;
		}
	}
	else if (m_isRun)
	{
		for (_uint i = 0; i < 3; ++i)
		{
			m_pChidoriTrail[i]->Appear();
			_vector vTrailPos = WorldMatrix.r[3] + XMVector3Normalize(WorldMatrix.r[0]) * m_pGameInstance->Get_Random(-0.7f, 0.7f) + XMVector3Normalize(WorldMatrix.r[1]) * m_pGameInstance->Get_Random(-0.7f, 0.7f);
			m_pChidoriTrail[i]->Set_TrailPos(vTrailPos);
		}
	}

	m_pSoundCom->Update(fTimeDelta);
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());

	m_pGameInstance->Add_ColliderList(m_pColliderCom);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CSkill_Chidori::Render()
{
#ifdef _DEBUG
	m_pColliderCom->Render();
#endif

	return S_OK;
}

void CSkill_Chidori::OnCollisionEnter(CGameObject* pOther)
{
	if (pOther->Get_LayerTag() == TEXT("Layer_Monster"))
	{
		CMonster* pMonster = dynamic_cast<CMonster*>(pOther);
		if (pMonster->Get_IsInvincible() || nullptr == pMonster)
			return;

		if (!m_isLack)
		{
			m_pGameInstance->Start_Lack(TEXT("Timer_60"), 5);
			m_isLack = true;
		}

		pMonster->Set_AnimationSpeed(0.01f);
		m_pPlayer->Set_AnimationSpeed(0.01f);

		m_fParticleTime = 0.f;

		m_pGameInstance->Find_Camera(m_iLevelIndex)->Start_Shake(0.1f, 1.f);
	}
}

void CSkill_Chidori::OnCollisionStay(CGameObject* pOther)
{
	if (pOther->Get_LayerTag() == TEXT("Layer_Monster"))
	{
		if (m_fParticleTime >= 0.1f)
		{
			_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			CParticle_Hit* pParticleHit = CObjectPool<CParticle_Hit>::Get_GameObject();
			pParticleHit->Appear(vPos, XMVectorSet(0.05f, 0.75f, 0.9f, 1.f), 10, 2.f);

			m_fParticleTime = 0.f;
		}
	}
}

void CSkill_Chidori::OnCollisionExit(CGameObject* pOther)
{
	if (pOther->Get_LayerTag() == TEXT("Layer_Monster"))
	{
		CMonster* pMonster = dynamic_cast<CMonster*>(pOther);
		if (nullptr == pMonster)
			return;

		pMonster->Damaged_To_Player(m_pPlayer->Get_Fsm()->Get_CurrentState(), 30.f, true);
		pMonster->Set_AnimationSpeed(1.f);
		m_pPlayer->Set_AnimationSpeed(1.f);
	}
}

void CSkill_Chidori::Appear()
{
	if (m_isActive)
		return;

	m_fEffectTime = 1.f;
	m_iNumEffect = 80;
	m_fCurrentTime = 0.f;

	m_pColliderCom->IsActive(false);
	m_pChidoriCenter->Appear();

	m_isLack = false;
	m_isRun = false;
	m_isExplosion = false;
	m_isActive = true;
	m_isPlayLoopSound = false;

	m_pSoundCom->Play3D(TEXT("Chidori_Create.wav"), g_fVolume - 0.1f);
}

void CSkill_Chidori::DisAppear()
{
	m_pChidoriCenter->DisAppear();
	m_pColliderCom->IsActive(false);
	m_isActive = false;
}

void CSkill_Chidori::Run()
{
	m_isRun = true;
}

void CSkill_Chidori::Explosion()
{
	if (m_isExplosion)
		return;

	m_iNumEffect = 250;
	m_fCurrentTime = 0.f;

	m_isExplosion = true;

	for (_uint i = 0; i <= m_iNumEffect; ++i)
	{
		CObjectPool<CLightning>::Get_GameObject()->Appear(Get_Position());
	}

	m_pColliderCom->IsActive(true);

	m_pChidoriCenter->DisAppear();

	m_pSoundCom->Play3D(TEXT("Chiori_Hit.wav"), g_fVolume);
}

HRESULT CSkill_Chidori::Ready_Components()
{
	__super::Ready_Components();

	_float3 vInitPos = {};
	XMStoreFloat3(&vInitPos, Get_Position());

	/* For.Com_Collider */
	CBounding_Sphere::BOUNDING_SPHERE_DESC			ColliderSphereDesc{};
	ColliderSphereDesc.fRadius = 0.5f;
	ColliderSphereDesc.vCenter = vInitPos;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderSphereDesc)))
		return E_FAIL;
	m_pColliderCom->Set_Owner(this);

	return S_OK;
}

CSkill_Chidori* CSkill_Chidori::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkill_Chidori* pInstance = new CSkill_Chidori(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSkill_Chidori"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CSkill_Chidori::Clone(void* pArg)
{
	CSkill_Chidori* pInstance = new CSkill_Chidori(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CSkill_Chidori"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkill_Chidori::Free()
{
	__super::Free();

	//Safe_Release(m_pChidoriCenter);
}
