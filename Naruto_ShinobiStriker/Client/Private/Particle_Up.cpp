#include "stdafx.h"
#include "..\Public\Particle_Up.h"

#include "GameInstance.h"

#include "ObjectPool.h"

CParticle_Up::CParticle_Up(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CParticle_Up::CParticle_Up(const CParticle_Up& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CParticle_Up::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CParticle_Up::Initialize(void* pArg)
{
	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_isActive = false;

	return S_OK;
}

void CParticle_Up::Priority_Update(_float fTimeDelta)
{

}

void CParticle_Up::Update(_float fTimeDelta)
{
	m_fCurrentTime += fTimeDelta;

	if (m_fCurrentTime >= 3.f)
	{
		CObjectPool<CParticle_Up>::Return_GameObject(this);
		m_isActive = false;
		return;
	}

	switch (m_eType)
	{
	case Client::CParticle_Up::PARTICLE_SPRED:
		m_pVIBufferCom->Spread(fTimeDelta);
		break;
	case Client::CParticle_Up::PARTICLE_UP:
		m_pVIBufferCom->Up(fTimeDelta);
		break;
	case Client::CParticle_Up::PARTICLE_DROP:
		m_pVIBufferCom->Drop(fTimeDelta);
		break;
	}
}

void CParticle_Up::Late_Update(_float fTimeDelta)
{
	/* 직교투영을 위한 월드행렬까지 셋팅하게 된다. */
	__super::Late_Update(fTimeDelta);


	m_pGameInstance->Add_RenderObject(CRenderer::RG_EFFECT, this);
}

HRESULT CParticle_Up::Render()
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

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;


	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CParticle_Up::Appear(_fvector vPos, _fvector vColor, PARTICLE_TYPE eType)
{
	if (m_isActive)
		false;

	m_fCurrentTime = 0.f;
	m_eType = eType;

	m_pVIBufferCom->Reset();
	XMStoreFloat4(&m_vColor, vColor);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);


	m_isActive = true;
}

HRESULT CParticle_Up::Ready_Components()
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
	ZeroMemory(&ParticleDesc, sizeof ParticleDesc);

	ParticleDesc.iNumInstance = 70;
	ParticleDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ParticleDesc.vRange = _float3(1.f, 1.3f, 1.f);
	ParticleDesc.vSize = _float2(0.02f, 0.04f);
	ParticleDesc.vPivot = _float3(0.f, 0.f, 0.f);
	ParticleDesc.vSpeed = _float2(0.8f, 1.5f);
	ParticleDesc.vLifeTime = _float2(0.5f, 1.f);
	ParticleDesc.isLoop = false;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(m_iLevelIndex, TEXT("Prototype_Component_VIBuffer_Particle_Snow"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), &ParticleDesc)))
		return E_FAIL;

	return S_OK;
}

CParticle_Up* CParticle_Up::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CParticle_Up* pInstance = new CParticle_Up(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CParticle_Up"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CParticle_Up::Clone(void* pArg)
{
	CParticle_Up* pInstance = new CParticle_Up(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CParticle_Up"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CParticle_Up::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
