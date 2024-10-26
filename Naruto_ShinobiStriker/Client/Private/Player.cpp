#include "stdafx.h"
#include "Player.h"

#include "ObjectPool.h"
#include "GameInstance.h"
#include "PlayerCamera.h"
#include "Skill_ChangeWood.h"
#include "Particle_Hit.h"

#include "Collider_PlayerAttack.h"
#include "FootTrail.h"
#include "WireTrail.h"
#include "JumpGuide.h"
#include "Kunai_Minato.h"

#include "ObjectPool.h"
#include "SmokeRing.h"
#include "Smoke11.h"

#include "UI_PlayerHP.h"
#include "UI_PlayerSP.h"
#include "UI_PlayerSkill.h"
#include "UI_HitNum.h"
#include "UI_Target.h"

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CContainerObject{ pDevice, pContext }
{
}

CPlayer::CPlayer(const CPlayer& Prototype)
	: CContainerObject{ Prototype }
{
}

void CPlayer::Find_NearTarget()
{
	m_pNearTarget = m_pGameInstance->Find_NearObject(this, m_pGameInstance->Get_CurLevelIndex(), TEXT("Layer_Monster"));
	m_pUI_Target->Set_Target(dynamic_cast<CMonster*>(m_pNearTarget));
}

_uint CPlayer::Get_CurrentAnimationIndex()
{
	return m_pModelCom->Get_CurrentAnimationIndex();
}

_char* CPlayer::Get_CurrentAnimationName()
{
	return m_pModelCom->Get_CurrentAnimationName();
}

_bool CPlayer::Get_IsEndAnimation(_uint iAnimationIndex)
{
	return m_pModelCom->Get_IsEnd_Animation(iAnimationIndex);
}

_uint CPlayer::Get_Frame(_uint iAnimIndex)
{
	return m_pModelCom->Get_Frame(iAnimIndex);
}

void CPlayer::Change_SkillType(SKILL_TYPE eType)
{
	if (m_eSkillType == eType || eType >= SKILL_END)
		return;

	m_eSkillType = eType;
	switch (m_eSkillType)
	{
	case Client::CPlayer::SKILL_SASUKE:
		m_pUI_SkillSlot[SLOT_SP]->Change_Skill(CUI_PlayerSkill::SP_KIRIN, &m_fCollTime_SkillSP);
		m_pUI_SkillSlot[SLOT_BASE1]->Change_Skill(CUI_PlayerSkill::SKILL_ICON::SKILL_CHIDORI, &m_fCollTime_Skill1);
		m_pUI_SkillSlot[SLOT_BASE2]->Change_Skill(CUI_PlayerSkill::SKILL_ICON::SKILL_FIREBALL, &m_fCollTime_Skill2);
		break;
	case Client::CPlayer::SKILL_NARUTO:
		m_pUI_SkillSlot[SLOT_SP]->Change_Skill(CUI_PlayerSkill::SP_MASSIVE, &m_fCollTime_SkillSP);
		m_pUI_SkillSlot[SLOT_BASE1]->Change_Skill(CUI_PlayerSkill::SKILL_ICON::SKILL_RASENRAN, &m_fCollTime_Skill1);
		m_pUI_SkillSlot[SLOT_BASE2]->Change_Skill(CUI_PlayerSkill::SKILL_ICON::SKILL_RASENSHURIKEN, &m_fCollTime_Skill2);
		break;
	case Client::CPlayer::SKILL_SARIN:
		m_pUI_SkillSlot[SLOT_SP]->Change_Skill(CUI_PlayerSkill::SP_SUSANOO, &m_fCollTime_SkillSP);
		m_pUI_SkillSlot[SLOT_BASE1]->Change_Skill(CUI_PlayerSkill::SKILL_ICON::SKILL_AMATERAS, &m_fCollTime_Skill1);
		m_pUI_SkillSlot[SLOT_BASE2]->Change_Skill(CUI_PlayerSkill::SKILL_ICON::SKILL_TSUKUYOMI, &m_fCollTime_Skill2);
		break;
	case Client::CPlayer::SKILL_MINATO:
		m_pUI_SkillSlot[SLOT_SP]->Change_Skill(CUI_PlayerSkill::SP_RASENKO, &m_fCollTime_SkillSP);
		m_pUI_SkillSlot[SLOT_BASE1]->Change_Skill(CUI_PlayerSkill::SKILL_ICON::SKILL_TYPEZERO, &m_fCollTime_Skill1);
		m_pUI_SkillSlot[SLOT_BASE2]->Change_Skill(CUI_PlayerSkill::SKILL_ICON::SKILL_RAJIN2, &m_fCollTime_Skill2);
		break;
	}

	m_fCollTime_Skill1 = 0.f;
	m_fCollTime_Skill2 = 0.f;
}

HRESULT CPlayer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
	PLAYER_DESC* pDesc = static_cast<PLAYER_DESC*>(pArg);

	/* 직교투영을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	//if (FAILED(Ready_Components()))
	//	return E_FAIL;

	m_fCurrentHP = 150.f;
	if (!m_isShadow && m_iLevelIndex != LEVEL_CUSTOM)
	{
		if (FAILED(Ready_UI()))
			return E_FAIL;
	}

	if (nullptr != pDesc)
	{
		m_pTransformCom->Set_Scaled(pDesc->vScale.x, pDesc->vScale.y, pDesc->vScale.z);
		m_pTransformCom->Rotation(pDesc->vRotation.x, pDesc->vRotation.y, pDesc->vRotation.z);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&pDesc->vPosition));
	}

	m_fAnimationSpeed = 1.f;

	return S_OK;
}

void CPlayer::Priority_Update(_float fTimeDelta)
{
	m_fCoolTime_ChangeWood -= fTimeDelta;
	if (m_fCoolTime_ChangeWood <= 0.f)
		m_fCoolTime_ChangeWood = 0.f;

	m_fCollTime_Skill1 -= fTimeDelta;
	m_fCollTime_Skill2 -= fTimeDelta;
	m_fCollTime_SkillSP -= fTimeDelta;

	if (m_fCollTime_Skill1 <= 0.f)
		m_fCollTime_Skill1 = 0.f;
	if (m_fCollTime_Skill2 <= 0.f)
		m_fCollTime_Skill2 = 0.f;
	if (m_fCollTime_SkillSP <= 0.f)
		m_fCollTime_SkillSP = 0.f;

	m_pCollider_Attack->Priority_Update(fTimeDelta);
}

void CPlayer::Update(_float fTimeDelta)
{
	if (m_iLevelIndex != LEVEL_CUSTOM)
	{
		m_pFsmCom->Update(fTimeDelta);
		m_pSoundCom->Update(fTimeDelta);
		m_pEffectSoundCom->Update(fTimeDelta);
		m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
		m_pCollider_Attack->Update(fTimeDelta);
	}
}

void CPlayer::Late_Update(_float fTimeDelta)
{
	/* 직교투영을 위한 월드행렬까지 셋팅하게 된다. */
	__super::Late_Update(fTimeDelta);

	_vector vHitDir;
	_float fHitLength;

	m_pGameInstance->Compute_Collision(&vHitDir, &fHitLength);

	if (m_iLevelIndex != LEVEL_CUSTOM)
	{
		Control_Gravity();
		m_pRigidBodyCom->Update(fTimeDelta);
	}

	if(m_isCanAttachWall)
		XMStoreFloat3(&m_vNormal, vHitDir);

	if (m_isWall)
	{
		_vector vHitPos, vNormal;
		_float fHitDistance;

		if (m_pGameInstance->RayCast_PlayerDown(&vHitPos, &vNormal, &fHitDistance))
		{
			_vector vPlayerPos = Get_Position();
			_vector vPlayerUp = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP));

			if (2.5f >= fHitDistance)
			{
				XMStoreFloat3(&m_vWallPos, vHitPos);
				XMStoreFloat3(&m_vNormal, vNormal);

				vPlayerPos = XMVectorLerp(vPlayerPos, XMLoadFloat3(&m_vWallPos), 8.0f * fTimeDelta);
				vPlayerUp = XMVectorLerp(vPlayerUp, XMLoadFloat3(&m_vNormal), 15.0f * fTimeDelta);
			}

			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPlayerPos);
			m_pTransformCom->Set_NewUp(vPlayerUp);

			_float fAngle = XMConvertToDegrees(acosf(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_vNormal), XMVectorSet(0.f, 1.f, 0.f, 0.f)))));
			if (fAngle > 150.f)
			{
				Change_State(4);
			}
		}
	}
	else if(!m_isWall && !m_isAttachWall && !m_isSkill)
	{
		_vector vPlayerUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
		m_pTransformCom->Set_NewUp(vPlayerUp);
	}

	m_pCollider_Attack->Late_Update(fTimeDelta);

	m_pGameInstance->Add_ColliderList(m_pColliderCom);

	if(m_isRender)
		m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CPlayer::Render()
{
#ifdef _DEBUG
	if (m_iLevelIndex != LEVEL_CUSTOM)
	{
		m_pColliderCom->Render();
		m_pCollider_Attack->Render();
	}
#endif

	return S_OK;
}

void CPlayer::Move_Dir(_fvector vDir, _float fSpeed, _float fTimeDelta, class CNavigation* pNavigation)
{
	m_pTransformCom->Turn_Lerp(vDir, 10.0f, fTimeDelta);
	m_pTransformCom->Go_Straight(fSpeed, fTimeDelta, pNavigation);
}

void CPlayer::Change_State(const _uint iState, void* pArg)
{
	if(m_isControl)
		m_pFsmCom->Change_State(iState, pArg);
}

void CPlayer::SetUp_NextAnimation(_uint iNextAnimationIndex, _bool isLoop, _float fChangeDuration, _uint iStartFrame)
{
	m_pModelCom->SetUp_NextAnimation(iNextAnimationIndex, isLoop, fChangeDuration, iStartFrame);
}

void CPlayer::Chase_Target(_float fTimeDelta, _float fMoveSpeed)
{
	if (nullptr == m_pNearTarget)
		return;

	_vector vPos = Get_Position();
	_vector vTargetPos = m_pNearTarget->Get_Position();

	// 상하에 관여받지 않는 dir을 구해야함
	_vector vDir = vTargetPos - vPos;

	_vector vUp = m_pTransformCom->Get_State(CTransform::STATE::STATE_UP);
	vUp = XMVector3Normalize(vUp);

	// Up 벡터 방향으로 얼마만큼 가는지 계산함
	_float fLength = -1.f;
	XMStoreFloat(&fLength, XMVector3Dot(vDir, vUp));

	// 그 길이만큼 up 벡터와 곱해서 뺌
	vTargetPos = vTargetPos - (vUp * fLength);
	vDir = vTargetPos - vPos;

	// 이동하는 양을 구함
	_vector vMove = vDir;
	vMove = XMVector3Normalize(vMove);;
	vMove *= fMoveSpeed * fTimeDelta;

	_float fDistance, fMoveAmount;

	// dir은 거리 move는 실제 움직이는 양
	XMStoreFloat(&fDistance, XMVector3Length(vDir));
	XMStoreFloat(&fMoveAmount, XMVector3Length(vMove));

	m_pTransformCom->Turn_Lerp(vDir, 10.0f, fTimeDelta);

	// 거리거 더 멀면 이동
	if (fDistance > 1.2f)
	{
		if (fMoveAmount > fDistance)
			vMove = vMove * (fDistance / fMoveAmount) * 0.5f; // 비율로 줄이기

		if(XMVectorGetX(XMVector3Length(vMove)) > 0.11f)
			vPos += vMove;
	}

	m_pTransformCom->Set_State(CTransform::STATE::STATE_POSITION, vPos);
}

void CPlayer::Go_Straight(_float fTimeDelta, _float fSpeed)
{
	m_pTransformCom->Go_Straight(fTimeDelta, fSpeed);
}

_vector CPlayer::Calculate_Direction_Straight()
{
	_vector vCameraLook = m_pPlayerCamera->Get_Transform()->Get_State(CTransform::STATE::STATE_LOOK);
	_vector vUp = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE::STATE_UP));
	_vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vCameraLook));

	_vector vDir = XMVector3Normalize(XMVector3Cross(vRight, vUp));

	return vDir;
}

_vector CPlayer::Calculate_Direction_Right()
{
	_vector vDir = m_pPlayerCamera->Get_Transform()->Get_State(CTransform::STATE::STATE_RIGHT);
	vDir = XMVector3Normalize(vDir);
	
	return vDir;
}

void CPlayer::Damaged(_float fDamage)
{
	m_fCurrentHP -= fDamage;
}

void CPlayer::KnockBack(_float fBackPower, _float fUpPower)
{
	_vector vBack = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * -1.f;
	_vector vUp = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP));

	m_pRigidBodyCom->Clear_All();
	m_pRigidBodyCom->Add_Force(CRigidBody::FORCE, vUp * fUpPower);
	m_pRigidBodyCom->Add_Force(CRigidBody::FORCE, vBack * fBackPower);

}

void CPlayer::Appear_FootTrail()
{
	for (_uint i = 0; i < 2; ++i)
	{
		if (m_pFootTrail[i] != nullptr)
		{
			m_pFootTrail[i]->Appear();
		}
	}

}

void CPlayer::Disappear_FootTrail()
{
	for (_uint i = 0; i < 2; ++i)
	{
		if (m_pFootTrail[i] != nullptr)
		{
			m_pFootTrail[i]->DisAppear();
		}
	}
}

void CPlayer::Appear_HitEffect(_fvector vColor, _uint iNumParticle)
{
	_vector vUp = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP));
	_vector vLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPos += vUp * 0.5f;
	vPos += vLook;

	CParticle_Hit* pParticleHit = CObjectPool<CParticle_Hit>::Get_GameObject();
	pParticleHit->Appear(vPos, vColor, iNumParticle, 1.f);
}

void CPlayer::Add_HitEffect()
{
	if (nullptr == m_pUI_Hit)
		return;

	m_pUI_Hit->Add_HitCount();
	m_pUI_Hit->Start_Appear();
}

void CPlayer::Appear_JumpGuide()
{
	m_pJumpGuide->Appear();
}

void CPlayer::Disappear_JumpGuide()
{
	m_pJumpGuide->Disappear();
}

void CPlayer::Start_AppearUI()
{
	m_pUI_HP->Start_Appear();
	m_pUI_SP->Start_Appear();
	for (_uint i = 0; i < SLOT_END; ++i)
	{
		m_pUI_SkillSlot[i]->Start_Appear();
	}
}

void CPlayer::Start_DisappearUI()
{
	m_pUI_HP->Start_Disappear();
	m_pUI_SP->Start_Disappear();
	for (_uint i = 0; i < SLOT_END; ++i)
	{
		m_pUI_SkillSlot[i]->Start_Disappear();
	}
}

void CPlayer::Scaled_AttackCollider(_float fX, _float fY, _float fZ)
{
	m_pCollider_Attack->Scaled(fX, fY, fZ);
}

void CPlayer::Offset_AttackCollider(_float fX, _float fY, _float fZ)
{
	m_pCollider_Attack->Offset(fX, fY, fZ);
}

void CPlayer::Appear_MinatoKunai()
{
	for (_uint i = 0; i < 6; ++i)
	{
		if (nullptr == m_pMinatoKunai[i])
			continue;

		m_pMinatoKunai[i]->Appear();
	}
}

void CPlayer::Shoot_MinatoKunai()
{
	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();

	_vector vRight = XMVector3Normalize(WorldMatrix.r[0]);
	_vector vUp = XMVector3Normalize(WorldMatrix.r[1]);
	_vector vLook = XMVector3Normalize(WorldMatrix.r[2]);
	_vector vPos = WorldMatrix.r[3];

	_vector vKunaiPos = vPos + vRight * 1.8f + vUp * 3.f;	//상좌
	m_pMinatoKunai[0]->Shoot(vKunaiPos, 5.f, true); 
	m_pMinatoKunai[0]->Set_HitEffectDuration(1.f);

	vKunaiPos = vPos - vRight * 0.2f + vUp * 2.3f;//상중
	m_pMinatoKunai[1]->Shoot(vKunaiPos, 5.f, true);
	m_pMinatoKunai[1]->Set_HitEffectDuration(1.f);

	vKunaiPos = vPos - vRight * 2.5f + vUp * 2.5f;//상우
	m_pMinatoKunai[2]->Shoot(vKunaiPos, 5.f, true);
	m_pMinatoKunai[2]->Set_HitEffectDuration(1.3f);

	vKunaiPos = vPos + vRight * 2.f + vUp * 0.1f;//하좌
	m_pMinatoKunai[3]->Shoot(vKunaiPos, 5.f, true);
	m_pMinatoKunai[3]->Set_HitEffectDuration(1.6f);

	vKunaiPos = vPos + vRight * 0.6f + vUp * 0.3f;//하중
	m_pMinatoKunai[4]->Shoot(vKunaiPos, 5.f, true);
	m_pMinatoKunai[4]->Set_HitEffectDuration(1.9f);

	vKunaiPos = vPos - vRight * 2.3f + vUp * 0.1f;
	m_pMinatoKunai[5]->Shoot(vKunaiPos, 5.f, true);
	m_pMinatoKunai[5]->Set_HitEffectDuration(2.2f);

	
}

void CPlayer::Move_KunaiCenter()
{
	_vector vPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);

	for (_uint i = 0; i < 6; ++i)
	{
		vPos += m_pMinatoKunai[i]->Get_Position();
	}

	vPos = vPos / 6.f;
	vPos = XMVectorSetY(vPos, XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
}

void CPlayer::Disappear_MinatoKunai()
{
	for (_uint i = 0; i < 6; ++i)
	{
		if (nullptr == m_pMinatoKunai[i])
			continue;

		m_pMinatoKunai[i]->Disappear();
	}
}

void CPlayer::Appear_MinatoKunai_One()
{
	if (nullptr == m_pMinatoKunai[0] || m_pMinatoKunai[0]->IsActive())
		return;

	m_pMinatoKunai[0]->Appear();
}

void CPlayer::Shoot_MinatoKunai_One()
{
	if (nullptr == m_pMinatoKunai[0] )
		return;

	Find_NearTarget();

	m_pMinatoKunai[0]->Shoot(XMVectorSet(0.f, 0.f, 0.f, 1.f), 17.f, false, true);
}

void CPlayer::Disappear_MinatoKunai_One()
{
	if (nullptr == m_pMinatoKunai[0])
		return;

	m_pMinatoKunai[0]->Disappear();
}

_bool CPlayer::Get_IsKunaiCollision_One()
{
	if (nullptr == m_pMinatoKunai[0])
		return false;

	return m_pMinatoKunai[0]->Get_IsCollision();
}

CGameObject* CPlayer::Get_KuaniHitObject_One()
{
	if (nullptr == m_pMinatoKunai[0])
		return nullptr;

	return m_pMinatoKunai[0]->Get_TargetObject();
	
}


HRESULT CPlayer::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Sound */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sound"),
		TEXT("Com_Sound"), reinterpret_cast<CComponent**>(&m_pSoundCom))))
		return E_FAIL;
	m_pSoundCom->Set_Owner(this);

	/* FOR.Com_Sound */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sound"),
		TEXT("Com_EffectSound"), reinterpret_cast<CComponent**>(&m_pEffectSoundCom))))
		return E_FAIL;
	m_pEffectSoundCom->Set_Owner(this);

	/* FOR.Com_RigidBody */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBodyCom))))
		return E_FAIL;
	m_pRigidBodyCom->SetOwner(this);
	m_pRigidBodyCom->Set_IsFriction(true);
	m_pRigidBodyCom->Set_Friction(_float3(20.f, 0.f, 20.f));
	m_pRigidBodyCom->Set_IsGravity(false);
	m_pRigidBodyCom->Set_GravityScale(15.f);
	m_pRigidBodyCom->Set_VelocityLimit(_float3(25.f, 30.f, 25.f));

	if(FAILED(Ready_Collider()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Ready_Collider()
{
	/* For.Com_Collider_Body */
	CBounding_OBB::BOUNDING_OBB_DESC			ColliderDesc_OBB{};
	ColliderDesc_OBB.vExtents = _float3(0.3f, 0.5f, 0.25f);
	ColliderDesc_OBB.vCenter = _float3(0.f, 0.7f, 0.1f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_Body"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc_OBB)))
		return E_FAIL;
	m_pColliderCom->Set_Owner(this);
	m_pColliderCom->Set_ColliderTag(TEXT("PlayerBody"));

	CColliderObject::OBB_COLLIDEROBJ_DESC	ColliderObj_Desc{};
	ColliderObj_Desc.pOwner = this;
	ColliderObj_Desc.Desc_OBB.vExtents = _float3(0.4f, 0.6f, 0.5f);
	ColliderObj_Desc.Desc_OBB.vCenter = _float3(0.f, 0.7f, 0.6f);
	ColliderObj_Desc.fRotationPerSec = 0.f;
	ColliderObj_Desc.fSpeedPerSec = 0.f;
	m_pCollider_Attack = CCollider_PlayerAttack::Create(m_pDevice, m_pContext, &ColliderObj_Desc);

	return S_OK;
}

HRESULT CPlayer::Ready_Skill()
{
	CSkill::SKILL_DESC Desc{};
	Desc.iLevelIndex = m_iLevelIndex;
	Desc.fRotationPerSec = 0.f;
	Desc.fSpeedPerSec = 0.f;
	Desc.pOwner = this;

	m_pSkill_ChangeWood = dynamic_cast<CSkill_ChangeWood*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Skill_ChangeWood"), &Desc));
	if (nullptr == m_pSkill_ChangeWood)
		return E_FAIL;
	m_pGameInstance->Add_Object_ToLayer(m_iLevelIndex, TEXT("Layer_Skill"), m_pSkill_ChangeWood);

	return S_OK;
}

HRESULT CPlayer::Ready_UI()
{
	CUIObject::UI_DESC			UIDesc{};
	UIDesc.iDepth = 1;
	UIDesc.isChild = false;
	UIDesc.iLevelIndex = m_iLevelIndex;
	UIDesc.fSpeedPerSec = 10.f;
	UIDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	UIDesc.fX = 230.f;
	UIDesc.fY = 630.f;
	UIDesc.fSizeX = 400.f;
	UIDesc.fSizeY = 120.f;
	m_pUI_SP = dynamic_cast<CUI_PlayerSP*>(m_pGameInstance->Get_CloneObject_AddLayer(m_iLevelIndex, TEXT("Layer_PlayerUI"), TEXT("Prototype_UI_PlayerSP"), &UIDesc));
	if (nullptr == m_pUI_SP)
		return E_FAIL;

	CUI_PlayerHP::HPUI_DESC		HPUIDesc{};
	memcpy(&HPUIDesc, &UIDesc, sizeof(CUIObject::UI_DESC));

	HPUIDesc.fX = 280.f;
	HPUIDesc.fY = 653.5f;
	HPUIDesc.fSizeX = 380.f;
	HPUIDesc.fSizeY = 50.f;
	HPUIDesc.pPlayer = this;
	HPUIDesc.pPlayerHP = &m_fCurrentHP;
	m_pUI_HP = dynamic_cast<CUI_PlayerHP*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_UI_PlayerHP"), &HPUIDesc));
	if (nullptr == m_pUI_HP)
		return E_FAIL;
	Safe_AddRef(m_pUI_HP);
	m_pGameInstance->Add_Object_ToLayer(m_iLevelIndex, TEXT("Layer_PlayerUI"), m_pUI_HP);

	CUI_PlayerSkill::SKILL_DESC		SkillDesc{};
	memcpy(&SkillDesc, &UIDesc, sizeof(CUIObject::UI_DESC));

	// 스페셜 스킬
	SkillDesc.fX = 110.f;
	SkillDesc.fY = 635.f;
	SkillDesc.fSizeX = 70.f;
	SkillDesc.fSizeY = 70.f;
	SkillDesc.isBaseSkill = false;
	m_pUI_SkillSlot[SLOT_SP] = dynamic_cast<CUI_PlayerSkill*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_UI_PlayerSkill"), &SkillDesc));
	if (nullptr == m_pUI_SkillSlot[SLOT_SP])
		return E_FAIL;

	SkillDesc.fX = 1150.f;
	SkillDesc.fY = 635.f;
	SkillDesc.fSizeX = 80.f;
	SkillDesc.fSizeY = 80.f;
	SkillDesc.isBaseSkill = true;
	m_pUI_SkillSlot[SLOT_BASE1] = dynamic_cast<CUI_PlayerSkill*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_UI_PlayerSkill"), &SkillDesc));
	if (nullptr == m_pUI_SkillSlot[SLOT_BASE1])
		return E_FAIL;

	SkillDesc.fX = 1055.f;
	SkillDesc.fY = 635.f;
	m_pUI_SkillSlot[SLOT_BASE2] = dynamic_cast<CUI_PlayerSkill*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_UI_PlayerSkill"), &SkillDesc));
	if (nullptr == m_pUI_SkillSlot[SLOT_BASE2])
		return E_FAIL;

	SkillDesc.fX = 965.f;
	SkillDesc.fY = 645.f;
	SkillDesc.fSizeX = 70.f;
	SkillDesc.fSizeY = 70.f;
	m_pUI_SkillSlot[SLOT_WOOD] = dynamic_cast<CUI_PlayerSkill*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_UI_PlayerSkill"), &SkillDesc));
	if (nullptr == m_pUI_SkillSlot[SLOT_WOOD])
		return E_FAIL;
	m_pUI_SkillSlot[SLOT_WOOD]->Change_Skill(CUI_PlayerSkill::SKILL_WOOD, &m_fCoolTime_ChangeWood);

	for (_uint i = 0; i < SLOT_END; ++i)
	{
		Safe_AddRef(m_pUI_SkillSlot[i]);
		m_pGameInstance->Add_Object_ToLayer(m_iLevelIndex, TEXT("Layer_PlayerUI"), m_pUI_SkillSlot[i]);
	}

	UIDesc.fX = 850.f;
	UIDesc.fY = 250.f;
	UIDesc.fSizeX = 300.f;
	UIDesc.fSizeY = 100.f;
	m_pUI_Hit = dynamic_cast<CUI_HitNum*>(m_pGameInstance->Get_CloneObject_AddLayer(m_iLevelIndex, TEXT("Layer_PlayerUI"), TEXT("Prototype_UI_Hit"), &UIDesc));
	if (nullptr == m_pUI_Hit)
		return E_FAIL;

	m_pUI_Target = dynamic_cast<CUI_Target*>(m_pGameInstance->Get_CloneObject_AddLayer(m_iLevelIndex, TEXT("Layer_PlayerUI"), TEXT("Prototype_UI_Target"), &UIDesc));
	if (nullptr == m_pUI_Hit)
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Ready_ETC()
{
	if (m_iLevelIndex == LEVEL_CUSTOM)
		return S_OK;

	CGameObject::GAMEOBJECT_DESC Desc{};
	ZeroMemory(&Desc, sizeof GAMEOBJECT_DESC);
	Desc.fRotationPerSec = 0.f;
	Desc.fSpeedPerSec = 0.f;
	Desc.iLevelIndex = m_iLevelIndex;


	CKuani_Minato::MINATOKUNAI_DESC KunaiDesc{};
	memcpy(&KunaiDesc, &Desc, sizeof(CGameObject::GAMEOBJECT_DESC));
	KunaiDesc.pWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();

	KunaiDesc.pSocketMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr("RightHandMiddle1");
	for (_uint i = 0; i < 3; ++i)
	{
		m_pMinatoKunai[i] = dynamic_cast<CKuani_Minato*>(m_pGameInstance->Get_CloneObject_AddLayer(m_iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_KunaiMinato"), &KunaiDesc));
		if (nullptr == m_pMinatoKunai[i])
			return E_FAIL;
	}

	KunaiDesc.pSocketMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr("LeftHandMiddle1");
	for (_uint i = 3; i < 6; ++i)
	{
		m_pMinatoKunai[i] = dynamic_cast<CKuani_Minato*>(m_pGameInstance->Get_CloneObject_AddLayer(m_iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_KunaiMinato"), &KunaiDesc));
		if (nullptr == m_pMinatoKunai[i])
			return E_FAIL;
	}



	m_iFootBoneIndex[0] = m_pModelCom->Get_BoneIndex("LeftToeBase_end");
	m_iFootBoneIndex[1] = m_pModelCom->Get_BoneIndex("RightToeBase_end");

	CFootTrail::FOOTTRAIL_DESC	FootDesc{};
	memcpy(&FootDesc, &Desc, sizeof(CGameObject::GAMEOBJECT_DESC));

	FootDesc.iLevelIndex = m_iLevelIndex;
	FootDesc.m_pOwnerWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();

	for (_uint i = 0; i < 2; ++i)
	{
		FootDesc.m_pSocketMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(m_iFootBoneIndex[i]);
		m_pFootTrail[i] = dynamic_cast<CFootTrail*>(m_pGameInstance->Get_CloneObject_AddLayer(m_iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_FootTrail"), &FootDesc));
		if (nullptr == m_pFootTrail[i])
			return E_FAIL;
	}

	CWireTrail::WIRETRAIL_DESC	WireDesc{};
	memcpy(&WireDesc, &FootDesc, sizeof(CFootTrail::FOOTTRAIL_DESC));

	WireDesc.m_pSocketMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr("RightHandMiddle1");
	m_pWireTrail = dynamic_cast<CWireTrail*>(m_pGameInstance->Get_CloneObject_AddLayer(m_iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_WireTrail"), &WireDesc));
	if (nullptr == m_pWireTrail)
		return E_FAIL;

	m_pJumpGuide = dynamic_cast<CJumpGuide*>(m_pGameInstance->Get_CloneObject_AddLayer(m_iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_JumpGuide"), &Desc));
	if (nullptr == m_pJumpGuide)
		return E_FAIL;
	m_pJumpGuide->Set_Owner(this);

	return S_OK;
}

void CPlayer::Control_Gravity()
{
	if (m_isFall)
	{
		m_pRigidBodyCom->Set_IsGravity(true);
		m_pRigidBodyCom->Set_IsGround(false);
	}
	else if (m_isJump || m_isDoubleJump)
	{
		if(m_isSkill)
			m_pRigidBodyCom->Set_IsGravity(false);
		else
			m_pRigidBodyCom->Set_IsGravity(true);
		m_pRigidBodyCom->Set_IsGround(false);
	}
	else if (!m_isCollision && m_isGravity)
	{
		m_pRigidBodyCom->Set_IsGravity(true);
		m_pRigidBodyCom->Set_IsGround(false);
	}
	else if (m_isGround)
	{
		m_pRigidBodyCom->Set_IsGravity(false);
		m_pRigidBodyCom->Set_IsGround(true);
	}
	else if (m_isWall)
	{
		m_pRigidBodyCom->Set_IsGravity(false);
		m_pRigidBodyCom->Set_IsGround(true);
	}
	else
	{
		m_pRigidBodyCom->Set_IsGravity(false);
	}
}

_bool CPlayer::Change_Wood()
{
	if (m_fCoolTime_ChangeWood > 0.f)
		return false;

	m_pSkill_ChangeWood->Appear();
	m_fCoolTime_ChangeWood = 8.f;

	_matrix		OwnerWorldMatirx = m_pTransformCom->Get_WorldMatrix();

	_vector		vRight = XMVector3Normalize(OwnerWorldMatirx.r[0]);
	_vector		vUp = XMVector3Normalize(OwnerWorldMatirx.r[1]);
	_vector		vLook = XMVector3Normalize(OwnerWorldMatirx.r[2]);
	_vector		vPosition = OwnerWorldMatirx.r[3];

	vPosition += vUp * 0.7f;
	CObjectPool<CSmokeRing>::Get_GameObject()->Appear(vPosition, 4.f, 0.3f, 5.f);

	vPosition = OwnerWorldMatirx.r[3];

	_float3 vPos;
	XMStoreFloat3(&vPos, vPosition);
	for (_uint i = 0; i < 10; ++i)
	{
		CObjectPool<CSmoke11>::Get_GameObject()->Appear(vPos, _float3(0.3f, 0.25f, 0.3f), 0.3f, CSmoke11::SMOKE_WHITE);
	}

	_vector vNewPos = OwnerWorldMatirx.r[3];
	vNewPos = vNewPos + vLook * -3.f + vUp - vRight;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vNewPos);

	return true;
}

CPlayer* CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer* pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer::Clone(void* pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	for (_uint i = 0; i < SLOT_END; ++i)
	{
		Safe_Release(m_pUI_SkillSlot[i]);
	}

	Safe_Release(m_pUI_HP);
	Safe_Release(m_pShaderCom);

	if (nullptr != m_pFsmCom)
		m_pFsmCom->Release_States();
	Safe_Release(m_pFsmCom);

	Safe_Release(m_pCollider_Attack);
	Safe_Release(m_pColliderCom);

	Safe_Release(m_pSoundCom);
	Safe_Release(m_pEffectSoundCom);
	Safe_Release(m_pRigidBodyCom);
}