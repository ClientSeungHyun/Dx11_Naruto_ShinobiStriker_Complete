#include "stdafx.h"
#include "..\Public\Loader.h"

#include "Terrain.h"
#include "BackGround.h"
#include "GameInstance.h"

#include "ModelController.h"

#include "NonAnimModel.h"
#include "FreeCamera.h"

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
	case LEVEL_GAMEPLAY:
		hr = Ready_Resources_For_GamePlayLevel();
		break;
	}

	LeaveCriticalSection(&m_CriticalSection);

	return hr;
}

void CLoader::Draw_LoadingText()
{
	SetWindowText(g_hWnd, m_szLoadingText);
}

HRESULT CLoader::Ready_Resources_For_LogoLevel()
{
	return S_OK;
}

HRESULT CLoader::Ready_Resources_For_GamePlayLevel()
{
	_matrix		PreTransformMatrix = XMMatrixIdentity();

	/* For. Prototype_Component_Model_Fiona*/
	//PreTransformMatrix =  XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Fiona"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Fiona/Fiona.fbx","../Bin/ModelData/NonAnim/Fiona/", PreTransformMatrix))))
	//	return E_FAIL;
	//CModelController::Get_Instance()->Add_ProtytypeTag(TEXT("Fiona"));

	///* For. Prototype_Component_Model_WhiteZetsu*/
	//PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("WhiteZetsu"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/WhiteZetsu/WhiteZetsu.fbx", "../Bin/ModelData/Anim/Monster/WhiteZetsu/", PreTransformMatrix))))
	//	return E_FAIL;

	///* For. Prototype_Component_Model_TenTail*/
	//PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("TenTail"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/TenTail/TenTailsClone.fbx", "../Bin/ModelData/Anim/Monster/TenTail/", PreTransformMatrix))))
	//	return E_FAIL;

	/* For. Prototype_Component_Model_Kurama*/
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Kurama"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Kurama/Kurama.fbx", "../Bin/ModelData/Anim/Monster/Kurama/", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Naruto*/
	//PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Naruto"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Naruto/Naruto.fbx", "../Bin/ModelData/Anim/Naruto/", PreTransformMatrix))))
	//	return E_FAIL;

	///* For. Prototype_Component_Model_Kirin*/
	//PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Kirin"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Kirin/Kirin.fbx", "../Bin/ModelData/Anim/Kirin/", PreTransformMatrix))))
	//	return E_FAIL;

	///* For. Prototype_Component_Model_JumpGuide*/
	//PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("JumpGuide"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/JumpGuide/JumpGuide.fbx", "../Bin/ModelData/NonAnim/JumpGuide/", PreTransformMatrix))))
	//	return E_FAIL;

	///* For. Prototype_Component_Model_MinatoKunai */
	//PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_MinatoKunai"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/MinatoKunai/MinatoKunai.fbx", "../Bin/ModelData/NonAnim/MinatoKunai/", PreTransformMatrix))))
	//	return E_FAIL;

	///* For. Prototype_Component_Model_Susanoo */
	//PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Susanoo"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Susanoo/Susanoo.fbx", "../Bin/ModelData/Anim/Susanoo/", PreTransformMatrix))))
	//	return E_FAIL;

	///* For. Prototype_Component_Model_SusanooBlade */
	//PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SusanooBlade"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/SusanooBlade/SusanooBlade.fbx", "../Bin/ModelData/NonAnim/SusanooBlade/", PreTransformMatrix))))
	//	return E_FAIL;

	///* For. Prototype_Component_Model_ExplosionSphere */
	//PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_ExplosionSphere"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/ExplosionSphere/ExplosionSphere.fbx", "../Bin/ModelData/NonAnim/ExplosionSphere/", PreTransformMatrix))))
	//	return E_FAIL;

	///* For. Prototype_Component_Model_RasenganRing */
	//PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_RasenganRing"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/RasenganRing/RasenganRing.fbx", "../Bin/ModelData/Effect/", PreTransformMatrix))))
	//	return E_FAIL;

	///* For. Prototype_Component_Model_Susanoo */
	//PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Sasuke_Next"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Sasuke_Next/Sasuke_Next.fbx", "../Bin/ModelData/Anim/Sasuke_Next/", PreTransformMatrix))))
	//	return E_FAIL;

	///* For. Prototype_Component_Model_SusanooSlash */
	//PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SusanooSlash"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/SusanooSlash/SusanooSlash.fbx", "../Bin/ModelData/Effect/", PreTransformMatrix))))
	//	return E_FAIL;

	///* For. Prototype_Component_Model_AreaCircle */
	//PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_AreaCircle"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/AreaCircle/AreaCircle.fbx", "../Bin/ModelData/NonAnim/AreaCircle/", PreTransformMatrix))))
	//	return E_FAIL;

	///* For. Prototype_Component_Model_RasenkoStar */
	//PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_RasenkoStar"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/RasenkoStar/RasenkoStar.fbx", "../Bin/ModelData/NonAnim/RasenkoStar/", PreTransformMatrix))))
	//	return E_FAIL;
	 
	///* For. Prototype_Component_Model_NewKonoha*/
	//PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_NewKonoha"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/NewKonoha/NewKonoha.fbx", "../Bin/ModelData/NonAnim/NewKonoha/", PreTransformMatrix))))
	//	return E_FAIL;

	///* For. Prototype_Component_Model_GroundBreak*/
	//PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_GroundBreak"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/GroundBreak/GroundBreak.fbx", "../Bin/ModelData/NonAnim/GroundBreak/", PreTransformMatrix))))
	//	return E_FAIL;

	///* For. Prototype_Component_Model_SmokeRing*/
	//PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SmokeRing"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/SmokeRing/SmokeRing.fbx", "../Bin/ModelData/Effect/", PreTransformMatrix))))
	//	return E_FAIL;

	//if (FAILED(Ready_Resources_For_AmeVillage()))
	//	return E_FAIL;

	//if (FAILED(Ready_Resources_For_KonohaVillage()))
	//	return E_FAIL;

	//if (FAILED(Ready_Resources_For_ChuninExam()))
	//	return E_FAIL;

	//if (FAILED(Ready_Resources_For_CustomPlayer()))
	//	return E_FAIL;

	//if (FAILED(Reayd_Effect()))
	//	return E_FAIL;

	//if (FAILED(Ready_Lightning()))
	//	return E_FAIL;

	//if (FAILED(Ready_Amaterasu()))
	//	return E_FAIL;

	//if (FAILED(Ready_TypeZeorSkill()))
	//	return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("변환 완료"));

	MSG_BOX(TEXT("변환이 완료되었습니다 종료해주세요"));

	m_isFinished = true;


	return S_OK;
}

HRESULT CLoader::Ready_Resources_For_CustomPlayer()
{
	_matrix		PreTransformMatrix = XMMatrixIdentity();

#pragma region 머리끝

	///* For. Prototype_Component_Model_Head_Cap*/
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Head_Cap"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Custom_Player/Head/Head_Cap.fbx",
	//		"../Bin/ModelData/Anim/Custom_Player/", PreTransformMatrix))))
	//	return E_FAIL;

	///* For. Prototype_Component_Model_Head_Frog*/
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Head_Frog"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Custom_Player/Head/Head_Frog.fbx",
	//		"../Bin/ModelData/Anim/Custom_Player/", PreTransformMatrix))))
	//	return E_FAIL;

	///* For. Prototype_Component_Model_Head_Hair1*/
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Head_Hair1"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Custom_Player/Head/Head_Hair1.fbx",
	//		"../Bin/ModelData/Anim/Custom_Player/", PreTransformMatrix))))
	//	return E_FAIL;

	///* For. Prototype_Component_Model_Head_Hair2*/
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Head_Hair2"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Custom_Player/Head/Head_Hair2.fbx",
	//		"../Bin/ModelData/Anim/Custom_Player/", PreTransformMatrix))))
	//	return E_FAIL;

	///* For. Prototype_Component_Model_Head_Helmet*/
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Head_Helmet"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Custom_Player/Head/Head_Helmet.fbx",
	//		"../Bin/ModelData/Anim/Custom_Player/", PreTransformMatrix))))
	//	return E_FAIL;

	///* For. Prototype_Component_Model_Head_Hokage*/
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Head_Hokage"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Custom_Player/Head/Head_Hokage.fbx",
	//		"../Bin/ModelData/Anim/Custom_Player/", PreTransformMatrix))))
	//	return E_FAIL;

	/* For. Prototype_Component_Model_Head_Hood*/
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Head_Hood"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Custom_Player/Head/Head_Hood.fbx",
	//		"../Bin/ModelData/Anim/Custom_Player/", PreTransformMatrix))))
	//	return E_FAIL;

	///* For. Prototype_Component_Model_Head_Pajama*/
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Head_Pajama"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Custom_Player/Head/Head_Pajama.fbx",
	//		"../Bin/ModelData/Anim/Custom_Player/", PreTransformMatrix))))
	//	return E_FAIL;

#pragma endregion

	
#pragma region 얼굴 끝

	///* For. Prototype_Component_Model_Face_Base*/
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Face_Base"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Custom_Player/Face/Face_Base.fbx",
	//		"../Bin/ModelData/Anim/Custom_Player/", PreTransformMatrix))))
	//	return E_FAIL;

	///* For. Prototype_Component_Model_Face_Bandage*/
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Face_Bandage"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Custom_Player/Face/Face_Bandag.fbx",
	//		"../Bin/ModelData/Anim/Custom_Player/", PreTransformMatrix))))
	//	return E_FAIL;

	///* For. Prototype_Component_Model_Face_Mask1*/
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Face_Mask1"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Custom_Player/Face/Face_Mask.fbx",
	//		"../Bin/ModelData/Anim/Custom_Player/", PreTransformMatrix))))
	//	return E_FAIL;

	///* For. Prototype_Component_Model_Face_Mask2*/
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Face_Mask2"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Custom_Player/Face/Face_Mask2.fbx",
	//		"../Bin/ModelData/Anim/Custom_Player/", PreTransformMatrix))))
	//	return E_FAIL;
#pragma endregion

#pragma region 상체끝
	
	///* For. Prototype_Component_Model_Upper_Trainer1*/
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Upper_Trainer1"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Custom_Player/Upper/Upper_Trainer1.fbx",
	//		"../Bin/ModelData/Anim/Custom_Player/", PreTransformMatrix))))
	//	return E_FAIL;

	///* For. Prototype_Component_Model_Upper_Trainer2*/
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Upper_Trainer2"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Custom_Player/Upper/Upper_Trainer2.fbx",
	//		"../Bin/ModelData/Anim/Custom_Player/", PreTransformMatrix))))
	//	return E_FAIL;

	///* For. Prototype_Component_Model_Upper_Anbu*/
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Upper_Anbu"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Custom_Player/Upper/Upper_Anbu.fbx",
	//		"../Bin/ModelData/Anim/Custom_Player/", PreTransformMatrix))))
	//	return E_FAIL;

	///* For. Prototype_Component_Model_Upper_Armor*/
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Upper_Armor"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Custom_Player/Upper/Upper_Armor.fbx",
	//		"../Bin/ModelData/Anim/Custom_Player/", PreTransformMatrix))))
	//	return E_FAIL;

	///* For. Prototype_Component_Model_Upper_Haori*/
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Upper_Haori"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Custom_Player/Upper/Upper_Haori.fbx",
	//		"../Bin/ModelData/Anim/Custom_Player/", PreTransformMatrix))))
	//	return E_FAIL;

	///* For. Prototype_Component_Model_Upper_Hoodie*/
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Upper_Hoodie"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Custom_Player/Upper/Upper_Hoodie.fbx",
	//		"../Bin/ModelData/Anim/Custom_Player/", PreTransformMatrix))))
	//	return E_FAIL;

	///* For. Prototype_Component_Model_Upper_Shirt*/
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Upper_Shirt"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Custom_Player/Upper/Upper_Shirt.fbx",
	//		"../Bin/ModelData/Anim/Custom_Player/", PreTransformMatrix))))
	//	return E_FAIL;

	///* For. Prototype_Component_Model_Upper_Vest*/
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Upper_Vest"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Custom_Player/Upper/Upper_Vest.fbx",
	//		"../Bin/ModelData/Anim/Custom_Player/", PreTransformMatrix))))
	//	return E_FAIL;
#pragma endregion

#pragma region 하체끝

	///* For. Prototype_Component_Model_Lower_Pants1*/
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Lower_Pants1"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Custom_Player/Lower/Lower_Pants1.fbx",
	//		"../Bin/ModelData/Anim/Custom_Player/", PreTransformMatrix))))
	//	return E_FAIL;

	///* For. Prototype_Component_Model_Lower_Pants2*/
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Lower_Pants2"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Custom_Player/Lower/Lower_Pants2.fbx",
	//		"../Bin/ModelData/Anim/Custom_Player/", PreTransformMatrix))))
	//	return E_FAIL;

	///* For. Prototype_Component_Model_Lower_Anbu*/
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Lower_Anbu"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Custom_Player/Lower/Lower_Anbu.fbx",
	//		"../Bin/ModelData/Anim/Custom_Player/", PreTransformMatrix))))
	//	return E_FAIL;

	///* For. Prototype_Component_Model_Lower_Armor*/
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Lower_Armor"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Custom_Player/Lower/Lower_Armor.fbx",
	//		"../Bin/ModelData/Anim/Custom_Player/", PreTransformMatrix))))
	//	return E_FAIL;

	///* For. Prototype_Component_Model_Lower_PantsCut*/
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Lower_PantsCut"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Custom_Player/Lower/Lower_PantsCut.fbx",
	//		"../Bin/ModelData/Anim/Custom_Player/", PreTransformMatrix))))
	//	return E_FAIL;

	///* For. Prototype_Component_Model_Lower_Short*/
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Lower_Short"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Custom_Player/Lower/Lower_Short.fbx",
	//		"../Bin/ModelData/Anim/Custom_Player/", PreTransformMatrix))))
	//	return E_FAIL;

	///* For. Prototype_Component_Model_Lower_Skirt*/
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Lower_Skirt"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Custom_Player/Lower/Lower_Skirt.fbx",
	//		"../Bin/ModelData/Anim/Custom_Player/", PreTransformMatrix))))
	//	return E_FAIL;

	///* For. Prototype_Component_Model_Lower_Vest*/
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Lower_Vest"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Custom_Player/Lower/Lower_Vest.fbx",
	//		"../Bin/ModelData/Anim/Custom_Player/", PreTransformMatrix))))
	//	return E_FAIL;
#pragma endregion

#pragma region 한벌옷끝

	/* For. Prototype_Component_Model_Once_Hokage4*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Once_Hokage4"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Custom_Player/Cloth_Once/Once_Hokage4.fbx",
			"../Bin/ModelData/Anim/Custom_Player/", PreTransformMatrix))))
		return E_FAIL;

	///* For. Prototype_Component_Model_Once_Armor*/
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Once_Armor"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Custom_Player/Cloth_Once/Once_Armor.fbx",
	//		"../Bin/ModelData/Anim/Custom_Player/", PreTransformMatrix))))
	//	return E_FAIL;

	///* For. Prototype_Component_Model_Once_FinalCoat*/
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Once_FinalCoat"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Custom_Player/Cloth_Once/Once_FinalCoat.fbx",
	//		"../Bin/ModelData/Anim/Custom_Player/", PreTransformMatrix))))
	//	return E_FAIL;

	///* For. Prototype_Component_Model_Once_Jacket*/
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Once_Jacket"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Custom_Player/Cloth_Once/Once_Jacket.fbx",
	//		"../Bin/ModelData/Anim/Custom_Player/", PreTransformMatrix))))
	//	return E_FAIL;

	///* For. Prototype_Component_Model_Once_Santa*/
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Once_Santa"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Custom_Player/Cloth_Once/Once_Santa.fbx",
	//		"../Bin/ModelData/Anim/Custom_Player/", PreTransformMatrix))))
	//	return E_FAIL;

	///* For. Prototype_Component_Model_Once_Sennin*/
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Once_Sennin"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Custom_Player/Cloth_Once/Once_Sennin.fbx",
	//		"../Bin/ModelData/Anim/Custom_Player/", PreTransformMatrix))))
	//	return E_FAIL;

	///* For. Prototype_Component_Model_Once_Track*/
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Once_Track"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Custom_Player/Cloth_Once/Once_Track.fbx",
	//		"../Bin/ModelData/Anim/Custom_Player/", PreTransformMatrix))))
	//	return E_FAIL;

	///* For. Prototype_Component_Model_Once_Yuukdo*/
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Once_Yuukdo"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Custom_Player/Cloth_Once/Once_Yuukdo.fbx",
	//		"../Bin/ModelData/Anim/Custom_Player/", PreTransformMatrix))))
	//	return E_FAIL;

#pragma endregion

	return S_OK;
}

HRESULT CLoader::Ready_Resources_For_AmeVillage()
{
	_matrix		PreTransformMatrix = XMMatrixIdentity();

	// _finddata_t : <io.h>에서 제공하며 파일 정보를 저장하는 구조체
	_finddata_t fd;

	// _findfirst : <io.h>에서 제공하며 사용자가 설정한 경로 내에서 가장 첫 번째 파일을 찾는 함수
	intptr_t handle = _findfirst("../Bin/Resources/Models/AmeVillage_Parts/*", &fd);

	if (handle == -1)
		return E_FAIL;

	int iResult = 0;

	char szCurPath[128] = "../Bin/Resources/Models/AmeVillage_Parts/";	 // 상대 경로
	char szFullPath[128] = "";

	_wstring strPrototype = TEXT("Part");
	_uint iNum = 0;

	while (iResult != -1)
	{
		strcpy_s(szFullPath, szCurPath);
		strcat_s(szFullPath, fd.name);

		if (!strcmp(fd.name, ".") || !strcmp(fd.name, ".."))
		{
			iResult = _findnext(handle, &fd);
			continue;
		}

		_wstring strPrototypeName;
		_wstring strNum = to_wstring(iNum++);
		strPrototypeName = strPrototype + strNum;

		PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, strPrototypeName,
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, szFullPath, "../Bin/ModelData/NonAnim/AmeVillage_Parts/", PreTransformMatrix))))
			return E_FAIL;

		//_findnext : <io.h>에서 제공하며 다음 위치의 파일을 찾는 함수, 더이상 없다면 -1을 리턴
		iResult = _findnext(handle, &fd);
	}

	return S_OK;
}

HRESULT CLoader::Ready_Resources_For_KonohaVillage()
{
	_matrix		PreTransformMatrix = XMMatrixIdentity();

	// _finddata_t : <io.h>에서 제공하며 파일 정보를 저장하는 구조체
	_finddata_t fd;

	// _findfirst : <io.h>에서 제공하며 사용자가 설정한 경로 내에서 가장 첫 번째 파일을 찾는 함수
	intptr_t handle = _findfirst("../Bin/Resources/Models/KonohaVillage_Parts/*", &fd);

	if (handle == -1)
		return E_FAIL;

	int iResult = 0;

	char szCurPath[128] = "../Bin/Resources/Models/KonohaVillage_Parts/";	 // 상대 경로
	char szFullPath[128] = "";

	_wstring strPrototype = TEXT("Part");
	_uint iNum = 0;

	while (iResult != -1)
	{
		strcpy_s(szFullPath, szCurPath);
		strcat_s(szFullPath, fd.name);

		if (!strcmp(fd.name, ".") || !strcmp(fd.name, ".."))
		{
			iResult = _findnext(handle, &fd);
			continue;
		}

		_wstring strPrototypeName;
		_wstring strNum = to_wstring(iNum++);
		strPrototypeName = strPrototype + strNum;

		PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, strPrototypeName,
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, szFullPath, "../Bin/ModelData/NonAnim/KonohaVillage_Parts/", PreTransformMatrix))))
			return E_FAIL;

		//_findnext : <io.h>에서 제공하며 다음 위치의 파일을 찾는 함수, 더이상 없다면 -1을 리턴
		iResult = _findnext(handle, &fd);
	}

	return S_OK;
}

HRESULT CLoader::Ready_Resources_For_ChuninExam()
{
	_matrix		PreTransformMatrix = XMMatrixIdentity();

	// _finddata_t : <io.h>에서 제공하며 파일 정보를 저장하는 구조체
	_finddata_t fd;

	// _findfirst : <io.h>에서 제공하며 사용자가 설정한 경로 내에서 가장 첫 번째 파일을 찾는 함수
	intptr_t handle = _findfirst("../Bin/Resources/Models/Chunin_Parts/*", &fd);

	if (handle == -1)
		return E_FAIL;

	int iResult = 0;

	char szCurPath[128] = "../Bin/Resources/Models/Chunin_Parts/";	 // 상대 경로
	char szFullPath[128] = "";

	_wstring strPrototype = TEXT("Part");
	_uint iNum = 0;

	while (iResult != -1)
	{
		strcpy_s(szFullPath, szCurPath);
		strcat_s(szFullPath, fd.name);

		if (!strcmp(fd.name, ".") || !strcmp(fd.name, ".."))
		{
			iResult = _findnext(handle, &fd);
			continue;
		}

		_wstring strPrototypeName;
		_wstring strNum = to_wstring(iNum++);
		strPrototypeName = strPrototype + strNum;

		PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, strPrototypeName,
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, szFullPath, "../Bin/ModelData/NonAnim/Chunin_Parts/", PreTransformMatrix))))
			return E_FAIL;

		//_findnext : <io.h>에서 제공하며 다음 위치의 파일을 찾는 함수, 더이상 없다면 -1을 리턴
		iResult = _findnext(handle, &fd);
	}

	return S_OK;
}

HRESULT CLoader::Reayd_Effect()
{
	_matrix		PreTransformMatrix = XMMatrixIdentity();

	///* For. Prototype_Component_Model_Rasengan_CreateSmoke*/
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Rasengan_CreateSmoke"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Effect/Rasengan_CreateSmoke.fbx",
	//		"../Bin/ModelData/Effect/", PreTransformMatrix))))
	//	return E_FAIL;

	//	/* For. Prototype_Component_Model_Rasengan_RunSmoke*/
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Rasengan_RunSmoke"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Effect/Rasengan_RunSmoke.fbx",
	//		"../Bin/ModelData/Effect/", PreTransformMatrix))))
	//	return E_FAIL;

	///* For. Prototype_Component_Model_Rasengan_Center*/
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Rasengan_Center"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Effect/Rasengan_Center.fbx",
	//		"../Bin/ModelData/Effect/", PreTransformMatrix))))
	//	return E_FAIL;

	///* For. Prototype_Component_Model_RasenShuriken_Blade*/
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_RasenShuriken_Blade"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Effect/RasenShuriken_Blade.fbx",
	//		"../Bin/ModelData/Effect/", PreTransformMatrix))))
	//	return E_FAIL;

	///* For. Prototype_Component_Model_RasenShuriken_Circle*/
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_RasenShuriken_Circle"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Effect/RasenShuriken_Circle.fbx",
	//		"../Bin/ModelData/Effect/", PreTransformMatrix))))
	//	return E_FAIL;

	///* For. Prototype_Component_Model_RasenShuriken_BladeFly*/
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_RasenShuriken_BladeFly"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Effect/RasenShuriken_BladeFly.fbx",
	//		"../Bin/ModelData/Effect/", PreTransformMatrix))))
	//	return E_FAIL;

	///* For. Prototype_Component_Model_RasenShuriken_BladeLine*/
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_RasenShuriken_BladeLine"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Effect/RasenShuriken_BladeLine.fbx",
	//		"../Bin/ModelData/Effect/", PreTransformMatrix))))
	//	return E_FAIL;

	///* For. Prototype_Component_Model_MassiveRasengan*/
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_MassiveRasengan"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Effect/MassiveRasengan.fbx",
	//		"../Bin/ModelData/Effect/", PreTransformMatrix))))
	//	return E_FAIL;

	///* For. Prototype_Component_Model_MassiveRasengan*/
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_MassiveRasengan"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Effect/MassiveRasengan.fbx",
	//		"../Bin/ModelData/Effect/", PreTransformMatrix))))
	//	return E_FAIL;

	/* For. Prototype_Component_Model_FireBall*/
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_FireBall"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Effect/FireBall.fbx",
	//		"../Bin/ModelData/Effect/", PreTransformMatrix))))
	//	return E_FAIL;

	///* For. Prototype_Component_Model_FireBallEffect*/
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_FireBallEffect"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Effect/FireBall_Effect.fbx",
	//		"../Bin/ModelData/Effect/", PreTransformMatrix))))
	//	return E_FAIL;

	///* For. Prototype_Component_Model_FireFlySmoke*/
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_FireFlySmoke"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Effect/FireBall_FlySmoke.fbx",
	//		"../Bin/ModelData/Effect/", PreTransformMatrix))))
	//	return E_FAIL;
	return S_OK;
}

HRESULT CLoader::Ready_Lightning()
{
	_matrix		PreTransformMatrix = XMMatrixIdentity();

	/* For. Prototype_Component_Model_LightningA1*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_LightningA1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Effect/Lightning/LightningA01.fbx",
			"../Bin/ModelData/Effect/", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_LightningA2*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_LightningA2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Effect/Lightning/LightningA02.fbx",
			"../Bin/ModelData/Effect/", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_LightningB1*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_LightningB1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Effect/Lightning/LightningB01.fbx",
			"../Bin/ModelData/Effect/", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_LightningB2*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_LightningB2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Effect/Lightning/LightningB02.fbx",
			"../Bin/ModelData/Effect/", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_LightningB3*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_LightningB3"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Effect/Lightning/LightningB03.fbx",
			"../Bin/ModelData/Effect/", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_LightningB4*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_LightningB4"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Effect/Lightning/LightningB04.fbx",
			"../Bin/ModelData/Effect/", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_LightningC*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_LightningC"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Effect/Lightning/LightningC.fbx",
			"../Bin/ModelData/Effect/", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_LightningD*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_LightningD"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Effect/Lightning/LightningD.fbx",
			"../Bin/ModelData/Effect/", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_LightningE*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_LightningE"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Effect/Lightning/LightningE.fbx",
			"../Bin/ModelData/Effect/", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_LightningF*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_LightningF"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Effect/Lightning/LightningF.fbx",
			"../Bin/ModelData/Effect/", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_LightningChidori*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_LightningChidori"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Effect/Lightning/Lightning_Chidori.fbx",
			"../Bin/ModelData/Effect/", PreTransformMatrix))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Ready_Amaterasu()
{
	_matrix		PreTransformMatrix = XMMatrixIdentity();

	/* For. Prototype_Component_Model_Amateras0*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Amateras0"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Effect/Amateras/Amateras0.fbx",
			"../Bin/ModelData/Effect/", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Amateras1*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Amateras1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Effect/Amateras/Amateras1.fbx",
			"../Bin/ModelData/Effect/", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Amateras2*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Amateras2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Effect/Amateras/Amateras2.fbx",
			"../Bin/ModelData/Effect/", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Amateras3*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Amateras3"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Effect/Amateras/Amateras3.fbx",
			"../Bin/ModelData/Effect/", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Amateras4*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Amateras4"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Effect/Amateras/Amateras4.fbx",
			"../Bin/ModelData/Effect/", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Amateras5*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Amateras5"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Effect/Amateras/Amateras5.fbx",
			"../Bin/ModelData/Effect/", PreTransformMatrix))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Ready_TypeZeorSkill()
{
	_matrix		PreTransformMatrix = XMMatrixIdentity();

	/* For. Prototype_Component_Model_TypeZero0 */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_TypeZero0"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/TypeZero/TypeZero0.fbx", "../Bin/ModelData/NonAnim/TypeZero/", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_TypeZero1 */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_TypeZero1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/TypeZero/TypeZero1.fbx", "../Bin/ModelData/NonAnim/TypeZero/", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_TypeZero2 */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_TypeZero2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/TypeZero/TypeZero2.fbx", "../Bin/ModelData/NonAnim/TypeZero/", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_TypeZero3 */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_TypeZero3"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/TypeZero/TypeZero3.fbx", "../Bin/ModelData/NonAnim/TypeZero/", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_TypeZero4 */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_TypeZero4"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/TypeZero/TypeZero4.fbx", "../Bin/ModelData/NonAnim/TypeZero/", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_TypeZero05 */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_TypeZero05"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/TypeZero/TypeZero5.fbx", "../Bin/ModelData/NonAnim/TypeZero/", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_TypeZero6 */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_TypeZero6"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/TypeZero/TypeZero6.fbx", "../Bin/ModelData/NonAnim/TypeZero/", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_TypeZero7 */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_TypeZero7"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/TypeZero/TypeZero7.fbx", "../Bin/ModelData/NonAnim/TypeZero/", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_TypeZero8 */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_TypeZero8"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/TypeZero/TypeZero8.fbx", "../Bin/ModelData/NonAnim/TypeZero/", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_TypeZero9 */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_TypeZero9"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/TypeZero/TypeZero9.fbx", "../Bin/ModelData/NonAnim/TypeZero/", PreTransformMatrix))))
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
