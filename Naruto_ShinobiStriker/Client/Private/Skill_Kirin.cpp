#include "stdafx.h"
#include "Skill_Kirin.h"

#include "GameInstance.h"
#include "Model.h"
#include "Collider.h"
#include "Lightning.h"
#include "ObjectPool.h"

CSkill_Kirin::CSkill_Kirin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSkill{ pDevice, pContext }
{
}

CSkill_Kirin::CSkill_Kirin(const CSkill_Kirin& Prototype)
	: CSkill{ Prototype }
{
}

HRESULT CSkill_Kirin::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSkill_Kirin::Initialize(void* pArg)
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

	m_pKirin = dynamic_cast<CKirin*>(m_pGameInstance->Get_CloneObject_AddLayer(LEVEL_CHUNIN, TEXT("Layer_Skill_Effect"), TEXT("Prototype_GameObject_Kirin"), &Desc));
	if (nullptr == m_pKirin)
		return E_FAIL;

	m_pChidoriCenter = dynamic_cast<CChidori_Center*>(m_pGameInstance->Get_CloneObject_AddLayer(LEVEL_CHUNIN, TEXT("Layer_Skill_Effect"), TEXT("Prototype_GameObject_ChidoriCenter"), &Desc));
	if (nullptr == m_pChidoriCenter)
		return E_FAIL;
	m_pChidoriCenter->Set_Owner(this);

	strcpy_s(m_szPositionBoneName, "LeftHandMiddle1");
	m_pSocketMatrix = dynamic_cast<CPlayer*>(m_pOwner)->Get_Model()->Get_BoneCombindTransformationMatrix_Ptr(m_szPositionBoneName);

	m_isActive = false;
	m_pColliderCom->IsActive(false);

	return S_OK;
}

void CSkill_Kirin::Priority_Update(_float fTimeDelta)
{


}

void CSkill_Kirin::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
	m_fEffectTime += fTimeDelta;

	if (m_fCurrentTime >= 4.5f)
	{
		m_isActive = false;
	}
	
	if (m_fCurrentTime <= 2.7f)
	{
		if (m_fEffectTime >= 0.5f && !m_isExplosion)
		{
			for (_uint i = 0; i <= 30; ++i)
			{
				CObjectPool<CLightning>::Get_GameObject()->Appear(Get_Position(), 0.3f);
			}
			m_fEffectTime = 0.f;
		}
	}
	else
	{
		m_pChidoriCenter->DisAppear();
	}

	_matrix		WorldMatrix = XMMatrixIdentity();
	_matrix		PlayerWorldMatrix = m_pOwner->Get_Transform()->Get_WorldMatrix();

	_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (size_t i = 0; i < 3; i++)
	{
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
	}

	WorldMatrix = SocketMatrix * PlayerWorldMatrix;

	_vector vPosition = WorldMatrix.r[3];
	vPosition = XMVectorSetY(vPosition, XMVectorGetY(vPosition));
	WorldMatrix.r[3] = vPosition;

	m_pTransformCom->Set_WorldMatrix(WorldMatrix);

	if (m_pKirin->Get_Model()->Get_IsEnd_Animation(m_pKirin->Get_Model()->Get_CurrentAnimationIndex()))
	{
		m_pKirin->Disappear();
	}

	if (m_pKirin->Get_Model()->Get_Frame(m_pKirin->Get_Model()->Get_CurrentAnimationIndex()) >= 20)
	{
		m_pColliderCom->IsActive(true);
	}
}

void CSkill_Kirin::Late_Update(_float fTimeDelta)
{
	m_pSoundCom->Update(fTimeDelta);
	m_pColliderCom->Update(m_pKirin->Get_Transform()->Get_WorldMatrix_Ptr());

	m_pGameInstance->Add_ColliderList(m_pColliderCom);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CSkill_Kirin::Render()
{
#ifdef _DEBUG
	m_pColliderCom->Render();
#endif

	return S_OK;
}

void CSkill_Kirin::OnCollisionEnter(CGameObject* pOther)
{
	if (pOther->Get_LayerTag() == TEXT("Layer_Monster"))
	{
		Explosion();
	}
}

void CSkill_Kirin::OnCollisionStay(CGameObject* pOther)
{
}

void CSkill_Kirin::OnCollisionExit(CGameObject* pOther)
{
}

void CSkill_Kirin::Appear(_fvector vKirinPos)
{
	if (m_isActive)
		return;

	m_fCurrentTime = 0.f;
	m_fEffectTime = 1.f;

	m_pKirin->Appear(XMVectorSetY(vKirinPos, XMVectorGetY(vKirinPos) - 5.f));
	m_pChidoriCenter->Appear();

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vKirinPos);

	m_pColliderCom->IsActive(false);
	m_isExplosion = false;
	m_isActive = true;

	m_pSoundCom->Stop();
	m_pSoundCom->Play3D(TEXT("RaseShuriken_Create.wav"), g_fVolume - 0.1f);
}

void CSkill_Kirin::DisAppear()
{
	m_pKirin->Disappear();
	m_isActive = false;
}

void CSkill_Kirin::Explosion()
{
	if (m_isExplosion)
		return;

	m_fCurrentTime = 0.f;
	m_isExplosion = true;

	m_pSoundCom->Stop();
	m_pSoundCom->Play3D(TEXT("RasenShuriken_Explore.wav"), g_fVolume);
}

HRESULT CSkill_Kirin::Ready_Components()
{
	__super::Ready_Components();

	_float3 vInitPos = {};
	XMStoreFloat3(&vInitPos, Get_Position());

	/* For.Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC			ColliderDesc_OBB{};
	ColliderDesc_OBB.vExtents = _float3(5.f, 50.5f, 5.f);
	ColliderDesc_OBB.vCenter = _float3(0.f, 0.7f, 0.1f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc_OBB)))
		return E_FAIL;
	m_pColliderCom->Set_Owner(this);
	m_pColliderCom->Set_ColliderTag(TEXT("PlayerBody"));

	return S_OK;
}

CSkill_Kirin* CSkill_Kirin::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkill_Kirin* pInstance = new CSkill_Kirin(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSkill_Kirin"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CSkill_Kirin::Clone(void* pArg)
{
	CSkill_Kirin* pInstance = new CSkill_Kirin(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CSkill_Kirin"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkill_Kirin::Free()
{
	__super::Free();
}
