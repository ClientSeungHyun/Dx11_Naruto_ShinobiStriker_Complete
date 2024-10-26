#include "stdafx.h"
#include "ModelController.h"

#include "GameInstance.h"

IMPLEMENT_SINGLETON(CModelController)

CModelController::CModelController()
	:m_pGameInstance{ CGameInstance::Get_Instance() }
{
	int a = 0;
}

HRESULT CModelController::Initialize()
{
	return S_OK;
}

void CModelController::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
}
