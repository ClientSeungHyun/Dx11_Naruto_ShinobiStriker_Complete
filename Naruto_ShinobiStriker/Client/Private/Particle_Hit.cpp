#include "stdafx.h"
#include "..\Public\Particle_Hit.h"

#include "GameInstance.h"
#include "Particle_Hit.h"
#include "ObjectPool.h"

CParticle_Hit::CParticle_Hit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CParticle_Hit::CParticle_Hit(const CParticle_Hit& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CParticle_Hit::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CParticle_Hit::Initialize(void* pArg)
{
	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_isActive = false;

	return S_OK;
}

void CParticle_Hit::Priority_Update(_float fTimeDelta)
{

}

void CParticle_Hit::Update(_float fTimeDelta)
{
	m_fCurrentTime += fTimeDelta;

	if (m_fCurrentTime >= 3.f)
	{
		CObjectPool<CParticle_Hit>::Return_GameObject(this);
		m_isActive = false;
		return;
	}

	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	ID3D11Buffer* pInstanceBuffer = m_pVIBufferCom->Get_InstanceBuffer();
	m_pContext->Map(pInstanceBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXEFFECTINSTANCE* pVertices = static_cast<VTXEFFECTINSTANCE*>(SubResource.pData);

	for (size_t i = 0; i < m_iNumParticle; i++)
	{
		_vector		vVelocity = XMLoadFloat4(&m_InstanceData[i].vVelocity);
		_vector		vFricitionDir = XMVector3Normalize(vVelocity) * -1.f;

		//중력 적용
		_vector vGravity = XMVectorSet(0.f, 1.f, 0.f, 0.f) * 5.f * fTimeDelta;
		vVelocity -= vGravity;

		// 저항 적용
		_vector vFriction = vFricitionDir * XMVectorSet(1.f, 0.f, 1.f, 0.f) * fTimeDelta;
		_float	fVelocityLength = XMVectorGetX(XMVector3Length(vVelocity));
		_float	fFrictionLength = XMVectorGetX(XMVector3Length(vFriction));

		if (fFrictionLength >= fVelocityLength)
		{
			vVelocity = XMVectorSet(0.f, 0.f, 0.f, 0.f);
		}
		else
		{
			vVelocity += vFriction;
		}

		_vector vNewPosition = XMLoadFloat4(&m_InstanceData[i].vPos) + vVelocity * fTimeDelta;

		XMStoreFloat4(&m_InstanceData[i].vPos, vNewPosition);
		XMStoreFloat4(&m_InstanceData[i].vVelocity, vVelocity);
		m_InstanceData[i].vLifeTime.y = m_fCurrentTime;
	}

	memcpy(pVertices, m_InstanceData.data(), sizeof(HITEFFECT_DATA) * m_iNumParticle);

	m_pContext->Unmap(pInstanceBuffer, 0);

}

void CParticle_Hit::Late_Update(_float fTimeDelta)
{
	/* 직교투영을 위한 월드행렬까지 셋팅하게 된다. */
	__super::Late_Update(fTimeDelta);


	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONLIGHT, this);
}

HRESULT CParticle_Hit::Render()
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

	if (FAILED(m_pShaderCom->Begin(1)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CParticle_Hit::Appear(_fvector vStartPos, _fvector vColor, _uint iNumParticle, _float fPower)
{
	if (m_isActive)
		false;

	m_InstanceData.clear();
	m_InstanceData.reserve(iNumParticle);
	m_iNumParticle = iNumParticle;
	m_fCurrentTime = 0.f;
	XMStoreFloat4(&m_vColor, vColor);

	CVIBuffer_Instancing::INSTANCE_PARTICLE_DESC Desc{};
	Desc.iNumInstance = iNumParticle;

	for (size_t i = 0; i < iNumParticle; i++)
	{
		HITEFFECT_DATA tData = {};
		XMStoreFloat4(&tData.vPos, vStartPos);

		_float fRange = 0.3f;
		tData.vPos = _float4(
			m_pGameInstance->Get_Random(tData.vPos.x - fRange, tData.vPos.x + fRange) + 0.01f,
			m_pGameInstance->Get_Random(tData.vPos.y - fRange, tData.vPos.y + fRange * 2.f) + 0.01f,
			m_pGameInstance->Get_Random(tData.vPos.z - fRange, tData.vPos.z + fRange) + 0.01f,
			1.f);

		_float4 vScale = {};
		tData.vScale = _float4(m_pGameInstance->Get_Random(1.f, 2.f), m_pGameInstance->Get_Random(1.f, 2.f), 0.f, 0.f);

		_vector vDir = XMVector3Normalize(XMLoadFloat4(&tData.vPos) - vStartPos);
		XMStoreFloat4(&tData.vVelocity, vDir * m_pGameInstance->Get_Random(1.f, 3.f) * fPower);

		tData.vLifeTime.x = m_pGameInstance->Get_Random(3.f, 5.f);
		tData.vLifeTime.y = 0.1f;

		m_InstanceData.emplace_back(tData);
	}

	VTXEFFECTINSTANCE* pInstanceVertices = new VTXEFFECTINSTANCE[iNumParticle];
	ZeroMemory(pInstanceVertices, sizeof(VTXEFFECTINSTANCE) * iNumParticle);
	memcpy(pInstanceVertices, m_InstanceData.data(), sizeof(HITEFFECT_DATA) * iNumParticle);

	m_pVIBufferCom->ReSetting_Instance(Desc, pInstanceVertices);

	m_isActive = true;
}

void CParticle_Hit::Calculate_Gravity(_float fTimeDelta)
{

}

HRESULT CParticle_Hit::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(m_iLevelIndex, TEXT("Prototype_Component_Texture_Particle"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	CVIBuffer_Instancing::INSTANCE_PARTICLE_DESC Desc{};
	Desc.iNumInstance = 50;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(m_iLevelIndex, TEXT("Prototype_Component_VIBuffer_Particle_Snow"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom),&Desc)))
		return E_FAIL;

	return S_OK;
}

CParticle_Hit* CParticle_Hit::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CParticle_Hit* pInstance = new CParticle_Hit(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CParticle_Hit"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CParticle_Hit::Clone(void* pArg)
{
	CParticle_Hit* pInstance = new CParticle_Hit(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CParticle_Hit"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CParticle_Hit::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
