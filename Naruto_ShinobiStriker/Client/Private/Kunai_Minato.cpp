#include "stdafx.h"
#include "Kunai_Minato.h"

#include "GameInstance.h"
#include "Camera.h"
#include "Player.h"
#include "Monster.h"
#include "ObjectPool.h"
#include "ExplosionCircle.h"
#include "Effect_RasenkoHit.h"
#include "Effect_Distortion.h"
#include "PlayerCamera.h"
#include "ChidoriTrail.h"

CKuani_Minato::CKuani_Minato(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CKuani_Minato::CKuani_Minato(const CKuani_Minato& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CKuani_Minato::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CKuani_Minato::Initialize(void* pArg)
{
	MINATOKUNAI_DESC* pDesc = static_cast<MINATOKUNAI_DESC*>(pArg);

	m_pWorldMatirx = pDesc->pWorldMatrix;
	m_pSocketMatrix = pDesc->pSocketMatrix;

	/* 직교투영을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	CGameObject::GAMEOBJECT_DESC	Desc;
	Desc.fRotationPerSec = 0.f;
	Desc.fSpeedPerSec = 0.;
	Desc.iLevelIndex = m_iLevelIndex;

	m_pTrail = dynamic_cast<CChidoriTrail*>(m_pGameInstance->Get_CloneObject_AddLayer(LEVEL_CHUNIN, TEXT("Layer_Skill_Effect"), TEXT("Prototype_GameObject_ChidoriTrail"), &Desc));
	if (nullptr == m_pTrail)
		return E_FAIL;
	m_pTrail->Set_TrailSize(0.01f);


	m_isActive = false;

	return S_OK;
}

void CKuani_Minato::Priority_Update(_float fTimeDelta)
{

}

void CKuani_Minato::Update(_float fTimeDelta)
{
	m_fCurrentTime += fTimeDelta;

	if (m_isShoot)
	{
		m_HitEffectTime += fTimeDelta;

		if (m_HitEffectTime > m_HitEffectDuration && !m_isRaijin)
		{
			_float3 vPos;
			XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));


			CObjectPool<CExplosionCircle>::Get_GameObject()->Appear(vPos, _float4(1.f, 0.9f, 0.f, 0.9f), 1.2f, 7.f, 3.f, 0.03f);
			CObjectPool<CExplosionCircle>::Get_GameObject()->Appear(vPos, _float4(1.f, 0.9f, 0.f, 0.9f), 0.1f, 5.f, 2.f, 0.06f);

			CObjectPool<CEffect_Distortion>::Get_GameObject()->Appear(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 0.1f, 1.5f, 5.f, 0.3f, CEffect_Distortion::DIST_CIRCLE);
			
			CObjectPool<CEffect_RasenkoHit>::Get_GameObject()->Appear(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

			dynamic_cast<CPlayerCamera*>(m_pGameInstance->Find_Camera(m_iLevelIndex))->Start_Shake(0.8f, 0.05f);

			m_HitEffectTime = 0.f;
		}
		
		m_pTransformCom->Go_Straight(fTimeDelta, m_fSpeed);
		m_pTrail->Set_TrailPos(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

		m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
	}
	else
	{
		Calculate_Position();
	}
}

void CKuani_Minato::Late_Update(_float fTimeDelta)
{
	if (m_isRaijin)
	{
		if (m_fCurrentTime >= 5.f)
		{
			m_pTrail->Clear();
			m_pTrail->IsActive(false);
			m_pColliderCom->IsActive(false);
			m_isActive = false;
		}
	}

	m_pGameInstance->Add_ColliderList(m_pColliderCom);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONLIGHT, this);
}

HRESULT CKuani_Minato::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i);

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;
		

		if (FAILED(m_pModelCom->Render((_uint)i)))
			return E_FAIL;
	}

#ifdef _DEBUG
	m_pColliderCom->Render();
#endif

	return S_OK;
}

void CKuani_Minato::OnCollisionEnter(CGameObject* pOther)
{
	if (pOther->Get_LayerTag() == TEXT("Layer_Monster"))
	{
		CMonster* pMonster = dynamic_cast<CMonster*>(pOther);
		pMonster->Change_State_GeneralHit();

		if (m_isRaijin)
		{
			m_pTrail->Clear();
			m_pTargetObject = pOther;
			m_isCollision = true;
			m_isActive = false;
		}
	}
}

void CKuani_Minato::Appear()
{
	m_fCurrentTime = 0.f;
	m_isShoot = false;
	m_HitEffectTime = 0.f;
	m_isCollision = false;
	m_pTargetObject = nullptr;

	Calculate_Position();
	m_pTrail->Clear();

	m_isActive = true;
}

void CKuani_Minato::Disappear()
{
	Calculate_Position();
	m_pTrail->IsActive(false);
	m_pTrail->Clear();
	m_pColliderCom->IsActive(false);
	m_isActive = false;
}

void CKuani_Minato::Shoot(_fvector vShootPos, _float fSpeed, _bool isSettingPos, _bool isRaijin)
{
	if (isSettingPos)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vShootPos);

		_matrix		PlayerWorldMatrix = XMLoadFloat4x4(m_pWorldMatirx);
		m_pTransformCom->Set_NewLook(PlayerWorldMatrix.r[2]);
	}
	else
	{
		Calculate_Position();
	}

	m_fSpeed = fSpeed;
	m_isRaijin = isRaijin;


	m_pTrail->Set_TrailPos(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pTrail->Appear(_float4(1.f, 0.96f, 0.02f, 0.9f), 10);

	m_pColliderCom->IsActive(true);

	m_isShoot = true;
}

void CKuani_Minato::Calculate_Position()
{
	_matrix		WorldMatrix = XMMatrixIdentity();
	_matrix		PlayerWorldMatrix = XMLoadFloat4x4(m_pWorldMatirx);

	_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (size_t i = 0; i < 3; i++)
	{
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
	}

	WorldMatrix = SocketMatrix * PlayerWorldMatrix;

	m_pTransformCom->Set_WorldMatrix(WorldMatrix);
	m_pTransformCom->Set_NewLook(PlayerWorldMatrix.r[2]);
}

HRESULT CKuani_Minato::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Model"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_MinatoKuani"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Collider_Body */
	CBounding_OBB::BOUNDING_OBB_DESC			ColliderDesc_OBB{};
	ColliderDesc_OBB.vExtents = _float3(0.2f, 0.2f, 0.2f);
	ColliderDesc_OBB.vCenter = _float3(0.f, 0.f, 0.1f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_Body"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc_OBB)))
		return E_FAIL;
	m_pColliderCom->Set_Owner(this);
	m_pColliderCom->Set_ColliderTag(TEXT("PlayerKunai"));

	return S_OK;
}

CKuani_Minato* CKuani_Minato::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CKuani_Minato* pInstance = new CKuani_Minato(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CKuani_Minato"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CKuani_Minato::Clone(void* pArg)
{
	CKuani_Minato* pInstance = new CKuani_Minato(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CKuani_Minato"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CKuani_Minato::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
