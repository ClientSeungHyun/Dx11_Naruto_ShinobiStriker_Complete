#include "stdafx.h"
#include "Sasuke_Next.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "Player.h"
#include "UI_TextWindow.h"
#include "PlayerCamera.h"

CSasuke_Next::CSasuke_Next(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CSasuke_Next::CSasuke_Next(const CSasuke_Next& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CSasuke_Next::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSasuke_Next::Initialize(void* pArg)
{
	NPC_DESC* pDesc = static_cast<NPC_DESC*>(pArg);

	/* 직교투영을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;
	if(FAILED(Ready_UI()))
		return E_FAIL;

	m_iAnimationIndex[ANIM_APPEAR] = m_pModelCom->Get_AnimationIndex("SasukeNext_etc_Appearance", 1.5);
	m_iAnimationIndex[ANIM_EXIT] = m_pModelCom->Get_AnimationIndex("SasukeNext_etc_Exit", 1.5);
	m_iAnimationIndex[ANIM_REAC1] = m_pModelCom->Get_AnimationIndex("SasukeNext_Reaction1", 1.5);
	m_iAnimationIndex[ANIM_REAC2] = m_pModelCom->Get_AnimationIndex("SasukeNext_Reaction2", 1.5);
	m_iAnimationIndex[ANIM_SPEAK] = m_pModelCom->Get_AnimationIndex("SasukeNext_spk0", 1.5);

	if (nullptr != pDesc)
	{
		m_pTransformCom->Set_Scaled(pDesc->vScale.x, pDesc->vScale.y, pDesc->vScale.z);
		m_pTransformCom->Rotation(XMConvertToRadians(pDesc->vRotation.x), XMConvertToRadians(pDesc->vRotation.y), XMConvertToRadians(pDesc->vRotation.z));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&pDesc->vPosition));
	}

	m_pModelCom->SetUp_Animation(m_iAnimationIndex[ANIM_SPEAK], true);

	m_eState = ST_EXIT;
	m_isRender = false;

	return S_OK;
}

void CSasuke_Next::Priority_Update(_float fTimeDelta)
{
}

void CSasuke_Next::Update(_float fTimeDelta)
{
	switch (m_iLevelIndex)
	{
	case LEVEL_CHUNIN:
		Update_Chunin(fTimeDelta);
		break;
	}

	if (m_pModelCom->Get_CurrentAnimationIndex() == m_iAnimationIndex[ANIM_APPEAR])
	{
		if (m_pModelCom->Get_IsEnd_Animation(m_iAnimationIndex[ANIM_APPEAR]))
		{
			m_pModelCom->SetUp_Animation(m_iAnimationIndex[ANIM_SPEAK], true);
		}

	}

	if (m_pModelCom->Get_CurrentAnimationIndex() == m_iAnimationIndex[ANIM_EXIT])
	{
		if (m_pModelCom->Get_IsEnd_Animation(m_iAnimationIndex[ANIM_EXIT]))
		{
			m_isRender = false;
			/*Safe_Release(m_pUITextWindow);
			m_pGameInstance->Event_DestoryObject(this);*/
		}

	}

	m_pModelCom->Play_Animation(fTimeDelta);
}

void CSasuke_Next::Late_Update(_float fTimeDelta)
{
	if (m_isRender)
	{
		m_pGameInstance->Add_RenderObject(CRenderer::RG_SHADOWOBJ, this);
		m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
	}
}

HRESULT CSasuke_Next::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render((_uint)i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CSasuke_Next::Render_LightDepth()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	_float4x4		ViewMatrix;
	XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(m_pGameInstance->Get_LigthDepthPosition(), m_pGameInstance->Get_LigthDepthAt(), XMVectorSet(0.f, 1.f, 0.f, 0.f)));

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i)))
			return E_FAIL;
		//if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", TEXTURE_TYPE::NORMALS, i)))
		//	return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(2)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render((_uint)i)))
			return E_FAIL;
	}

	return S_OK;
}

void CSasuke_Next::Appear()
{
	m_pModelCom->SetUp_NextAnimation(m_iAnimationIndex[ANIM_APPEAR]);
	m_isRender = true;
}

void CSasuke_Next::Exit()
{
	m_pUITextWindow->Start_Disappear();
	m_pModelCom->SetUp_NextAnimation(m_iAnimationIndex[ANIM_EXIT]);
	m_isRender = false;
}

void CSasuke_Next::Change_SpeakMode(CPlayerCamera* pPlayerCamera, _bool isCameraiControl)
{
	if (m_eState == ST_SPEAK)
		return;

	if (isCameraiControl)
	{
		_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();

		_vector vStartPos = WorldMatrix.r[3] + XMVector3Normalize(WorldMatrix.r[0]) * 0.7f + XMVector3Normalize(WorldMatrix.r[1]) * 1.3f + XMVector3Normalize(WorldMatrix.r[2]) * 1.6f;

		pPlayerCamera->Setting_CameraControl(vStartPos, vStartPos, vStartPos - XMVector3Normalize(WorldMatrix.r[2]) * 3.f);
		pPlayerCamera->Start_CameraControl();

		m_eState = ST_SPEAK;

		m_pUITextWindow->Change_WindowType(CUI_TextWindow::CENTER);
	}
	else
	{
		m_pUITextWindow->Change_WindowType(CUI_TextWindow::LEFT);
	}
}

void CSasuke_Next::End_SpeakMode()
{
	m_eState = ST_EXIT;
	m_pUITextWindow->Start_Disappear();
	m_pModelCom->SetUp_NextAnimation(m_iAnimationIndex[ANIM_EXIT]);
}

void CSasuke_Next::Change_TextWindow(_wstring strText)
{
	m_pUITextWindow->Change_Text(strText);
	if (!m_pUITextWindow->IsActive())
		m_pUITextWindow->Start_Appear();
}

HRESULT CSasuke_Next::Ready_Components()
{
	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_SasukeNext"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CSasuke_Next::Ready_UI()
{
	CGameObject::GAMEOBJECT_DESC Desc{};
	Desc.iLevelIndex = m_iLevelIndex;

	m_pUITextWindow = dynamic_cast<CUI_TextWindow*>(m_pGameInstance->Get_CloneObject_AddLayer(m_iLevelIndex, TEXT("Layer_UI"), TEXT("Prototype_UI_TextWindow"), &Desc));
	if (nullptr == m_pUITextWindow)
		return E_FAIL;

	Safe_AddRef(m_pUITextWindow);

	return S_OK;
}

void CSasuke_Next::Update_Chunin(_float fTimeDelta)
{

}

CSasuke_Next* CSasuke_Next::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSasuke_Next* pInstance = new CSasuke_Next(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSasuke_Next"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CSasuke_Next::Clone(void* pArg)
{
	CSasuke_Next* pInstance = new CSasuke_Next(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CSasuke_Next"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSasuke_Next::Free()
{
	__super::Free();

	Safe_Release(m_pUITextWindow);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
