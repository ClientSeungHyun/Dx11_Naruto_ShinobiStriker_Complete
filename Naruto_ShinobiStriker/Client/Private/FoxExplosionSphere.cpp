#include "stdafx.h"
#include "FoxExplosionSphere.h"

#include "GameInstance.h"

#include "Model.h"

CFoxExplosionSphere::CFoxExplosionSphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect{ pDevice, pContext }
{
}

CFoxExplosionSphere::CFoxExplosionSphere(const CFoxExplosionSphere& Prototype)
	: CEffect{ Prototype }
{
}

HRESULT CFoxExplosionSphere::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CFoxExplosionSphere::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_isActive = false;
	m_vEffectColor = _float4(0.12f, 0.f, 0.4f, 1.f);

	m_pTransformCom->Set_Scaled(0.01f, 0.01f, 0.01f);
	m_fScaledSpeed = 15.f;
	m_fTargetScale = 7.f;
	m_fAlphaRatio = 1.f;

	return S_OK;
}

void CFoxExplosionSphere::Priority_Update(_float fTimeDelta)
{
}

void CFoxExplosionSphere::Update(_float fTimeDelta)
{
	m_fCurrentTime += fTimeDelta;
	if (m_fTexCoordX < -1.9f)
		m_isActive = false;

	m_fTexCoordX -= 1.f * fTimeDelta;

	if (m_fCurrentScale <= m_fTargetScale)
	{
		m_fCurrentScale += m_fScaledSpeed * fTimeDelta;

		if (m_fCurrentScale > m_fTargetScale)
		{
			m_fCurrentScale = m_fTargetScale;
		}
	}

	m_pTransformCom->Set_Scaled(m_fCurrentScale, m_fCurrentScale, m_fCurrentScale);

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
}

void CFoxExplosionSphere::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_ColliderList(m_pColliderCom);

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONLIGHT, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_EFFECT, this);
}

HRESULT CFoxExplosionSphere::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vEffectColor, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTexCoordX", &m_fTexCoordX, sizeof(_float))))
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

		if (FAILED(m_pShaderCom->Begin(10)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render((_uint)i)))
			return E_FAIL;
	}

#ifdef _DEBUG
	m_pColliderCom->Render();
#endif

	return S_OK;
}

void CFoxExplosionSphere::Appear()
{
	m_fTexCoordX = 0.f;
	m_fCurrentScale = 0.1f;
	m_fAlphaRatio = 1.f;
	m_fCurrentTime = 0.f;

	m_pTransformCom->Set_Scaled(0.1f, 0.1f, 0.1f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pOwner->Get_Position());


	m_pSoundCom->Play3D(TEXT("Effect_ExplosionFoxSphere.wav"), g_fVolume + 0.2f);

	m_isActive = true;
}

void CFoxExplosionSphere::DisAppear()
{
	m_isActive = false;
}

HRESULT CFoxExplosionSphere::Ready_Components()
{
	/* FOR.Com_Sound */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sound"),
		TEXT("Com_Sound"), reinterpret_cast<CComponent**>(&m_pSoundCom))))
		return E_FAIL;
	m_pSoundCom->Set_Owner(this);

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
	CBounding_Sphere::BOUNDING_SPHERE_DESC			ColliderDesc_Sphere{};
	ColliderDesc_Sphere.fRadius = 0.45f;
	ColliderDesc_Sphere.vCenter = _float3(0.f, vInitPos.y, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc_Sphere)))
		return E_FAIL;
	m_pColliderCom->Set_Owner(this);

	return S_OK;
}

CFoxExplosionSphere* CFoxExplosionSphere::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFoxExplosionSphere* pInstance = new CFoxExplosionSphere(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CFoxExplosionSphere"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CFoxExplosionSphere::Clone(void* pArg)
{
	CFoxExplosionSphere* pInstance = new CFoxExplosionSphere(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CFoxExplosionSphere"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFoxExplosionSphere::Free()
{
	__super::Free();

	Safe_Release(m_pSoundCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

}
