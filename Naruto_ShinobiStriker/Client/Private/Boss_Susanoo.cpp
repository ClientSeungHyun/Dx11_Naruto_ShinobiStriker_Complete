#include "stdafx.h"
#include "Boss_Susanoo.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "SusanooBlade.h"

#include "Player.h"
#include "Collider_PlayerAttack.h"
#include "Collider_MonsterAttack.h"

#include "State_Susanoo_Appear.h"
#include "State_Susanoo_Roar.h"
#include "State_Susanoo_Idle.h"
#include "State_Susanoo_Die.h"
#include "State_Susanoo_ChanceTime.h"
#include "State_Susanoo_Hit.h"
#include "State_Susanoo_DoubleSlash.h"
#include "State_Susanoo_GroundPunch.h"
#include "State_Susanoo_Rising.h"
#include "State_Susanoo_GroundSlash.h"

#include "Skill_GroundSlash.h"
#include "ObjectPool.h"
#include "BladeSlash.h"
#include "Effect_GroundBreak.h"
#include "Smoke11.h"

#include "UI_BossHP.h"
#include "UI_BossTarget.h"

CBoss_Susanoo::CBoss_Susanoo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster{ pDevice, pContext }
{
	m_eMonsterID = SUSANOO;
}

CBoss_Susanoo::CBoss_Susanoo(const CBoss_Susanoo& Prototype)
	: CMonster{ Prototype }
{
	m_eMonsterID = SUSANOO;
}

HRESULT CBoss_Susanoo::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBoss_Susanoo::Initialize(void* pArg)
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

	m_eMonsterID = SUSANOO;

	m_fCurrentHP = 200.f;
	m_fMaxHP = m_fCurrentHP;

	if(FAILED(Ready_UI()))
		return E_FAIL;

	//m_pSoundCom->Play3D(TEXT("BGM_01.wav"), 1.f);


	m_fDissloveRatio = 0.f;
	m_isChance = false;

	m_iRootBoneIndeox = m_pModelCom->Get_BoneIndex("CharacterRoot");

	return S_OK;
}

void CBoss_Susanoo::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

	if (KEY_TAP(KEY::X))
		m_pRigidBodyCom->Set_IsGravity(true);

	if(nullptr != m_pBlade)
		m_pBlade->Priority_Update(fTimeDelta);
}

void CBoss_Susanoo::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if (KEY_HOLD(KEY::K))
	{
		m_fDissloveRatio +=  0.8f * fTimeDelta;
	}

	m_pSoundCom->Update(fTimeDelta);

	if (nullptr != m_pBlade)
		m_pBlade->Update(fTimeDelta);
}

void CBoss_Susanoo::Late_Update(_float fTimeDelta)
{
	//if (m_fCurrentHP <= 0 && m_pFsmCom->Get_CurrentState() != DIE)
	//{
	//	m_pFsmCom->Change_State(DIE);
	//	//m_isDead = true;
	//}

	__super::Late_Update(fTimeDelta);

	if (nullptr != m_pBlade)
		m_pBlade->Late_Update(fTimeDelta);


	//Contorl_RootBone();
}

HRESULT CBoss_Susanoo::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pDissloveTexture->Bind_ShadeResource(m_pShaderCom, "g_DissloveTexture", 0)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveRatio", &m_fDissloveRatio, sizeof(_float))))
		return E_FAIL;
	
	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(3)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render((_uint)i)))
			return E_FAIL;
	}

	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CBoss_Susanoo::OnCollisionEnter(CGameObject* pOther)
{
	if (m_fCurrentHP <= 0.f)
		return;


}

void CBoss_Susanoo::OnCollisionStay(CGameObject* pOther)
{
}

void CBoss_Susanoo::OnCollisionExit(CGameObject* pOther)
{
}

void CBoss_Susanoo::Damaged_To_Player(_uint iState, _float fDamage, _bool isStrongAttack)
{
	m_fCurrentHP -= 10.f;

	if (m_fCurrentHP <= 0.f)
	{
		m_pFsmCom->Change_State(DIE);
		return;
	}

	if (!m_isChance && m_fCurrentHP <= m_fCurrentHP * 0.5f)
	{
		m_pFsmCom->Change_State(CHANCE);
		m_isChance = true;
	}

	if (m_pFsmCom->Get_CurrentState() != IDLE)
		return;


}

void CBoss_Susanoo::Contorl_RootBone()
{
	_vector vCurrentPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vRootBonePos = XMLoadFloat3(&m_vRootBonePos); 
	_vector vNewPos = XMVectorAdd(vCurrentPos, vRootBonePos);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vNewPos);
}

HRESULT CBoss_Susanoo::Ready_Components(void* pArg)
{
	__super::Ready_Components(pArg);

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Smoke27"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pDissloveTexture))))
		return E_FAIL;
	

	/* For.Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC			ColliderDesc{};
	ColliderDesc.vExtents = _float3(2.5f, 5.f, 2.5f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, -1.22f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;
	m_pColliderCom->Set_Owner(this);

	CColliderObject::OBB_COLLIDEROBJ_DESC	ColliderObj_Desc{};
	ColliderObj_Desc.pOwner = this;
	ColliderObj_Desc.Desc_OBB.vExtents = _float3(2.5f, 4.5f, 4.5f);
	ColliderObj_Desc.Desc_OBB.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 1.f);
	ColliderObj_Desc.fRotationPerSec = 0.f;
	ColliderObj_Desc.fSpeedPerSec = 0.f;
	m_pCollider_Attack = CCollider_MonsterAttack::Create(m_pDevice, m_pContext, &ColliderObj_Desc);


	return S_OK;
}

HRESULT CBoss_Susanoo::Ready_State()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Fsm"),
		TEXT("Com_Fsm"), reinterpret_cast<CComponent**>(&m_pFsmCom), this)))
		return E_FAIL;

	m_pFsmCom->Add_State(CState_Susanoo_Appear::Create(m_pFsmCom, this, APPEAR));
	m_pFsmCom->Add_State(CState_Susanoo_Roar::Create(m_pFsmCom, this, ROAR));
	m_pFsmCom->Add_State(CState_Susanoo_Idle::Create(m_pFsmCom, this, IDLE));
	m_pFsmCom->Add_State(CState_Susanoo_Die::Create(m_pFsmCom, this, DIE));
	m_pFsmCom->Add_State(CState_Susanoo_ChanceTime::Create(m_pFsmCom, this, CHANCE));
	m_pFsmCom->Add_State(CState_Susanoo_Hit::Create(m_pFsmCom, this, HIT));

	m_pFsmCom->Add_State(CState_Susanoo_DoubleSlash::Create(m_pFsmCom, this, DOUBLESLASH));
	m_pFsmCom->Add_State(CState_Susanoo_GroundPunch::Create(m_pFsmCom, this, GROUNDPUNCH));
	m_pFsmCom->Add_State(CState_Susanoo_Rising::Create(m_pFsmCom, this, RISING));
	m_pFsmCom->Add_State(CState_Susanoo_GroundSlash::Create(m_pFsmCom, this, GROUNDSLASH));

	m_pFsmCom->Set_State(APPEAR);

	return S_OK;
}

HRESULT CBoss_Susanoo::Ready_Skill()
{
	CSkill::SKILL_DESC Desc{};
	Desc.iLevelIndex = m_iLevelIndex;
	Desc.fRotationPerSec = 0.f;
	Desc.fSpeedPerSec = 0.f;
	Desc.pOwner = this;

	m_pSkill_GroundSlash = dynamic_cast<CSkill_GroundSlash*>(m_pGameInstance->Get_CloneObject_AddLayer(m_iLevelIndex, TEXT("Layer_MonsterSkill"), TEXT("Prototype_GameObject_Skill_GroundSlash"), &Desc));
	if (nullptr == m_pSkill_GroundSlash)
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_Susanoo::Ready_UI()
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

HRESULT CBoss_Susanoo::Summon_Blade()
{
	CSusanooBlade::SUSANOOBLADE_DESC		BladeDesc{};
	BladeDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	BladeDesc.pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr("L_Hand_Weapon_cnt_tr");// L_Hand_Weapon_cnt_tr
	BladeDesc.iLevelIndex = m_iLevelIndex;

	m_pBlade = dynamic_cast<CSusanooBlade*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_SusanooBlade"), &BladeDesc));
	if (nullptr == m_pBlade)
		return E_FAIL;

	return S_OK;
}

void CBoss_Susanoo::Appear_GroundSlash()
{
	m_pSkill_GroundSlash->Appear();
}

void CBoss_Susanoo::Appear_BladeSlsh()
{
	Find_PlayerTarget();

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPos = vPos + XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP)) * 4.f;
		
	_vector vPos2 = m_pPlayerTarget->Get_Position();
	vPos2 = vPos2 + XMVector3Normalize(m_pPlayerTarget->Get_Transform()->Get_State(CTransform::STATE_UP)) * 1.f;
	CObjectPool<CBladeSlash>::Get_GameObject()->Appear(vPos, vPos2);
}

void CBoss_Susanoo::Appear_GroundBreak(_float3 vSize)
{
	_matrix WorldMatrix = XMMatrixIdentity();
	_matrix SocketMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix(m_pModelCom->Get_BoneIndex("RightHandMiddle1"));
	for (size_t i = 0; i < 3; i++)
	{
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
	}
	WorldMatrix = WorldMatrix * SocketMatrix * m_pTransformCom->Get_WorldMatrix();
	WorldMatrix.r[3] = WorldMatrix.r[3] - XMVector3Normalize(WorldMatrix.r[1]) * 0.7f;

	CObjectPool<CEffect_GroundBreak>::Get_GameObject()->Appear(WorldMatrix.r[3], vSize);

	_float3 vCenterPos;
	XMStoreFloat3(&vCenterPos, WorldMatrix.r[3]);
	for (_uint i = 0; i < 40; ++i)
	{
		CObjectPool<CSmoke11>::Get_GameObject()->Appear(vCenterPos, _float3(3.f, 1.5f, 3.f), 1.5f, CSmoke11::SMOKE_BROWN);
	}
}

void CBoss_Susanoo::Appear_TargetUI()
{
	m_pUI_Icon->IsActive(true);
}

CBoss_Susanoo* CBoss_Susanoo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBoss_Susanoo* pInstance = new CBoss_Susanoo(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBoss_Susanoo"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CBoss_Susanoo::Clone(void* pArg)
{
	CBoss_Susanoo* pInstance = new CBoss_Susanoo(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CBoss_Susanoo"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoss_Susanoo::Free()
{
	__super::Free();

	Safe_Release(m_pDissloveTexture);
	Safe_Release(m_pBlade);
	Safe_Release(m_pColliderCom);
}
