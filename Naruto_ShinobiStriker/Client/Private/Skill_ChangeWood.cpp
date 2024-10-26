#include "stdafx.h"
#include "Skill_ChangeWood.h"

#include "GameInstance.h"
#include "Model.h"
#include "Collider.h"

CSkill_ChangeWood::CSkill_ChangeWood(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSkill{ pDevice, pContext }
{
}

CSkill_ChangeWood::CSkill_ChangeWood(const CSkill_ChangeWood& Prototype)
	: CSkill{ Prototype }
{
}

HRESULT CSkill_ChangeWood::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSkill_ChangeWood::Initialize(void* pArg)
{
	/* 직교투영을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	CGameObject::GAMEOBJECT_DESC	Desc;
	Desc.fRotationPerSec = 0.f;
	Desc.fSpeedPerSec = 0.;
	Desc.iLevelIndex = m_iLevelIndex;

	m_pWood = dynamic_cast<CWood*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Wood"), &Desc));
	if (nullptr == m_pWood)
		return E_FAIL;
	m_pGameInstance->Add_Object_ToLayer(m_iLevelIndex, TEXT("Layer_SkillEffect"), m_pWood);

	m_isActive = false;

	return S_OK;
}

void CSkill_ChangeWood::Priority_Update(_float fTimeDelta)
{
}

void CSkill_ChangeWood::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if (m_fCurrentTime > 0.2f)
	{
		dynamic_cast<CPlayer*>(m_pOwner)->Set_IsInvincible(false);
	}

	if (!m_pWood->IsActive())
		m_isActive = false;
}

void CSkill_ChangeWood::Late_Update(_float fTimeDelta)
{

	m_pGameInstance->Add_ColliderList(m_pColliderCom);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CSkill_ChangeWood::Render()
{

	return S_OK;
}

void CSkill_ChangeWood::Appear()
{
	m_fCurrentTime = 0.f;
	
	dynamic_cast<CPlayer*>(m_pOwner)->Set_IsInvincible(true);

	m_pTransformCom->Set_WorldMatrix(m_pOwner->Get_Transform()->Get_WorldMatrix());
	m_pWood->Appear(m_pTransformCom->Get_WorldMatrix());

	m_isActive = true;
}

HRESULT CSkill_ChangeWood::Ready_Components()
{
	__super::Ready_Components();

	return S_OK;
}

CSkill_ChangeWood* CSkill_ChangeWood::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkill_ChangeWood* pInstance = new CSkill_ChangeWood(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSkill_ChangeWood"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CSkill_ChangeWood::Clone(void* pArg)
{
	CSkill_ChangeWood* pInstance = new CSkill_ChangeWood(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CSkill_ChangeWood"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkill_ChangeWood::Free()
{
	__super::Free();
}
