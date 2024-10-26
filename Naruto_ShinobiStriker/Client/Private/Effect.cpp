#include "stdafx.h"
#include "Effect.h"

#include "GameInstance.h"

CEffect::CEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CEffect::CEffect(const CEffect& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CEffect::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CEffect::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CEffect::Priority_Update(_float fTimeDelta)
{
}

void CEffect::Update(_float fTimeDelta)
{
}

void CEffect::Late_Update(_float fTimeDelta)
{
}

HRESULT CEffect::Render()
{
	return S_OK;
}

HRESULT CEffect::Ready_Components()
{


	return S_OK;
}

void CEffect::Look_Camera()
{
	_vector vCameraPos = m_pGameInstance->Get_CamPosition_Vector();

	m_pTransformCom->LookAt(vCameraPos);
}

void CEffect::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

}
