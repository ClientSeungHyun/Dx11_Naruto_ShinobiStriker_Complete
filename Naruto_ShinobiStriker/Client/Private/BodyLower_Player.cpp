#include "stdafx.h"
#include "BodyLower_Player.h"

#include "Player_Custom.h"

#include "GameInstance.h"

CBodyLower_Player::CBodyLower_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CBodyLower_Player::CBodyLower_Player(const CBodyLower_Player& Prototype)
	: CPartObject{ Prototype }
{
}

const _float4x4* CBodyLower_Player::Get_BoneMatrix_Ptr(const _char* pBoneName) const
{
	return m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(pBoneName);
}

HRESULT CBodyLower_Player::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBodyLower_Player::Initialize(void* pArg)
{
	PARTOBJ_DESC* pDesc = static_cast<PARTOBJ_DESC*>(pArg);
	m_eID = (LOWER_ID)pDesc->iPartID;

	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CBodyLower_Player::Priority_Update(_float fTimeDelta)
{

}

void CBodyLower_Player::Update(_float fTimeDelta)
{

}

void CBodyLower_Player::Late_Update(_float fTimeDelta)
{
	/* 직교투영을 위한 월드행렬까지 셋팅하게 된다. */
	__super::Late_Update(fTimeDelta);

	XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * XMLoadFloat4x4(m_pParentMatrix));

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_SHADOWOBJ, this);
}

HRESULT CBodyLower_Player::Render()
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

HRESULT CBodyLower_Player::Render_LightDepth()
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


HRESULT CBodyLower_Player::Change_Parts(_int eID, CModel* pAnimModel)
{
	if (m_iLevelIndex == LEVEL_CUSTOM)
	{
		if (!m_isModel[eID])
		{
			m_isModel[eID] = true;
			switch (eID)
			{
			case Client::CBodyLower_Player::LOWER_PANTS1:
				m_Models[eID] = (dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(m_iLevelIndex, TEXT("Prototype_Component_Model_BodyLower_Pants1"))));
				break;

			case Client::CBodyLower_Player::LOWER_PANTS2:
				m_Models[eID] = (dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(m_iLevelIndex, TEXT("Prototype_Component_Model_BodyLower_Pants2"))));
				break;

			case Client::CBodyLower_Player::LOWER_ANBU:
				m_Models[eID] = (dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(m_iLevelIndex, TEXT("Prototype_Component_Model_Lower_Anbu"))));
				break;

			case Client::CBodyLower_Player::LOWER_ARMOR:
				m_Models[eID] = (dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(m_iLevelIndex, TEXT("Prototype_Component_Model_Lower_Armor"))));
				break;

			case Client::CBodyLower_Player::LOWER_CUT:
				m_Models[eID] = (dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(m_iLevelIndex, TEXT("Prototype_Component_Model_Lower_PantsCut"))));
				break;

			case Client::CBodyLower_Player::LOWER_SHORT:
				m_Models[eID] = (dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(m_iLevelIndex, TEXT("Prototype_Component_Model_Lower_Short"))));
				break;

			case Client::CBodyLower_Player::LOWER_SKIRT:
				m_Models[eID] = (dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(m_iLevelIndex, TEXT("Prototype_Component_Model_Lower_Skirt"))));
				break;

			case Client::CBodyLower_Player::LOWER_VEST:
				m_Models[eID] = (dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(m_iLevelIndex, TEXT("Prototype_Component_Model_Lower_Vest"))));
				break;
			}
		}


		m_eID = (LOWER_ID)eID;

		if (nullptr == m_Models[eID])
			return E_FAIL;

		m_pModelCom->Change_PartInfo(m_Models[eID], pAnimModel);
	}
	else
	{
		m_pModelCom->Change_PartInfo(m_pModelCom, pAnimModel);
	}

	return S_OK;
}

HRESULT CBodyLower_Player::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (m_iLevelIndex == LEVEL_CUSTOM)
	{
		/* FOR.Com_Model */
		if (FAILED(__super::Add_Component(m_iLevelIndex, TEXT("Prototype_Component_Model_BodyLower_Pants1"),
			TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
			return E_FAIL;

		m_Models.resize(LOWER_END);
		m_eID = LOWER_PANTS1;

		for (size_t i = 0; i < LOWER_END; ++i)
			m_isModel[i] = false;
	}
	else
	{
		_wstring strPrototypeTag;

		switch (m_eID)
		{
		case Client::CBodyLower_Player::LOWER_PANTS1:
			strPrototypeTag = TEXT("Prototype_Component_Model_BodyLower_Pants1");
			break;
		case Client::CBodyLower_Player::LOWER_PANTS2:
			strPrototypeTag = TEXT("Prototype_Component_Model_BodyLower_Pants2");
			break;
		case Client::CBodyLower_Player::LOWER_ANBU:
			strPrototypeTag = TEXT("Prototype_Component_Model_Lower_Anbu");
			break;
		case Client::CBodyLower_Player::LOWER_ARMOR:
			strPrototypeTag = TEXT("Prototype_Component_Model_Lower_Armor");
			break;
		case Client::CBodyLower_Player::LOWER_CUT:
			strPrototypeTag = TEXT("Prototype_Component_Model_Lower_PantsCut");
			break;
		case Client::CBodyLower_Player::LOWER_SHORT:
			strPrototypeTag = TEXT("Prototype_Component_Model_Lower_Short");
			break;
		case Client::CBodyLower_Player::LOWER_SKIRT:
			strPrototypeTag = TEXT("Prototype_Component_Model_Lower_Skirt");
			break;
		case Client::CBodyLower_Player::LOWER_VEST:
			strPrototypeTag = TEXT("Prototype_Component_Model_Lower_Vest");
			break;
		}

		if (FAILED(__super::Add_Component(m_iLevelIndex, strPrototypeTag,
			TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
			return E_FAIL;
	}

	return S_OK;
}

CBodyLower_Player* CBodyLower_Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBodyLower_Player* pInstance = new CBodyLower_Player(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBodyLower_Player"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CBodyLower_Player::Clone(void* pArg)
{
	CBodyLower_Player* pInstance = new CBodyLower_Player(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CBodyLower_Player"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBodyLower_Player::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);

	for (auto& Model : m_Models)
		Safe_Release(Model);
	m_Models.clear();

	Safe_Release(m_pModelCom);
}
