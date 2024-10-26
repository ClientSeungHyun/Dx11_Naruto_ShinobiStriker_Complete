#include "stdafx.h"
#include "BodyOnce_Player.h"

#include "Player_Custom.h"
#include "GameInstance.h"
#include "Player_MotionTrail.h"

CBodyOnce_Player::CBodyOnce_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CBodyOnce_Player::CBodyOnce_Player(const CBodyOnce_Player& Prototype)
	: CPartObject{ Prototype }
{
}

const _float4x4* CBodyOnce_Player::Get_BoneMatrix_Ptr(const _char* pBoneName) const
{
	return m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(pBoneName);
}

HRESULT CBodyOnce_Player::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBodyOnce_Player::Initialize(void* pArg)
{
	PARTOBJ_DESC* pDesc = static_cast<PARTOBJ_DESC*>(pArg);
	m_eID = (ONCE_ID)pDesc->iPartID;

	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	//m_pModelCom->SetUp_Animation(3, true);
	
	if (m_iLevelIndex != LEVEL_CUSTOM)
	{
		if (FAILED(Ready_MotionTrail()))
			return E_FAIL;
	}

	return S_OK;
}

void CBodyOnce_Player::Priority_Update(_float fTimeDelta)
{
	int a = 10;
}

void CBodyOnce_Player::Update(_float fTimeDelta)
{
	if (nullptr != m_pMotionTrail)
	{
		m_pMotionTrail->Update(fTimeDelta);

		m_fMotionTrailTime += fTimeDelta;
		if (m_isRenderMotionTrail && m_fMotionTrailTime >= 0.2f)
		{
			m_pMotionTrail->Add_Info(m_BoneMatrices, m_WorldMatrix);
			m_fMotionTrailTime = 0.f;
		}
	}
}

void CBodyOnce_Player::Late_Update(_float fTimeDelta)
{
	if (nullptr != m_pMotionTrail)
	{
		m_pMotionTrail->Late_Update(fTimeDelta);
	}

	/* 직교투영을 위한 월드행렬까지 셋팅하게 된다. */
	__super::Late_Update(fTimeDelta);

	XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * XMLoadFloat4x4(m_pParentMatrix));

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_SHADOWOBJ, this);


	//Render_MotionTrai();

}

HRESULT CBodyOnce_Player::Render()
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
		if(m_iLevelIndex != LEVEL_CUSTOM)
			m_pModelCom->Bind_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i, m_BoneMatrices[i]);
		else
			m_pModelCom->Bind_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i)))
			return E_FAIL;
		//if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", TEXTURE_TYPE::NORMALS, i)))
		//	return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render((_uint)i)))
			return E_FAIL;
	}

	if (m_pMotionTrail == nullptr)
		return S_OK;

	_uint		iMotionCount = m_pMotionTrail->Get_MotionCount();
	if (iMotionCount <= 0 )
		return S_OK;

	for (_uint i = 0; i < m_pMotionTrail->Get_MotionCount(); ++i)
	{
		_float4x4* WorldMatrix = m_pMotionTrail->Get_WorldMatrix(i);
		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", WorldMatrix)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", m_pMotionTrail->Get_Alpha(i), sizeof(_float))))
			return E_FAIL;


		for (size_t j = 0; j < iNumMeshes; j++)
		{
			_float4x4* BoneMatrix = m_pMotionTrail->Get_BoneMatrix(i)[j];
			if (FAILED(m_pShaderCom->Bind_Matrices("g_BoneMatrices", BoneMatrix, m_pModelCom->Get_NumBone((_uint)j))))
				return E_FAIL;

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, (_uint)j)))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(4)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Render((_uint)j)))
				return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CBodyOnce_Player::Render_LightDepth()
{
	if (!m_isRender)
		return E_FAIL;

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

void CBodyOnce_Player::On_MotionTrail()
{
	m_fMotionTrailTime = 0.f;
	m_isRenderMotionTrail = true;

}

void CBodyOnce_Player::Off_Motiontrail()
{
	m_fMotionTrailTime = 0.f;
	m_isRenderMotionTrail = false;
}

HRESULT CBodyOnce_Player::Change_Parts(_int iID)
{
	if (!m_isModel[iID])
	{
		m_isModel[iID] = true;
		switch (iID)
		{
		case Client::CBodyOnce_Player::ONCE_HOKAGE:
			m_isModel[ONCE_HOKAGE] = true;
			m_Models[iID] = (dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Once_Hokage4"))));
			break;

		case Client::CBodyOnce_Player::ONCE_FINALCOAT:
			m_isModel[ONCE_FINALCOAT] = true;
			m_Models[iID] = (dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(m_iLevelIndex, TEXT("Prototype_Component_Model_Once_FinalCoat"))));
			break;

		case Client::CBodyOnce_Player::ONCE_ARMOR:
			m_isModel[ONCE_ARMOR] = true;
			m_Models[iID] = (dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(m_iLevelIndex, TEXT("Prototype_Component_Model_Once_Armor"))));
			break;

		case Client::CBodyOnce_Player::ONCE_JACKET:
			m_isModel[ONCE_JACKET] = true;
			m_Models[iID] = (dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(m_iLevelIndex, TEXT("Prototype_Component_Model_Once_Jacket"))));
			break;

		case Client::CBodyOnce_Player::ONCE_SANTA:
			m_isModel[ONCE_SANTA] = true;
			m_Models[iID] = (dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(m_iLevelIndex, TEXT("Prototype_Component_Model_Once_Santa"))));
			break;

		case Client::CBodyOnce_Player::ONCE_SENNIN:
			m_isModel[ONCE_SENNIN] = true;
			m_Models[iID] = (dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(m_iLevelIndex, TEXT("Prototype_Component_Model_Once_Sennin"))));
			break;

		case Client::CBodyOnce_Player::ONCE_TRACK:
			m_isModel[ONCE_TRACK] = true;
			m_Models[iID] = (dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(m_iLevelIndex, TEXT("Prototype_Component_Model_Once_Track"))));
			break;

		case Client::CBodyOnce_Player::ONCE_YUUKDO:
			m_isModel[ONCE_YUUKDO] = true;
			m_Models[iID] = (dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(m_iLevelIndex, TEXT("Prototype_Component_Model_Once_Yuukdo"))));
			break;
		}
	}

	m_eID = ONCE_ID(iID);

	if (nullptr == m_Models[iID])
		return E_FAIL;

	m_pModelCom->Change_PartInfo(m_Models[iID]);

	return S_OK;
}

HRESULT CBodyOnce_Player::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Once_Hokage4"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;



	if (m_iLevelIndex == LEVEL_CUSTOM)
	{
		m_eID = ONCE_HOKAGE;

		m_Models.resize(ONCE_END);

		for (size_t i = 0; i < ONCE_END; ++i)
			m_isModel[i] = false;
	}
	else
	{
		switch (m_eID)
		{
		case Client::CBodyOnce_Player::ONCE_HOKAGE:
			Safe_Release(m_pModelCom);
			return S_OK;
		case Client::CBodyOnce_Player::ONCE_FINALCOAT:
			m_Models.emplace_back(dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Once_FinalCoat"))));
			break;
		case Client::CBodyOnce_Player::ONCE_ARMOR:
			m_Models.emplace_back(dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(m_iLevelIndex, TEXT("Prototype_Component_Model_Once_Armor"))));
			break;
		case Client::CBodyOnce_Player::ONCE_JACKET:
			m_Models.emplace_back(dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(m_iLevelIndex, TEXT("Prototype_Component_Model_Once_Jacket"))));
			break;
		case Client::CBodyOnce_Player::ONCE_SANTA:
			m_Models.emplace_back(dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(m_iLevelIndex, TEXT("Prototype_Component_Model_Once_Santa"))));
			break;
		case Client::CBodyOnce_Player::ONCE_SENNIN:
			m_Models.emplace_back(dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(m_iLevelIndex, TEXT("Prototype_Component_Model_Once_Sennin"))));
			break;
		case Client::CBodyOnce_Player::ONCE_TRACK:
			m_Models.emplace_back(dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(m_iLevelIndex, TEXT("Prototype_Component_Model_Once_Track"))));
			break;
		case Client::CBodyOnce_Player::ONCE_YUUKDO:
			m_Models.emplace_back(dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(m_iLevelIndex, TEXT("Prototype_Component_Model_Once_Yuukdo"))));
			break;
		}

		Safe_Release(m_pModelCom);
		m_pModelCom->Change_PartInfo(m_Models.front());
	}

	return S_OK;
}

HRESULT CBodyOnce_Player::Ready_MotionTrail()
{
	m_BoneMatrices = new _float4x4 * [m_pModelCom->Get_NumMeshes()];
	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); ++i)
	{
		m_BoneMatrices[i] = new _float4x4[g_iMaxMeshBones];
	}

	m_pMotionTrail = CPlayer_MotionTrail::Create(m_pDevice, m_pContext, m_pModelCom->Get_NumMeshes());
	return S_OK;
}

CBodyOnce_Player* CBodyOnce_Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBodyOnce_Player* pInstance = new CBodyOnce_Player(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBodyOnce_Player"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CBodyOnce_Player::Clone(void* pArg)
{
	CBodyOnce_Player* pInstance = new CBodyOnce_Player(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CBodyOnce_Player"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBodyOnce_Player::Free()
{
	if (m_pModelCom != nullptr && m_iLevelIndex != LEVEL_CUSTOM)
	{
		for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); ++i)
		{
			Safe_Delete_Array(m_BoneMatrices[i]);
		}
		Safe_Delete_Array(m_BoneMatrices);
	}

	__super::Free();


	Safe_Release(m_pShaderCom);
	Safe_Release(m_pMotionTrail);

	for (auto& Model : m_Models)
		Safe_Release(Model);
	m_Models.clear();

	Safe_Release(m_pModelCom);
}
