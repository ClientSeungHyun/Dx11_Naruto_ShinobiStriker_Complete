#include "stdafx.h"
#include "FoxSphere.h"

#include "GameInstance.h"

#include "Player.h"
#include "Model.h"
#include "ObjectPool.h"

#include "FoxExplosionSphere.h"

CFoxSphere::CFoxSphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect{ pDevice, pContext }
{
}

CFoxSphere::CFoxSphere(const CFoxSphere& Prototype)
	: CEffect{ Prototype }
{
}

HRESULT CFoxSphere::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CFoxSphere::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_isActive = false;
	m_vEffectColor = _float4(0.08f, 0.08f, 0.42f, 1.f);
	m_fReSizeTime = 0.f;
	m_fInitScale = 2.5f;
	m_pTransformCom->Set_Scaled(m_fInitScale, m_fInitScale, 1.f);

	CGameObject::GAMEOBJECT_DESC	Desc;
	Desc.fRotationPerSec = 0.f;
	Desc.fSpeedPerSec = 0.;
	Desc.iLevelIndex = m_iLevelIndex;

	m_pFoxExplosionSphere = dynamic_cast<CFoxExplosionSphere*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_FoxExplosionSphere"), &Desc));
	if (nullptr == m_pFoxExplosionSphere)
		return E_FAIL;
	Safe_AddRef(m_pFoxExplosionSphere);
	m_pFoxExplosionSphere->Set_Owner(this);
	m_pGameInstance->Add_Object_ToLayer(m_iLevelIndex, TEXT("Layer_SkillEffect"), m_pFoxExplosionSphere);

	return S_OK;
}

void CFoxSphere::Priority_Update(_float fTimeDelta)
{
}

void CFoxSphere::Update(_float fTimeDelta)
{
	if (!m_isExplosion)
	{
		m_fCurrentTime += fTimeDelta;
		m_fReSizeTime += fTimeDelta;
	}

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPos = vPos + XMLoadFloat3(&m_vMoeDir) * 50.f * fTimeDelta;

	if (m_fReSizeTime > 0.07f)
	{
		if (m_isExplosion && m_fCurrentTime > 1.5f)
		{
			m_pTransformCom->Set_Scaled(m_pGameInstance->Get_Random(m_fInitScale + 3.f, m_fInitScale + 1.4f), m_pGameInstance->Get_Random(m_fInitScale + 3.f, m_fInitScale + 1.4f), 1.f);
		}
		else
			m_pTransformCom->Set_Scaled(m_pGameInstance->Get_Random(m_fInitScale, m_fInitScale + 0.4f), m_pGameInstance->Get_Random(m_fInitScale, m_fInitScale + 0.4f), 1.f);
		
		m_fReSizeTime = 0.f;
	}
	
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
	m_pTransformCom->BillBoard();
}

void CFoxSphere::Late_Update(_float fTimeDelta)
{
	if (m_fCurrentTime > 1.f)
	{
		m_pFoxExplosionSphere->Appear();
		CObjectPool<CFoxSphere>::Return_GameObject(this);
		m_isActive = false;
		m_isExplosion = true;
	}

	if (m_isExplosion)
	{
		CObjectPool<CFoxSphere>::Return_GameObject(this);
		m_isActive = false;
	}

	m_pGameInstance->Add_ColliderList(m_pColliderCom);

	if (m_fCurrentTime < 1.f || m_fCurrentTime > 1.5f)
	{
		m_pGameInstance->Add_RenderObject(CRenderer::RG_NONLIGHT, this);
		m_pGameInstance->Add_RenderObject(CRenderer::RG_EFFECT, this);
	}
}

HRESULT CFoxSphere::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vEffectColor, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_isChange", &m_isExplosion, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;
	if (FAILED(m_pSmallTexture->Bind_ShadeResource(m_pShaderCom, "g_MaskTexture", 0)))
		return E_FAIL;
	
	if (FAILED(m_pShaderCom->Begin(7)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

#ifdef _DEBUG
	m_pColliderCom->Render();
#endif

	return S_OK;
}

void CFoxSphere::OnCollisionEnter(CGameObject* pOther)
{
	if (pOther->Get_LayerTag() == TEXT("Layer_Player"))
	{
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(pOther);
		if (pPlayer->Get_IsInvincible() || pPlayer->Get_Fsm()->Get_CurrentState() == CPlayer::HIT_SPIN)
			return;

		_float2 vForce = _float2(15.f, 10.f);
		pPlayer->Damaged(15.f);
		pPlayer->Change_State(CPlayer::HIT_SPIN, &vForce);

		m_pFoxExplosionSphere->Appear();
		CObjectPool<CFoxSphere>::Return_GameObject(this);
		m_isActive = false;
		m_isExplosion = true;
	}
}

void CFoxSphere::OnCollisionStay(CGameObject* pOther)
{
}

void CFoxSphere::OnCollisionExit(CGameObject* pOther)
{
}

void CFoxSphere::Appear(_fvector vInitPos, _fvector vTargetPos)
{
	m_fReSizeTime = 0.f;
	m_fCurrentTime = 0.f;

	m_vEffectColor = _float4(0.08f, 0.08f, 0.42f, 1.f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vInitPos);
	m_pTransformCom->LookAt(vTargetPos);

	_vector vMoveDir = XMVector3Normalize(vTargetPos - vInitPos);
	XMStoreFloat3(&m_vMoeDir, vMoveDir);

	m_pSoundCom->Play3D(TEXT("Effect_ShootFoxSphere.wav"), g_fVolume + 0.2f);

	m_isExplosion = false;
	m_isActive = true;
}

void CFoxSphere::DisAppear()
{
	m_isActive = false;
}

HRESULT CFoxSphere::Ready_Components()
{
	/* FOR.Com_Sound */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sound"),
		TEXT("Com_Sound"), reinterpret_cast<CComponent**>(&m_pSoundCom))))
		return E_FAIL;
	m_pSoundCom->Set_Owner(this);

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Effect_Texture"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Sphere08"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* FOR.Com_SmallTexture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Sphere03"),
		TEXT("Com_SmallTexture"), reinterpret_cast<CComponent**>(&m_pSmallTexture))))
		return E_FAIL;

	///* FOR.Com_Model */
	//if (FAILED(__super::Add_Component(m_iLevelIndex, TEXT("Prototype_Component_Model_ExplosionSphere"),
	//	TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	//	return E_FAIL;

	_float3 vInitPos = {};
	XMStoreFloat3(&vInitPos, Get_Position());

	/* For.Com_Collider */
	CBounding_Sphere::BOUNDING_SPHERE_DESC			ColliderDesc_Sphere{};
	ColliderDesc_Sphere.fRadius = 0.4f;
	ColliderDesc_Sphere.vCenter = _float3(0.f, vInitPos.y, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc_Sphere)))
		return E_FAIL;
	m_pColliderCom->Set_Owner(this);

	return S_OK;
}

CFoxSphere* CFoxSphere::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFoxSphere* pInstance = new CFoxSphere(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CFoxSphere"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CFoxSphere::Clone(void* pArg)
{
	CFoxSphere* pInstance = new CFoxSphere(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CFoxSphere"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFoxSphere::Free()
{
	__super::Free();

	Safe_Release(m_pSoundCom);
	Safe_Release(m_pFoxExplosionSphere);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pSmallTexture);
	Safe_Release(m_pVIBufferCom);
}
