#include "stdafx.h"
#include "Particle_FireBall.h"

#include "GameInstance.h"

#include "ObjectPool.h"

CParticle_FireBall::CParticle_FireBall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CParticle_FireBall::CParticle_FireBall(const CParticle_FireBall& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CParticle_FireBall::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CParticle_FireBall::Initialize(void* pArg)
{
	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_isActive = false;

	return S_OK;
}

void CParticle_FireBall::Priority_Update(_float fTimeDelta)
{

}

void CParticle_FireBall::Update(_float fTimeDelta)
{
	m_fCurrentTime += fTimeDelta;
	if (m_fCurrentTime >= 5.5f)
	{
		m_isActive = false;
	}

	_vector		vVelocity = XMLoadFloat3(&m_vVelocity);
	_vector		vFricitionDir = XMVector3Normalize(vVelocity) * -1.f;

	//중력 적용
	_vector vGravity = XMVectorSet(0.f, 1.f, 0.f, 0.f) * 15.f * fTimeDelta;
	vVelocity -= vGravity;


	// 저항 적용
	_vector vFriction = vFricitionDir * XMVectorSet(1.f, 0.f, 1.f, 0.f) * fTimeDelta;
	_float	fVelocityLength = XMVectorGetX(XMVector3Length(vVelocity));
	_float	fFrictionLength = XMVectorGetX(XMVector3Length(vFriction));

	if (fFrictionLength >= fVelocityLength)
	{
		vVelocity = XMVectorSet(1.f, XMVectorGetY(vVelocity) , 1.f, 0.f);
	}
	else
	{
		vVelocity += vFriction;
	}

	m_pVIBufferCom->Explosion(fTimeDelta, vVelocity);

	XMStoreFloat3(&m_vVelocity, vVelocity);
}

void CParticle_FireBall::Late_Update(_float fTimeDelta)
{
	/* 직교투영을 위한 월드행렬까지 셋팅하게 된다. */
	__super::Late_Update(fTimeDelta);


	m_pGameInstance->Add_RenderObject(CRenderer::RG_EFFECT, this);
}

HRESULT CParticle_FireBall::Render()
{

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(2)))
		return E_FAIL;


	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CParticle_FireBall::Appear(_fvector vPos)
{
	if (m_isActive)
		false;

	m_fCurrentTime = 0.f;
	m_vColor = _float4(1.f, 0.44f, 0.f, 1.f);

	_vector vVelocity = XMVectorSet(m_pGameInstance->Get_Random(1.f, 7.f), m_pGameInstance->Get_Random(10.f, 15.f), m_pGameInstance->Get_Random(1.f, 7.f), 1.f);
	XMStoreFloat3(&m_vVelocity, vVelocity);

	m_pVIBufferCom->Reset();
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);


	m_isActive = true;
}

HRESULT CParticle_FireBall::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(m_iLevelIndex, TEXT("Prototype_Component_Texture_Particle"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	CVIBuffer_Instancing::INSTANCE_PARTICLE_DESC			ParticleDesc{};
	/* For. Prototype_Component_VIBuffer_Particle_Explosion */
	ZeroMemory(&ParticleDesc, sizeof ParticleDesc);

	ParticleDesc.iNumInstance = 1000;
	ParticleDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ParticleDesc.vRange = _float3(3.5f, 3.5f, 3.5f);
	ParticleDesc.vSize = _float2(0.06f, 0.1f);
	ParticleDesc.vPivot = _float3(0.f, 0.f, 0.f);
	ParticleDesc.vSpeed = _float2(0.8f, 1.5f);
	ParticleDesc.vLifeTime = _float2(4.5f, 5.5f);
	ParticleDesc.isLoop = false;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(m_iLevelIndex, TEXT("Prototype_Component_VIBuffer_Particle_Snow"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), &ParticleDesc)))
		return E_FAIL;

	return S_OK;
}

CParticle_FireBall* CParticle_FireBall::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CParticle_FireBall* pInstance = new CParticle_FireBall(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CParticle_FireBall"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CParticle_FireBall::Clone(void* pArg)
{
	CParticle_FireBall* pInstance = new CParticle_FireBall(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CParticle_FireBall"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CParticle_FireBall::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
