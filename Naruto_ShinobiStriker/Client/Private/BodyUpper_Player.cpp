#include "stdafx.h"
#include "BodyUpper_Player.h"

#include "Player_Custom.h"

#include "GameInstance.h"

CBodyUpper_Player::CBodyUpper_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CBodyUpper_Player::CBodyUpper_Player(const CBodyUpper_Player& Prototype)
	: CPartObject{ Prototype }
{
}

const _float4x4* CBodyUpper_Player::Get_BoneMatrix_Ptr(const _char* pBoneName) const
{
	return m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(pBoneName);
}

HRESULT CBodyUpper_Player::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBodyUpper_Player::Initialize(void* pArg)
{
	PARTOBJ_DESC* pDesc = static_cast<PARTOBJ_DESC*>(pArg);
	m_eID = (UPPER_ID)pDesc->iPartID;

	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	//m_pModelCom->SetUp_Animation(3, true);

	return S_OK;
}

void CBodyUpper_Player::Priority_Update(_float fTimeDelta)
{

}

void CBodyUpper_Player::Update(_float fTimeDelta)
{

}

void CBodyUpper_Player::Late_Update(_float fTimeDelta)
{
	/* 직교투영을 위한 월드행렬까지 셋팅하게 된다. */
	__super::Late_Update(fTimeDelta);

	XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * XMLoadFloat4x4(m_pParentMatrix));

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_SHADOWOBJ, this);
}

HRESULT CBodyUpper_Player::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(__super::Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
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

	return S_OK;
}

HRESULT CBodyUpper_Player::Render_LightDepth()
{
	if (FAILED(__super::Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	_float4x4		ViewMatrix;
	XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(m_pGameInstance->Get_LigthDepthPosition(), m_pGameInstance->Get_LigthDepthAt(), XMVectorSet(0.f, 1.f, 0.f, 0.f)));

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i)))
			return E_FAIL;
		//if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", TEXTURE_TYPE::NORMALS, i)))
		//	return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(2)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render((_uint)i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CBodyUpper_Player::Change_Parts(_int iID, CModel* pAnimModel)
{
	if (m_iLevelIndex == LEVEL_CUSTOM)
	{
		if (!m_isModel[iID])
		{
			m_isModel[iID] = true;
			switch (iID)
			{
			case Client::CBodyUpper_Player::UPPER_TRAINER1:
				m_Models[iID] = (dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(m_iLevelIndex, TEXT("Prototype_Component_Model_BodyUpper_Trainer1"))));
				break;
			case Client::CBodyUpper_Player::UPPER_TRAINER2:
				m_Models[iID] = (dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(m_iLevelIndex, TEXT("Prototype_Component_Model_BodyUpper_Trainer2"))));
				break;
			case Client::CBodyUpper_Player::UPPER_ANBU:
				m_Models[iID] = (dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(m_iLevelIndex, TEXT("Prototype_Component_Model_Upper_Anbu"))));
				break;
			case Client::CBodyUpper_Player::UPPER_ARMOR:
				m_Models[iID] = (dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(m_iLevelIndex, TEXT("Prototype_Component_Model_Upper_Armor"))));
				break;
			case Client::CBodyUpper_Player::UPPER_HAORI:
				m_Models[iID] = (dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(m_iLevelIndex, TEXT("Prototype_Component_Model_Upper_Haori"))));
				break;
			case Client::CBodyUpper_Player::UPPER_HOODIE:
				m_Models[iID] = (dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(m_iLevelIndex, TEXT("Prototype_Component_Model_Upper_Hoodie"))));
				break;
			case Client::CBodyUpper_Player::UPPER_SHIRT:
				m_Models[iID] = (dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(m_iLevelIndex, TEXT("Prototype_Component_Model_Upper_Shirt"))));
				break;
			case Client::CBodyUpper_Player::UPPER_VEST:
				m_Models[iID] = (dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(m_iLevelIndex, TEXT("Prototype_Component_Model_Upper_Vest"))));
				break;


			}
		}

		m_eID = (UPPER_ID)iID;

		if (nullptr == m_Models[iID])
			return E_FAIL;

		m_pModelCom->Change_PartInfo(m_Models[iID], pAnimModel);
	}
	else
	{
		m_pModelCom->Change_PartInfo(m_pModelCom, pAnimModel);
	}

	return S_OK;
}

HRESULT CBodyUpper_Player::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (m_iLevelIndex == LEVEL_CUSTOM)
	{
		/* FOR.Com_Model */
		if (FAILED(__super::Add_Component(m_iLevelIndex, TEXT("Prototype_Component_Model_BodyUpper_Trainer1"),
			TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
			return E_FAIL;

		m_Models.resize(UPPER_END);
		m_eID = UPPER_TRAINER1;

		for (size_t i = 0; i < UPPER_END; ++i)
			m_isModel[i] = false;
	}
	else
	{
		_wstring strPrototypeTag;

		switch (m_eID)
		{
		case Client::CBodyUpper_Player::UPPER_TRAINER1:
			strPrototypeTag = TEXT("Prototype_Component_Model_BodyUpper_Trainer1");
			break;
		case Client::CBodyUpper_Player::UPPER_TRAINER2:
			strPrototypeTag = TEXT("Prototype_Component_Model_BodyUpper_Trainer2");
			break;
		case Client::CBodyUpper_Player::UPPER_ANBU:
			strPrototypeTag = TEXT("Prototype_Component_Model_Upper_Anbu");
			break;
		case Client::CBodyUpper_Player::UPPER_ARMOR:
			strPrototypeTag = TEXT("Prototype_Component_Model_Upper_Armor");
			break;
		case Client::CBodyUpper_Player::UPPER_HAORI:
			strPrototypeTag = TEXT("Prototype_Component_Model_Upper_Haori");
			break;
		case Client::CBodyUpper_Player::UPPER_HOODIE:
			strPrototypeTag = TEXT("Prototype_Component_Model_Upper_Hoodie");
			break;
		case Client::CBodyUpper_Player::UPPER_SHIRT:
			strPrototypeTag = TEXT("Prototype_Component_Model_Upper_Shirt");
			break;
		case Client::CBodyUpper_Player::UPPER_VEST:
			strPrototypeTag = TEXT("Prototype_Component_Model_Upper_Vest");
			break;
		}

		if (FAILED(__super::Add_Component(m_iLevelIndex, strPrototypeTag,
			TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
			return E_FAIL;
	}

	return S_OK;
}

CBodyUpper_Player* CBodyUpper_Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBodyUpper_Player* pInstance = new CBodyUpper_Player(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBodyUpper_Player"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CBodyUpper_Player::Clone(void* pArg)
{
	CBodyUpper_Player* pInstance = new CBodyUpper_Player(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CBodyUpper_Player"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBodyUpper_Player::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);

	for (auto& Model : m_Models)
		Safe_Release(Model);
	m_Models.clear();

	Safe_Release(m_pModelCom);
}
