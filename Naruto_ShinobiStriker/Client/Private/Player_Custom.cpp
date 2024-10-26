#include "stdafx.h"
#include "Player_Custom.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "Navigation.h"

#include "Head_Player.h"
#include "Face_Player.h"
#include "BodyOnce_Player.h"
#include "BodyUpper_Player.h"
#include "BodyLower_Player.h"

#include "Shadow_Custom.h"

#include "State_Player_Dash.h"
#include "State_Custom_Idle.h"
#include "State_Custom_Run.h"
#include "State_Custom_RunEnd.h"
#include "State_Custom_Jump.h"
#include "State_Custom_DoubleJump.h"
#include "State_Custom_FallFront.h"
#include "State_Custom_Land.h"
#include "State_Custom_ChakraJump.h"
#include "State_Custom_WallAttach.h"
#include "State_Custom_HIt1.h"
#include "State_Custom_HIt2.h"
#include "State_Custom_HItSpin.h"
#include "State_Custom_WireReady.h"
#include "State_Custom_WireDash.h"

#include "State_Custom_PunchLeft.h"
#include "State_Custom_PunchRight.h"
#include "State_Custom_DoubleSlash.h"
#include "State_Custom_GroundPunch.h"
#include "State_Custom_Slash.h"
#include "State_Custom_RisingKick.h"
#include "State_Custom_DoubleHandPunch.h"

#include "State_Custom_BackBlow.h"
#include "State_Custom_TurnKick.h"
#include "State_Custom_WindMill.h"

#include "State_Custom_Chidori_Charge.h"
#include "State_Custom_Chidori_Run.h"
#include "State_Custom_Chidori_Attack.h"
#include "State_Custom_AerialChidori_Charge.h"
#include "State_Custom_AerialChidori_Run.h"
#include "State_Custom_AerialChidori_Attack.h"
#include "State_Custom_FireBall.h"
#include "State_Custom_AerialFireBall.h"
#include "State_Custom_Kirin.h"

#include "State_Custom_Rasengun_Charge.h"
#include "State_Custom_Rasengun_Run.h"
#include "State_Custom_Rasengun_Attack.h"
#include "State_Custom_AerialRasengun_Charge.h"
#include "State_Custom_AerialRasengun_Run.h"
#include "State_Custom_AerialRasengun_Attack.h"
#include "State_Custom_RasenShuriken.h"
#include "State_Custom_AerialRasenShuriken.h"
#include "State_Custom_MassiveRasengan.h"

#include "State_Custom_Amateras.h"
#include "State_Custom_Tsukuyomi.h"
#include "State_Custom_Susanoo.h"

#include "State_Custom_TypeZeroAttack.h"
#include "State_Custom_Raijin.h"
#include "State_Custom_Rasenko.h"

#include "UI_PlayerHP.h"
#include "UI_PlayerSkill.h"

#include "Skill_ChangeWood.h"
#include "Skill_Chidori.h"
#include "Skill_FireBall.h"
#include "Skill_Kirin.h"
#include "Skill_Rasengan.h"
#include "Skill_RasenShuriken.h"
#include "Skill_MassiveRasengan.h"
#include "Skill_Amateras.h"
#include "Skill_Tsukuyomi.h"
#include "Skill_Susanoo.h"

#include "Effect_TypeZero.h"
#include "Effect_RasenkoStar.h"

#include "Player_MotionTrail.h"

CPlayer_Custom::CPlayer_Custom(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayer{ pDevice, pContext }
{
}

CPlayer_Custom::CPlayer_Custom(const CPlayer_Custom& Prototype)
	: CPlayer{ Prototype }
{
}

HRESULT CPlayer_Custom::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer_Custom::Initialize(void* pArg)
{
	CUSTOM_DESC* pDesc = static_cast<CUSTOM_DESC*>(pArg);

	/* 직교투영을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	if (FAILED(__super::Ready_ETC()))
		return E_FAIL;

	if (FAILED(Ready_State()))
		return E_FAIL;


	if (m_iLevelIndex != LEVEL_CUSTOM)
	{
		if (FAILED(Ready_Skill()))
			return E_FAIL;

		if (FAILED(Ready_Shadow()))
			return E_FAIL;

		m_pGameInstance->SetUp_Player(this);
	}

	m_pTransformCom->Rotation(pDesc->vRotation.x, pDesc->vRotation.y, pDesc->vRotation.z);

	return S_OK;
}

void CPlayer_Custom::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

	if (nullptr != m_pNearTarget)
	{
		if (m_pNearTarget->Get_Dead())
		{
			m_pNearTarget = nullptr;
		}
	}

	if (KEY_TAP(KEY::C))
	{
		SKILL_TYPE eType = (SKILL_TYPE)(((_int)m_eSkillType + 1) % SKILL_END);
		Change_SkillType(eType);
	}

	Parts_Priority_Update(fTimeDelta);

	for (size_t i = 0; i < 2; ++i)
	{
		if(nullptr != m_pShadow[i])
			m_pShadow[i]->Priority_Update(fTimeDelta);
	}
}

void CPlayer_Custom::Update(_float fTimeDelta)
{
	Parts_Update(fTimeDelta);

	if (KEY_TAP(KEY::P))
	{
		if (Change_Wood())
		{
			m_pSoundCom->Play3D(TEXT("Voice_Here.wav"), g_fVolume + 0.2f );
			m_pFsmCom->Change_State(FALL_FRONT);
		}
	}

	if (KEY_TAP(KEY::RIGHT))
	{
		//m_pFsmCom->Change_State(SUSANOO);
	}

	__super::Update(fTimeDelta);

	for (size_t i = 0; i < 2; ++i)
	{
		if (nullptr != m_pShadow[i])
			m_pShadow[i]->Update(fTimeDelta);
	}
	_vector vRootBonePos = m_pModelCom->Play_Animation(fTimeDelta * m_fAnimationSpeed, 0);

	XMStoreFloat3(&m_vRootBonePos, vRootBonePos);
}

void CPlayer_Custom::Late_Update(_float fTimeDelta)
{
	_vector vRootBonePos = m_pModelCom->Get_BoneCombindTransformationMatrix(3).r[3];

	_vector vHitPos, vNormal;
	_float fHitDistance;

	if (m_iLevelIndex != LEVEL_CUSTOM)
		m_pGameInstance->RayCast_PlayerDown(&vHitPos, &vNormal, &fHitDistance);

	__super::Late_Update(fTimeDelta);

	Parts_Late_Update(fTimeDelta);

	for (size_t i = 0; i < 2; ++i)
	{
		if (nullptr != m_pShadow[i])
			m_pShadow[i]->Late_Update(fTimeDelta);
	}
}

HRESULT CPlayer_Custom::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CPlayer_Custom::OnCollisionEnter(CGameObject* pOther)
{
	if (pOther->Get_LayerTag() == TEXT("Layer_Monster"))
	{
		
	}

	if (pOther->Get_LayerTag() == TEXT("Layer_MonsterAttack"))
	{
		if (m_isInvincible)
			return;

		//m_pFsmCom->Change_State(HIT1);
	}
	
}

void CPlayer_Custom::OnCollisionStay(CGameObject* pOther)
{

}

void CPlayer_Custom::OnCollisionExit(CGameObject* pOther)
{
}

void CPlayer_Custom::Change_IsRender(_bool isRender)
{
	m_isRender = isRender;

	for (auto& Part : m_Parts)
	{
		if (Part == nullptr)
			continue;
		Part->Set_IsRender(m_isRender);
	}
}

void CPlayer_Custom::On_MotionTrail()
{
	if (m_isRenderMotionTrail)
		return;

	for (size_t i = 0; i < PART_END; ++i)
	{
		if (m_Parts[i] == nullptr)
			continue;

		m_Parts[i]->On_MotionTrail();
	}
}

void CPlayer_Custom::Off_Motiontrail()
{
	for (size_t i = 0; i < PART_END; ++i)
	{
		if (m_Parts[i] == nullptr)
			continue;

		m_Parts[i]->Off_Motiontrail();
	}
}

void CPlayer_Custom::Create_Shadow(CShadow_Custom::SHADOW_DESC tDesc)
{
	if (m_pShadow[SHA_LEFT]->IsActive() ||
		m_pShadow[SHA_RIGHT]->IsActive())
		return;

	tDesc.eDir = CShadow_Custom::LEFT;
	m_pShadow[SHA_LEFT]->Create_Shadow(tDesc);

	tDesc.eDir = CShadow_Custom::RIGHT;
	m_pShadow[SHA_RIGHT]->Create_Shadow(tDesc);
}

void CPlayer_Custom::Change_Face(_uint iID)
{
	m_tCustomInfo.iFaceID = iID;

	dynamic_cast<CFace_Player*>(m_Parts[PART_FACE])->Change_Parts(CFace_Player::FACE_ID(m_tCustomInfo.iFaceID), m_pModelCom);

	m_Parts[PART_FACE]->Get_Model()->Set_Bones(m_pModelCom->Get_Bones());
	m_Parts[PART_FACE]->Get_Model()->Set_Animations(m_pModelCom->Get_Animations(), m_pModelCom->Get_KeyFrameIndicies());

	Save_CustomInfo();
}

void CPlayer_Custom::Change_Upper(_uint iID)
{
	if (m_tCustomInfo.isOnceClothes)
	{
		m_tCustomInfo.isOnceClothes = false;

		dynamic_cast<CBodyLower_Player*>(m_Parts[PART_BODYLOWER])->Change_Parts(CBodyLower_Player::LOWER_ID(m_tCustomInfo.iLowerID), m_pModelCom);
	}

	m_tCustomInfo.iUpperID = iID;

	dynamic_cast<CBodyUpper_Player*>(m_Parts[PART_BODYUPPER])->Change_Parts(CBodyUpper_Player::UPPER_ID(m_tCustomInfo.iUpperID), m_pModelCom);

	for (size_t i = 0; i < PART_END; ++i)
	{
		if (m_Parts[i] == nullptr || i == PART_BODYONCE)
			continue;

		m_Parts[i]->Get_Model()->Set_Bones(m_pModelCom->Get_Bones());
		m_Parts[i]->Get_Model()->Set_Animations(m_pModelCom->Get_Animations(), m_pModelCom->Get_KeyFrameIndicies());
	}

	Save_CustomInfo();
}

void CPlayer_Custom::Change_Lower(_uint iID)
{
	if (m_tCustomInfo.isOnceClothes) 
	{
		m_tCustomInfo.isOnceClothes = false;

		dynamic_cast<CBodyUpper_Player*>(m_Parts[PART_BODYUPPER])->Change_Parts(CBodyUpper_Player::UPPER_ID(m_tCustomInfo.iUpperID), m_pModelCom);
	}

	m_tCustomInfo.iLowerID = iID;

	dynamic_cast<CBodyLower_Player*>(m_Parts[PART_BODYLOWER])->Change_Parts(CBodyLower_Player::LOWER_ID(m_tCustomInfo.iLowerID), m_pModelCom);

	for (size_t i = 0; i < PART_END; ++i)
	{
		if (m_Parts[i] == nullptr || i == PART_BODYONCE)
			continue;

		m_Parts[i]->Get_Model()->Set_Bones(m_pModelCom->Get_Bones());
		m_Parts[i]->Get_Model()->Set_Animations(m_pModelCom->Get_Animations(), m_pModelCom->Get_KeyFrameIndicies());
	}

	Save_CustomInfo();
}

void CPlayer_Custom::Change_Once(_uint iID)
{
	m_tCustomInfo.isOnceClothes = true;
	m_tCustomInfo.iOnceID = iID;

	dynamic_cast<CBodyOnce_Player*>(m_Parts[PART_BODYONCE])->Change_Parts((m_tCustomInfo.iOnceID));

	Safe_Release(m_pModelCom);
	m_pModelCom = m_Parts[PART_BODYONCE]->Get_Model();
	Safe_AddRef(m_pModelCom);

	for (size_t i = 0; i < PART_END; ++i)
	{
		if (m_Parts[i] == nullptr || i == PART_BODYONCE)
			continue;

		m_Parts[i]->Get_Model()->Set_Bones(m_pModelCom->Get_Bones());
		m_Parts[i]->Get_Model()->Set_Animations(m_pModelCom->Get_Animations(), m_pModelCom->Get_KeyFrameIndicies());
	}

	Save_CustomInfo();
}

void CPlayer_Custom::Change_Head(_uint iID)
{
	m_tCustomInfo.iHeadID = iID;

	dynamic_cast<CHead_Player*>(m_Parts[PART_HEAD])->Change_Parts(CHead_Player::HEAD_ID(m_tCustomInfo.iHeadID), m_pModelCom);

	m_Parts[PART_HEAD]->Get_Model()->Set_Bones(m_pModelCom->Get_Bones());
	m_Parts[PART_HEAD]->Get_Model()->Set_Animations(m_pModelCom->Get_Animations(), m_pModelCom->Get_KeyFrameIndicies());
}

_float CPlayer_Custom::Get_SkillCollTime1()
{
	if (m_fCollTime_Skill1 <= 0.f)
	{
		m_fCollTime_Skill1 = 10.f;
		return 0.f;
	}

	return m_fCollTime_Skill1;
}

_float CPlayer_Custom::Get_SkillCollTime2()
{
	if (m_fCollTime_Skill2 <= 0.f)
	{
		m_fCollTime_Skill2 = 10.f;
		return 0.f;
	}

	return m_fCollTime_Skill2;
}

_float CPlayer_Custom::Get_SkillCollTimeSP()
{
	if (m_fCollTime_SkillSP <= 0.f)
	{
		m_fCollTime_SkillSP = 10.f;
		return 0.f;
	}

	return m_fCollTime_SkillSP;
}

void CPlayer_Custom::Clear_SkillCollTime1()
{
	m_fCollTime_Skill1 = 0.f;
}

void CPlayer_Custom::Clear_SkillCollTime2()
{
	m_fCollTime_Skill2 = 0.f;
}

void CPlayer_Custom::Clear_SkillCollTimeSP()
{
	m_fCollTime_SkillSP = 0.f;
}

void CPlayer_Custom::Change_Eye(_uint iID)
{
	m_tCustomInfo.iEyeID = iID;

	dynamic_cast<CFace_Player*>(m_Parts[PART_FACE])->Change_Eye(CFace_Player::EYE_ID(m_tCustomInfo.iEyeID));

}

void CPlayer_Custom::Create_Chidori()
{
	if (!m_pSkill_Chidori->IsActive())
		m_pSkill_Chidori->Appear();
}

void CPlayer_Custom::Run_Chidori()
{
	m_pSkill_Chidori->Run();
}

void CPlayer_Custom::Explosion_Chidori()
{
	m_pSkill_Chidori->Explosion();
}

void CPlayer_Custom::Create_FireBall()
{
	m_pSkill_FireBall->Appear();
}

void CPlayer_Custom::Create_Kirin(_fvector vKirinPos)
{
	m_pSkill_Kirin->Appear(vKirinPos);
}

void CPlayer_Custom::Create_Amateras()
{
	m_pSkill_Amateras->Appear(m_pNearTarget);
}

void CPlayer_Custom::Create_Tsukuyomi()
{
	m_pSkill_Tsukuyomi->Appear(m_pNearTarget);
}

void CPlayer_Custom::Create_Rasengan()
{
	if (!m_pSkill_Rasengan->IsActive())
		m_pSkill_Rasengan->Appear();
}

void CPlayer_Custom::Run_Rasengan()
{
	m_pSkill_Rasengan->Run();
}

void CPlayer_Custom::Explosion_Rasengan()
{
	m_pSkill_Rasengan->Explosion();
}

void CPlayer_Custom::Disappear_Rasengan()
{
	m_pSkill_Rasengan->DisAppear();
}

void CPlayer_Custom::Create_RasenShuriken()
{
	if (!m_pSkill_RasenShuriken->IsActive())
		m_pSkill_RasenShuriken->Appear();
}

void CPlayer_Custom::Shoot_RasenShuriken()
{
	Find_NearTarget();
	m_pSkill_RasenShuriken->Shoot();
}

void CPlayer_Custom::Create_MassiveRasengan()
{
	m_pSkill_MassiveRasengan->Appear();
}

void CPlayer_Custom::Shoot_MassiveRasengan()
{
	m_pSkill_MassiveRasengan->Shoot();
}

void CPlayer_Custom::Create_EffectTypeZero(_uint iEffectNum, _float4x4 vWorldMatrix)
{
	if (iEffectNum > 9 || m_pEffectTypeZero[iEffectNum]->IsActive())
		return;

	m_pEffectTypeZero[iEffectNum]->Appear(XMLoadFloat4x4(&vWorldMatrix));
}

void CPlayer_Custom::Create_RasnkoStar()
{
	m_pEffectRasenkoStar->Appear(this);
}

void CPlayer_Custom::Disappear_RasnkoStar()
{
	m_pEffectRasenkoStar->DisAppear();
}

void CPlayer_Custom::Parts_Priority_Update(_float fTimeDelta)
{
	for (size_t i = 0; i < PART_END; ++i)
	{
		if (m_Parts[i] == nullptr)
			continue;

		if (m_tCustomInfo.isOnceClothes && (i == PART_BODYUPPER || i == PART_BODYLOWER))
			continue;
		else if (!m_tCustomInfo.isOnceClothes && i == PART_BODYONCE)
			continue;

		m_Parts[i]->Priority_Update(fTimeDelta);
	}

	Save_CustomInfo();
}

void CPlayer_Custom::Parts_Update(_float fTimeDelta)
{
	for (size_t i = 0; i < PART_END; ++i)
	{
		if (m_Parts[i] == nullptr)
			continue;

		if (m_tCustomInfo.isOnceClothes && (i == PART_BODYUPPER || i == PART_BODYLOWER))
			continue;
		else if (!m_tCustomInfo.isOnceClothes && i == PART_BODYONCE)
			continue;

		m_Parts[i]->Update(fTimeDelta);
	}
}

void CPlayer_Custom::Parts_Late_Update(_float fTimeDelta)
{
	for (size_t i = 0; i < PART_END; ++i)
	{
		if (m_Parts[i] == nullptr)
			continue;

		if (m_tCustomInfo.isOnceClothes && (i == PART_BODYUPPER || i == PART_BODYLOWER))
			continue;
		else if (!m_tCustomInfo.isOnceClothes && i == PART_BODYONCE)
			continue;

		m_Parts[i]->Late_Update(fTimeDelta);
	}
}

HRESULT CPlayer_Custom::Ready_Components()
{
	__super::Ready_Components();

	if (m_iLevelIndex != LEVEL_CUSTOM)
	{
		/* For.Com_Navigation */
		CNavigation::NAVIGATION_DESC			NaviDesc{};

		NaviDesc.iCurrentIndex = 0;

		if (FAILED(__super::Add_Component(m_iLevelIndex, TEXT("Prototype_Component_Navigation"),
			TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CPlayer_Custom::Ready_State()
{
	if (m_iLevelIndex != LEVEL_CUSTOM)
	{
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Fsm"),
			TEXT("Com_Fsm"), reinterpret_cast<CComponent**>(&m_pFsmCom), this)))
			return E_FAIL;

		m_pFsmCom->Add_State(CState_Player_Dash::Create(m_pFsmCom, this, DASH));

		m_pFsmCom->Add_State(CState_Custom_Idle::Create(m_pFsmCom, this, IDLE));
		m_pFsmCom->Add_State(CState_Custom_Run::Create(m_pFsmCom, this, RUN));
		m_pFsmCom->Add_State(CState_Custom_RunEnd::Create(m_pFsmCom, this, RUN_END));
		m_pFsmCom->Add_State(CState_Custom_Jump::Create(m_pFsmCom, this, JUMP));
		m_pFsmCom->Add_State(CState_Custom_DoubleJump::Create(m_pFsmCom, this, DOUBLEJUMP));
		m_pFsmCom->Add_State(CState_Custom_FallFront::Create(m_pFsmCom, this, FALL_FRONT));
		m_pFsmCom->Add_State(CState_Custom_ChakraJump::Create(m_pFsmCom, this, CHAKRAJUMP));
		m_pFsmCom->Add_State(CState_Custom_Land::Create(m_pFsmCom, this, LAND));
		m_pFsmCom->Add_State(CState_Custom_WallAttach::Create(m_pFsmCom, this, WALLATTACH));

		m_pFsmCom->Add_State(CState_Custom_Hit1::Create(m_pFsmCom, this, HIT1));
		m_pFsmCom->Add_State(CState_Custom_Hit2::Create(m_pFsmCom, this, HIT2));
		m_pFsmCom->Add_State(CState_Custom_HitSpin::Create(m_pFsmCom, this, HIT_SPIN));

		m_pFsmCom->Add_State(CState_Custom_PunchRight::Create(m_pFsmCom, this, PUNCH_RIGHT));
		m_pFsmCom->Add_State(CState_Custom_PunchLeft::Create(m_pFsmCom, this, PUNCH_LEFT));
		m_pFsmCom->Add_State(CState_Custom_DoubleSlash::Create(m_pFsmCom, this, DOUBLESLASH));
		m_pFsmCom->Add_State(CState_Custom_GroundPunch::Create(m_pFsmCom, this, GROUNDPUNCH));
		m_pFsmCom->Add_State(CState_Custom_Slash::Create(m_pFsmCom, this, SLASH));
		m_pFsmCom->Add_State(CState_Custom_RisingKick::Create(m_pFsmCom, this, RISINGKICK));
		m_pFsmCom->Add_State(CState_Custom_DoubleHandPunch::Create(m_pFsmCom, this, HANDDOUBLEPUNCH));

		//m_pFsmCom->Add_State(CState_Custom_BackBlow::Create(m_pFsmCom, this, ATTACK2));
		//m_pFsmCom->Add_State(CState_Custom_TurnKick::Create(m_pFsmCom, this, ATTACK3));
		//m_pFsmCom->Add_State(CState_Custom_WindMill::Create(m_pFsmCom, this, ATTACK_STRONG));

		m_pFsmCom->Add_State(CState_Custom_WireReady::Create(m_pFsmCom, this, WIRE_READY));
		m_pFsmCom->Add_State(CState_Custom_WireDash::Create(m_pFsmCom, this, WIRE_DASH));

		m_pFsmCom->Add_State(CState_Custom_Chidori_Charge::Create(m_pFsmCom, this, CHIDORI_CHARGE));
		m_pFsmCom->Add_State(CState_Custom_Chidori_Run::Create(m_pFsmCom, this, CHIDORI_RUN));
		m_pFsmCom->Add_State(CState_Custom_Chidori_Attack::Create(m_pFsmCom, this, CHIDORI_ATTACK));
		m_pFsmCom->Add_State(CState_Custom_AerialChidori_Charge::Create(m_pFsmCom, this, AERIAL_CHIDORI_CHARGE));
		m_pFsmCom->Add_State(CState_Custom_AerialChidori_Run::Create(m_pFsmCom, this, AERIAL_CHIDORI_RUN));
		m_pFsmCom->Add_State(CState_Custom_AerialChidori_Attack::Create(m_pFsmCom, this, AERIAL_CHIDORI_ATTACK));
		m_pFsmCom->Add_State(CState_Custom_FireBall::Create(m_pFsmCom, this, FIREBALL));
		m_pFsmCom->Add_State(CState_Custom_AerialFireBall::Create(m_pFsmCom, this, AERIAL_FIREBALL));
		m_pFsmCom->Add_State(CState_Custom_Kirin::Create(m_pFsmCom, this, KIRIN));

		m_pFsmCom->Add_State(CState_Custom_Rasengun_Charge::Create(m_pFsmCom, this, RASENGUN_CHARGE));
		m_pFsmCom->Add_State(CState_Custom_Rasengun_Run::Create(m_pFsmCom, this, RASENGUN_RUN));
		m_pFsmCom->Add_State(CState_Custom_Rasengun_Attack::Create(m_pFsmCom, this, RASENGUN_ATTACK));
		m_pFsmCom->Add_State(CState_Custom_AerialRasengun_Charge::Create(m_pFsmCom, this, AERIAL_RASENGUN_CHARGE));
		m_pFsmCom->Add_State(CState_Custom_AerialRasengun_Run::Create(m_pFsmCom, this, AERIAL_RASENGUN_RUN));
		m_pFsmCom->Add_State(CState_Custom_AerialRasengun_Attack::Create(m_pFsmCom, this, AERIAL_RASENGUN_ATTACK));
		m_pFsmCom->Add_State(CState_Custom_RasenShuriken::Create(m_pFsmCom, this, RASENSHURIKEN));
		m_pFsmCom->Add_State(CState_Custom_AerialRasenShuriken::Create(m_pFsmCom, this, AERIAL_RASENSHURIKEN));
		m_pFsmCom->Add_State(CState_Custom_MassiveRasengan::Create(m_pFsmCom, this, MASSIVERASENGAN));

		m_pFsmCom->Add_State(CState_Custom_Amateras::Create(m_pFsmCom, this, AMATERAS));
		m_pFsmCom->Add_State(CState_Custom_Tsukuyomi::Create(m_pFsmCom, this, TSUKUYOMI));
		m_pFsmCom->Add_State(CState_Custom_Susanoo::Create(m_pFsmCom, this, SUSANOO));

		m_pFsmCom->Add_State(CState_Custom_TypeZeroAttack::Create(m_pFsmCom, this, TYPEZERO));
		m_pFsmCom->Add_State(CState_Custom_Raijin::Create(m_pFsmCom, this, RAIJIN));
		m_pFsmCom->Add_State(CState_Custom_Rasenko::Create(m_pFsmCom, this, RASENKO));
		

		m_pFsmCom->Set_State(IDLE);
	}
	else
		m_pModelCom->SetUp_Animation(m_pModelCom->Get_AnimationIndex("CustomMan_CharaSelect_Idle"), true);

	return S_OK;
}

HRESULT CPlayer_Custom::Ready_PartObjects()
{
	m_Parts.resize(PART_END);

	if (m_iLevelIndex == LEVEL_CUSTOM)
	{
		ZeroMemory(&m_tCustomInfo, sizeof(CUSTOM_INFO));
		m_tCustomInfo.isOnceClothes = true;
	}
	else
	{
		Load_CustomInfo();
	}

	CPartObject::PARTOBJ_DESC PartDesc{};
	PartDesc.iLevelIndex = m_iLevelIndex;
	PartDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();

	PartDesc.iPartID = m_tCustomInfo.iOnceID;
	if (FAILED(__super::Add_PartObject(PART_BODYONCE, TEXT("Prototype_GameObject_BodyOnce_Player"), &PartDesc)))
		return E_FAIL;
	m_pModelCom = m_Parts[PART_BODYONCE]->Get_Model();
	Safe_AddRef(m_pModelCom);

	PartDesc.iPartID = m_tCustomInfo.iFaceID;
	if (FAILED(__super::Add_PartObject(PART_FACE, TEXT("Prototype_GameObject_Face_Player"), &PartDesc)))
		return E_FAIL;
	dynamic_cast<CFace_Player*>(m_Parts[PART_FACE])->Change_Parts(CFace_Player::FACE_ID(m_tCustomInfo.iFaceID), m_pModelCom);

	PartDesc.iPartID = m_tCustomInfo.iHeadID;
	if (FAILED(__super::Add_PartObject(PART_HEAD, TEXT("Prototype_GameObject_Head_Player"), &PartDesc)))
		return E_FAIL;
	dynamic_cast<CHead_Player*>(m_Parts[PART_HEAD])->Change_Parts(CHead_Player::HEAD_ID(m_tCustomInfo.iHeadID), m_pModelCom);

	if (m_iLevelIndex == LEVEL_CUSTOM || (m_iLevelIndex != LEVEL_CUSTOM && false == m_tCustomInfo.isOnceClothes))
	{
		PartDesc.iPartID = m_tCustomInfo.iUpperID;
		if (FAILED(__super::Add_PartObject(PART_BODYUPPER, TEXT("Prototype_GameObject_BodyUpper_Player"), &PartDesc)))
			return E_FAIL;
		dynamic_cast<CBodyUpper_Player*>(m_Parts[PART_BODYUPPER])->Change_Parts(CBodyUpper_Player::UPPER_ID(m_tCustomInfo.iUpperID), m_pModelCom);

		PartDesc.iPartID = m_tCustomInfo.iLowerID;
		if (FAILED(__super::Add_PartObject(PART_BODYLOWER, TEXT("Prototype_GameObject_BodyLower_Player"), &PartDesc)))
			return E_FAIL;
		dynamic_cast<CBodyLower_Player*>(m_Parts[PART_BODYLOWER])->Change_Parts(CBodyLower_Player::LOWER_ID(m_tCustomInfo.iLowerID), m_pModelCom);
	}

	
	for (size_t i = 0; i < PART_END; ++i)
	{
		if (m_Parts[i] == nullptr || i == PART_BODYONCE)
			continue;

		m_Parts[i]->Get_Model()->Set_Bones(m_pModelCom->Get_Bones());
		m_Parts[i]->Get_Model()->Set_Animations(m_pModelCom->Get_Animations(), m_pModelCom->Get_KeyFrameIndicies());
	}

	return S_OK;
}

HRESULT CPlayer_Custom::Ready_Skill()
{
	if (m_iLevelIndex == LEVEL_CUSTOM)
		return S_OK;

	__super::Ready_Skill();

	CSkill::SKILL_DESC Desc{};
	Desc.iLevelIndex = m_iLevelIndex;
	Desc.fRotationPerSec = 0.f;
	Desc.fSpeedPerSec = 0.f;
	Desc.pOwner = this;

#pragma region 사스케스킬
	m_pSkill_Chidori = dynamic_cast<CSkill_Chidori*>(m_pGameInstance->Get_CloneObject_AddLayer(m_iLevelIndex, TEXT("Layer_Skill"), TEXT("Prototype_GameObject_Skill_Chidori"), &Desc));
	if (nullptr == m_pSkill_Chidori)
		return E_FAIL;

	m_pSkill_FireBall = dynamic_cast<CSkill_FireBall*>(m_pGameInstance->Get_CloneObject_AddLayer(m_iLevelIndex, TEXT("Layer_Skill"), TEXT("Prototype_GameObject_Skill_FireBall"), &Desc));
	if (nullptr == m_pSkill_FireBall)
		return E_FAIL;

	m_pSkill_Kirin = dynamic_cast<CSkill_Kirin*>(m_pGameInstance->Get_CloneObject_AddLayer(m_iLevelIndex, TEXT("Layer_Skill"), TEXT("Prototype_GameObject_Skill_Kirin"), &Desc));
	if (nullptr == m_pSkill_Kirin)
		return E_FAIL;
#pragma endregion 

#pragma region 나루토스킬
	m_pSkill_RasenShuriken = dynamic_cast<CSkill_RasenShuriken*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Skill_RasenShuriken"), &Desc));
	if (nullptr == m_pSkill_RasenShuriken)
		return E_FAIL;
	m_pGameInstance->Add_Object_ToLayer(m_iLevelIndex, TEXT("Layer_Skill"), m_pSkill_RasenShuriken);

	m_pSkill_Rasengan = dynamic_cast<CSkill_Rasengan*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Skill_Rasengan"), &Desc));
	if (nullptr == m_pSkill_Rasengan)
		return E_FAIL;
	m_pGameInstance->Add_Object_ToLayer(m_iLevelIndex, TEXT("Layer_Skill"), m_pSkill_Rasengan);

	m_pSkill_MassiveRasengan = dynamic_cast<CSkill_MassiveRasengan*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Skill_MassiveRasengan"), &Desc));
	if (nullptr == m_pSkill_MassiveRasengan)
		return E_FAIL;
	m_pGameInstance->Add_Object_ToLayer(m_iLevelIndex, TEXT("Layer_Skill"), m_pSkill_MassiveRasengan);
#pragma endregion 

#pragma region 사륜안스킬
	m_pSkill_Amateras = dynamic_cast<CSkill_Amateras*>(m_pGameInstance->Get_CloneObject_AddLayer(m_iLevelIndex, TEXT("Layer_Skill"), TEXT("Prototype_GameObject_Skill_Amateras"), &Desc));
	if (nullptr == m_pSkill_Amateras)
		return E_FAIL;

	m_pSkill_Tsukuyomi = dynamic_cast<CSkill_Tsukuyomi*>(m_pGameInstance->Get_CloneObject_AddLayer(m_iLevelIndex, TEXT("Layer_Skill"), TEXT("Prototype_GameObject_Skill_Tsukuyomi"), &Desc));
	if (nullptr == m_pSkill_Tsukuyomi)
		return E_FAIL;

	m_pSkill_Susanoo = dynamic_cast<CSkill_Susanoo*>(m_pGameInstance->Get_CloneObject_AddLayer(m_iLevelIndex, TEXT("Layer_Skill"), TEXT("Prototype_GameObject_SkillSusanoo"), &Desc));
	if (nullptr == m_pSkill_Susanoo)
		return E_FAIL;
	m_pSkill_Susanoo->Set_Owner(this);
#pragma endregion
	
	CEffect_TypeZero::TYPEZERO_DESC TypeZeroDesc;
	TypeZeroDesc.fRotationPerSec = 0.f;
	TypeZeroDesc.fSpeedPerSec = 0.;
	TypeZeroDesc.iLevelIndex = LEVEL_CHUNIN;

	for (_uint i = 0; i < 10; ++i)
	{
		TypeZeroDesc.eType = CEffect_TypeZero::TYPEZERO(i);
		m_pEffectTypeZero[i] = dynamic_cast<CEffect_TypeZero*>(m_pGameInstance->Get_CloneObject_AddLayer(LEVEL_CHUNIN, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_TypeZero"), &TypeZeroDesc));
		if (nullptr == m_pEffectTypeZero[i])
			return E_FAIL;
	}

	m_pEffectRasenkoStar = dynamic_cast<CEffect_RasenkoStar*>(m_pGameInstance->Get_CloneObject_AddLayer(LEVEL_CHUNIN, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_RasenkoStar"), &Desc));
	if (nullptr == m_pEffectRasenkoStar)
		return E_FAIL;

	m_pUI_SkillSlot[SLOT_SP]->Change_Skill(CUI_PlayerSkill::SP_KIRIN, &m_fCollTime_SkillSP);
	m_pUI_SkillSlot[SLOT_BASE1]->Change_Skill(CUI_PlayerSkill::SKILL_ICON::SKILL_CHIDORI, &m_fCollTime_Skill1);
	m_pUI_SkillSlot[SLOT_BASE2]->Change_Skill(CUI_PlayerSkill::SKILL_ICON::SKILL_FIREBALL, &m_fCollTime_Skill2);

	return S_OK;
}

HRESULT CPlayer_Custom::Ready_Shadow()
{
	CShadow_Custom::SHADOW_DESC tDesc = {};
	tDesc.fRotationPerSec = 0.f;
	tDesc.fSpeedPerSec = 0.f;
	tDesc.vPosition = _float3(0.f, 0.f, 0.f);
	tDesc.iLevelIndex = m_iLevelIndex;

	for (size_t i = 0; i < SHA_END; ++i)
	{
		m_pShadow[i] = dynamic_cast<CShadow_Custom*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_ShadowCustom"), &tDesc));
		m_pShadow[i]->Set_Owner(this);
	}

	return S_OK;
}

HRESULT CPlayer_Custom::Save_CustomInfo()
{
	_ulong			dwByte = {};
	HANDLE			hFile = CreateFile(TEXT("../Bin/DataFiles/Custom.dat"), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	WriteFile(hFile, &m_tCustomInfo, sizeof(CPlayer_Custom::CUSTOM_INFO), &dwByte, nullptr);

	for (size_t i = 0; i < PART_END; ++i)
	{
		if (m_Parts[i] == nullptr)
			continue;

		m_Parts[i]->Get_Model()->Set_Animations(m_pModelCom->Get_Animations(), m_pModelCom->Get_KeyFrameIndicies());
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CPlayer_Custom::Load_CustomInfo()
{
	_ulong			dwByte = {};
	HANDLE			hFile = CreateFile(TEXT("../Bin/DataFiles/Custom.dat"), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	ReadFile(hFile, &m_tCustomInfo, sizeof(CPlayer_Custom::CUSTOM_INFO), &dwByte, nullptr);

	CloseHandle(hFile);

	return S_OK;
}

CPlayer_Custom* CPlayer_Custom::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer_Custom* pInstance = new CPlayer_Custom(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer_Custom"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CPlayer_Custom::Clone(void* pArg)
{
	CPlayer_Custom* pInstance = new CPlayer_Custom(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CPlayer_Custom"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Custom::Free()
{
	__super::Free();

	/*if(m_iLevelIndex == LEVEL_CUSTOM)
		Safe_Release(m_pModelCom);*/

	for (_uint i = 0; i < 2; ++i)
	{
		Safe_Release(m_pShadow[i]);
	}

	Safe_Release(m_pNavigationCom);
}
