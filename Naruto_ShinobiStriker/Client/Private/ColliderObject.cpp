#include "stdafx.h"
#include "ColliderObject.h"

#include "GameInstance.h"

CColliderObject::CColliderObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CGameObject{ pDevice, pContext }
{
}

HRESULT CColliderObject::Initialize(void* pArg)
{
	OBB_COLLIDEROBJ_DESC* pDesc = static_cast<OBB_COLLIDEROBJ_DESC*>(pArg);
	m_pOwner = (pDesc->pOwner);

	/* 직교투영을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	CBounding_OBB::BOUNDING_OBB_DESC			ColliderDesc_OBB = pDesc->Desc_OBB;
	
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_Attack"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc_OBB)))
		return E_FAIL;
	m_pColliderCom->Set_Owner(this);

	m_vScale = _float3(1.f, 1.f, 1.f);

    return S_OK;
}

void CColliderObject::Priority_Update(_float fTimeDelta)
{
}

void CColliderObject::Update(_float fTimeDelta)
{

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());

}

void CColliderObject::Late_Update(_float fTimeDelta)
{
	m_pColliderCom->IsActive(m_isActive);

	m_pGameInstance->Add_ColliderList(m_pColliderCom);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CColliderObject::Render()
{
	if (!m_isActive)
		return S_OK;

#ifdef _DEBUG
	m_pColliderCom->Render();
#endif

	return S_OK;
}

void CColliderObject::OnCollisionEnter(CGameObject* pOther)
{
}

void CColliderObject::OnCollisionStay(CGameObject* pOther)
{
}

void CColliderObject::OnCollisionExit(CGameObject* pOther)
{
}

void CColliderObject::Scaled(_float fX, _float fY, _float fZ)
{
	m_vScale.x = fX;
	m_vScale.y = fY;
	m_vScale.z = fZ;
}

void CColliderObject::Offset(_float fX, _float fY, _float fZ)
{
	m_vOffset.x = fX;
	m_vOffset.y = fY;
	m_vOffset.z = fZ;
}

void CColliderObject::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
}
