#include "stdafx.h"
#include "WireTrail.h"

#include "GameInstance.h"

#include "Camera.h"
#include "Model.h"
#include "Player.h"

CWireTrail::CWireTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect{ pDevice, pContext }
{
}

CWireTrail::CWireTrail(const CWireTrail& Prototype)
	: CEffect{ Prototype }
{
}

HRESULT CWireTrail::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CWireTrail::Initialize(void* pArg)
{
	WIRETRAIL_DESC* Desc = static_cast<WIRETRAIL_DESC*>(pArg);
	if (nullptr == Desc)
		return E_FAIL;

	m_pSocketMatrix = Desc->m_pSocketMatrix;
	m_PlayerWorldMatrix = Desc->m_pOwnerWorldMatrix;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_fTrailSize = 0.02f;
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_isActive = false;
	m_vEffectColor = _float4(0.07f, 0.36f, 0.91f, 1.f);

	return S_OK;
}

void CWireTrail::Priority_Update(_float fTimeDelta)
{
}

void CWireTrail::Update(_float fTimeDelta)
{
	_matrix		HandWorldMatrix = XMMatrixIdentity();
	_matrix		PlayerWorldMatrix = XMLoadFloat4x4(m_PlayerWorldMatrix);

	_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (size_t i = 0; i < 3; i++)
	{
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
	}

	HandWorldMatrix = SocketMatrix * PlayerWorldMatrix;

	_vector vHandPos = HandWorldMatrix.r[3];
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if (m_isDisappear)
	{
		_float fLength = XMVectorGetX(XMVector3Length(vPos - vHandPos));
		if (fLength < 2.f)
			m_isActive = false;
		else
		{
			m_pVIBufferCom->Add_TrailPoint_ForWire(m_pTransformCom->Get_WorldMatrix(), vHandPos, 0.05f);
		}
	}
	else
	{
		vPos = XMVectorLerp(vPos, XMLoadFloat3(&m_vTrailEndPos), 5.f * fTimeDelta);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

		m_pVIBufferCom->Add_TrailPoint_ForWire(m_pTransformCom->Get_WorldMatrix(), vHandPos, 0.05f);
	}

	m_pVIBufferCom->Update_Trail_ForWire();
}

void CWireTrail::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_EFFECT, this);
}

HRESULT CWireTrail::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vEffectColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

_bool CWireTrail::Appear()
{
	if (m_isActive)
		return false;

	_matrix		WorldMatrix = XMMatrixIdentity();
	_matrix		PlayerWorldMatrix = XMLoadFloat4x4(m_PlayerWorldMatrix);

	_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (size_t i = 0; i < 3; i++)
	{
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
	}

	WorldMatrix = SocketMatrix * PlayerWorldMatrix;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, WorldMatrix.r[3]);

	m_pVIBufferCom->Clear_Trail();

	if (true == RayCast())
	{
		m_vEffectColor.w = 0.8f;
		m_isDisappear = false;
		m_isActive = true;

		return true;
	}

	return false;
}

void CWireTrail::DisAppear()
{
	m_isDisappear = true;
}

HRESULT CWireTrail::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTrailInstance"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Trail"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;
	m_pVIBufferCom->Set_TrailSize(m_fTrailSize);

	return S_OK;
}

_bool CWireTrail::RayCast()
{
	_vector vHitPos, vNormal;
	_float fHitDistance;

	_vector vCameraWorldPos = m_pGameInstance->Get_CamPosition_Vector();

	_matrix ViewMatrix = m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW);
	_matrix ViewMatrixInv = XMMatrixInverse(nullptr, ViewMatrix);

	_vector vCameraViewPos = XMVector4Transform(vCameraWorldPos, ViewMatrix);
	_vector vViewCameraPosForward = XMVectorSetZ(vCameraViewPos, XMVectorGetZ(vCameraViewPos) + 0.1f);

	// 뷰 스페이스의 위치를 월드 스페이스로 변환
	_vector vForwardWorldPos = XMVector4Transform(vViewCameraPosForward, ViewMatrixInv);

	_vector vRayDir = XMVector3Normalize(vForwardWorldPos - vCameraWorldPos);
	if (m_pGameInstance->RayCast(vCameraWorldPos, vRayDir, &vHitPos, &vNormal, &fHitDistance))
	{
		if (fHitDistance <= 80.f && fHitDistance >= 5.f)
		{
			XMStoreFloat3(&m_vTrailEndPos, vHitPos);

			return true;
		}
	}

	return false;
}

CWireTrail* CWireTrail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWireTrail* pInstance = new CWireTrail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CWireTrail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CWireTrail::Clone(void* pArg)
{
	CWireTrail* pInstance = new CWireTrail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CWireTrail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWireTrail::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
}
