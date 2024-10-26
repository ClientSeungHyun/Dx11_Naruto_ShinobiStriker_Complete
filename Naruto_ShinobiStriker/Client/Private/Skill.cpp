#include "stdafx.h"
#include "Skill.h"

#include "GameInstance.h"

CSkill::CSkill(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CSkill::CSkill(const CSkill& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CSkill::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSkill::Initialize(void* pArg)
{
	SKILL_DESC* SkillDesc = nullptr != pArg ? static_cast<SKILL_DESC*>(pArg) : nullptr;

	/* 직교투영을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(SkillDesc)))
		return E_FAIL;

	if (nullptr != SkillDesc)
		m_pOwner = SkillDesc->pOwner;

	return S_OK;
}

void CSkill::Priority_Update(_float fTimeDelta)
{
}

void CSkill::Update(_float fTimeDelta)
{
	m_fCurrentTime += fTimeDelta;
}

void CSkill::Late_Update(_float fTimeDelta)
{
}

HRESULT CSkill::Render()
{
	return S_OK;
}

void CSkill::Find_NearTarget()
{
	if (nullptr != m_pNearTarget)
	{
		Safe_Release(m_pNearTarget);
		m_pNearTarget = nullptr;
	}

	m_pNearTarget = m_pGameInstance->Find_NearObject(this, m_pGameInstance->Get_CurLevelIndex(), TEXT("Layer_Monster"));
	if (nullptr != m_pNearTarget)
		Safe_AddRef(m_pNearTarget);
	else
		m_pNearTarget = nullptr;
}

void CSkill::Move_Dir(_fvector vDir, _float fSpeed, _float fTimeDelta)
{
	m_pTransformCom->Turn_Lerp(vDir, 10.0f, fTimeDelta);
	m_pTransformCom->Go_Straight(fSpeed, fTimeDelta, nullptr);
}

void CSkill::Chanse_Target(_float fTimeDelta)
{
	if (!m_pNearTarget)
		return;

	_vector vPos = Get_Position();
	_vector vTargetPos = m_pNearTarget->Get_Position();
	//vTargetPos.y += 1.5f;

	_vector vChaseDir = XMVector3Normalize(vTargetPos - vPos);

	m_pTransformCom->Turn_Lerp(vChaseDir, 1.f, fTimeDelta);
	m_pTransformCom->LookAt_Forward(vChaseDir);

	m_pTransformCom->Go_Straight(13.0f, fTimeDelta);
}

HRESULT CSkill::Ready_Components()
{
	/* FOR.Com_Sound */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sound"),
		TEXT("Com_Sound"), reinterpret_cast<CComponent**>(&m_pSoundCom))))
		return E_FAIL;
	m_pSoundCom->Set_Owner(this);

	return S_OK;
}

CSkill* CSkill::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	return nullptr;
}

CGameObject* CSkill::Clone(void* pArg)
{
	return nullptr;
}

void CSkill::Free()
{
	__super::Free();

	Safe_Release(m_pSoundCom);
	Safe_Release(m_pNearTarget);
	Safe_Release(m_pColliderCom);
}
