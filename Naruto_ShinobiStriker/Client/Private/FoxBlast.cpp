#include "stdafx.h"
#include "FoxBlast.h"

#include "GameInstance.h"

#include "Player.h"
#include "Model.h"

#include "ObjectPool.h"

CFoxBlast::CFoxBlast(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect{ pDevice, pContext }
{
}

CFoxBlast::CFoxBlast(const CFoxBlast& Prototype)
	: CEffect{ Prototype }
{
}

HRESULT CFoxBlast::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CFoxBlast::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_isActive = false;
	m_vEffectColor = _float4(0.85f, 0.4f, 0.02f, 1.f);

	m_pTransformCom->Set_Scaled(0.01f, 0.01f, 0.01f);
	m_fAlphaRatio = 1.f;

	return S_OK;
}

void CFoxBlast::Priority_Update(_float fTimeDelta)
{
}

void CFoxBlast::Update(_float fTimeDelta)
{
	m_fCurrentTime += fTimeDelta;
	m_fDamageTime += fTimeDelta;

	_float fRatio = min(1.f, m_fCurrentTime / 0.2f);
	_vector vCurrentScale = XMVectorLerp(XMLoadFloat3(&m_vInitScale), XMLoadFloat3(&m_vTargetScale), fRatio);

	m_pTransformCom->Set_Scaled(XMVectorGetX(vCurrentScale), XMVectorGetY(vCurrentScale), XMVectorGetZ(vCurrentScale));

	_float fScaleZ = XMVectorGetZ(vCurrentScale) - 5.f;

	_vector vPos = XMLoadFloat3(&m_vInitPos);
	vPos += XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * fScaleZ * 0.5f;  // 스케일 변화량 만큼 이동

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	m_fTexCoordY -= 3.f * fTimeDelta;

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
}

void CFoxBlast::Late_Update(_float fTimeDelta)
{
	if (m_fCurrentTime >= 1.f)
	{
		m_fAlphaRatio -= 3.f * fTimeDelta;
		if (m_fAlphaRatio <= 0.f)
		{
			CObjectPool<CFoxBlast>::Return_GameObject(this);
			m_pColliderCom->IsActive(false);
			m_isActive = false;
		}
	}

	m_pGameInstance->Add_ColliderList(m_pColliderCom);

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONLIGHT, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_EFFECT, this);
}

HRESULT CFoxBlast::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vEffectColor, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTexCoordX", &m_fTexCoordY, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolve", &m_fAlphaRatio, sizeof(_float))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i)))
			return E_FAIL;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DissolveTexture", TEXTURE_TYPE::OPACITY, (_uint)i)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(6)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render((_uint)i)))
			return E_FAIL;
	}

#ifdef _DEBUG
	m_pColliderCom->Render();
#endif

	return S_OK;
}

void CFoxBlast::OnCollisionEnter(CGameObject* pOther)
{
	if (pOther->Get_LayerTag() == TEXT("Layer_Player"))
	{
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(pOther);
		if (pPlayer->Get_IsInvincible())
			return;

		m_fDamageTime = 0.f;
		pPlayer->Set_AnimationSpeed(0.01f);
	}
}

void CFoxBlast::OnCollisionStay(CGameObject* pOther)
{
	if (pOther->Get_LayerTag() == TEXT("Layer_Player"))
	{
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(pOther);
		if (pPlayer->Get_IsInvincible())
			return;

		if (m_fDamageTime > 0.1f)
		{
			pPlayer->Damaged(5.f);
			m_fDamageTime = 0.f;
		}
	}
}

void CFoxBlast::OnCollisionExit(CGameObject* pOther)
{
	if (pOther->Get_LayerTag() == TEXT("Layer_Player"))
	{
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(pOther);
		pPlayer->Set_AnimationSpeed(1.f);

		if (pPlayer->Get_IsInvincible())
			return;

		_float2 vForce = _float2(25.f, 15.f);
		pPlayer->Change_State(CPlayer::HIT_SPIN, &vForce);

	}
}

void CFoxBlast::Appear(_fvector vInitPos, _fvector vTargetPos)
{
	m_fTexCoordY = 0.f;
	m_fAlphaRatio = 1.f;
	m_fCurrentTime = 0.f;

	m_vInitScale = _float3(3.f, 3.f, 3.f);
	m_vTargetScale = _float3(5.f, 5.f, 80.f);

	m_pTransformCom->Set_Scaled(m_vInitScale.x, m_vInitScale.y, m_vInitScale.z);

	XMStoreFloat3(&m_vInitPos, vInitPos);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vInitPos);
	m_pColliderCom->IsActive(true);
	m_pTransformCom->LookAt(vTargetPos);
	m_isActive = true;
}

void CFoxBlast::DisAppear()
{
	m_isActive = false;
}

HRESULT CFoxBlast::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Effect"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_ExplosionSphere"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	_float3 vInitPos = {};
	XMStoreFloat3(&vInitPos, Get_Position());

	/* For.Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC		ColliderDesc{};
	ColliderDesc.vExtents = _float3(0.5f, 1.f, 0.5f);
	ColliderDesc.vCenter = _float3(0.f, vInitPos.y, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;
	m_pColliderCom->Set_Owner(this);

	return S_OK;
}

CFoxBlast* CFoxBlast::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFoxBlast* pInstance = new CFoxBlast(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CFoxBlast"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CFoxBlast::Clone(void* pArg)
{
	CFoxBlast* pInstance = new CFoxBlast(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CFoxBlast"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFoxBlast::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

}
