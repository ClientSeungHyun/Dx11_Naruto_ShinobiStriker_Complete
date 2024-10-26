#include "stdafx.h"
#include "Skill_MassiveRasengan.h"

#include "GameInstance.h"
#include "Model.h"
#include "Collider.h"

#include "ObjectPool.h"
#include "RasenganRing.h"
#include "PlayerCamera.h"
#include "Effect_Distortion.h"

CSkill_MassiveRasengan::CSkill_MassiveRasengan(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSkill{ pDevice, pContext }
{
}

CSkill_MassiveRasengan::CSkill_MassiveRasengan(const CSkill_MassiveRasengan& Prototype)
	: CSkill{ Prototype }
{
}

HRESULT CSkill_MassiveRasengan::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSkill_MassiveRasengan::Initialize(void* pArg)
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

	m_pMassiveRasengan_Circle = dynamic_cast<CMassiveRasengan_Circle*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_MassiveRasenganCircle"), &Desc));
	if (nullptr == m_pMassiveRasengan_Circle)
		return E_FAIL;
	m_pMassiveRasengan_Circle->Set_Owner(this);
	m_pGameInstance->Add_Object_ToLayer(m_iLevelIndex, TEXT("Layer_SkillEffect"), m_pMassiveRasengan_Circle);

	m_pMassiveRasengan_Spher = dynamic_cast<CMassiveRasengan_Sphere*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_MassiveRasengan_Sphere"), &Desc));
	if (nullptr == m_pMassiveRasengan_Spher)
		return E_FAIL;
	m_pMassiveRasengan_Spher->Set_Owner(this);
	m_pGameInstance->Add_Object_ToLayer(m_iLevelIndex, TEXT("Layer_SkillEffect"), m_pMassiveRasengan_Spher);

	strcpy_s(m_szPositionBoneName, "RightHandMiddle1");
	m_pSocketMatrix = dynamic_cast<CPlayer*>(m_pOwner)->Get_Model()->Get_BoneCombindTransformationMatrix_Ptr(m_szPositionBoneName);

	m_isActive = false;
	m_pColliderCom->IsActive(false);

	m_pPlayer = dynamic_cast<CPlayer*>(m_pOwner);

	return S_OK;
}

void CSkill_MassiveRasengan::Priority_Update(_float fTimeDelta)
{
}

void CSkill_MassiveRasengan::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
	m_fRingTime += fTimeDelta;

	if (!m_isShoot)
	{
		_matrix		WorldMatrix = XMMatrixIdentity();
		_matrix		PlayerWorldMatrix = m_pOwner->Get_Transform()->Get_WorldMatrix();

		_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

		for (size_t i = 0; i < 3; i++)
		{
			SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
		}

		WorldMatrix = SocketMatrix * PlayerWorldMatrix;

		_vector vPosition = WorldMatrix.r[3] + XMVector3Normalize(WorldMatrix.r[1]) * 3.1f;
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
			vPos += m_pOwner->Get_Transform()->Get_State(CTransform::STATE_LOOK) - m_pOwner->Get_Transform()->Get_State(CTransform::STATE_UP);
			m_pTransformCom->LookAt(vPos);
		}
	}

	if (m_fRingTime >= 0.2f)
	{
		_float4 vColor = { 1.f,1.f,1.f,1.f };
		for (_uint i = 0; i < 3; ++i)
		{
			CObjectPool<CRasenganRing>::Get_GameObject()->Appear(this, 1.5f, 26.f, 15.f, 0.25f, vColor);
		}
		for (_uint i = 0; i < 3; ++i)
		{
			CObjectPool<CRasenganRing>::Get_GameObject()->Appear(this, 0.5f, 24.f, 13.f, 0.25f, vColor);
		}
		m_fRingTime = 0.f;
	}

}

void CSkill_MassiveRasengan::Late_Update(_float fTimeDelta)
{
	if (m_isShoot)
	{
		if (m_pNearTarget)
		{
			m_pTransformCom->Go_Straight(13.0f, fTimeDelta);
		}

		if (m_fCurrentTime >= 2.f)
			DisAppear();
	}

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());

	m_pGameInstance->Add_ColliderList(m_pColliderCom);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CSkill_MassiveRasengan::Render()
{
#ifdef _DEBUG
	m_pColliderCom->Render();
#endif

	return S_OK;
}

void CSkill_MassiveRasengan::OnCollisionEnter(CGameObject* pOther)
{
	if (pOther->Get_LayerTag() == TEXT("Layer_Monster"))
	{
		CMonster* pMonster = dynamic_cast<CMonster*>(pOther);
		if (nullptr == pMonster)
			return;


		pMonster->Set_AnimationSpeed(0.01f);

		m_pPlayer->Set_AnimationSpeed(0.01f);

		m_pGameInstance->Find_Camera(m_iLevelIndex)->Start_Shake(0.1f, 1.f);
	}
}

void CSkill_MassiveRasengan::OnCollisionStay(CGameObject* pOther)
{
	if (pOther->Get_LayerTag() == TEXT("Layer_Monster"))
	{
		Explosion();
	}
}

void CSkill_MassiveRasengan::OnCollisionExit(CGameObject* pOther)
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

void CSkill_MassiveRasengan::Appear()
{
	if (m_isActive)
		return;

	m_fCurrentTime = 0.f;

	m_pMassiveRasengan_Circle->Appear();
	m_pMassiveRasengan_Spher->Appear();

	m_pColliderCom->IsActive(false);

	m_isShoot = false;
	m_isExplosion = false;
	m_isActive = true;
}

void CSkill_MassiveRasengan::DisAppear()
{
	m_pMassiveRasengan_Circle->DisAppear();
	m_pMassiveRasengan_Spher->DisAppear();
	m_pColliderCom->IsActive(false);

	CObjectPool<CEffect_Distortion>::Get_GameObject()->Appear(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 15.f, 0.1f, 15.f, 1.5f, CEffect_Distortion::DIST_RASEN);

	m_isActive = false;
}

void CSkill_MassiveRasengan::Shoot()
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

void CSkill_MassiveRasengan::Explosion()
{
	if (m_isExplosion)
		return;

	m_fCurrentTime = 0.f;

	//m_pRasengan_Center->DisAppear();
	//m_pRasengan_RunSmoke->DisAppear();

	m_isExplosion = true;
}

HRESULT CSkill_MassiveRasengan::Ready_Components()
{
	__super::Ready_Components();

	_float3 vInitPos = {};
	XMStoreFloat3(&vInitPos, Get_Position());

	/* For.Com_Collider */
	CBounding_Sphere::BOUNDING_SPHERE_DESC			ColliderSphereDesc{};
	ColliderSphereDesc.fRadius = 3.5f;
	ColliderSphereDesc.vCenter = vInitPos;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderSphereDesc)))
		return E_FAIL;
	m_pColliderCom->Set_Owner(this);

	return S_OK;
}

CSkill_MassiveRasengan* CSkill_MassiveRasengan::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkill_MassiveRasengan* pInstance = new CSkill_MassiveRasengan(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSkill_KuramaRasengan"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CSkill_MassiveRasengan::Clone(void* pArg)
{
	CSkill_MassiveRasengan* pInstance = new CSkill_MassiveRasengan(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CSkill_KuramaRasengan"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkill_MassiveRasengan::Free()
{
	__super::Free();
}
