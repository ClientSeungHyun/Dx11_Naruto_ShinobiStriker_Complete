#include "stdafx.h"
#include "Lightning.h"

#include "GameInstance.h"
#include "ObjectPool.h"
#include "Model.h"

CLightning::CLightning(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect{ pDevice, pContext }
{
}

CLightning::CLightning(const CLightning& Prototype)
	: CEffect{ Prototype }
{
}

HRESULT CLightning::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CLightning::Initialize(void* pArg)
{
	LIGHTNING_DESC* pDesc = static_cast<LIGHTNING_DESC*>(pArg);
	m_eType = pDesc->eType;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_isActive = false;
	m_vEffectColor = _float4(0.07f, 0.36f, 0.91f, 1.f);


	m_fAliveTime = 1.0f;

	return S_OK;
}

void CLightning::Priority_Update(_float fTimeDelta)
{
}

void CLightning::Update(_float fTimeDelta)
{
	if (m_eType == LIGHTNING_B3 || m_eType == LIGHTNING_B4)
	{
		m_fTexCoord += 4.f * fTimeDelta;

		if (m_fTexCoord >= 4.f)
			DisAppear();

		if (m_isEnd)
		{
			m_vEffectColor.w -= 7.f * fTimeDelta;
			if (m_vEffectColor.w <= 0.0f)
			{
				CObjectPool<CLightning>::Return_GameObject(this);
				m_isActive = false;
			}
		}
	}
	else
	{
		m_fTexCoord -= 4.f * fTimeDelta;

		if (m_fTexCoord <= 0.f)
			DisAppear();

		if (m_isEnd)
		{
			m_vEffectColor.w -= 7.f * fTimeDelta;
			if (m_vEffectColor.w <= 2.0f)
			{
				CObjectPool<CLightning>::Return_GameObject(this);
				m_isActive = false;
			}
		}
	}

	

}

void CLightning::Late_Update(_float fTimeDelta)
{
	//_vector vPos = m_pOwner->Get_Position();
	//_vector vLook = XMVector3Normalize(m_pGameInstance->Find_Player(m_iLevelIndex)->Get_Transform()->Get_State(CTransform::STATE_LOOK));

	//m_pTransformCom->LookAt(vPos + vLook);
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	m_pGameInstance->Add_RenderObject(CRenderer::RG_EFFECT, this);
}

HRESULT CLightning::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vEffectColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTexCoordX", &m_fTexCoord, sizeof(_float))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_MaskTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render((_uint)i)))
			return E_FAIL;
	}

	return S_OK;
}

void CLightning::Appear(_fvector vPos, _float fScale)
{
	if (m_isActive)
		return;

	_float fX = XMConvertToRadians((_float)(rand() % 360));
	_float fY = XMConvertToRadians((_float)(rand() % 360));
	_float fZ = XMConvertToRadians((_float)(rand() % 360));

	m_pTransformCom->Set_Scaled(fScale, fScale, fScale);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	m_pTransformCom->Rotation(fX, fY, fZ);

	m_fTexCoord = 3.f;
	m_fCurrentTime = 0.f;
	m_vEffectColor.w = 1.0f;

	m_isEnd = false;
	m_isActive = true;
}

void CLightning::DisAppear()
{
	m_isEnd = true;
}

HRESULT CLightning::Ready_Components()
{
	_wstring strPrototypeTag = TEXT("Prototype_Component_Model_");
	_wstring strModelName;


	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Effect"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	switch (m_eType)
	{
	case Client::CLightning::LIGHTNING_A1:
		strModelName = TEXT("LightningA01");
		break;
	case Client::CLightning::LIGHTNING_A2:
		strModelName = TEXT("LightningA02");
		break;
	case Client::CLightning::LIGHTNING_B1:
		strModelName = TEXT("LightningB01");
		break;
	case Client::CLightning::LIGHTNING_B2:
		strModelName = TEXT("LightningB02");
		break;
	case Client::CLightning::LIGHTNING_B3:
		strModelName = TEXT("LightningB03");
		break;
	case Client::CLightning::LIGHTNING_B4:
		strModelName = TEXT("LightningB04");
		break;
	case Client::CLightning::LIGHTNING_C:
		strModelName = TEXT("LightningC");
		break;
	case Client::CLightning::LIGHTNING_D:
		strModelName = TEXT("LightningD");
		break;
	case Client::CLightning::LIGHTNING_E:
		strModelName = TEXT("LightningE");
		break;
	case Client::CLightning::LIGHTNING_F:
		strModelName = TEXT("LightningF");
		break;
	case Client::CLightning::LIGHTNING_CHIDORI:
		strModelName = TEXT("LightningChidori");
		break;
	}

	strPrototypeTag += strModelName;
	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, strPrototypeTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

CLightning* CLightning::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLightning* pInstance = new CLightning(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CLightning"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CLightning::Clone(void* pArg)
{
	CLightning* pInstance = new CLightning(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CLightning"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLightning::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

}
