#include "stdafx.h"
#include "..\Public\Terrain.h"

#include "GameInstance.h"
#include "ImGui_Manager.h"
#include "TerrainController.h"

CTerrain::CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CTerrain::CTerrain(const CTerrain& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CTerrain::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTerrain::Initialize(void* pArg)
{
	GAMEOBJECT_DESC Desc{};

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	m_pTerrainController = CTerrainController::Get_Instance();
	Safe_AddRef(m_pTerrainController);

	return S_OK;
}

void CTerrain::Priority_Update(_float fTimeDelta)
{
	int a = 10;
}

void CTerrain::Update(_float fTimeDelta)
{
	CTerrainController::TERRAIN_DESC TerrainDesc = m_pTerrainController->Get_TerrainDesc();
	_vector vPosition = XMVectorSet(TerrainDesc.VIBuffer_Terrain_Desc.fPositionX, TerrainDesc.VIBuffer_Terrain_Desc.fPositionY, TerrainDesc.VIBuffer_Terrain_Desc.fPositionZ, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

	_float3		vPickPos{};

	if (KEY_AWAY(KEY::LBUTTON) &&
		true == m_pVIBufferCom->isPicking(m_pTransformCom->Get_WorldMatrix(), &vPickPos))
	{
		if (CImGui_Manager::Get_Instance()->Get_IsPickingTerrain())
		{
			CImGui_Manager::Get_Instance()->Set_ModelPos(vPickPos);
		}
	}
}

void CTerrain::Late_Update(_float fTimeDelta)
{
	/* 직교투영을 위한 월드행렬까지 셋팅하게 된다. */
	__super::Late_Update(fTimeDelta);


	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CTerrain::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 1)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;


	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

_bool CTerrain::Is_Pick(_float3* vPickPos)
{
	if (true == m_pVIBufferCom->isPicking(m_pTransformCom->Get_WorldMatrix(), vPickPos))
	{
		m_vPickPos = *vPickPos;
		CTerrainController::Get_Instance()->Set_PickPos(*vPickPos);

		return true;
	}

	return false;
}

HRESULT CTerrain::Ready_Components(void* pArg)
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxNorTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	CVIBuffer_Terrain::VIBUFFER_TERRAIN_DESC VIBuffer_TerrainDesc = (static_cast<CTerrainController::TERRAIN_DESC*>(pArg)->VIBuffer_Terrain_Desc);

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain_Normal"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), &VIBuffer_TerrainDesc)))
		return E_FAIL;

	return S_OK;
}

CTerrain* CTerrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTerrain* pInstance = new CTerrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CTerrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CTerrain::Clone(void* pArg)
{
	CTerrain* pInstance = new CTerrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CTerrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTerrain::Free()
{
	__super::Free();


	Safe_Release(m_pTerrainController);

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
