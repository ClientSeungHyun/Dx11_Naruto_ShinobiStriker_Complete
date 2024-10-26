#include "stdafx.h"
#include "Skill_Amateras.h"

#include "GameInstance.h"
#include "Model.h"
#include "Collider.h"
#include "Lightning.h"
#include "ObjectPool.h"

#include "Amateras.h"
#include "Fire05.h"

CSkill_Amateras::CSkill_Amateras(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSkill{ pDevice, pContext }
{
}

CSkill_Amateras::CSkill_Amateras(const CSkill_Amateras& Prototype)
	: CSkill{ Prototype }
{
}

HRESULT CSkill_Amateras::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSkill_Amateras::Initialize(void* pArg)
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

	m_isActive = false;

	m_iNumEffect = 80;

	return S_OK;
}

void CSkill_Amateras::Priority_Update(_float fTimeDelta)
{


}

void CSkill_Amateras::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
	m_fAmaterasTime += fTimeDelta;
	m_fFireTime += fTimeDelta;
	m_fDamagedTime += fTimeDelta;

	if (m_pTargetMonster == nullptr || m_pTargetMonster->Get_Dead())
	{
		m_fCurrentTime = 10.f;
	}

	if (m_fCurrentTime >= 8.f
		|| nullptr == m_pTargetMonster)
	{
		DisAppear();
		return;
	}

	if (m_fDamagedTime >= 0.05f)
	{
		dynamic_cast<CMonster*>(m_pTargetMonster)->Damaged(0.5f);
		m_fDamagedTime = 0.f;
	}

	if(m_fCurrentTime >= 0.5f && m_isFirst)
	{
		m_isFirst = false;
		m_fCurrentTime = 0.f;
		Create_Fire05(50);
	}

	if (!m_isFirst)
	{
		if (m_fAmaterasTime >= 0.15f)
		{
			Create_AmaterasEffect(10);
			m_fAmaterasTime = 0.f;
		}

		if (m_fFireTime >= 0.1f)
		{
			Create_Fire05(2);
			m_fFireTime = 0.f;
		}
	}

	_vector vPos = m_pTargetMonster->Get_Position() + XMVectorSet(0.f, 1.f, 0.f, 0.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
}

void CSkill_Amateras::Late_Update(_float fTimeDelta)
{
	m_pSoundCom->Update(fTimeDelta);

	m_pGameInstance->Add_ColliderList(m_pColliderCom);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CSkill_Amateras::Render()
{

	return S_OK;
}

void CSkill_Amateras::Appear(CGameObject* pTargetMonster, _float fScale)
{
	if (m_isActive)
		return;

	m_iNumEffect = 80;
	m_fScale = fScale;

	_vector vPos;
	if (pTargetMonster != nullptr)
	{
		m_pTargetMonster = pTargetMonster;
		vPos = pTargetMonster->Get_Position() + XMVectorSet(0.f, 1.f, 0.f, 0.f);

		if (dynamic_cast<CMonster*>(pTargetMonster)->Get_MonsterID() == KURAMA
			|| dynamic_cast<CMonster*>(pTargetMonster)->Get_MonsterID() == SUSANOO)
			m_isBoss = true;
		else
			m_isBoss = false;
	}
	else
	{
		_vector vLook = XMVector3Normalize(m_pOwner->Get_Transform()->Get_State(CTransform::STATE_LOOK));
		vPos = m_pOwner->Get_Position() + vLook * 5.f;
		m_isBoss = false;
	}
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	

	Create_Fire05(3);


	m_isFirst = true;
	m_isActive = true;

	m_fDamagedTime = 0.f;
	m_fAmaterasTime = 0.f;
	m_fFireTime = 0.f;
	m_fCurrentTime = 0.f;


	m_pSoundCom->Play3D(TEXT("Sarin.wav"), g_fVolume + 0.2f);

}

void CSkill_Amateras::DisAppear()
{
	m_pSoundCom->Stop();
	m_pTargetMonster = nullptr;
	m_isActive = false;
}

HRESULT CSkill_Amateras::Ready_Components()
{
	__super::Ready_Components();

	return S_OK;
}

void CSkill_Amateras::Create_AmaterasEffect(_uint iNum)
{
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vRight = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT));
	_vector vUp = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP));
	_vector vLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
	_float fRange = 1.f;
	_float fScale = m_fScale;
	if (m_isBoss)
	{
		fRange = 2.5f;
		vPos += vUp * 6.f;
		fScale *= 1.5f;
	}

	vPos += vUp * 0.5f + vLook * m_pGameInstance->Get_Random(0.f, 0.1f);
	for (_uint i = 0; i < iNum; ++i)
	{
		_vector vRangePos = vPos + vRight * m_pGameInstance->Get_Random(-fRange, fRange) + vUp * m_pGameInstance->Get_Random(-fRange, fRange);
		CAmateras* pAmateras = CObjectPool<CAmateras>::Get_GameObject();
		pAmateras->Appear(vRangePos, fScale);
	}
}

void CSkill_Amateras::Create_Fire05(_uint iNum)
{
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vRight = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT));
	_vector vUp = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP));
	_vector vLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
	_float fRange = 1.f;
	_float fScale = m_fScale;
	if (m_isBoss)
	{
		fRange = 2.5f;
		vPos += vUp * 6.f;
		fScale *= 1.5f;
	}

	vPos += vUp * 0.5f + vLook * m_pGameInstance->Get_Random(0.f, 0.15f);
	for (_uint i = 0; i < iNum; ++i)
	{
		_vector vRangePos = vPos + vRight * m_pGameInstance->Get_Random(-fRange + 0.4f, fRange - 0.4f) + vUp * m_pGameInstance->Get_Random(-fRange, fRange);
		CFire05* pFire = CObjectPool<CFire05>::Get_GameObject();
		pFire->Appear(vRangePos, fScale);
	}
}

CSkill_Amateras* CSkill_Amateras::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkill_Amateras* pInstance = new CSkill_Amateras(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSkill_Amateras"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CSkill_Amateras::Clone(void* pArg)
{
	CSkill_Amateras* pInstance = new CSkill_Amateras(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CSkill_Amateras"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkill_Amateras::Free()
{
	__super::Free();
}
