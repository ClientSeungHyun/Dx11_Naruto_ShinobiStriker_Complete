#include "stdafx.h"
#include "AreaCircle.h"

#include "GameInstance.h"
#include "Camera.h"
#include "Player.h"

CAreaCircle::CAreaCircle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CAreaCircle::CAreaCircle(const CAreaCircle& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CAreaCircle::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAreaCircle::Initialize(void* pArg)
{
	AREACIRCLE_DESC* pDesc = static_cast<AREACIRCLE_DESC*>(pArg);
	/* 직교투영을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&pDesc->vPos));

	m_vInitSize = _float3(0.8f, 0.5f, 0.8f);
	m_vColor = _float4(0.1f, 0.1f, 1.f, 0.8f);

	return S_OK;
}

void CAreaCircle::Priority_Update(_float fTimeDelta)
{

}

void CAreaCircle::Update(_float fTimeDelta)
{
	CGameObject* pPlayer = m_pGameInstance->Find_Player(m_iLevelIndex);

	_float fDist = XMVectorGetX(XMVector3Length(pPlayer->Get_Position() - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
	if (fDist <= 3.f)
	{
		if (!m_isPlaySound)
		{
			m_pSoundCom->Play3D(TEXT("Effect_AreaCircle.wav"), g_fVolume + 0.2f);
			m_isPlaySound = true;
		}
		m_isCollision = true;
	}

	if (m_isCollision)
	{
		m_fCurrentTime += fTimeDelta;
		m_vColor.w -= 1.5f * fTimeDelta;

		if (m_vColor.w <= 0.f)
			m_isActive = false;
	}

	m_pTransformCom->Set_Scaled(m_vInitSize.x + m_fCurrentTime, 0.5f, m_vInitSize.z + m_fCurrentTime);
	m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta, 4.f);
}

void CAreaCircle::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_EFFECT, this);
}

HRESULT CAreaCircle::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, "g_MaskTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i);

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render((_uint)i)))
			return E_FAIL;
	}

	return S_OK;
}

void CAreaCircle::Appear(_fvector vPos)
{
	m_isCollision = false;
	m_fCurrentTime = 0.f;
	m_vColor = _float4(0.1f, 0.1f, 1.f, 0.8f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	m_isPlaySound = false;
	
	m_isActive = true;
}

void CAreaCircle::Disappear()
{
	m_isActive = false;
}

HRESULT CAreaCircle::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Effect"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_AreaCircle"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* FOR.Com_Sound */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sound"),
		TEXT("Com_Sound"), reinterpret_cast<CComponent**>(&m_pSoundCom))))
		return E_FAIL;
	m_pSoundCom->Set_Owner(this);

	return S_OK;
}

CAreaCircle* CAreaCircle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAreaCircle* pInstance = new CAreaCircle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CAreaCircle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CAreaCircle::Clone(void* pArg)
{
	CAreaCircle* pInstance = new CAreaCircle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CAreaCircle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAreaCircle::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
