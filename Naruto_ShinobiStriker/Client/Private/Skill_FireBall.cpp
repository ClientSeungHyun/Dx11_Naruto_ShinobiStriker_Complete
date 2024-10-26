#include "stdafx.h"
#include "Skill_FireBall.h"

#include "GameInstance.h"
#include "Model.h"
#include "Collider.h"
#include "ObjectPool.h"
#include "ExplosionCircle.h"
#include "Fire05.h"
#include "Smoke11.h"
#include "SmokeRing.h"
#include "Effect_Distortion.h"
#include "Particle_FireBall.h"

CSkill_FireBall::CSkill_FireBall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSkill{ pDevice, pContext }
{
}

CSkill_FireBall::CSkill_FireBall(const CSkill_FireBall& Prototype)
	: CSkill{ Prototype }
{
}

HRESULT CSkill_FireBall::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSkill_FireBall::Initialize(void* pArg)
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

	m_pFireBall = dynamic_cast<CFireBall*>(m_pGameInstance->Get_CloneObject_AddLayer(LEVEL_CHUNIN, TEXT("Layer_Skill_Effect"), TEXT("Prototype_GameObject_FireBall"), &Desc));
	if (nullptr == m_pFireBall)
		return E_FAIL;
	m_pFireBall->Set_Owner(this);

	m_pFireBallEffect = dynamic_cast<CFireBall_Effect*>(m_pGameInstance->Get_CloneObject_AddLayer(LEVEL_CHUNIN, TEXT("Layer_Skill_Effect"), TEXT("Prototype_GameObject_FireBallEffect"), &Desc));
	if (nullptr == m_pFireBallEffect)
		return E_FAIL;
	m_pFireBallEffect->Set_Owner(this);

	m_pFireBallFlySmoke = dynamic_cast<CFireBall_FlySmoke*>(m_pGameInstance->Get_CloneObject_AddLayer(LEVEL_CHUNIN, TEXT("Layer_Skill_Effect"), TEXT("Prototype_GameObject_FireBallFlySmoke"), &Desc));
	if (nullptr == m_pFireBallFlySmoke)
		return E_FAIL;
	m_pFireBallFlySmoke->Set_Owner(this);

	m_pParticle_FireBAll = dynamic_cast<CParticle_FireBall*>(m_pGameInstance->Get_CloneObject_AddLayer(LEVEL_CHUNIN, TEXT("Layer_Particle"), TEXT("Prototype_GameObject_Particle_FireBall"), &Desc));
	if (nullptr == m_pFireBallFlySmoke)
		return E_FAIL;

	strcpy_s(m_szPositionBoneName, "LeftHandMiddle1");
	m_pSocketMatrix = dynamic_cast<CPlayer*>(m_pOwner)->Get_Model()->Get_BoneCombindTransformationMatrix_Ptr(m_szPositionBoneName);

	m_isActive = false;
	m_pColliderCom->IsActive(false);

	return S_OK;
}

void CSkill_FireBall::Priority_Update(_float fTimeDelta)
{


}

void CSkill_FireBall::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	m_fTrailFireTime += fTimeDelta;

	if (m_fCurrentTime >= 1.3f)
		Explosion();

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vRight = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT));
	_vector vUp = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP));
	_vector vLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
	_float fRange = 1.f;
	if (m_fTrailFireTime > 0.2f && !m_isExplosion)
	{

		_vector vRangePos = vPos + vRight * m_pGameInstance->Get_Random(-fRange + 0.4f, fRange - 0.4f) + vUp * m_pGameInstance->Get_Random(-fRange, fRange) - vLook * 1.5f;
		CObjectPool<CFire05>::Get_GameObject()->Appear(vRangePos, 2.5f, CFire05::FIREBALL);

		vRangePos = vPos + vRight * m_pGameInstance->Get_Random(-fRange + 0.4f, fRange - 0.4f) + vUp * m_pGameInstance->Get_Random(-fRange, fRange) - vLook * 1.5f;
		CObjectPool<CFire05>::Get_GameObject()->Appear(vRangePos, 2.5f, CFire05::FIREBALL);
		
		m_fTrailFireTime = 0.f;
	}

	if (!m_isExplosion && m_fCurrentTime >= 0.1f)
	{
		m_pTransformCom->Go_Straight(13.0f, fTimeDelta);
	}
	else if(m_isExplosion)
	{
		if (m_fExplosionTime > 0.3f)
			m_isActive = false;

		if (m_fExplosionTime <= 0.15f)
		{
			m_pTransformCom->Set_Scaled(5.f, 5.f, 5.f);
			m_pColliderCom->IsActive(true);
		}
		else
		{
			m_pTransformCom->Set_Scaled(5.f, 5.f, 5.f);
			m_pColliderCom->IsActive(false);
		}
	}
}

void CSkill_FireBall::Late_Update(_float fTimeDelta)
{
	m_pSoundCom->Update(fTimeDelta);
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());

	m_pGameInstance->Add_ColliderList(m_pColliderCom);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CSkill_FireBall::Render()
{
#ifdef _DEBUG
	m_pColliderCom->Render();
#endif

	return S_OK;
}

void CSkill_FireBall::OnCollisionEnter(CGameObject* pOther)
{
	if (pOther->Get_LayerTag() == TEXT("Layer_Monster"))
	{
		CMonster* pMonster = dynamic_cast<CMonster*>(pOther);

		if (pMonster->Get_MonsterID() != KURAMA && pMonster->Get_MonsterID() != SUSANOO)
		{
			pMonster->Change_State_FlyAway();
			pMonster->Force_BackUp(15.f, 10.f);
			pMonster->Damaged(30.f);
		}
		else
		{
			pMonster->Damaged_To_Player(0, 30.f, true);

		}

		dynamic_cast<CPlayer*>(m_pOwner)->Add_HitEffect();

		Explosion();
	}
}

void CSkill_FireBall::OnCollisionStay(CGameObject* pOther)
{
}

void CSkill_FireBall::OnCollisionExit(CGameObject* pOther)
{
}

void CSkill_FireBall::Appear()
{
	if (m_isActive)
		return;

	_matrix PlayerMatirx = m_pOwner->Get_Transform()->Get_WorldMatrix();

	_vector vLook = XMVector3Normalize(PlayerMatirx.r[2]);
	_vector vUp = XMVector3Normalize(PlayerMatirx.r[1]);
	_vector vNewPos = PlayerMatirx.r[3] + vLook * 2.2f + vUp * 1.3f;
	PlayerMatirx.r[3] = vNewPos;

	m_pTransformCom->Set_WorldMatrix(PlayerMatirx);

	m_fCurrentTime = 0.f;

	m_pFireBall->Appear();
	m_pFireBallEffect->Appear();
	m_pFireBallFlySmoke->Appear();

	_float3 vPos;
	XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	CObjectPool<CExplosionCircle>::Get_GameObject()->Appear(vPos, _float4(1.f, 0.f, 0.f, 0.8f), 0.22f, 25.f, 1.f, 0.1f);
	CObjectPool<CExplosionCircle>::Get_GameObject()->Appear(vPos, _float4(1.f, 0.f, 0.f, 0.8f), 0.11f, 25.f, 1, 0.1f);

	m_pColliderCom->IsActive(true);
	m_isExplosion = false;
	m_isActive = true;

	m_pSoundCom->Stop();
}

void CSkill_FireBall::DisAppear()
{
	m_pFireBall->DisAppear();
	m_isActive = false;
}

void CSkill_FireBall::Explosion()
{
	if (m_isExplosion)
		return;

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vRight = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT));
	_vector vUp = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP));
	_vector vLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));

	m_pFireBall->DisAppear();
	m_pFireBallEffect->DisAppear();
	m_pFireBallFlySmoke->DisAppear();
	m_pParticle_FireBAll->Appear(vPos);

	_float3 vPosFloat;

	XMStoreFloat3(&vPosFloat, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	CObjectPool<CExplosionCircle>::Get_GameObject()->Appear(vPosFloat, _float4(1.f, 0.f, 0.f, 0.8f), 2.f, 25.f, 35.f, 0.3f);
	CObjectPool<CExplosionCircle>::Get_GameObject()->Appear(vPosFloat, _float4(1.f, 0.f, 0.f, 0.8f), 1.f, 25.f, 30.f, 0.3f);

	CObjectPool<CSmokeRing>::Get_GameObject()->Appear(vPos + vUp, 8.f, 1.f, 20.f);
	CObjectPool<CSmokeRing>::Get_GameObject()->Appear(vPos - vUp * 0.5f, 8.f, 1.f, 20.f);

	CObjectPool<CEffect_Distortion>::Get_GameObject()->Appear(vPos - vLook, 0.1f, 5.f, 15.f);

	for (_uint i = 0; i < 30; ++i)
	{
		
		_float fRange = 3.f;
		_vector vRangePos = vPos + vRight * m_pGameInstance->Get_Random(-fRange + 0.4f, fRange - 0.4f) + vUp * m_pGameInstance->Get_Random(-fRange, fRange) - vLook * m_pGameInstance->Get_Random(-fRange, fRange);
		CObjectPool<CFire05>::Get_GameObject()->Appear(vRangePos, 2.5f, CFire05::FIREBALL);
	}

	for (_uint i = 0; i < 30; ++i)
	{
		CObjectPool<CSmoke11>::Get_GameObject()->Appear(vPosFloat, _float3(2.f, 1.5f, 2.f), 0.5f, CSmoke11::SMOKE_BLACK);
	}

	m_fCurrentTime = 0.f;
	m_isExplosion = true;

	m_pSoundCom->Stop();
	m_pSoundCom->Play3D(TEXT("FireBall_Explosion.wav"), g_fVolume);
}

HRESULT CSkill_FireBall::Ready_Components()
{
	__super::Ready_Components();

	_float3 vInitPos = {};
	XMStoreFloat3(&vInitPos, Get_Position());

	/* For.Com_Collider */
	CBounding_Sphere::BOUNDING_SPHERE_DESC			ColliderSphereDesc{};
	ColliderSphereDesc.fRadius = 1.5f;
	ColliderSphereDesc.vCenter = vInitPos;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderSphereDesc)))
		return E_FAIL;
	m_pColliderCom->Set_Owner(this);

	return S_OK;
}

CSkill_FireBall* CSkill_FireBall::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkill_FireBall* pInstance = new CSkill_FireBall(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSkill_FireBall"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CSkill_FireBall::Clone(void* pArg)
{
	CSkill_FireBall* pInstance = new CSkill_FireBall(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CSkill_FireBall"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkill_FireBall::Free()
{
	__super::Free();
}
