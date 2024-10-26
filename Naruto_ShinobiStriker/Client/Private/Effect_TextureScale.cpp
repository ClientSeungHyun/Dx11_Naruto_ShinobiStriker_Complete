#include "stdafx.h"
#include "Effect_TextureScale.h"

#include "GameInstance.h"

#include "Camera.h"
#include "Model.h"
#include "Monster.h"

CEffect_TextureScale::CEffect_TextureScale(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect{ pDevice, pContext }
{
}

CEffect_TextureScale::CEffect_TextureScale(const CEffect_TextureScale& Prototype)
	: CEffect{ Prototype }
{
}

HRESULT CEffect_TextureScale::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CEffect_TextureScale::Initialize(void* pArg)
{
	TEXTURE_SCALE_DESC* pDesc = static_cast<TEXTURE_SCALE_DESC*>(pArg);
	if (nullptr == pDesc)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pDesc->strTextureTag)))
		return E_FAIL;

	m_isActive = false;
	m_vEffectColor = pDesc->vColor;
	m_fScaleDuration = pDesc->fDuration;

	return S_OK;
}

void CEffect_TextureScale::Priority_Update(_float fTimeDelta)
{
}

void CEffect_TextureScale::Update(_float fTimeDelta)
{
	m_vEffectColor.w -= 2.5f * fTimeDelta;
	if (m_vEffectColor.w <= 0.f)
		m_isActive = false;

	m_fCurrentTime += fTimeDelta;
	m_fSize += 100.f * fTimeDelta;

	m_pTransformCom->Set_Scaled(m_fSize, m_fSize, m_fSize);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pOwner->Get_Position());
}

void CEffect_TextureScale::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONLIGHT, this);
}

HRESULT CEffect_TextureScale::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vEffectColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(3)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CEffect_TextureScale::Appear(_float fInitSize, _float fAlpha, _bool isRandomRotation)
{
	m_fCurrentTime = 0.f;
	m_vEffectColor.w = 0.7f;
	m_fSize = fInitSize;

	m_pTransformCom->BillBoard();
	m_pTransformCom->Set_Scaled(m_fSize, m_fSize, m_fSize);

	if (isRandomRotation)
	{
		_float fY = XMConvertToRadians((_float)(rand() % 360));
		m_pTransformCom->Rotation(0, fY, 0);
	}

	m_isActive = true;
}

void CEffect_TextureScale::DisAppear()
{
	m_isActive = false;
}

HRESULT CEffect_TextureScale::Ready_Components(_wstring strTextureTag)
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Effect_Texture"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, strTextureTag,
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CEffect_TextureScale* CEffect_TextureScale::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffect_TextureScale* pInstance = new CEffect_TextureScale(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CEffect_RoarBase"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CEffect_TextureScale::Clone(void* pArg)
{
	CEffect_TextureScale* pInstance = new CEffect_TextureScale(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CEffect_RoarBase"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_TextureScale::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
}
