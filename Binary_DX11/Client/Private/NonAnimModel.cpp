#include "stdafx.h"
#include "NonAnimModel.h"

#include "GameInstance.h"
#include "ImGui_Manager.h"

CNonAnimModel::CNonAnimModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CNonAnimModel::CNonAnimModel(const CNonAnimModel& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CNonAnimModel::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CNonAnimModel::Initialize(void* pArg)
{
	NONMODEL_DESC* pDesc = static_cast<NONMODEL_DESC*>(pArg);

	/* ���������� ���� �����͵��� ��� ���ϳ�. */
	if (FAILED(__super::Initialize(&pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	m_pTransformCom->Set_Scaled(pDesc->vScale.x, pDesc->vScale.y, pDesc->vScale.z);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&pDesc->vPosition));

	memcpy(&m_tDesc, pDesc, sizeof(NONMODEL_DESC));
	return S_OK;
}

void CNonAnimModel::Priority_Update(_float fTimeDelta)
{
	int a = 10;
}

void CNonAnimModel::Update(_float fTimeDelta)
{
	/*if ((GetKeyState(VK_LBUTTON) & 0x8000) &&
		true == m_pVIBufferCom->isPicking(m_pTransformCom->Get_WorldMatrix(), &vPickPos))
	{
		if (CImGui_Manager::Get_Instance()->Get_IsPicking())
		{
			CImGui_Manager::Get_Instance()->Set_ModelPos(vPickPos);
		}
	}*/
}

void CNonAnimModel::Late_Update(_float fTimeDelta)
{
	/* ���������� ���� ������ı��� �����ϰ� �ȴ�. */
	__super::Late_Update(fTimeDelta);

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CNonAnimModel::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_bSelect", &m_isPicked, sizeof(_bool))))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, (_uint)i);

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render((_uint)i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CNonAnimModel::Ready_Components(NONMODEL_DESC* pNonAnimDesc)
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Model"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	_tchar szModelTag[MAX_PATH];
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pNonAnimDesc->szModelTag, MAX_PATH, szModelTag, MAX_PATH);

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, szModelTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

CNonAnimModel* CNonAnimModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CNonAnimModel* pInstance = new CNonAnimModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMonster"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CNonAnimModel::Clone(void* pArg)
{
	CNonAnimModel* pInstance = new CNonAnimModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMonster"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNonAnimModel::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
