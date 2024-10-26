#include "stdafx.h"
#include "Player_Naruto.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"

#include "State_Naruto_Idle.h"
#include "State_Naruto_Run.h"
#include "State_Naruto_RunEnd.h"
#include "State_Naruto_Jump.h"
#include "State_Naruto_DoubleJump.h"
#include "State_Naruto_FallFront.h"
#include "State_Naruto_Land.h"
#include "State_Naruto_WallAttach.h"
#include "State_Naruto_ChakraJump.h"
#include "State_Player_Dash.h"

#include "State_Naruto_PunchLeft.h"
#include "State_Naruto_PunchRight.h"
#include "State_Naruto_ElbowStrike.h"
#include "State_Naruto_DoubleKick.h"

#include "State_Naruto_Rasengun_Charge.h"
#include "State_Naruto_Rasengun_Run.h"
#include "State_Naruto_Rasengun_Attack.h"

#include "State_Naruto_AerialRasengun_Charge.h"
#include "State_Naruto_AerialRasengun_Run.h"
#include "State_Naruto_AerialRasengun_Attack.h"

#include "State_Naruto_RasenShuriken.h"
#include "State_Naruto_AerialRasenShuriken.h"

#include "Skill_Rasengan.h"
#include "Skill_RasenShuriken.h"

#include "UI_PlayerHP.h"
#include "UI_PlayerSP.h"
#include "UI_PlayerSkill.h"

CPlayer_Naruto::CPlayer_Naruto(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayer{ pDevice, pContext }
{
}

CPlayer_Naruto::CPlayer_Naruto(const CPlayer_Naruto& Prototype)
	: CPlayer{ Prototype }
{
}

HRESULT CPlayer_Naruto::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer_Naruto::Initialize(void* pArg)
{
	PLAYER_DESC* pDesc = static_cast<PLAYER_DESC*>(pArg);

	/* 직교투영을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_State()))
		return E_FAIL;

	if (FAILED(Ready_Shadow()))
		return E_FAIL;

	if (FAILED(Ready_Skill()))
		return E_FAIL;

	m_pGameInstance->SetUp_Player(this);
	//m_pGameInstance->Set_Player_Picking(this);

	m_pTransformCom->Rotation(pDesc->vRotation.x, pDesc->vRotation.y, pDesc->vRotation.z);

	m_pUI_HP->Start_Appear();

	return S_OK;
}

void CPlayer_Naruto::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

	if (nullptr != m_pNearTarget)
	{
		if (m_pNearTarget->Get_Dead())
		{

			Safe_Release(m_pNearTarget);
			m_pNearTarget = nullptr;
		}
	}

	//for (size_t i = 0; i < 2; ++i)
	//{
	//	m_pShadow[i]->Priority_Update(fTimeDelta);
	//}
}

void CPlayer_Naruto::Update(_float fTimeDelta)
{
	m_fCoolTime_RasenShuriken -= fTimeDelta;
	m_fCoolTime_Rasengan -= fTimeDelta;

	if (m_fCoolTime_RasenShuriken <= 0.f)
		m_fCoolTime_RasenShuriken = 0.f;
	if (m_fCoolTime_Rasengan <= 0.f)
		m_fCoolTime_Rasengan = 0.f;

	//for (size_t i = 0; i < 2; ++i)
	//{
	//	m_pShadow[i]->Update(fTimeDelta);
	//}

	if (KEY_TAP(KEY::P))
	{
		if (Change_Wood())
		{
			m_pSoundCom->Play3D(TEXT("Naruto_Slow.wav"), g_fVolume);
			m_pFsmCom->Change_State(FALL_FRONT);
		}
	}

	__super::Update(fTimeDelta);
}

void CPlayer_Naruto::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CPlayer_Naruto::Render()
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

		if (m_isSenninEye
			&& (m_iSenninIndex == i || (m_iSenninIndex + 1) == i))
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i, 1)))
				return E_FAIL;
		}


		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render((_uint)i)))
			return E_FAIL;
	}

#ifdef _DEBUG
	if (m_iLevelIndex != LEVEL_CUSTOM)
	{
		m_pColliderCom->Render();
	}
#endif

	return S_OK;
}

void CPlayer_Naruto::OnCollisionEnter(CGameObject* pOther)
{
	if (pOther->Get_LayerTag() == TEXT("Layer_Monster"))
	{
		if (m_pFsmCom->Get_CurrentState() == RASENGUN_RUN)
		{
			m_pFsmCom->Change_State(CPlayer_Naruto::RASENGUN_ATTACK);
		}

		if (m_pFsmCom->Get_CurrentState() == AERIAL_RASENGUN_RUN)
		{
			m_pFsmCom->Change_State(CPlayer_Naruto::AERIAL_RASENGUN_ATTACK);
		}
	}
	
	if (pOther->Get_LayerTag() == TEXT("Layer_MonsterAttack"))
	{
		m_pFsmCom->Change_State(HIT1);
	}


}

void CPlayer_Naruto::OnCollisionStay(CGameObject* pOther)
{
}

void CPlayer_Naruto::OnCollisionExit(CGameObject* pOther)
{
}

void CPlayer_Naruto::Create_ShadowNaruto(CShadow_Naruto::SHADOW_NARUTO_DESC tDesc)
{
	tDesc.eDir = CShadow_Naruto::LEFT;
	m_pShadow[SHA_LEFT]->Create_ShadowNaruto(tDesc);

	tDesc.eDir = CShadow_Naruto::RIGHT;
	m_pShadow[SHA_RIGHT]->Create_ShadowNaruto(tDesc);
}

void CPlayer_Naruto::Create_RasenShuriken()
{
	if(!m_pSkill_RasenShuriken->IsActive())
		m_pSkill_RasenShuriken->Appear();
}

void CPlayer_Naruto::Shoot_RasenShuriken()
{
	Find_NearTarget();
	m_pSkill_RasenShuriken->Shoot();
}

_float CPlayer_Naruto::Get_RasenShurikenCoolTime()
{
	if (m_fCoolTime_RasenShuriken <= 0.f)
	{
		m_fCoolTime_RasenShuriken = 10.f;
		return 0.f;
	}

	return m_fCoolTime_RasenShuriken;
}

void CPlayer_Naruto::Create_Rasengan()
{
	if (!m_pSkill_Rasengan->IsActive())
		m_pSkill_Rasengan->Appear();
}

void CPlayer_Naruto::Run_Rasengan()
{
	m_pSkill_Rasengan->Run();
}

void CPlayer_Naruto::Explosion_Rasengan()
{
	m_pSkill_Rasengan->Explosion();
}

_float CPlayer_Naruto::Get_RasenganCoolTime()
{
	if (m_fCoolTime_Rasengan <= 0.f)
	{
		m_fCoolTime_Rasengan = 10.f;
		return 0.f;
	}

	return m_fCoolTime_Rasengan;
}

HRESULT CPlayer_Naruto::Ready_Components()
{
	__super::Ready_Components();

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(m_iLevelIndex, TEXT("Prototype_Component_Model_Naruto"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	// 추가 텍스쳐 세팅
	m_iSenninIndex = 2;
	m_pModelCom->Add_Texture_to_Material(TEXT("../Bin/ModelData/Anim/Naruto/T_CHR_Naruto_Sennin_Eye_BC.dds"), TEXTURE_TYPE::DIFFUSE, m_iSenninIndex);
	m_pModelCom->Add_Texture_to_Material(TEXT("../Bin/ModelData/Anim/Naruto/T_CHR_Naruto_Sennin_Eye_BC.dds"), TEXTURE_TYPE::DIFFUSE, m_iSenninIndex + 1);

	return S_OK;
}

HRESULT CPlayer_Naruto::Ready_State()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Fsm"),
		TEXT("Com_Fsm"), reinterpret_cast<CComponent**>(&m_pFsmCom), this)))
		return E_FAIL;

	m_pFsmCom->Add_State(CState_Player_Dash::Create(m_pFsmCom, this, DASH));

	m_pFsmCom->Add_State(CState_Naruto_Idle::Create(m_pFsmCom, this, IDLE));
	m_pFsmCom->Add_State(CState_Naruto_Run::Create(m_pFsmCom, this, RUN));
	m_pFsmCom->Add_State(CState_Naruto_RunEnd::Create(m_pFsmCom, this, RUN_END));
	m_pFsmCom->Add_State(CState_Naruto_Jump::Create(m_pFsmCom, this, JUMP));
	m_pFsmCom->Add_State(CState_Naruto_DoubleJump::Create(m_pFsmCom, this, DOUBLEJUMP));
	m_pFsmCom->Add_State(CState_Naruto_FallFront::Create(m_pFsmCom, this, FALL_FRONT));
	m_pFsmCom->Add_State(CState_Naruto_Land::Create(m_pFsmCom, this, LAND));
	m_pFsmCom->Add_State(CState_Naruto_WallAttach::Create(m_pFsmCom, this, WALLATTACH));
	m_pFsmCom->Add_State(CState_Naruto_ChakraJump::Create(m_pFsmCom, this, CHAKRAJUMP));

	/* 공격 순서 */
	//m_pFsmCom->Add_State(CState_Naruto_PunchLeft::Create(m_pFsmCom, this, ATTACK1));
	//m_pFsmCom->Add_State(CState_Naruto_PunchRight::Create(m_pFsmCom, this, ATTACK2));
	//m_pFsmCom->Add_State(CState_Naruto_ElbowStrike::Create(m_pFsmCom, this, ATTACK3));
	//m_pFsmCom->Add_State(CState_Naruto_DoubleKick::Create(m_pFsmCom, this, ATTACK_STRONG));	// 강 공격

	/* 나선환 */
	m_pFsmCom->Add_State(CState_Naruto_Rasengun_Charge::Create(m_pFsmCom, this, RASENGUN_CHARGE));
	m_pFsmCom->Add_State(CState_Naruto_Rasengun_Run::Create(m_pFsmCom, this, RASENGUN_RUN));
	m_pFsmCom->Add_State(CState_Naruto_Rasengun_Attack::Create(m_pFsmCom, this, RASENGUN_ATTACK));

	/* 공중 나선환 */
	m_pFsmCom->Add_State(CState_Naruto_AerialRasengun_Charge::Create(m_pFsmCom, this, AERIAL_RASENGUN_CHARGE));
	m_pFsmCom->Add_State(CState_Naruto_AerialRasengun_Run::Create(m_pFsmCom, this, AERIAL_RASENGUN_RUN));
	m_pFsmCom->Add_State(CState_Naruto_AerialRasengun_Attack::Create(m_pFsmCom, this, AERIAL_RASENGUN_ATTACK));

	/* 나선 수리검 */
	m_pFsmCom->Add_State(CState_Naruto_RasenShuriken::Create(m_pFsmCom, this, RASENSHURIKEN));
	m_pFsmCom->Add_State(CState_Naruto_AerialRasenShuriken::Create(m_pFsmCom, this, AERIAL_RASENSHURIKEN));


	m_pFsmCom->Set_State(IDLE);

	return S_OK;
}

HRESULT CPlayer_Naruto::Ready_Shadow()
{
	CShadow_Naruto::SHADOW_NARUTO_DESC tDesc = {};
	tDesc.fRotationPerSec = 0.f;
	tDesc.fSpeedPerSec = 0.f;
	tDesc.vPosition = _float3(0.f, 0.f, 0.f);
	tDesc.iLevelIndex = m_iLevelIndex;

	for (size_t i = 0; i < SHA_END; ++i)
	{
		m_pShadow[i] = dynamic_cast<CShadow_Naruto*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_ShadowNaruto"), &tDesc));
		m_pShadow[i]->Set_Owner(this);
	}

	return S_OK;
}

HRESULT CPlayer_Naruto::Ready_Skill()
{
	__super::Ready_Skill();

	CSkill::SKILL_DESC Desc{};
	Desc.iLevelIndex = m_iLevelIndex;
	Desc.fRotationPerSec = 0.f;
	Desc.fSpeedPerSec = 0.f;
	Desc.pOwner = this;

	m_pSkill_RasenShuriken = dynamic_cast<CSkill_RasenShuriken*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Skill_RasenShuriken"), &Desc));
	if (nullptr == m_pSkill_RasenShuriken)
		return E_FAIL;
	m_pGameInstance->Add_Object_ToLayer(m_iLevelIndex, TEXT("Layer_Skill"), m_pSkill_RasenShuriken);

	m_pSkill_Rasengan = dynamic_cast<CSkill_Rasengan*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Skill_Rasengan"), &Desc));
	if (nullptr == m_pSkill_Rasengan)
		return E_FAIL;
	m_pGameInstance->Add_Object_ToLayer(m_iLevelIndex, TEXT("Layer_Skill"), m_pSkill_Rasengan);

	m_pUI_SkillSlot[SLOT_SP]->Change_Skill(CUI_PlayerSkill::SKILL_ICON::SKILL_AMATERAS, &m_fCoolTime_Rasengan);
	m_pUI_SkillSlot[SLOT_BASE1]->Change_Skill(CUI_PlayerSkill::SKILL_ICON::SKILL_RASENRAN, &m_fCoolTime_Rasengan);
	m_pUI_SkillSlot[SLOT_BASE2]->Change_Skill(CUI_PlayerSkill::SKILL_ICON::SKILL_RASENSHURIKEN, &m_fCoolTime_RasenShuriken);

	return S_OK;
}

CPlayer_Naruto* CPlayer_Naruto::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer_Naruto* pInstance = new CPlayer_Naruto(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer_Naruto"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CPlayer_Naruto::Clone(void* pArg)
{
	CPlayer_Naruto* pInstance = new CPlayer_Naruto(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CPlayer_Naruto"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Naruto::Free()
{
	__super::Free();

	for (size_t i = 0; i < 2; ++i)
	{
		Safe_Release(m_pShadow[i]);
	}

	Safe_Release(m_pModelCom);
}
