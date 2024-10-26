#include "stdafx.h"

#include "TerrainController.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CTerrainController)

CTerrainController::CTerrainController()
	:m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CTerrainController::Initialize()
{
	return S_OK;
}


void CTerrainController::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
}