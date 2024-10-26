#include "stdafx.h"
#include "..\Public\Loader.h"

#include "Sky.h"
#include "Terrain.h"
#include "GameInstance.h"

#include "Background_Title.h"
#include "Title_Logo.h"
#include "UI_TitleButton.h"

#include "Background_Custom.h"
#include "CustomPannel.h"
#include "Custom_Button.h"
#include "Custom_Head.h"
#include "UI_ConfirmButton.h"

#include "UI_PlayerHP.h"
#include "UI_PlayerSP.h"
#include "UI_PlayerSkill.h"
#include "UI_HitNum.h"
#include "UI_MonsterHP.h"
#include "UI_TextWindow.h"
#include "UI_BossHP.h"
#include "UI_Target.h"
#include "UI_BossTarget.h"

#include "NonAnimModel.h"

#include "Player_Naruto.h"
#include "Player_Custom.h"
#include "Shadow_Custom.h"
#include "WhiteZetsu.h"
#include "TenTail.h"
#include "Boss_Kurama.h"
#include "Boss_Susanoo.h"
#include "SusanooBlade.h"

#include "FreeCamera.h"
#include "CustomCamera.h"
#include "PlayerCamera.h"

#include "Face_Player.h"
#include "BodyOnce_Player.h"
#include "BodyUpper_Player.h"
#include "BodyLower_Player.h"
#include "Head_Player.h"

#include "FootTrail.h"
#include "ChidoriTrail.h"
#include "WireTrail.h"

#include "JumpGuide.h"
#include "Effect_GroundBreak.h"
#include "Rasengan_Center.h"
#include "Rasengan_CreateSmoke.h"
#include "Rasengan_RunSmoke.h"

#include "RasenShuriken_Blade.h"
#include "RasenShuriken_Circle.h"
#include "RasenShuriken_BladeFly.h"
#include "RasenShuriken_BladeLine.h"
#include "RasenShuriken_Sphere.h"
#include "ExplosionSphere.h"

#include "MassiveRasengan_Sphere.h"
#include "MassiveRasengan_Circle.h"
#include "RasenganRing.h"

#include "Chidori_Center.h"
#include "Lightning.h"
#include "Amateras.h"
#include "Fire05.h"

#include "FireBall.h"
#include "FireBall_Effect.h"
#include "FireBall_FlySmoke.h"

#include "Effect_TypeZero.h"
#include "Effect_RasenkoStar.h"
#include "Effect_RasenkoHit.h"

#include "Effect_TextureScale.h"
#include "ExplosionCircle.h"
#include "FoxSphere.h"
#include "FoxExplosionSphere.h"
#include "FoxBlast.h"
#include "BladeSlash.h"

#include "Kirin.h"
#include "Wood.h"
#include "AreaCircle.h"
#include "Kunai_Minato.h"
#include "Effect_Tsukuyomi01.h"

#include "Skill_RasenShuriken.h"
#include "Skill_Rasengan.h"
#include "Skill_MassiveRasengan.h"
#include "Skill_ChangeWood.h"
#include "Skill_Chidori.h"
#include "Skill_FireBall.h"
#include "Skill_Kirin.h"
#include "Skill_Amateras.h"
#include "Skill_Tsukuyomi.h"
#include "Skill_Susanoo.h"
#include "Skill_KuramaRasengan.h"

#include "Skill_Roar.h"
#include "Skill_GroundSlash.h"

#include "Particle_Snow.h"
#include "Particle_Hit.h"
#include "Particle_Up.h"
#include "Particle_FireBall.h"
#include "Smoke11.h"
#include "SmokeRing.h"
#include "Sasuke_Next.h"

#include "Effect_Distortion.h"

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}



_uint APIENTRY LoadingMain(void* pArg)
{
	CoInitializeEx(nullptr, 0);

	CLoader*		pLoader = static_cast<CLoader*>(pArg);

	if(FAILED(pLoader->Loading()))
		return 1;
		
	return 0;
}


HRESULT CLoader::Initialize(LEVELID eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingMain, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;


	return S_OK;
}

HRESULT CLoader::Loading()
{
	EnterCriticalSection(&m_CriticalSection);

	HRESULT			hr = {};

	switch (m_eNextLevelID)
	{
	case LEVEL_LOGO:
		hr = Ready_Resources_For_LogoLevel();
		break;

	case LEVEL_CUSTOM:
		hr = Ready_Resources_For_CustomLevel();
		break;

	case LEVEL_AME:
		hr = Ready_Resources_For_AmeLevel();
		break;

	case LEVEL_KONOHA:
		hr = Ready_Resources_For_KonohaLevel();
		break;

	case LEVEL_CHUNIN:
		hr = Ready_Resources_For_ChuninLevel();
	}

	LeaveCriticalSection(&m_CriticalSection);

	return hr;
}

void CLoader::Draw_LoadingText()
{
	SetWindowText(g_hWnd, m_szLoadingText);
}

HRESULT CLoader::Ready_Protytype()
{
	/* For. Prototype_GameObject_Distortion */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Distortion"),
		CEffect_Distortion::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SkillSusanoo"),
		CSkill_Susanoo::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Smoke11 */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Smoke11"),
		CSmoke11::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_SmokeRing */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SmokeRing"),
		CSmokeRing::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_GroundBreak */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GroundBreak"),
		CEffect_GroundBreak::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_SasukeNext */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SasukeNext"),
		CSasuke_Next::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_TypeZero */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TypeZero"),
		CEffect_TypeZero::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_RasenkoStar */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RasenkoStar"),
		CEffect_RasenkoStar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_RasenkoHIt */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RasenkoHIt"),
		CEffect_RasenkoHit::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_KunaiMinato */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_KunaiMinato"),
		CKuani_Minato::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma region 로고
	/* For. Prototype_GameObject_BackGround_Title */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround_Title"),
		CBackGround_Title::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_TitleLogo */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TitleLogo"),
		CTitle_Logo::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_UI_TitleButton */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_TitleButton"),
		CUI_TitleButton::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

#pragma region 커스텀 요소
	/* For. Prototype_GameObject_Head_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Head_Player"),
		CHead_Player::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Face_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Face_Player"),
		CFace_Player::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_BodyOnce_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BodyOnce_Player"),
		CBodyOnce_Player::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_BodyUpper_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BodyUpper_Player"),
		CBodyUpper_Player::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_BodyLower_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BodyLower_Player"),
		CBodyLower_Player::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

#pragma region 커스텀UI
	/* For. Prototype_GameObject_Head_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_UI_CustomPannel"),
		CCustomPannel::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_UI_CustomButton */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_UI_CustomButton"),
		CCustom_Button::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_UI_CustomHead */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_UI_CustomHead"),
		CCustom_Head::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_UI_ConfirmButton */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_UI_ConfirmButton"),
		CUI_ConfirmButton::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

#pragma region 플레이어UI
	/* For. Prototype_UI_PlayerHP */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_UI_PlayerHP"),
		CUI_PlayerHP::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_UI_PlayerSP */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_UI_PlayerSP"),
		CUI_PlayerSP::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_UI_PlayerSkill */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_UI_PlayerSkill"),
		CUI_PlayerSkill::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_UI_Hit */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_UI_Hit"),
		CUI_HitNum::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

#pragma region 몬스터UI
	/* For. Prototype_UI_MonsterHP */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_UI_MonsterHP"),
		CUI_MonsterHP::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_UI_BossHP */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_UI_BossHP"),
		CUI_BossHP::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_UI_Target */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_UI_Target"),
		CUI_Target::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_UI_BossTarget */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_UI_BossTarget"),
		CUI_BossTarget::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

	/* For. Prototype_UI_TextWindow */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_UI_TextWindow"),
		CUI_TextWindow::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma region 플레이어
	/* For. Prototype_GameObject_PlayerCustom */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PlayerCustom"),
		CPlayer_Custom::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Naruto */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Naruto"),
		CPlayer_Naruto::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_ShadowCustom */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ShadowCustom"),
		CShadow_Custom::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

#pragma region 몬스터
	/* For. Prototype_GameObject_WhitZetsu */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WhitZetsu"),
		CWhiteZetsu::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_TenTail */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TenTail"),
		CTenTail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Kurama */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Kurama"),
		CBoss_Kurama::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Susanoo */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Susanoo"),
		CBoss_Susanoo::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_SusanooBlade */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SusanooBlade"),
		CSusanooBlade::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

#pragma region 카메라
	/* For. Prototype_GameObject_FreeCamera */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FreeCamera"),
		CFreeCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_PlayerCamera */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PlayerCamera"),
		CPlayerCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

#pragma region 이펙트
	/* For. Prototype_GameObject_Tsukuyomi01 */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Tsukuyomi01"),
		CEffect_Tsukuyomi01::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Rasengan_CreateSmoke */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Rasengan_CreateSmoke"),
		CRasengan_CreateSmoke::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Rasengan_RunSmoke */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Rasengan_RunSmoke"),
		CRasengan_RunSmoke::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Rasengan_Center */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Rasengan_Center"),
		CRasengan_Center::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_RasenShuriken_Blade */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RasenShuriken_Blade"),
		CRasenShuriken_Blade::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_RasenShuriken_Circle */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RasenShuriken_Circle"),
		CRasenShuriken_Circle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_RasenShuriken_BladeFly */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RasenShuriken_BladeFly"),
		CRasenShuriken_BladeFly::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_RasenShuriken_BladeLine */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RasenShuriken_BladeLine"),
		CRasenShuriken_BladeLine::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_RasenShuriken_Sphere */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RasenShuriken_Sphere"),
		CRasenShuriken_Sphere::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_ExplosionSphere */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ExplosionSphere"),
		CExplosionSphere::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_MassiveRasengan */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MassiveRasengan_Sphere"),
		CMassiveRasengan_Sphere::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_MassiveRasenganCircle */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MassiveRasenganCircle"),
		CMassiveRasengan_Circle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_RasenganRing */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RasenganRing"),
		CRasenganRing::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Wood */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Wood"),
		CWood::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_ChidoriCenter */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ChidoriCenter"),
		CChidori_Center::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For. Prototype_GameObject_Lightning */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Lightning"),
		CLightning::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_FireBall */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FireBall"),
		CFireBall::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_FireBallEffect */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FireBallEffect"),
		CFireBall_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_FireBallFlySmoke */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FireBallFlySmoke"),
		CFireBall_FlySmoke::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Kirin */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Kirin"),
		CKirin::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_TextureScale */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TextureScale"),
		CEffect_TextureScale::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Amateras */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Amateras"),
		CAmateras::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Fire */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Fire"),
		CFire05::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_ExplosionCircle */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ExplosionCircle"),
		CExplosionCircle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_BladeSlash */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BladeSlash"),
		CBladeSlash::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_HitParticle */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HitParticle"),
		CParticle_Hit::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_UpParticle */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UpParticle"),
		CParticle_Up::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_FoxSphere */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FoxSphere"),
		CFoxSphere::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_FoxExplosionSphere */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FoxExplosionSphere"),
		CFoxExplosionSphere::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_FoxBlast */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FoxBlast"),
		CFoxBlast::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

#pragma region 스킬
	/* For. Prototype_GameObject_Skill_KuramaRasengan */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Skill_KuramaRasengan"),
		CSkill_KuramaRasengan::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Skill_RasenShuriken */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Skill_RasenShuriken"),
		CSkill_RasenShuriken::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Skill_Rasengan */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Skill_Rasengan"),
		CSkill_Rasengan::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Skill_MassiveRasengan */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Skill_MassiveRasengan"),
		CSkill_MassiveRasengan::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Skill_ChangeWood */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Skill_ChangeWood"),
		CSkill_ChangeWood::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Skill_Chidori */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Skill_Chidori"),
		CSkill_Chidori::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Skill_Chidori */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Skill_FireBall"),
		CSkill_FireBall::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Skill_Kirin */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Skill_Kirin"),
		CSkill_Kirin::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Skill_Amateras */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Skill_Amateras"),
		CSkill_Amateras::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Skill_Tsukuyomi */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Skill_Tsukuyomi"),
		CSkill_Tsukuyomi::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Skill_Roar */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Skill_Roar"),
		CSkill_Roar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Skill_GroundSlash */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Skill_GroundSlash"),
		CSkill_GroundSlash::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion

	/* For. Prototype_GameObject_FootTrail */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FootTrail"),
		CFootTrail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_ChidoriTrail */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ChidoriTrail"),
		CChidoriTrail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_WireTrail */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WireTrail"),
		CWireTrail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_JumpGuide */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_JumpGuide"),
		CJumpGuide::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_AreaiCircle */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_AreaiCircle"),
		CAreaCircle::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	/* For. Prototype_GameObject_NonAnim */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_NonAnim"),
		CNonAnimModel::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Particle_FireBall*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_FireBall"),
		CParticle_FireBall::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Particle_Snow */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_Snow"),
		CParticle_Snow::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Ready_Resources_For_LogoLevel()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));

	/* For. Prototype_Component_Texture_BackGround_Title */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_BackGround_Title"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Title/Title_BackGround.png")))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_BackGround_Title */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_TitleLogo"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Title/Title_Logo.png")))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_TitleButton */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_TitleButton"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Title/TitleButton%d.dds"), 2))))
		return E_FAIL;

	if (FAILED(Ready_UITexture()))
		return E_FAIL;

	if (FAILED(Ready_Effect()))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("모델을(를) 로딩중입니다."));
	_matrix		PreTransformMatrix = XMMatrixIdentity();

	/* For. Prototype_Component_Model_SasukeNext*/
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_SasukeNext"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Sasuke_Next/Sasuke_Next.dat", PreTransformMatrix))))
		return E_FAIL;

	// 이건 무조건 로딩 해야함
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Once_Hokage4"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Once/Once_Hokage4.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_AreaCircle*/
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_AreaCircle"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/AreaCircle/AreaCircle.dat", PreTransformMatrix))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("사운드을(를) 로딩중입니다."));
 	m_pGameInstance->LoadSoundFile("BGM");
	m_pGameInstance->LoadSoundFile("Character");
	m_pGameInstance->LoadSoundFile("Skill");
	m_pGameInstance->LoadSoundFile("Voice");
	m_pGameInstance->LoadSoundFile("Effect");

	lstrcpy(m_szLoadingText, TEXT("객체원형을(를) 로딩중입니다."));
	Ready_Protytype();

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Ready_Resources_For_CustomLevel()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));

	/* For. Prototype_Component_Texture_Custom_Background */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CUSTOM, TEXT("Prototype_Component_Texture_Custom_Background"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Custom/Custom_Background.png")))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_CustomPannel */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CUSTOM, TEXT("Prototype_Component_Texture_CustomPannel"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Custom/Custom_Pannel.png")))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Custom_PannelHead */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CUSTOM, TEXT("Prototype_Component_Texture_Custom_PannelHead"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Custom/Custom_PannelHead.png")))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Custom_Button */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CUSTOM, TEXT("Prototype_Component_Texture_Custom_Button"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Custom/Custom_Button%d.png"), 2))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Custom_ConfirmButton */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CUSTOM, TEXT("Prototype_Component_Texture_Custom_ConfirmButton"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Custom/Custom_ConfirmButton%d.png"), 2))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("모델을(를) 로딩중입니다."));

	if (FAILED(Ready_CustomPlayer_CustomLevel()))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("객체원형을(를) 로딩중입니다."));

	/* For. Prototype_GameObject_BackGround_Custom */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround_Custom"),
		CBackGround_Custom::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_CustomCamera */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CustomCamera"),
		CCustomCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;



	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Ready_Resources_For_AmeLevel()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));
	/* For. Prototype_Component_Texture_Terrain*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_AME, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Grass_%d.dds"), 2))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("모델을(를) 로딩중입니다."));
	/* For. Prototype_Component_VIBuffer_Terrain*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_AME, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
		return E_FAIL;

	_matrix		PreTransformMatrix = XMMatrixIdentity();

	/* For. Prototype_Component_Model_WhiteZetsu*/
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_AME, TEXT("Prototype_Component_Model_WhiteZetsu"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Monster/WhiteZetsu/WhiteZetsu.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_TenTail*/
	PreTransformMatrix = XMMatrixScaling(0.0006f, 0.0006f, 0.0006f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_AME, TEXT("Prototype_Component_Model_TenTail"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Monster/TenTail/TenTailsClone.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Kurama*/
	PreTransformMatrix = XMMatrixScaling(0.001f, 0.001f, 0.001f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_AME, TEXT("Prototype_Component_Model_Kurama"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Monster/Kurama/Kurama.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Naruto*/
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_AME, TEXT("Prototype_Component_Model_Naruto"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Naruto/Naruto.dat", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(Ready_Resources_For_AmeVillage()))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("네비게이션을(를) 로딩중입니다."));
	/* For.Prototype_Component_Navigation */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_AME, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/Navigation.dat")))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("셰이더을(를) 로딩중입니다."));


	lstrcpy(m_szLoadingText, TEXT("사운드을(를) 로딩중입니다."));

	m_pGameInstance->LoadSoundFile("Character");

	m_pGameInstance->LoadSoundFile("Skill");

	lstrcpy(m_szLoadingText, TEXT("커스텀 요소을(를) 로딩중입니다."));
	if (FAILED(Ready_CustomPlayer_GamePlayLevel()))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Ready_Resources_For_KonohaLevel()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));
	/* For. Prototype_Component_Texture_Terrain*/
	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Grass_%d.dds"), 2))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Particle */
	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Texture_Particle"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Effect/T_EFF_Sphere_08_M.dds")))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 5))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("모델을(를) 로딩중입니다."));
	/* For. Prototype_Component_VIBuffer_Terrain*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_KONOHA, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
		return E_FAIL;

	_matrix		PreTransformMatrix = XMMatrixIdentity();

	/* For. Prototype_Component_Model_WhiteZetsu*/
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_KONOHA, TEXT("Prototype_Component_Model_WhiteZetsu"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Monster/WhiteZetsu/WhiteZetsu.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_TenTail*/
	PreTransformMatrix = XMMatrixScaling(0.0006f, 0.0006f, 0.0006f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_KONOHA, TEXT("Prototype_Component_Model_TenTail"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Monster/TenTail/TenTailsClone.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Kurama*/
	PreTransformMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_KONOHA, TEXT("Prototype_Component_Model_Kurama"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Monster/Kurama/Kurama.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Susanoo*/
	PreTransformMatrix = XMMatrixScaling(0.015f, 0.015f, 0.015f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_KONOHA, TEXT("Prototype_Component_Model_Susanoo"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Susanoo/Susanoo.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_JumpGuide*/
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_KONOHA, TEXT("Prototype_Component_Model_JumpGuide"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/JumpGuide/JumpGuide.dat", PreTransformMatrix))))
		return E_FAIL;

	Ready_Particle();

	if (FAILED(Ready_Resources_For_KonohaVillage()))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("네비게이션을(를) 로딩중입니다."));
	/* For.Prototype_Component_Navigation */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_KONOHA, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/Navigation.dat")))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("사운드을(를) 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("커스텀 요소을(를) 로딩중입니다."));
	if (FAILED(Ready_CustomPlayer_GamePlayLevel()))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Ready_Resources_For_ChuninLevel()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));
	/* For. Prototype_Component_Texture_Terrain*/
	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Grass_%d.dds"), 2))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 5))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Particle */
	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Texture_Particle"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Effect/T_EFF_Sphere_02_M.dds")))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_TextWindow */
	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Texture_TextWindow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/TextWindow/TextWindow%d.dds"), 2))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_TextIcon */
	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Texture_TextIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/TextWindow/TextIcon%d.dds"), 1))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("모델을(를) 로딩중입니다."));

	_matrix		PreTransformMatrix = XMMatrixIdentity();

	/* For. Prototype_Component_Model_WhiteZetsu*/
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHUNIN, TEXT("Prototype_Component_Model_WhiteZetsu"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Monster/WhiteZetsu/WhiteZetsu.dat", PreTransformMatrix))))
		return E_FAIL;

	///* For. Prototype_Component_Model_TenTail*/
	//PreTransformMatrix = XMMatrixScaling(0.0006f, 0.0006f, 0.0006f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHUNIN, TEXT("Prototype_Component_Model_TenTail"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Monster/TenTail/TenTailsClone.dat", PreTransformMatrix))))
	//	return E_FAIL;

	/* For. Prototype_Component_Model_Kurama*/
	PreTransformMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHUNIN, TEXT("Prototype_Component_Model_Kurama"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Monster/Kurama/Kurama.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Susanoo*/
	PreTransformMatrix = XMMatrixScaling(0.015f, 0.015f, 0.015f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHUNIN, TEXT("Prototype_Component_Model_Susanoo"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Susanoo/Susanoo.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_SusanooBlade*/
	PreTransformMatrix = XMMatrixScaling(0.015f, 0.015f, 0.015f) * XMMatrixRotationZ(XMConvertToRadians(270.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHUNIN, TEXT("Prototype_Component_Model_SusanooBlade"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/SusanooBlade/SusanooBlade.dat", PreTransformMatrix))))
		return E_FAIL;

	///* For. Prototype_Component_Model_Naruto*/
	//PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHUNIN, TEXT("Prototype_Component_Model_Naruto"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Naruto/Naruto.dat", PreTransformMatrix))))
	//	return E_FAIL;

	/* For. Prototype_Component_Model_JumpGuide*/
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHUNIN, TEXT("Prototype_Component_Model_JumpGuide"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/JumpGuide/JumpGuide.dat", PreTransformMatrix))))
		return E_FAIL;

	Ready_Particle();

	if (FAILED(Ready_Resources_For_ChuninExam()))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("네비게이션을(를) 로딩중입니다."));
	/* For.Prototype_Component_Navigation */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHUNIN, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/Navigation.dat")))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("사운드을(를) 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("커스텀 요소을(를) 로딩중입니다."));
	if (FAILED(Ready_CustomPlayer_GamePlayLevel()))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Ready_CustomPlayer_CustomLevel()
{
	_matrix		PreTransformMatrix = XMMatrixIdentity();

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	CPlayer_Custom::CUSTOM_INFO tCustomInfo;
	ZeroMemory(&tCustomInfo, sizeof(CPlayer_Custom::CUSTOM_INFO));

	if (m_eNextLevelID != LEVEL_CUSTOM)
	{
		_ulong			dwByte = {};
		HANDLE			hFile = CreateFile(TEXT("../Bin/DataFiles/Custom.dat"), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (0 == hFile)
			return E_FAIL;

		ReadFile(hFile, &tCustomInfo, sizeof(CPlayer_Custom::CUSTOM_INFO), &dwByte, nullptr);

		CloseHandle(hFile);
	}

#pragma region 머리

	/* For. Prototype_Component_Model_Head_Hair1*/
	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Head_Hair1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Head/Head_Hair1.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Head_Hair2*/
	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Head_Hair2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Head/Head_Hair2.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Head_Cap*/
	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Head_Cap"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Head/Head_Cap.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Head_Frog*/
	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Head_Frog"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Head/Head_Frog.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Head_Helmet*/
	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Head_Helmet"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Head/Head_Helmet.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Head_Hokage*/
	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Head_Hokage"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Head/Head_Hokage.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Head_Hood*/
	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Head_Hood"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Head/Head_Hood.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Head_Pajama*/
	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Head_Pajama"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Head/Head_Pajama.dat", PreTransformMatrix))))
		return E_FAIL;

#pragma endregion

#pragma region 얼굴

	/* For. Prototype_Component_Model_Face_Base*/
	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Face_Base"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Face/Face_Base.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Face_Band*/
	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Face_Band"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Face/Face_Band.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Face_Mask1*/
	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Face_Mask1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Face/Face_Mask.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Face_Mask2*/
	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Face_Mask2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Face/Face_Mask2.dat", PreTransformMatrix))))
		return E_FAIL;
#pragma endregion


#pragma region 한벌옷

	/* For. Prototype_Component_Model_Once_Armor*/
	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Once_Armor"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Once/Once_Armor.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Once_FinalCoat*/
	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Once_FinalCoat"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Once/Once_FinalCoat.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Once_Jacket*/
	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Once_Jacket"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Once/Once_Jacket.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Once_Santa*/
	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Once_Santa"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Once/Once_Santa.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Once_Sennin*/
	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Once_Sennin"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Once/Once_Sennin.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Once_Track*/
	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Once_Track"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Once/Once_Track.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Once_Yuukdo*/
	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Once_Yuukdo"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Once/Once_Yuukdo.dat", PreTransformMatrix))))
		return E_FAIL;

#pragma endregion

#pragma region 상체

	/* For. Prototype_Component_Model_BodyUpper_Trainer1*/
	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_BodyUpper_Trainer1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Upper/Upper_Trainer1.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_BodyUpper_Trainer2*/
	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_BodyUpper_Trainer2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Upper/Upper_Trainer2.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Upper_Anbu*/
	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Upper_Anbu"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Upper/Upper_Anbu.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Upper_Armor*/
	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Upper_Armor"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Upper/Upper_Armor.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Upper_Haori*/
	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Upper_Haori"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Upper/Upper_Haori.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Upper_Hoodie*/
	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Upper_Hoodie"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Upper/Upper_Hoodie.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Upper_Shirt*/
	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Upper_Shirt"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Upper/Upper_Shirt.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Upper_Vest*/
	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Upper_Vest"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Upper/Upper_Vest.dat", PreTransformMatrix))))
		return E_FAIL;

#pragma endregion

#pragma region 하체

	/* For. Prototype_Component_Model_BodyLower_Pants1*/
	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_BodyLower_Pants1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Lower/Lower_Pants1.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_BodyLower_Pants2*/
	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_BodyLower_Pants2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Lower/Lower_Pants2.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Lower_Anbu*/
	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Lower_Anbu"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Lower/Lower_Anbu.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Lower_Armor*/
	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Lower_Armor"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Lower/Lower_Armor.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Lower_PantsCut*/
	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Lower_PantsCut"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Lower/Lower_PantsCut.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Lower_Short*/
	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Lower_Short"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Lower/Lower_Short.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Lower_Skirt*/
	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Lower_Skirt"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Lower/Lower_Skirt.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Lower_Vest*/
	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Lower_Vest"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Lower/Lower_Vest.dat", PreTransformMatrix))))
		return E_FAIL;

#pragma endregion

#pragma region 눈 텍스쳐
	/* For. Prototype_Component_Texture_EyeBase */
	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Texture_Eye"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Anim/Custom_Player/Custom_Eye%d.dds"), 8))))
		return E_FAIL;
#pragma endregion
	return S_OK;
}

HRESULT CLoader::Ready_CustomPlayer_GamePlayLevel()
{
	_ulong			dwByte = {};
	HANDLE			hFile = CreateFile(TEXT("../Bin/DataFiles/Custom.dat"), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	CPlayer_Custom::CUSTOM_INFO tCustomInfo;

	ReadFile(hFile, &tCustomInfo, sizeof(CPlayer_Custom::CUSTOM_INFO), &dwByte, nullptr);
	
	_matrix		PreTransformMatrix = XMMatrixIdentity();

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

#pragma region 머리

	switch (tCustomInfo.iHeadID)
	{
	case 0:
		/* For. Prototype_Component_Model_Head_Hair1*/
		if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Head_Hair1"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Head/Head_Hair1.dat", PreTransformMatrix))))
			return E_FAIL;
		break;

	case 1:
		/* For. Prototype_Component_Model_Head_Hair2*/
		if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Head_Hair2"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Head/Head_Hair2.dat", PreTransformMatrix))))
			return E_FAIL;
		break;

	case 2:
		/* For. Prototype_Component_Model_Head_Cap*/
		if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Head_Cap"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Head/Head_Cap.dat", PreTransformMatrix))))
			return E_FAIL;
		break;

	case 3:
		/* For. Prototype_Component_Model_Head_Frog*/
		if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Head_Frog"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Head/Head_Frog.dat", PreTransformMatrix))))
			return E_FAIL;
		break;

	case 4:
		/* For. Prototype_Component_Model_Head_Helmet*/
		if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Head_Helmet"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Head/Head_Helmet.dat", PreTransformMatrix))))
			return E_FAIL;
		break;

	case 5:
		/* For. Prototype_Component_Model_Head_Hokage*/
		if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Head_Hokage"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Head/Head_Hokage.dat", PreTransformMatrix))))
			return E_FAIL;
		break;

	case 6:
		/* For. Prototype_Component_Model_Head_Hood*/
		if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Head_Hood"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Head/Head_Hood.dat", PreTransformMatrix))))
			return E_FAIL;
		break;

	case 7:
		/* For. Prototype_Component_Model_Head_Pajama*/
		if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Head_Pajama"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Head/Head_Pajama.dat", PreTransformMatrix))))
			return E_FAIL;
		break;
	}

#pragma endregion

#pragma region 얼굴
	switch (tCustomInfo.iFaceID)
	{
	case 0:
		/* For. Prototype_Component_Model_Face_Base*/
		if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Face_Base"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Face/Face_Base.dat", PreTransformMatrix))))
			return E_FAIL;
		break;

	case 1:
		/* For. Prototype_Component_Model_Face_Band*/
		if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Face_Band"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Face/Face_Band.dat", PreTransformMatrix))))
			return E_FAIL;
		break;

	case 2:
		/* For. Prototype_Component_Model_Face_Mask1*/
		if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Face_Mask1"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Face/Face_Mask.dat", PreTransformMatrix))))
			return E_FAIL;
		break;

	case 3:
		/* For. Prototype_Component_Model_Face_Mask2*/
		if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Face_Mask2"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Face/Face_Mask2.dat", PreTransformMatrix))))
			return E_FAIL;
		break;
	}
#pragma endregion

#pragma region 한벌옷

	if (tCustomInfo.isOnceClothes)
	{
		switch (tCustomInfo.iOnceID)
		{
		case 1:
			/* For. Prototype_Component_Model_Once_Armor*/
			if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Once_Armor"),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Once/Once_Armor.dat", PreTransformMatrix))))
				return E_FAIL;
			break;

		case 2:
			/* For. Prototype_Component_Model_Once_FinalCoat*/
			if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Once_FinalCoat"),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Once/Once_FinalCoat.dat", PreTransformMatrix))))
				return E_FAIL;
			break;

		case 3:
			/* For. Prototype_Component_Model_Once_Jacket*/
			if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Once_Jacket"),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Once/Once_Jacket.dat", PreTransformMatrix))))
				return E_FAIL;
			break;

		case 4:
			/* For. Prototype_Component_Model_Once_Santa*/
			if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Once_Santa"),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Once/Once_Santa.dat", PreTransformMatrix))))
				return E_FAIL;
			break;

		case 5:
			/* For. Prototype_Component_Model_Once_Sennin*/
			if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Once_Sennin"),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Once/Once_Sennin.dat", PreTransformMatrix))))
				return E_FAIL;
			break;

		case 6:
			/* For. Prototype_Component_Model_Once_Track*/
			if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Once_Track"),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Once/Once_Track.dat", PreTransformMatrix))))
				return E_FAIL;
			break;

		case 7:
			/* For. Prototype_Component_Model_Once_Yuukdo*/
			if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Once_Yuukdo"),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Once/Once_Yuukdo.dat", PreTransformMatrix))))
				return E_FAIL;
			break;
		}
	}

#pragma endregion

	
	if (!tCustomInfo.isOnceClothes)
	{
#pragma region 상체

		switch (tCustomInfo.iUpperID)
		{
		case 0:
			/* For. Prototype_Component_Model_BodyUpper_Trainer1*/
			if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_BodyUpper_Trainer1"),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Upper/Upper_Trainer1.dat", PreTransformMatrix))))
				return E_FAIL;
			break;
		case 1:
			/* For. Prototype_Component_Model_BodyUpper_Trainer2*/
			if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_BodyUpper_Trainer2"),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Upper/Upper_Trainer2.dat", PreTransformMatrix))))
				return E_FAIL;
			break;

		case 2:
			/* For. Prototype_Component_Model_Upper_Anbu*/
			if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Upper_Anbu"),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Upper/Upper_Anbu.dat", PreTransformMatrix))))
				return E_FAIL;
			break;

		case 3:
			/* For. Prototype_Component_Model_Upper_Armor*/
			if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Upper_Armor"),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Upper/Upper_Armor.dat", PreTransformMatrix))))
				return E_FAIL;
			break;

		case 4:
			/* For. Prototype_Component_Model_Upper_Haori*/
			if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Upper_Haori"),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Upper/Upper_Haori.dat", PreTransformMatrix))))
				return E_FAIL;
			break;

		case 5:
			/* For. Prototype_Component_Model_Upper_Hoodie*/
			if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Upper_Hoodie"),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Upper/Upper_Hoodie.dat", PreTransformMatrix))))
				return E_FAIL;
			break;

		case 6:
			/* For. Prototype_Component_Model_Upper_Shirt*/
			if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Upper_Shirt"),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Upper/Upper_Shirt.dat", PreTransformMatrix))))
				return E_FAIL;
			break;

		case 7:
			/* For. Prototype_Component_Model_Upper_Vest*/
			if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Upper_Vest"),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Upper/Upper_Vest.dat", PreTransformMatrix))))
				return E_FAIL;
			break;

		}

#pragma endregion

#pragma region 하체
		switch (tCustomInfo.iLowerID)
		{
		case 0:
			/* For. Prototype_Component_Model_BodyLower_Pants1*/
			if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_BodyLower_Pants1"),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Lower/Lower_Pants1.dat", PreTransformMatrix))))
				return E_FAIL;
			break;

		case 1:
			/* For. Prototype_Component_Model_BodyLower_Pants2*/
			if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_BodyLower_Pants2"),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Lower/Lower_Pants2.dat", PreTransformMatrix))))
				return E_FAIL;
			break;

		case 2:
			/* For. Prototype_Component_Model_Lower_Anbu*/
			if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Lower_Anbu"),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Lower/Lower_Anbu.dat", PreTransformMatrix))))
				return E_FAIL;
			break;

		case 3:
			/* For. Prototype_Component_Model_Lower_Armor*/
			if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Lower_Armor"),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Lower/Lower_Armor.dat", PreTransformMatrix))))
				return E_FAIL;
			break;

		case 4:
			/* For. Prototype_Component_Model_Lower_PantsCut*/
			if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Lower_PantsCut"),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Lower/Lower_PantsCut.dat", PreTransformMatrix))))
				return E_FAIL;
			break;

		case 5:
			/* For. Prototype_Component_Model_Lower_Short*/
			if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Lower_Short"),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Lower/Lower_Short.dat", PreTransformMatrix))))
				return E_FAIL;
			break;

		case 6:
			/* For. Prototype_Component_Model_Lower_Skirt*/
			if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Lower_Skirt"),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Lower/Lower_Skirt.dat", PreTransformMatrix))))
				return E_FAIL;
			break;

		case 7:
			/* For. Prototype_Component_Model_Lower_Vest*/
			if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_Lower_Vest"),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Custom_Player/Lower/Lower_Vest.dat", PreTransformMatrix))))
				return E_FAIL;
			break;
		}

#pragma endregion

	}

#pragma region 눈 텍스쳐
	/* For. Prototype_Component_Texture_EyeBase */
	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Texture_Eye"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Anim/Custom_Player/Custom_Eye%d.dds"), 8))))
		return E_FAIL;
#pragma endregion

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CLoader::Ready_Resources_For_AmeVillage()
{
	_matrix		PreTransformMatrix = XMMatrixIdentity();

	// _finddata_t : <io.h>에서 제공하며 파일 정보를 저장하는 구조체
	_finddata_t fd;

	// _findfirst : <io.h>에서 제공하며 사용자가 설정한 경로 내에서 가장 첫 번째 파일을 찾는 함수
	intptr_t handle = _findfirst("../Bin/ModelData/NonAnim/AmeVillage_Parts/*", &fd);

	if (handle == -1)
		return E_FAIL;

	int iResult = 0;

	char szCurPath[128] = "../Bin/ModelData/NonAnim/AmeVillage_Parts/";	 // 상대 경로
	char szFullPath[128] = "";

	_wstring strPrototype = TEXT("Part");
	_uint iNum = 0;

	while (iResult != -1)
	{
		strcpy_s(szFullPath, szCurPath);
		strcat_s(szFullPath, fd.name);

		_char szFileName[MAX_PATH] = "";
		_char szExt[MAX_PATH] = "";
		_splitpath_s(szFullPath, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

		if (!strcmp(fd.name, ".") || !strcmp(fd.name, "..")
			|| strcmp(szExt, ".dat"))
		{
			iResult = _findnext(handle, &fd);
			continue;
		}

		string strFileName = szFileName;
		_wstring strPrototypeName;

		strPrototypeName.assign(strFileName.begin(), strFileName.end());
		wprintf(strPrototypeName.c_str());

		PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_AME, strPrototypeName,
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, szFullPath, PreTransformMatrix))))
			return E_FAIL;

		//_findnext : <io.h>에서 제공하며 다음 위치의 파일을 찾는 함수, 더이상 없다면 -1을 리턴
		iResult = _findnext(handle, &fd);
	}

	return S_OK;
}

HRESULT CLoader::Ready_Resources_For_KonohaVillage()
{
	_matrix		PreTransformMatrix = XMMatrixIdentity();

	PreTransformMatrix = XMMatrixScaling(0.008f, 0.008f, 0.008f) * XMMatrixRotationY(XMConvertToRadians(180.0f)) * XMMatrixTranslation(0.f, -25.f, 0.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_KONOHA, TEXT("Prototype_Model_MapKonoha"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/NewKonoha/NewKonoha.dat", PreTransformMatrix))))
		return E_FAIL;

	//// _finddata_t : <io.h>에서 제공하며 파일 정보를 저장하는 구조체
	//_finddata_t fd;

	//// _findfirst : <io.h>에서 제공하며 사용자가 설정한 경로 내에서 가장 첫 번째 파일을 찾는 함수
	//intptr_t handle = _findfirst("../Bin/ModelData/NonAnim/KonohaVillage_Parts/*", &fd);

	//if (handle == -1)
	//	return E_FAIL;

	//int iResult = 0;

	//char szCurPath[128] = "../Bin/ModelData/NonAnim/KonohaVillage_Parts/";	 // 상대 경로
	//char szFullPath[128] = "";

	//_wstring strPrototype = TEXT("Part");
	//_uint iNum = 0;

	//while (iResult != -1)
	//{
	//	strcpy_s(szFullPath, szCurPath);
	//	strcat_s(szFullPath, fd.name);

	//	_char szFileName[MAX_PATH] = "";
	//	_char szExt[MAX_PATH] = "";
	//	_splitpath_s(szFullPath, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

	//	if (!strcmp(fd.name, ".") || !strcmp(fd.name, "..")
	//		|| strcmp(szExt, ".dat"))
	//	{
	//		iResult = _findnext(handle, &fd);
	//		continue;
	//	}

	//	string strFileName = szFileName;
	//	_wstring strPrototypeName;

	//	strPrototypeName.assign(strFileName.begin(), strFileName.end());
	//	wprintf(strPrototypeName.c_str());

	//	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f)) * XMMatrixTranslation(0.f,-25.f,0.f);
	//	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_KONOHA, strPrototypeName,
	//		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, szFullPath, PreTransformMatrix))))
	//		return E_FAIL;

	//	//_findnext : <io.h>에서 제공하며 다음 위치의 파일을 찾는 함수, 더이상 없다면 -1을 리턴
	//	iResult = _findnext(handle, &fd);
	//}

	return S_OK;
}

HRESULT CLoader::Ready_Resources_For_ChuninExam()
{
	_matrix		PreTransformMatrix = XMMatrixIdentity();

	// _finddata_t : <io.h>에서 제공하며 파일 정보를 저장하는 구조체
	_finddata_t fd;

	// _findfirst : <io.h>에서 제공하며 사용자가 설정한 경로 내에서 가장 첫 번째 파일을 찾는 함수
	intptr_t handle = _findfirst("../Bin/ModelData/NonAnim/Chunin_Parts/*", &fd);

	if (handle == -1)
		return E_FAIL;

	int iResult = 0;

	char szCurPath[128] = "../Bin/ModelData/NonAnim/Chunin_Parts/";	 // 상대 경로
	char szFullPath[128] = "";

	_wstring strPrototype = TEXT("Part");
	_uint iNum = 0;

	while (iResult != -1)
	{
		strcpy_s(szFullPath, szCurPath);
		strcat_s(szFullPath, fd.name);

		_char szFileName[MAX_PATH] = "";
		_char szExt[MAX_PATH] = "";
		_splitpath_s(szFullPath, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

		if (!strcmp(fd.name, ".") || !strcmp(fd.name, "..")
			|| strcmp(szExt, ".dat"))
		{
			iResult = _findnext(handle, &fd);
			continue;
		}

		string strFileName = szFileName;
		_wstring strPrototypeName;

		strPrototypeName.assign(strFileName.begin(), strFileName.end());
		wprintf(strPrototypeName.c_str());

		PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f)) * XMMatrixTranslation(0.f, -25.f, 0.f);
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHUNIN, strPrototypeName,
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, szFullPath, PreTransformMatrix))))
			return E_FAIL;

		//_findnext : <io.h>에서 제공하며 다음 위치의 파일을 찾는 함수, 더이상 없다면 -1을 리턴
		iResult = _findnext(handle, &fd);
	}

	return S_OK;
}

HRESULT CLoader::Ready_Effect()
{
	_matrix		PreTransformMatrix = XMMatrixIdentity();

	/* For. Prototype_Component_Model_SmokeRing */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_SmokeRing"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/Effect/SmokeRing/SmokeRing.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_GroundBreak */
	PreTransformMatrix = XMMatrixScaling(0.05f, 0.05f, 0.05f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_GroundBreak"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/GroundBreak/GroundBreak.dat", PreTransformMatrix))))
		return E_FAIL;


#pragma region 나선환
	/* For. Prototype_Component_Texture_Effect_Ring*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Ring"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Effect/T_EFF_Ring_02_M.dds")))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Effect_Sphere*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Sphere"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Effect/T_EFF_Sphere_02_M.dds")))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Effect_Sphere03*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Sphere03"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Effect/T_EFF_Sphere_03_M.dds")))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Effect_Sphere08*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Sphere08"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Effect/T_EFF_Sphere_08_M.dds")))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Effect_Rasengan*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Rasengan"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Effect/T_EFF_Decal_Rasengan_01_M.dds")))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Rasengan_RunSmoke */
	PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(250.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Rasengan_RunSmoke"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/Effect/Skill/Rasengan_RunSmoke.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Rasengan_CreateSmoke */
	PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(70.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Rasengan_CreateSmoke"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/Effect/Skill/Rasengan_CreateSmoke.dat", PreTransformMatrix))))
		return E_FAIL;


#pragma endregion

#pragma region 나선수리검
	/* For. Prototype_Component_Model_Rasengan_Center */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Rasengan_Center"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/Effect/Skill/Rasengan_Center.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_RasenShuriken_Blade */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_RasenShuriken_Blade"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/Effect/Skill/RasenShuriken_Blade.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_RasenShuriken_Circle */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_RasenShuriken_Circle"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/Effect/Skill/RasenShuriken_Circle.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_RasenShuriken_BladeFly */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_RasenShuriken_BladeFly"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/Effect/Skill/RasenShuriken_BladeFly.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_RasenShuriken_BladeLine */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_RasenShuriken_BladeLine"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/Effect/Skill/RasenShuriken_BladeLine.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_ExplosionSphere */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_ExplosionSphere"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/ExplosionSphere/ExplosionSphere.dat", PreTransformMatrix))))
		return E_FAIL;
#pragma endregion

#pragma region 대옥나선환
	/* For. Prototype_Component_Model_MassiveRasengan */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_MassiveRasengan"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/Effect/Skill/MassiveRasengan.dat", PreTransformMatrix))))
		return E_FAIL;
#pragma endregion

	/* For. Prototype_Component_Model_RasenganRing */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(90.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_RasenganRing"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/Effect/RasenganRing/RasenganRing.dat", PreTransformMatrix))))
		return E_FAIL;

#pragma region 치도리

	/* For. Prototype_Component_Texture_Effect_Chidori*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Chidori"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Effect/T_EFF_Hit_07_M.dds")))))
		return E_FAIL;

	/* For. Prototype_Component_Model_LightningA01 */
	PreTransformMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_LightningA01"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/Effect/Lightning/LightningA01.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_LightningA02 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_LightningA02"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/Effect/Lightning/LightningA02.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_LightningB01 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_LightningB01"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/Effect/Lightning/LightningB01.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_LightningB02 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_LightningB02"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/Effect/Lightning/LightningB02.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_LightningB03 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_LightningB03"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/Effect/Lightning/LightningB03.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_LightningB04 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_LightningB04"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/Effect/Lightning/LightningB04.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_LightningC */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_LightningC"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/Effect/Lightning/LightningC.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_LightningD */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_LightningD"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/Effect/Lightning/LightningD.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_LightningE */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_LightningE"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/Effect/Lightning/LightningE.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_LightningF */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_LightningF"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/Effect/Lightning/LightningF.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_LightningChidori */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_LightningChidori"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/Effect/Lightning/Lightning_Chidori.dat", PreTransformMatrix))))
		return E_FAIL;
#pragma endregion

#pragma region 호화구

	/* For. Prototype_Component_Model_FireBall */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_FireBall"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/Effect/Skill/FireBall.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_FireBallEffect */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_FireBallEffect"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/Effect/Skill/FireBall_Effect.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_FireFlySmoke */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_FireFlySmoke"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/Effect/Skill/FireBall_FlySmoke.dat", PreTransformMatrix))))
		return E_FAIL;

#pragma endregion

#pragma region 기린
	/* For. Prototype_Component_Model_Kirin */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Kirin"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Kirin/Kirin.dat", PreTransformMatrix))))
		return E_FAIL;
#pragma endregion

#pragma region 아마테라스
	/* For. Prototype_Component_Model_Amateras0 */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(90.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Amateras0"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/Effect/Amateras/Amateras0.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Amateras1 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Amateras1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/Effect/Amateras/Amateras1.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Amateras2 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Amateras2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/Effect/Amateras/Amateras2.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Amateras3 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Amateras3"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/Effect/Amateras/Amateras3.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Amateras4 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Amateras4"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/Effect/Amateras/Amateras4.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Amateras5 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Amateras5"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/Effect/Amateras/Amateras5.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Effect_Fire5*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Fire5"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Effect/T_EFF_Fire_05_BC.dds")))))
		return E_FAIL;

#pragma endregion

#pragma region 츠쿠요미
	/* For. Prototype_Component_Texture_Effect_Tsukuyomi*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Tsukuyomi"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Effect/Tsukuyomi/Effect_Tsukuyomi%d.dds"), 3))))
		return E_FAIL;
#pragma endregion

#pragma region 미나토
	/* For. Prototype_Component_Model_RasenkoStar */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(90.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_RasenkoStar"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/RasenkoStar/RasenkoStar.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_TypeZero0 */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_TypeZero0"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/TypeZero/TypeZero0.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_TypeZero1 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_TypeZero1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/TypeZero/TypeZero1.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_TypeZero2 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_TypeZero2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/TypeZero/TypeZero2.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_TypeZero3 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_TypeZero3"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/TypeZero/TypeZero3.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_TypeZero4 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_TypeZero4"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/TypeZero/TypeZero4.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_TypeZero5 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_TypeZero5"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/TypeZero/TypeZero5.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_TypeZero6 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_TypeZero6"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/TypeZero/TypeZero6.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_TypeZero7 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_TypeZero7"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/TypeZero/TypeZero7.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_TypeZero8 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_TypeZero8"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/TypeZero/TypeZero8.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_TypeZero9 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_TypeZero9"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/TypeZero/TypeZero9.dat", PreTransformMatrix))))
		return E_FAIL;
#pragma endregion

	/* For. Prototype_Component_Model_MinatoKuani */
	PreTransformMatrix = XMMatrixScaling(0.015f, 0.015f, 0.015f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_MinatoKuani"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/MinatoKunai/MinatoKunai.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Effect_Smoke11*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Smoke11"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Effect/T_EFF_Smoke_11_M.dds")))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Effect_Smoke20*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Smoke20"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Effect/T_EFF_Smoke_20_M.dds")))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Effect_Smoke27*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Smoke27"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Effect/T_EFF_Smoke_27_M.dds")))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Effect_Noise04*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Noise04"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Effect/T_EFF_Noise_04_BC.dds")))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Effect_Noise09*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Noise09"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Effect/T_EFF_Noise_09_M.dds")))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Effect_Noise14*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Noise14"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Effect/T_EFF_Noise_14_M.dds")))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Effect_Noise16*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Noise16"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Effect/T_EFF_Noise_16_M.dds")))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Effect_Hit03*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Hit09"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Effect/T_EFF_Hit_03_M.dds")))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Effect_Hit014*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Hit014"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Effect/T_EFF_Hit_14_M.dds")))))
		return E_FAIL;

	/* For. Prototype_Component_Model_SusanooSlash */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(90.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_SusanooSlash"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/Effect/SusanooSlash/SusanooSlash.dat", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Wood"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/Wood/Wood.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_RoarEffect1*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_RoarEffect1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Effect/T_EFF_Radiation_01_M.dds")))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_RoarEffect2*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_RoarEffect2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Effect/T_EFF_Radiation_03_M.dds")))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_RoarRing*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_RoarRing"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Effect/T_EFF_Ring_09_M.dds")))))
		return E_FAIL;


	/* For. Prototype_Component_Texture_BossHPMask */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Noise06"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Effect/T_EFF_Noise_06_BC.dds")))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Distortion */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Distortion"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Effect/Distortion%d.dds"), 4))))
		return E_FAIL;

	CVIBuffer_Trail_Instance::INSTACNE_TRAIL_DESC tTrailDesc{};
	ZeroMemory(&tTrailDesc, sizeof tTrailDesc);

	tTrailDesc.iNumInstance = 40;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Trail"),
		CVIBuffer_Trail_Instance::Create(m_pDevice, m_pContext, tTrailDesc))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Ready_UITexture()
{
	/* For. Prototype_Component_Texture_TargetCursol */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_TargetCursol"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/PlayerUI/T_UI_TargetCursol%d.dds"), 3))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_TextWindow */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_TextWindow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/TextWindow/TextWindow%d.dds"), 2))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_TextIcon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_TextIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/TextWindow/TextIcon%d.dds"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_SPSkillIcon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SPSkillIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/SkillIcon/SpecialSkill%d.dds"), 4))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_SPBase */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SPBase"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/PlayerUI/SP_Base.dds")))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_HPBase */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HPBase"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/PlayerUI/HP_Base.dds")))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_HPMask */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HPMask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/PlayerUI/HP_Mask.dds")))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_HPDamageEffect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HPDamageEffect"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/PlayerUI/HP_DamageEffect.dds")))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_SkillBase */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SkillSlotBase"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/PlayerUI/Skill_Base%d.png"), 2))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_SkillIcon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SkillIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/SkillIcon/BaseSkill%d.dds"), 9))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_HitNum */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HitNum"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/HitNum/HitNum%d.dds"), 10))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_HitText */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HitText"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/HitNum/HitText%d.dds"), 2))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_BossIcon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BossIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Boss/BossIcon%d.dds"), 2))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_BossHPGuage */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BossHPGuage"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Boss/T_UI_Boss_Gauge%d.dds"), 2))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_BossHPMask */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BossHPMask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Boss/T_UI_Boss_Gauge_M.dds"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_BossTarget */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BossTarget"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Boss/BossTargetIcon.dds"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_AreaCircleIcon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_AreaCircleIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/AreaCircleIcon.dds"), 1))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Ready_Particle()
{
	CVIBuffer_Instancing::INSTANCE_PARTICLE_DESC			ParticleDesc{};
	/* For. Prototype_Component_VIBuffer_Particle_Explosion */
	ZeroMemory(&ParticleDesc, sizeof ParticleDesc);

	ParticleDesc.iNumInstance = 200;
	ParticleDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ParticleDesc.vRange = _float3(0.3f, 0.3f, 0.3f);
	ParticleDesc.vSize = _float2(0.05f, 0.1f);
	ParticleDesc.vPivot = _float3(0.f, 0.f, 0.f);
	ParticleDesc.vSpeed = _float2(1.f, 3.f);
	ParticleDesc.vLifeTime = _float2(0.4f, 0.6f);
	ParticleDesc.isLoop = true;

	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_VIBuffer_Particle_Explosion"),
		CVIBuffer_Rect_Instance::Create(m_pDevice, m_pContext, ParticleDesc))))
		return E_FAIL;

	/* For. Prototype_Component_VIBuffer_Particle_Explosion */
	ZeroMemory(&ParticleDesc, sizeof ParticleDesc);

	ParticleDesc.iNumInstance = 3000;
	ParticleDesc.vCenter = _float3(64.f, 20.f, 64.f);
	ParticleDesc.vRange = _float3(128.f, 1.f, 128.f);
	ParticleDesc.vSize = _float2(0.2f, 0.4f);
	ParticleDesc.vPivot = _float3(0.f, 0.f, 0.f);
	ParticleDesc.vSpeed = _float2(1.f, 3.f);
	ParticleDesc.vLifeTime = _float2(4.f, 8.f);
	ParticleDesc.isLoop = true;

	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_VIBuffer_Particle_Snow"),
		CVIBuffer_Point_Instance::Create(m_pDevice, m_pContext, ParticleDesc))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_ParticleSnow */
	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Texture_ParticleSnow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Snow/Snow.png"), 1))))
		return E_FAIL;

	return S_OK;
}

CLoader * CLoader::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, LEVELID eNextLevelID)
{
	CLoader*		pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX(TEXT("Failed to Created : CLoader"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	__super::Free();

	WaitForSingleObject(m_hThread, INFINITE);

	DeleteObject(m_hThread);

	CloseHandle(m_hThread);

	DeleteCriticalSection(&m_CriticalSection);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
