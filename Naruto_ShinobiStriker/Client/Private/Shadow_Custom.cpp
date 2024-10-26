#include "stdafx.h"
#include "Shadow_Custom.h"
#include "Player_Custom.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"

#include "Head_Player.h"
#include "Face_Player.h"
#include "BodyOnce_Player.h"
#include "BodyUpper_Player.h"
#include "BodyLower_Player.h"

#include "ObjectPool.h"
#include "Smoke11.h"
#include "SmokeRing.h"

CShadow_Custom::CShadow_Custom(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayer{ pDevice, pContext }
{
}

CShadow_Custom::CShadow_Custom(const CShadow_Custom& Prototype)
	: CPlayer{ Prototype }
{
}

HRESULT CShadow_Custom::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CShadow_Custom::Initialize(void* pArg)
{
	PLAYER_DESC* pDesc = static_cast<PLAYER_DESC*>(pArg);
	m_isShadow = true;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	m_isActive = false;

	return S_OK;
}

void CShadow_Custom::Priority_Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	Parts_Priority_Update(fTimeDelta);
}

void CShadow_Custom::Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	if (m_pModelCom->Get_IsEnd_Animation(m_iAnimationIndex))
	{
		_matrix		WorldMatirx = m_pOwner->Get_Transform()->Get_WorldMatrix();

		_vector		vRight = XMVector3Normalize(WorldMatirx.r[0]);
		_vector		vUp = XMVector3Normalize(WorldMatirx.r[1]);
		_vector		vLook = XMVector3Normalize(WorldMatirx.r[2]);
		_vector		vPosition = WorldMatirx.r[3];

		if (LEFT == m_eDir)
		{
			vPosition += (vRight * -1.f) + (vUp * 0.7f) + (vLook * -1.f);
			CObjectPool<CSmokeRing>::Get_GameObject()->Appear(vPosition, 4.f, 0.3f, 5.f);
		}
		else if (RIGHT == m_eDir)
		{
			vPosition += (vRight * 1.f) + (vUp * 0.7f) + (vLook * -1.f);
			CObjectPool<CSmokeRing>::Get_GameObject()->Appear(vPosition, 4.f, 0.3f, 5.f);

		}


		vPosition -= XMVector3Normalize(vLook) * 0.3f;

		_float3 vPos;
		XMStoreFloat3(&vPos, vPosition);

		for (_uint i = 0; i < 10; ++i)
		{
			CObjectPool<CSmoke11>::Get_GameObject()->Appear(vPos, _float3(0.1f, 0.25f, 0.1f), 0.3f, CSmoke11::SMOKE_WHITE);
		}

		m_isActive = false;
	}

	m_pModelCom->Play_Animation(fTimeDelta);

	Parts_Update(fTimeDelta);
}

void CShadow_Custom::Late_Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	Parts_Late_Update(fTimeDelta);
}

HRESULT CShadow_Custom::Render()
{
	return S_OK;
}

void CShadow_Custom::Parts_Priority_Update(_float fTimeDelta)
{
	for (size_t i = 0; i < PART_END; ++i)
	{
		if (m_Parts[i] == nullptr)
			continue;

		if (m_tCustomInfo.isOnceClothes && (i == PART_BODYUPPER || i == PART_BODYLOWER))
			continue;
		else if (!m_tCustomInfo.isOnceClothes && i == PART_BODYONCE)
			continue;

		m_Parts[i]->Priority_Update(fTimeDelta);
	}
}

void CShadow_Custom::Parts_Update(_float fTimeDelta)
{
	for (size_t i = 0; i < PART_END; ++i)
	{
		if (m_Parts[i] == nullptr)
			continue;

		if (m_tCustomInfo.isOnceClothes && (i == PART_BODYUPPER || i == PART_BODYLOWER))
			continue;
		else if (!m_tCustomInfo.isOnceClothes && i == PART_BODYONCE)
			continue;

		m_Parts[i]->Update(fTimeDelta);
	}
}

void CShadow_Custom::Parts_Late_Update(_float fTimeDelta)
{
	for (size_t i = 0; i < PART_END; ++i)
	{
		if (m_Parts[i] == nullptr)
			continue;

		if (m_tCustomInfo.isOnceClothes && (i == PART_BODYUPPER || i == PART_BODYLOWER))
			continue;
		else if (!m_tCustomInfo.isOnceClothes && i == PART_BODYONCE)
			continue;

		m_Parts[i]->Late_Update(fTimeDelta);
	}
}

void CShadow_Custom::Create_Shadow(SHADOW_DESC tDesc)
{
	if (m_isActive)
		return;

	m_eSkill = tDesc.eSkill;
	m_eDir = tDesc.eDir;

	_matrix		OwnerWorldMatirx = m_pOwner->Get_Transform()->Get_WorldMatrix();

	m_pTransformCom->Set_WorldMatrix(OwnerWorldMatirx);

	_vector		vRight = XMVector3Normalize(OwnerWorldMatirx.r[0]);
	_vector		vUp = XMVector3Normalize(OwnerWorldMatirx.r[1]);
	_vector		vLook = XMVector3Normalize(OwnerWorldMatirx.r[2]);
	_vector		vPosition = OwnerWorldMatirx.r[3];

	if (RASENSHURIKEN == m_eSkill)
	{
		if (LEFT == m_eDir)
		{
			m_iAnimationIndex = m_pModelCom->Get_AnimationIndex("CustomMan_ShadowClone_Ninjutsu_TrueRasenShuriken_Left", 1.5);
		}
		else
		{
			m_iAnimationIndex = m_pModelCom->Get_AnimationIndex("CustomMan_ShadowClone_Ninjutsu_TrueRasenShuriken_Right", 1.5);
		}
	}
	else if (RASENSHURIKEN_AERIAL == m_eSkill)
	{
		if (LEFT == m_eDir)
		{
			m_iAnimationIndex = m_pModelCom->Get_AnimationIndex("CustomMan_ShadowClone_Ninjutsu_Aerial_TrueRasenShuriken_Left", 1.5);
		}
		else
		{
			m_iAnimationIndex = m_pModelCom->Get_AnimationIndex("CustomMan_ShadowClone_Ninjutsu_Aerial_TrueRasenShuriken_Right", 1.5);
		}
	}
	else if (MASSIVERASENGAN == m_eSkill)
	{
		if (LEFT == m_eDir)
		{
			m_iAnimationIndex = m_pModelCom->Get_AnimationIndex("CustomMan_ShadowClone_Ninjutsu_MassiveRasengan_Left_Loop", 0.9);
		}
		else
		{
			m_iAnimationIndex = m_pModelCom->Get_AnimationIndex("CustomMan_ShadowClone_Ninjutsu_MassiveRasengan_Right_Loop", 0.9);
		}
	}

	if (LEFT == m_eDir)
	{
		vPosition += (vRight * -1.f) + (vUp * 0.7f) + (vLook * -1.f);
		CObjectPool<CSmokeRing>::Get_GameObject()->Appear(vPosition, 4.f, 0.3f, 5.f);
	}
	else if (RIGHT == m_eDir)
	{
		vPosition += (vRight * 1.f) + (vUp * 0.7f) + (vLook * -1.f);
		CObjectPool<CSmokeRing>::Get_GameObject()->Appear(vPosition, 4.f, 0.3f, 5.f);

	}

	vPosition -= XMVector3Normalize(vLook) * 0.3f;

	_float3 vPos;
	XMStoreFloat3(&vPos, vPosition);

	m_pModelCom->SetUp_NextAnimation(m_iAnimationIndex, false, 0.f);

	
	for (_uint i = 0; i < 10; ++i)
	{
		CObjectPool<CSmoke11>::Get_GameObject()->Appear(vPos, _float3(0.3f, 0.25f, 0.3f), 0.3f, CSmoke11::SMOKE_WHITE);
	}

	m_isActive = true;
}

HRESULT CShadow_Custom::Ready_PartObjects()
{
	m_Parts.resize(PART_END);

	if (m_iLevelIndex == LEVEL_CUSTOM)
	{
		ZeroMemory(&m_tCustomInfo, sizeof(CUSTOM_INFO));
		m_tCustomInfo.isOnceClothes = true;
	}
	else
	{
		Load_CustomInfo();
	}

	CPartObject::PARTOBJ_DESC PartDesc{};
	PartDesc.iLevelIndex = m_iLevelIndex;
	PartDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();

	PartDesc.iPartID = m_tCustomInfo.iOnceID;
	if (FAILED(__super::Add_PartObject(PART_BODYONCE, TEXT("Prototype_GameObject_BodyOnce_Player"), &PartDesc)))
		return E_FAIL;
	m_pModelCom = m_Parts[PART_BODYONCE]->Get_Model();
	Safe_AddRef(m_pModelCom);

	PartDesc.iPartID = m_tCustomInfo.iFaceID;
	if (FAILED(__super::Add_PartObject(PART_FACE, TEXT("Prototype_GameObject_Face_Player"), &PartDesc)))
		return E_FAIL;
	dynamic_cast<CFace_Player*>(m_Parts[PART_FACE])->Change_Parts(CFace_Player::FACE_ID(m_tCustomInfo.iFaceID), m_pModelCom);

	PartDesc.iPartID = m_tCustomInfo.iHeadID;
	if (FAILED(__super::Add_PartObject(PART_HEAD, TEXT("Prototype_GameObject_Head_Player"), &PartDesc)))
		return E_FAIL;
	dynamic_cast<CHead_Player*>(m_Parts[PART_HEAD])->Change_Parts(CHead_Player::HEAD_ID(m_tCustomInfo.iHeadID), m_pModelCom);

	if (m_iLevelIndex == LEVEL_CUSTOM || (m_iLevelIndex != LEVEL_CUSTOM && false == m_tCustomInfo.isOnceClothes))
	{
		PartDesc.iPartID = m_tCustomInfo.iUpperID;
		if (FAILED(__super::Add_PartObject(PART_BODYUPPER, TEXT("Prototype_GameObject_BodyUpper_Player"), &PartDesc)))
			return E_FAIL;
		dynamic_cast<CBodyUpper_Player*>(m_Parts[PART_BODYUPPER])->Change_Parts(CBodyUpper_Player::UPPER_ID(m_tCustomInfo.iUpperID), m_pModelCom);

		PartDesc.iPartID = m_tCustomInfo.iLowerID;
		if (FAILED(__super::Add_PartObject(PART_BODYLOWER, TEXT("Prototype_GameObject_BodyLower_Player"), &PartDesc)))
			return E_FAIL;
		dynamic_cast<CBodyLower_Player*>(m_Parts[PART_BODYLOWER])->Change_Parts(CBodyLower_Player::LOWER_ID(m_tCustomInfo.iLowerID), m_pModelCom);
	}


	for (size_t i = 0; i < PART_END; ++i)
	{
		if (m_Parts[i] == nullptr || i == PART_BODYONCE)
			continue;

		m_Parts[i]->Get_Model()->Set_Bones(m_pModelCom->Get_Bones());
		m_Parts[i]->Get_Model()->Set_Animations(m_pModelCom->Get_Animations(), m_pModelCom->Get_KeyFrameIndicies());
	}

	return S_OK;
}

HRESULT CShadow_Custom::Load_CustomInfo()
{
	_ulong			dwByte = {};
	HANDLE			hFile = CreateFile(TEXT("../Bin/DataFiles/Custom.dat"), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	ReadFile(hFile, &m_tCustomInfo, sizeof(CPlayer_Custom::CUSTOM_INFO), &dwByte, nullptr);

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CShadow_Custom::Ready_Components()
{

	return S_OK;
}

CShadow_Custom* CShadow_Custom::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CShadow_Custom* pInstance = new CShadow_Custom(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer_MotionTrail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CShadow_Custom::Clone(void* pArg)
{
	CShadow_Custom* pInstance = new CShadow_Custom(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CPlayer_MotionTrail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShadow_Custom::Free()
{

	__super::Free();

	//Safe_Release(m_pModelCom);
}
