#include "stdafx.h"
#include "Boss_Kurama.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"

#include "Player.h"
#include "Collider_PlayerAttack.h"
#include "Collider_MonsterAttack.h"

#include "State_Kurama_Appear.h"
#include "State_Kurama_Roar.h"
#include "State_Kurama_Idle.h"
#include "State_Kurama_Attack1.h"
#include "State_Kurama_Attack2.h"
#include "State_Kurama_Bite.h"
#include "State_Kurama_JumpCrush.h"
#include "State_Kurama_ChanceTime.h"
#include "State_Kurama_Hit.h"
#include "State_Kurama_FoxBlast.h"
#include "State_Kurama_AerialBlast.h"
#include "State_Kurama_Die.h"
#include "State_Kurama_Leave.h"
#include "State_Kurama_Heal.h"
#include "State_Kurama_ThreeBlast.h"
#include "State_Kurama_SpecialAttack.h"
#include "State_Kurama_Jump.h"
#include "State_Kurama_Fall.h"

#include "Skill_Roar.h"
#include "Skill_KuramaRasengan.h"
#include "ObjectPool.h"
#include "FoxSphere.h"
#include "FoxBlast.h"

#include "UI_BossHP.h"
#include "UI_BossTarget.h"

CBoss_Kurama::CBoss_Kurama(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster{ pDevice, pContext }
{
	m_eMonsterID = KURAMA;
}

CBoss_Kurama::CBoss_Kurama(const CBoss_Kurama& Prototype)
	: CMonster{ Prototype }
{
	m_eMonsterID = KURAMA;
}

HRESULT CBoss_Kurama::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBoss_Kurama::Initialize(void* pArg)
{
	/* 직교투영을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (FAILED(Ready_State()))
		return E_FAIL;

	if (FAILED(Ready_Skill()))
		return E_FAIL;

	m_eMonsterID = KURAMA;

	m_fCurrentHP = 500.f;
	m_fMaxHP = m_fCurrentHP;

	if (FAILED(Ready_UI()))
		return E_FAIL;
	//m_pSoundCom->Play3D(TEXT("BGM_01.wav"), 1.f);

	m_isChance = false;

	return S_OK;
}

void CBoss_Kurama::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

	if (KEY_TAP(KEY::X))
		m_pRigidBodyCom->Set_IsGravity(true);

}

void CBoss_Kurama::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if (KEY_TAP(KEY::UP))
	{
		Change_State(JUMP);
	}

	if (KEY_TAP(KEY::DOWN))
		m_pFsmCom->Change_State(ROAR);

	m_pSoundCom->Update(fTimeDelta);
}

void CBoss_Kurama::Late_Update(_float fTimeDelta)
{
	if (m_fCurrentHP <= 0 && m_pFsmCom->Get_CurrentState() != DIE)
	{
		m_pFsmCom->Change_State(DIE);
	}

	__super::Late_Update(fTimeDelta);
}

HRESULT CBoss_Kurama::Render()
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

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render((_uint)i)))
			return E_FAIL;
	}

	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CBoss_Kurama::OnCollisionEnter(CGameObject* pOther)
{
	if (m_fCurrentHP <= 0.f)
		return;

	
}

void CBoss_Kurama::OnCollisionStay(CGameObject* pOther)
{
}

void CBoss_Kurama::OnCollisionExit(CGameObject* pOther)
{
}

void CBoss_Kurama::Damaged_To_Player(_uint iState, _float fDamage, _bool isStrongAttack)
{
	m_fCurrentHP -= fDamage;

	if (m_fCurrentHP <= 0.f)
	{
		m_pFsmCom->Change_State(DIE);
		return;
	}

	if (!m_isChance && m_fCurrentHP <= m_fMaxHP * 0.5f)
	{
		m_pFsmCom->Change_State(CHANCE);
		m_isChance = true;
	}

	if (m_pFsmCom->Get_CurrentState() != IDLE)
		return;

}

void CBoss_Kurama::Start_Roar()
{
	m_pSkill_Roar->Appear();
}

void CBoss_Kurama::End_Roar()
{
	m_pSkill_Roar->Disappear();
}

void CBoss_Kurama::Shoot_FoxSphere(_int iBoneIndex)
{
	Find_PlayerTarget();

	_matrix		SocektMatrix;
	if(iBoneIndex < 0)
		SocektMatrix = XMLoadFloat4x4(m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr("Tongue4"));
	else
		SocektMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix(iBoneIndex);

	_matrix		WorldMatrix = SocektMatrix * m_pTransformCom->Get_WorldMatrix();

	_vector vPos = WorldMatrix.r[3];
	_vector vTargetPos = m_pPlayerTarget->Get_Position() + XMVector3Normalize(m_pPlayerTarget->Get_Transform()->Get_State(CTransform::STATE_UP));

	CObjectPool<CFoxSphere>::Get_GameObject()->Appear(vPos, vTargetPos);
}

void CBoss_Kurama::Shoot_FoxBlast(_fvector vTargetPos)
{

	_matrix		SocektMatrix = XMLoadFloat4x4(m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr("Tongue4"));
	_matrix		WorldMatrix = SocektMatrix * m_pTransformCom->Get_WorldMatrix();

	_vector vPos = WorldMatrix.r[3];

	CObjectPool<CFoxBlast>::Get_GameObject()->Appear(vPos, vTargetPos);
}

void CBoss_Kurama::Create_Rasengan()
{
	m_pSkill_KuramaRasengan->Appear();
}

void CBoss_Kurama::Shoot_Rasengan()
{
	m_pSkill_KuramaRasengan->Shoot();
}

void CBoss_Kurama::Appear_TargetUI()
{
	m_pUI_Icon->IsActive(true);
}

HRESULT CBoss_Kurama::Ready_Components(void* pArg)
{
	__super::Ready_Components(pArg);

	/* For.Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC			ColliderDesc{};
	ColliderDesc.vExtents = _float3(2.5f, 4.5f, 2.5f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, -1.22f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;
	m_pColliderCom->Set_Owner(this);

	CColliderObject::OBB_COLLIDEROBJ_DESC	ColliderObj_Desc{};
	ColliderObj_Desc.pOwner = this;
	ColliderObj_Desc.Desc_OBB.vExtents = _float3(2.5f, 4.5f, 2.5f);
	ColliderObj_Desc.Desc_OBB.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 1.f);
	ColliderObj_Desc.fRotationPerSec = 0.f;
	ColliderObj_Desc.fSpeedPerSec = 0.f;
	m_pCollider_Attack = CCollider_MonsterAttack::Create(m_pDevice, m_pContext, &ColliderObj_Desc);

	return S_OK;
}

HRESULT CBoss_Kurama::Ready_State()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Fsm"),
		TEXT("Com_Fsm"), reinterpret_cast<CComponent**>(&m_pFsmCom), this)))
		return E_FAIL;

	m_pFsmCom->Add_State(CState_Kurama_Appear::Create(m_pFsmCom, this, APPEAR));
	m_pFsmCom->Add_State(CState_Kurama_Roar::Create(m_pFsmCom, this, ROAR));
	m_pFsmCom->Add_State(CState_Kurama_Idle::Create(m_pFsmCom, this, IDLE));
	m_pFsmCom->Add_State(CState_Kurama_ChanceTime::Create(m_pFsmCom, this, CHANCE));
	m_pFsmCom->Add_State(CState_Kurama_Hit::Create(m_pFsmCom, this, HIT));
	m_pFsmCom->Add_State(CState_Kurama_Die::Create(m_pFsmCom, this, DIE));
	m_pFsmCom->Add_State(CState_Kurama_Jump::Create(m_pFsmCom, this, JUMP));
	m_pFsmCom->Add_State(CState_Kurama_Fall::Create(m_pFsmCom, this, FALL));
	m_pFsmCom->Add_State(CState_Kurama_Leave::Create(m_pFsmCom, this, LEAVE));
	m_pFsmCom->Add_State(CState_Kurama_Heal::Create(m_pFsmCom, this, HEAL));

	// 공격
	m_pFsmCom->Add_State(CState_Kurama_Attack1::Create(m_pFsmCom, this, ATTACK1));
	m_pFsmCom->Add_State(CState_Kurama_Attack2::Create(m_pFsmCom, this, ATTACK2));
	m_pFsmCom->Add_State(CState_Kurama_Bite::Create(m_pFsmCom, this, BITE));
	m_pFsmCom->Add_State(CState_Kurama_JumpCrush::Create(m_pFsmCom, this, JUMPCRUSH));
	
	// 패턴
	m_pFsmCom->Add_State(CState_Kurama_FoxBlast::Create(m_pFsmCom, this, BLAST));
	m_pFsmCom->Add_State(CState_Kurama_AerialBlast::Create(m_pFsmCom, this, AERIAL_BLAST));
	m_pFsmCom->Add_State(CState_Kurama_ThreeBlast::Create(m_pFsmCom, this, THREE_BLAST));
	m_pFsmCom->Add_State(CState_Kurama_SpecialAttack::Create(m_pFsmCom, this, SPECIALATTACK));

	m_pFsmCom->Set_State(APPEAR);

	return S_OK;
}

HRESULT CBoss_Kurama::Ready_Skill()
{
	CSkill::SKILL_DESC Desc{};
	Desc.iLevelIndex = m_iLevelIndex;
	Desc.fRotationPerSec = 0.f;
	Desc.fSpeedPerSec = 0.f;
	Desc.pOwner = this;

	m_pSkill_Roar = dynamic_cast<CSkill_Roar*>(m_pGameInstance->Get_CloneObject_AddLayer(m_iLevelIndex, TEXT("Layer_MonsterSkill"), TEXT("Prototype_GameObject_Skill_Roar"), &Desc));
	if (nullptr == m_pSkill_Roar)
		return E_FAIL;

	m_pSkill_KuramaRasengan = dynamic_cast<CSkill_KuramaRasengan*>(m_pGameInstance->Get_CloneObject_AddLayer(m_iLevelIndex, TEXT("Layer_MonsterSkill"), TEXT("Prototype_GameObject_Skill_KuramaRasengan"), &Desc));
	if (nullptr == m_pSkill_Roar)
		return E_FAIL;
	return S_OK;
}

HRESULT CBoss_Kurama::Ready_UI()
{
	CUI_BossHP::HPUI_DESC UIDesc{};
	UIDesc.iDepth = 1;
	UIDesc.isChild = false;
	UIDesc.iLevelIndex = m_iLevelIndex;
	UIDesc.fSpeedPerSec = 10.f;
	UIDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	UIDesc.fX = g_iWinSizeX * 0.5f;
	UIDesc.fY = 55.f;
	UIDesc.fSizeX = 500.f;
	UIDesc.fSizeY = 120.f;

	UIDesc.pBossHP = &m_fCurrentHP;
	UIDesc.pMonster = this;

	m_pUI_HP = dynamic_cast<CUI_BossHP*>(m_pGameInstance->Get_CloneObject_AddLayer(m_iLevelIndex, TEXT("Layer_UI"), TEXT("Prototype_UI_BossHP"), &UIDesc));
	if (nullptr == m_pUI_HP)
		return E_FAIL;

	m_pUI_Icon = dynamic_cast<CUI_BossTarget*>(m_pGameInstance->Get_CloneObject_AddLayer(m_iLevelIndex, TEXT("Layer_UI"), TEXT("Prototype_UI_BossTarget"), &UIDesc));
	if (nullptr == m_pUI_Icon)
		return E_FAIL;
	m_pUI_Icon->Set_Owner(this);

	return S_OK;
}

CBoss_Kurama* CBoss_Kurama::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBoss_Kurama* pInstance = new CBoss_Kurama(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBoss_Kurama"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CBoss_Kurama::Clone(void* pArg)
{
	CBoss_Kurama* pInstance = new CBoss_Kurama(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CBoss_Kurama"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoss_Kurama::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
}
