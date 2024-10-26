#include "stdafx.h"
#include "Fire05.h"

#include "GameInstance.h"
#include "Model.h"
#include "ObjectPool.h"

CFire05::CFire05(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect{ pDevice, pContext }
{
}

CFire05::CFire05(const CFire05& Prototype)
	: CEffect{ Prototype }
{
}

HRESULT CFire05::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CFire05::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_isActive = false;
	m_vEffectColor = _float4(0.f, 0.f, 0.f, 1.f);

	m_iNumRow = 8; //8За
	m_iNumCol = 8; //8ї­

	return S_OK;
}

void CFire05::Priority_Update(_float fTimeDelta)
{
}

void CFire05::Update(_float fTimeDelta)
{
	m_fCurrentTime += fTimeDelta;

	if (m_isDisappear || m_fCurrentTime >= 0.5f)
	{
		m_vEffectColor.w -= 1.5f * fTimeDelta;
		if (m_vEffectColor.w <= 0.01f)
		{
			CObjectPool<CFire05>::Return_GameObject(this);
			m_isActive = false;
		}
	}

	if (m_fCurrentScale <= m_fTargetScale)
	{
		m_fCurrentScale += 7.f * fTimeDelta;
	}
	else
	{
		m_fCurrentScale = m_fTargetScale;
	}
	

	m_fFrame += 40.f * fTimeDelta;
	m_iFrameIndex = (_uint)floor(m_fFrame) % 64;

	m_iTexCoordCol = m_iFrameIndex % m_iNumCol;
	m_iTexCoordRow = m_iFrameIndex / m_iNumCol;

	m_pTransformCom->Set_Scaled(m_fCurrentScale, m_fCurrentScale, m_fCurrentScale);
	m_pTransformCom->BillBoard();
}

void CFire05::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONLIGHT, this);
	if(m_eType == FIREBALL)
		m_pGameInstance->Add_RenderObject(CRenderer::RG_EFFECT, this);
}

HRESULT CFire05::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vEffectColor, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vMaskColor", &m_vMaskColor, sizeof(_float4))))
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


	if (FAILED(m_pShaderCom->Begin(4)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CFire05::Appear(_fvector vPos, _float fScale, FIRE_TYPE eType)
{
	if (m_isActive)
		return;

	m_fCurrentScale = 0.1f;
	m_fTargetScale = fScale;
	m_pTransformCom->Set_Scaled(0.1f, 0.1f, 0.1f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	m_pTransformCom->BillBoard();
	m_iTexCoordRow = 0;
	m_iTexCoordCol = 0;
	m_iFrameIndex = 0;

	m_fCurrentTime = 0.f;
	m_fFrame = 0.f;

	m_eType = eType;
	if (m_eType == AMATERAS)
	{
		m_vEffectColor = _float4(0.f, 0.f, 0.f, 1.f);
		m_vMaskColor = _float4(0.f, 0.f, 1.f, 1.f);
	}
	else
	{
		m_vEffectColor = _float4(1.f, 0.f, 0.f, 1.f);
		m_vMaskColor = _float4(1.f, 0.44f, 0.f, 1.f);
	}




	m_isDisappear = false;
	m_isActive = true;
}

void CFire05::DisAppear()
{
	m_isDisappear = true;
}

HRESULT CFire05::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Effect_Texture"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Fire5"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CFire05* CFire05::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFire05* pInstance = new CFire05(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CFire05"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CFire05::Clone(void* pArg)
{
	CFire05* pInstance = new CFire05(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CFire05"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFire05::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);

}
