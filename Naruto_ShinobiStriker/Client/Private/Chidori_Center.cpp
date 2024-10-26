#include "stdafx.h"
#include "Chidori_Center.h"

#include "GameInstance.h"

#include "Camera.h"
#include "Model.h"
#include "Player.h"

CChidori_Center::CChidori_Center(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect{ pDevice, pContext }
{
}

CChidori_Center::CChidori_Center(const CChidori_Center& Prototype)
	: CEffect{ Prototype }
{
}

HRESULT CChidori_Center::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CChidori_Center::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_isActive = false;
	m_vEffectColor = _float4(0.07f, 0.36f, 0.91f, 1.f);
	m_vTexCoord = _float2(0.5f, 0.5f);
	m_vSize = _float3(0.05f, 0.05f, 0.05f);

	m_pOwner = dynamic_cast<CPlayer*>(m_pGameInstance->Find_Player(LEVEL_CHUNIN));
	return S_OK;
}

void CChidori_Center::Priority_Update(_float fTimeDelta)
{
}

void CChidori_Center::Update(_float fTimeDelta)
{
	m_fCurrentTime += fTimeDelta;

	if (m_fCurrentTime >= 0.03f)
	{
		m_vTexCoord.x += 0.5f;

		if (m_vTexCoord.x > 1.f)
		{
			m_vTexCoord.x = 0.5f;
			m_vTexCoord.y += 0.5f;
		}

		if (m_vTexCoord.y > 1.f)
		{
			m_vTexCoord.x = 0.5f;
			m_vTexCoord.y = 0.5f;
		}

		m_fCurrentTime = 0.f;
	}

	if (m_vSize.x <= 0.8f)
	{
		m_vSize.x += 2.f * fTimeDelta;
		m_vSize = _float3(m_vSize.x , m_vSize.x, m_vSize.x);
		m_vEffectColor.w -= fTimeDelta;
	}
	else
	{
		m_vSize.x = 0.05f;
		m_vEffectColor.w = 1.0f;
		m_pTransformCom->Rotation(m_pGameInstance->Get_Random(0.f, 1.f), m_pGameInstance->Get_Random(0.f, 1.f), m_pGameInstance->Get_Random(0.f, 1.f));
	}

	m_pTransformCom->Set_Scaled(m_vSize.x, m_vSize.y, m_vSize.z);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pOwner->Get_Position());
	m_pTransformCom->BillBoard();
}

void CChidori_Center::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_EFFECT, this);
}

HRESULT CChidori_Center::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vEffectColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vTexcoord", &m_vTexCoord, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(2)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CChidori_Center::Appear()
{
	m_fCurrentTime = 0.f;
	m_vTexCoord = _float2(0.5f,0.5f);
	m_vEffectColor.w = 1.0f;
	m_vSize = _float3(0.05f, 0.05f, 0.05f);

	m_pTransformCom->Rotation(0.f, m_pGameInstance->Get_Random(0.f, 1.f), 0.f);
	m_pTransformCom->Set_Scaled(m_vSize.x, m_vSize.y, m_vSize.z);

	m_isActive = true;
}

void CChidori_Center::DisAppear()
{
	m_isActive = false;
}

HRESULT CChidori_Center::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Effect_Texture"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Chidori"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CChidori_Center* CChidori_Center::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CChidori_Center* pInstance = new CChidori_Center(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CChidori_Center"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CChidori_Center::Clone(void* pArg)
{
	CChidori_Center* pInstance = new CChidori_Center(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CChidori_Center"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChidori_Center::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
}
