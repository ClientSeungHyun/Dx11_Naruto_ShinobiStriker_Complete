#include "stdafx.h"
#include "Skill_Susanoo.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "Player_Custom.h"
#include "ChidoriTrail.h"

#include "ObjectPool.h"
#include "Lightning.h"

#include "PlayerCamera.h"

CSkill_Susanoo::CSkill_Susanoo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CSkill_Susanoo::CSkill_Susanoo(const CSkill_Susanoo& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CSkill_Susanoo::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSkill_Susanoo::Initialize(void* pArg)
{
	/* 직교투영을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	m_iAnimationIndex[SKILL_START] = m_pModelCom->Get_AnimationIndex("D23_Ninjutsu_D23NJ3_susanoo_Start");
	m_iAnimationIndex[SKIL_LOOP] = m_pModelCom->Get_AnimationIndex("D23_Ninjutsu_D23NJ3_susanoo_Loop");
	m_iAnimationIndex[SKILL_END] = m_pModelCom->Get_AnimationIndex("D23_Ninjutsu_D23NJ3_susanoo_End");

	CGameObject::GAMEOBJECT_DESC	Desc;
	Desc.fRotationPerSec = 0.f;
	Desc.fSpeedPerSec = 0.;
	Desc.iLevelIndex = m_iLevelIndex;

	m_pChidoriTrail[0] = dynamic_cast<CChidoriTrail*>(m_pGameInstance->Get_CloneObject_AddLayer(LEVEL_CHUNIN, TEXT("Layer_Skill_Effect"), TEXT("Prototype_GameObject_ChidoriTrail"), &Desc));
	if (nullptr == m_pChidoriTrail[0])
		return E_FAIL;
	m_pChidoriTrail[0]->Set_TrailSize(0.25f);

	m_pChidoriTrail[1] = dynamic_cast<CChidoriTrail*>(m_pGameInstance->Get_CloneObject_AddLayer(LEVEL_CHUNIN, TEXT("Layer_Skill_Effect"), TEXT("Prototype_GameObject_ChidoriTrail"), &Desc));
	if (nullptr == m_pChidoriTrail[1])
		return E_FAIL;
	m_pChidoriTrail[1]->Set_TrailSize(0.08f);

	m_pChidoriTrail[2] = dynamic_cast<CChidoriTrail*>(m_pGameInstance->Get_CloneObject_AddLayer(LEVEL_CHUNIN, TEXT("Layer_Skill_Effect"), TEXT("Prototype_GameObject_ChidoriTrail"), &Desc));
	if (nullptr == m_pChidoriTrail[2])
		return E_FAIL;
	m_pChidoriTrail[2]->Set_TrailSize(0.08f);

	return S_OK;
}

void CSkill_Susanoo::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

	
}

void CSkill_Susanoo::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
	
}

void CSkill_Susanoo::Late_Update(_float fTimeDelta)
{
	if (m_eState < SKILL_END)
	{
		m_fEffectTime += fTimeDelta;

		_matrix WorldMatrix = XMMatrixIdentity();
		_matrix PlayerWorldMatrix = m_pOwner->Get_Transform()->Get_WorldMatrix();

		m_fDissloveRatio -= fTimeDelta;
		if (m_fDissloveRatio <= 0.f)
			m_fDissloveRatio = 0.f;

		for (size_t i = 0; i < 3; i++)
		{
			PlayerWorldMatrix.r[i] = XMVector3Normalize(PlayerWorldMatrix.r[i]);
		}

		// 스사노오 위치
		WorldMatrix = PlayerWorldMatrix;
		WorldMatrix.r[3] = XMVectorSetY(WorldMatrix.r[3], XMVectorGetY(WorldMatrix.r[3]) - 1.f);
		m_pTransformCom->Set_WorldMatrix(WorldMatrix);


		_matrix		SocketMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix(m_pModelCom->Get_BoneIndex("RightHandMiddle1"));

		for (size_t i = 0; i < 3; i++)
		{
			SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
		}
		// 치도리 위치
		_matrix ChidoriWorldMatrix = XMMatrixIdentity();
		ChidoriWorldMatrix = SocketMatrix * PlayerWorldMatrix;
		XMStoreFloat4x4(&m_ChidoriWorldMatrix, ChidoriWorldMatrix);

		m_pChidoriTrail[0]->Appear();
		_vector vTrailPos = ChidoriWorldMatrix.r[3];
		m_pChidoriTrail[0]->Set_TrailPos(vTrailPos);

		for (_uint i = 1; i < 3; ++i)
		{
			m_pChidoriTrail[i]->Appear();
			_vector vTrailPos = ChidoriWorldMatrix.r[3] + XMVector3Normalize(ChidoriWorldMatrix.r[0]) * m_pGameInstance->Get_Random(-0.7f, 0.7f) + XMVector3Normalize(ChidoriWorldMatrix.r[1]) * m_pGameInstance->Get_Random(-0.7f, 0.7f);
			m_pChidoriTrail[i]->Set_TrailPos(vTrailPos);
		}

		if (m_fEffectTime >= 0.05f && m_eState < SKILL_END)
		{
			for (_uint i = 0; i <= 10; ++i)
			{
				CObjectPool<CLightning>::Get_GameObject()->Appear(ChidoriWorldMatrix.r[3], 4.f);
			}
			m_fEffectTime = 0.f;
		}

		if (m_eState < SKIL_LOOP)
		{
			m_pColliderCom->IsActive(true);
		}

		CPlayerCamera* pPlayerCamera = dynamic_cast<CPlayerCamera*>(m_pGameInstance->Find_Camera(m_iLevelIndex));
		if (m_eState == SKILL_START)
		{
			_vector vCameraPos = PlayerWorldMatrix.r[3] + PlayerWorldMatrix.r[2] * 2.f + PlayerWorldMatrix.r[1] * 1.5f;
			pPlayerCamera->Setting_CameraControl(vCameraPos, vCameraPos, vCameraPos - PlayerWorldMatrix.r[2] * 2.f);
			pPlayerCamera->Start_CameraControl();
		}
		else
		{
			_vector vCameraPos = PlayerWorldMatrix.r[3] - PlayerWorldMatrix.r[2] * 7.f + PlayerWorldMatrix.r[1] * 5.5f;
			pPlayerCamera->Setting_CameraControl(vCameraPos, vCameraPos, PlayerWorldMatrix.r[3]);
		}
	}
	else
	{

		m_fDissloveRatio += fTimeDelta;
		if (m_fDissloveRatio >= 1.f)
		{
			for (_uint i = 0; i < 3; ++i)
			{
				m_pChidoriTrail[i]->DisAppear();
			}

			m_isActive = false;
			m_fDissloveRatio = 1.f;
		}

		m_pColliderCom->IsActive(false);
	}

	m_pModelCom->Play_Animation(fTimeDelta);
	m_pColliderCom->Update(&m_ChidoriWorldMatrix);

	m_pGameInstance->Add_ColliderList(m_pColliderCom);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONLIGHT, this);
}

HRESULT CSkill_Susanoo::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pDissloveTexture->Bind_ShadeResource(m_pShaderCom, "g_DissloveTexture", 0)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveRatio", &m_fDissloveRatio, sizeof(_float))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(3)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render((_uint)i)))
			return E_FAIL;
	}

#ifdef _DEBUG
	m_pColliderCom->Render();
#endif

	return S_OK;
}

void CSkill_Susanoo::OnCollisionEnter(CGameObject* pOther)
{
	if (pOther->Get_LayerTag() == TEXT("Layer_Monster"))
	{
		CMonster* pMonster = dynamic_cast<CMonster*>(pOther);
		
		if (pMonster->Get_MonsterID() == WHITEZETSU)
		{
			pMonster->Change_State_FlyAway();
			pMonster->Force_BackUp(15.f, 13.f);
		}

		pMonster->Damaged(50.f);
		m_pOwner->Add_HitEffect();
	}
}

void CSkill_Susanoo::OnCollisionStay(CGameObject* pOther)
{
}

void CSkill_Susanoo::OnCollisionExit(CGameObject* pOther)
{
}

void CSkill_Susanoo::Appear()
{
	m_fDissloveRatio = 1.f;
	m_eState = SKILL_START;
	m_pModelCom->SetUp_NextAnimation(m_iAnimationIndex[SKILL_START], false, 0.f);

	m_pColliderCom->IsActive(false);
	m_isActive = true;
}

void CSkill_Susanoo::Change_Animation(SKILLSUSANOOSTATE eState)
{
	if (eState == m_eState)
		return;

	if (eState < SKILL_DISAPPEAR)
	{
		_bool isLoop = false;
		if (eState == SKIL_LOOP)
			isLoop = true;

		m_pModelCom->SetUp_NextAnimation(m_iAnimationIndex[eState], isLoop, 0.f);
	}

	m_eState = eState;
}

HRESULT CSkill_Susanoo::Ready_Components(void* pArg)
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Smoke27"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pDissloveTexture))))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(m_iLevelIndex, TEXT("Prototype_Component_Model_Susanoo"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	_float3 vInitPos = {};
	XMStoreFloat3(&vInitPos, Get_Position());

	/* For.Com_Collider */
	CBounding_Sphere::BOUNDING_SPHERE_DESC			ColliderSphereDesc{};
	ColliderSphereDesc.fRadius = 2.5f;
	ColliderSphereDesc.vCenter = vInitPos;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderSphereDesc)))
		return E_FAIL;
	m_pColliderCom->Set_Owner(this);

	///* FOR.Com_Sound */
	//if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sound"),
	//	TEXT("Com_Sound"), reinterpret_cast<CComponent**>(&m_pSoundCom))))
	//	return E_FAIL;
	//m_pSoundCom->Set_Owner(this);

	return S_OK;
}

CSkill_Susanoo* CSkill_Susanoo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkill_Susanoo* pInstance = new CSkill_Susanoo(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSkill_Susanoo"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CSkill_Susanoo::Clone(void* pArg)
{
	CSkill_Susanoo* pInstance = new CSkill_Susanoo(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CSkill_Susanoo"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkill_Susanoo::Free()
{
	__super::Free();

	Safe_Release(m_pDissloveTexture);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
