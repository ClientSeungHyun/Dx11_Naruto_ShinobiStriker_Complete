#include "stdafx.h"
#include "Smoke11.h"

#include "GameInstance.h"
#include "Model.h"
#include "ObjectPool.h"

CSmoke11::CSmoke11(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect{ pDevice, pContext }
{
}

CSmoke11::CSmoke11(const CSmoke11& Prototype)
	: CEffect{ Prototype }
{
}

HRESULT CSmoke11::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CSmoke11::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_isActive = true;
	m_vEffectColor = _float4(1.f, 1.f, 1.f, 1.f);

	m_pTransformCom->Set_Scaled(5.f, 5.f, 5.f);
	m_fScaledSpeed = 6.f;
	m_fTargetScale = 5.f;

	m_iNumRow = 8; //8За
	m_iNumCol = 8; //8ї­

	return S_OK;
}

void CSmoke11::Priority_Update(_float fTimeDelta)
{
}

void CSmoke11::Update(_float fTimeDelta)
{
	m_fCurrentTime += 0.2f * fTimeDelta;
	m_fCurrentScale += 4.f * fTimeDelta;

		m_vEffectColor.x = max(0.f, m_vInitColor.x - m_fCurrentTime);
		m_vEffectColor.y = max(0.f, m_vInitColor.y - m_fCurrentTime);
		m_vEffectColor.z = max(0.f, m_vInitColor.z - m_fCurrentTime);
		m_vEffectColor.w = max(0.f, m_vInitColor.w - m_fCurrentTime);
	
	if (m_fCurrentScale >= m_fTargetScale)
		m_fCurrentScale = m_fTargetScale;

	m_fFrame += 60.f * fTimeDelta;
	m_iFrameIndex = (_uint)floor(m_fFrame); // % 64;

	m_iTexCoordCol = m_iFrameIndex % m_iNumCol;
	m_iTexCoordRow = m_iFrameIndex / m_iNumCol;

	if (m_iFrameIndex >= m_iNumRow * m_iNumCol)
	{
		CObjectPool<CSmoke11>::Return_GameObject(this);
		m_isActive = false;
	}

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPos = vPos + XMLoadFloat3(&m_vMoveDir) * 2.5f * fTimeDelta;

	
	m_pTransformCom->Set_Scaled(m_fCurrentScale, m_fCurrentScale, m_fCurrentScale);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	m_pTransformCom->BillBoard();
}

void CSmoke11::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONLIGHT, this);
}

HRESULT CSmoke11::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vEffectColor, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iNumCol", &m_iNumCol, sizeof(_int))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iNumRow", &m_iNumRow, sizeof(_int))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iCol", &m_iTexCoordCol, sizeof(_int))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iRow", &m_iTexCoordRow, sizeof(_int))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(11)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CSmoke11::Appear(_float3 vCenterPos, _float3  vRange, _float fInitScale, SMOKETYPE eType)
{
	m_fFrame = 0.f;
	m_iFrameIndex = 0;
	m_fCurrentTime = 0.f;

	m_fCurrentScale = fInitScale;
	m_fTargetScale = m_fCurrentScale * 4.f;
	m_pTransformCom->Set_Scaled(m_fCurrentScale, m_fCurrentScale, m_fCurrentScale);

	_vector vPos = XMVectorSet(vCenterPos.x + m_pGameInstance->Get_Random(-vRange.x, vRange.x)
		, vCenterPos.y + m_pGameInstance->Get_Random(-vRange.y, vRange.y)
		, vCenterPos.z + m_pGameInstance->Get_Random(-vRange.z, vRange.z), 1.f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	_vector vMoveDir = XMVector3Normalize(XMVectorSetY(vPos - XMLoadFloat3(&vCenterPos), 1.f));
	XMStoreFloat3(&m_vMoveDir, vMoveDir);

	m_eType = eType;
	if (m_eType == SMOKE_WHITE)
		m_vInitColor = _float4(0.9f, 0.9f, 0.9f, 0.8f);
	else if (m_eType == SMOKE_BLACK)
		m_vInitColor = _float4(0.1f, 0.1f, 0.1f, 0.8f);
	else if (m_eType == SMOKE_BROWN)
		m_vInitColor = _float4(0.365f, 0.33f, 0.32f, 0.8f);
	m_isActive = true;
}

void CSmoke11::DisAppear()
{
	m_isActive = false;
}

HRESULT CSmoke11::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Effect_Texture"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Smoke11"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CSmoke11* CSmoke11::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSmoke11* pInstance = new CSmoke11(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSmoke11"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CSmoke11::Clone(void* pArg)
{
	CSmoke11* pInstance = new CSmoke11(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CSmoke11"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSmoke11::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);

}
