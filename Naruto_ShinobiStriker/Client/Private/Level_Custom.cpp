#include "stdafx.h"
#include "..\Public\Level_Custom.h"
#include "Level_Loading.h"

#include "UIObject.h"
#include "CustomPannel.h"
#include "UI_ConfirmButton.h"

#include "GameInstance.h"
#include "CustomCamera.h"
#include "PlayerCamera.h"
#include "NonAnimModel.h"
#include "Player_Custom.h"
#include "WhiteZetsu.h"

CLevel_Custom::CLevel_Custom(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Custom::Initialize()
{
	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera()))
		return E_FAIL;
	if (FAILED(Ready_Layer_BackGround()))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI()))
		return E_FAIL;

	if (FAILED(Ready_CustomPlayer()))
		return E_FAIL;

	m_pCustomPlayer = dynamic_cast<CPlayer_Custom*>(m_pGameInstance->Find_Player(LEVEL_CUSTOM));

	m_pGameInstance->Play_BGM(TEXT("BGM_Custom.wav"), 0.3f);

	return S_OK;
}

void CLevel_Custom::Update(_float fTimeDelta)
{
	if ((KEY_TAP(KEY::LSHIFT)) && m_pConfirmButton->Get_IsSelect())
	{
		if (!m_isSelectConfirm)
		{
			m_pGameInstance->Play_Effect(TEXT("Select1.wav"), g_fVolume);
			m_isSelectConfirm = true;
		}

		if (FAILED(m_pGameInstance->Change_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_CHUNIN))))
			return;
		return;
	}

	if (m_iNextPannelIndex != m_Pannels[m_iCurrentPannelIndex]->Get_SelectIndex() + 1)
	{
		m_iNextPannelIndex = m_Pannels[m_iCurrentPannelIndex]->Get_SelectIndex() + 1;

		if (m_iCurrentPannelIndex != CCustomPannel::PANNEL_MAIN)
		{
			switch (m_iCurrentPannelIndex)
			{
			case CCustomPannel::PANNEL_HEAD:
				m_pCustomPlayer->Change_Head(m_iNextPannelIndex - 1);
				break;
			case CCustomPannel::PANNEL_UPPER:
				m_pCustomPlayer->Change_Upper(m_iNextPannelIndex - 1);
				break;
			case CCustomPannel::PANNEL_LOWER:
				m_pCustomPlayer->Change_Lower(m_iNextPannelIndex - 1);
				break;
			case CCustomPannel::PANNEL_ONCE:
				m_pCustomPlayer->Change_Once(m_iNextPannelIndex - 1);
				break;
			case CCustomPannel::PANNEL_FACE:
				m_pCustomPlayer->Change_Face(m_iNextPannelIndex - 1);
				break;
			case CCustomPannel::PANNEL_EYE:
				m_pCustomPlayer->Change_Eye(m_iNextPannelIndex - 1);
				break;
			default:
				break;
			}
		}
	}

	if ((KEY_TAP(KEY::ENTER)) && !m_isChanging)
	{
		m_pGameInstance->Play_Effect(TEXT("Select1.wav"), g_fVolume);
		m_Pannels[m_iCurrentPannelIndex]->Start_Disappear();
		m_isChanging = true;
	}

	if (KEY_TAP(KEY::LSHIFT))
	{
		m_pGameInstance->Play_Effect(TEXT("Select2.wav"), g_fVolume);
		m_Pannels[m_iCurrentPannelIndex]->Clear_Select();
		m_pConfirmButton->Set_IsSelect(true);
	}

	if (m_isChanging)
	{
		if (!m_Pannels[m_iCurrentPannelIndex]->IsActive())
		{
			if (m_iCurrentPannelIndex != CCustomPannel::PANNEL_MAIN)
			{
				m_iCurrentPannelIndex = CCustomPannel::PANNEL_MAIN;
			}
			else
			{
				m_iCurrentPannelIndex = m_iNextPannelIndex;
			}

			m_Pannels[m_iCurrentPannelIndex]->Start_Appear();
			m_isChanging = false;
		}
	}

	if (m_iCurrentPannelIndex == CCustomPannel::PANNEL_MAIN)
	{
		m_pConfirmButton->IsActive(true);
	}
	else
	{
		m_pConfirmButton->IsActive(false);
	}
}

HRESULT CLevel_Custom::Render()
{
	return S_OK;
}
HRESULT CLevel_Custom::Ready_Lights()
{
	m_pGameInstance->Clear_Light();

	LIGHT_DESC			LightDesc{};

	ZeroMemory(&LightDesc, sizeof LightDesc);
	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	// 스태틱 라이트 설정
	m_vStaticLightDepthPos = _float4(31.f, -5.f, 6.7f, 1.f);
	m_vStaticLightDepthAt = _float4(0.f, -35.f, 0.f, 1.f);
	m_pGameInstance->Set_StaticLightViewMatrix(XMMatrixLookAtLH(XMLoadFloat4(&m_vStaticLightDepthPos), XMLoadFloat4(&m_vStaticLightDepthAt), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	m_pGameInstance->Load_StaticLightDepthTexture(TEXT("../Bin/DataFiles/StaticLight/StaticLighDepth_Chunin.dds"));

	return S_OK;
}

HRESULT CLevel_Custom::Ready_Layer_Camera()
{
	CCustomCamera::CAMERA_CUSTOM_DESC	Desc{};

	Desc.vEye = _float4(0.f, 0.f, -10.f, 1.f);
	Desc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	Desc.fFovy = XMConvertToRadians(60.0f);
	Desc.fAspect = (_float)g_iWinSizeX / (_float)g_iWinSizeY;
	Desc.fNear = 0.1f;
	Desc.fFar = 1000.f;
	Desc.fSpeedPerSec = 30.f;
	Desc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_CUSTOM, TEXT("Layer_Camera"), TEXT("Prototype_GameObject_CustomCamera"), &Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Custom::Ready_Layer_BackGround()
{
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_CUSTOM, TEXT("Layer_BackGround"),
		TEXT("Prototype_GameObject_BackGround_Custom"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Custom::Ready_CustomPlayer()
{
	CPlayer_Custom::CUSTOM_DESC PlayerDesc;
	PlayerDesc.vPosition = _float3(1.0f, -1.f, -8.0f);
	PlayerDesc.vRotation = _float3(0.f, XMConvertToRadians(180.f), 0.f);
	PlayerDesc.iLevelIndex = LEVEL_CUSTOM;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_CUSTOM, TEXT("Layer_Player"), TEXT("Prototype_GameObject_PlayerCustom"), &PlayerDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Custom::Ready_Layer_UI()
{
	CCustomPannel::PANNEL_DESC			Desc{};
	Desc.fX = (g_iWinSizeX >> 2) + 20.f;
	Desc.fY = g_iWinSizeY >> 1;
	Desc.fSizeX = (g_iWinSizeX >> 1) - 160.f;
	Desc.fSizeY = g_iWinSizeY * 0.8f;
	Desc.iDepth = 1;
	Desc.isChild = false;
	Desc.iLevelIndex = LEVEL_CUSTOM;
	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(90.0f);

	CCustomPannel* pCustomPannel = nullptr;

	for (_uint i = 0; i < CCustomPannel::PANNEL_END; ++i)
	{
		Desc.eType = CCustomPannel::CUSTOMPANNES_TYPE(i);
		
		switch (Desc.eType)
		{
		case  CCustomPannel::PANNEL_MAIN:
			Desc.iNumButton = 6;
			break;
		case  CCustomPannel::PANNEL_FACE:
			Desc.iNumButton = 4;
			break;
		default:
			Desc.iNumButton = 8;
			break;
		}
		pCustomPannel = dynamic_cast<CCustomPannel*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_UI_CustomPannel"), &Desc));
		if (nullptr == pCustomPannel)
			return E_FAIL;

		m_pGameInstance->Add_Object_ToLayer(LEVEL_CUSTOM, TEXT("Layer_UI"), pCustomPannel);
		m_Pannels.emplace_back(pCustomPannel);
		Safe_AddRef(pCustomPannel);
	}

	Desc.fSizeX = 400.f;
	Desc.fSizeY = 55.f;
	Desc.fX = 340;
	Desc.fY = 670;
	m_pConfirmButton = dynamic_cast<CUI_ConfirmButton*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_UI_ConfirmButton"), &Desc));
	if (nullptr == pCustomPannel)
		return E_FAIL;

	m_pGameInstance->Add_Object_ToLayer(LEVEL_CUSTOM, TEXT("Layer_UI"), m_pConfirmButton);

	m_iCurrentPannelIndex = 0;

	return S_OK;
}

CLevel_Custom* CLevel_Custom::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Custom* pInstance = new CLevel_Custom(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_GamePlay"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Custom::Free()
{
	//m_pGameInstance->Reset_PhsyX();

	__super::Free();

	CObjectPool<CParticle_Snow>::Free();

	for (auto& Pannel : m_Pannels)
		Safe_Release(Pannel);
	m_Pannels.clear();
}
