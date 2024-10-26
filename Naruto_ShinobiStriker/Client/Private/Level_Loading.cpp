#include "stdafx.h"
#include "..\Public\Level_Loading.h"

#include "Loader.h"
#include "Level_Logo.h"
#include "Level_Custom.h"
#include "Level_Ame.h"
#include "Level_Konoha.h"
#include "Level_Chunin.h"

#include "Background_Loading.h"
#include "GameInstance.h"

CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel { pDevice, pContext }
{
}

HRESULT CLevel_Loading::Initialize(LEVELID eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	m_pLoader = CLoader::Create(m_pDevice, m_pContext, m_eNextLevelID);
	if (nullptr == m_pLoader)
		return E_FAIL;

	/* For. Prototype_Component_Texture_BackGround_Loading */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOADING, TEXT("Prototype_Component_Texture_BackGround_Loading"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Loading/LoadingScreen_%d.dds"), 4))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_LOADING, TEXT("Layer_BackGround"),
		TEXT("Prototype_GameObject_BackGround_Loading"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround()))
		return E_FAIL;

	m_pGameInstance->Stop_BGM();

	return S_OK;
}

void CLevel_Loading::Update(_float fTimeDelta)
{
	
	if ((KEY_TAP(KEY::SPACE)) &&
		true == m_pLoader->isFinished())
	{
		CLevel*			pNewLevel = { nullptr };
		//m_pGameInstance->Set_CurLevelIndex(m_eNextLevelID);

		switch (m_eNextLevelID)
		{
		case LEVEL_LOGO:
			pNewLevel = CLevel_Logo::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_CUSTOM:
			pNewLevel = CLevel_Custom::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_AME:
			pNewLevel = CLevel_Ame::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_KONOHA:
			pNewLevel = CLevel_Konoha::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_CHUNIN:
			pNewLevel = CLevel_Chunin::Create(m_pDevice, m_pContext);
			break;
		}

		if (nullptr == pNewLevel)
			return;

		if (FAILED(m_pGameInstance->Change_Level(m_eNextLevelID, pNewLevel)))
			return;
	}	
}

HRESULT CLevel_Loading::Render()
{
	m_pLoader->Draw_LoadingText();

	return S_OK;
}

HRESULT CLevel_Loading::Ready_Layer_BackGround()
{
	//if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_LOGO, TEXT("Layer_BackGround"),
	//	TEXT("Prototype_GameObject_BackGround"))))
	//	return E_FAIL;

	return S_OK;
}

CLevel_Loading * CLevel_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevelID)
{
	CLevel_Loading*		pInstance = new CLevel_Loading(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Loading"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Loading::Free()
{
	__super::Free();

	Safe_Release(m_pLoader);
}
