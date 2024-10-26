#include "..\Public\GameObject.h"
#include "GameInstance.h"


CGameObject::CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice { pDevice }, m_pContext { pContext }
	, m_pGameInstance { CGameInstance::Get_Instance() } 
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

CGameObject::CGameObject(const CGameObject & Prototype)
	: m_pDevice { Prototype.m_pDevice }
	, m_pContext { Prototype.m_pContext }
	, m_pGameInstance{ Prototype.m_pGameInstance }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CGameObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGameObject::Initialize(void* pArg)
{
	GAMEOBJECT_DESC* pDesc = pArg != nullptr ? static_cast<GAMEOBJECT_DESC*>(pArg) : nullptr;

	m_pTransformCom = CTransform::Create(m_pDevice, m_pContext, pDesc);
	if (nullptr == m_pTransformCom)
		return E_FAIL;

	if (nullptr != pDesc)
		m_iLevelIndex = pDesc->iLevelIndex;

	return S_OK;
}

void CGameObject::Priority_Update(_float fTimeDelta)
{
}

void CGameObject::Update(_float fTimeDelta)
{
}

void CGameObject::Late_Update(_float fTimeDelta)
{
}

HRESULT CGameObject::Render()
{
	if (!m_isRender)
		return E_FAIL;

	return S_OK;
}

CComponent * CGameObject::Find_Component(const _wstring & strComponentTag)
{
	auto	iter = m_Components.find(strComponentTag);

	if (iter == m_Components.end())
		return nullptr;

	return iter->second;
}

CComponent* CGameObject::Find_Component(const COMPONENT_TYPE eType)
{
	for (auto& Component : m_Components)
	{
		if (eType == Component.second->Get_ComponentType())
			return Component.second;
	}

	return nullptr;
}

_float2 CGameObject::Calculate_ProjPosition(_float vOffsetY)
{
	// 카메라의 뷰 행렬과 투영 행렬을 가져옴
	_matrix		ViewMatrix = m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW);
	_matrix		ProjMatrix = m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ);

	// 뷰 행렬과 투영행렬을 곱해여 투영 스페이스로 변환함 (-1 ~ 1)사이의 값 좌상단이 (-1,1)
	_vector		vPos = Get_Position();
	vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + vOffsetY);
	vPos = XMVector3TransformCoord(vPos, ViewMatrix);

	if (XMVectorGetZ(vPos) < 0.0f)
	{
		return _float2(-1.0f, -1.0f); // 카메라 뒤에 있으면 UI를 그리지 않음
	}

	vPos = XMVector3TransformCoord(vPos, ProjMatrix);

	if (XMVectorGetX(vPos) < -1.0f || XMVectorGetX(vPos) > 1.0f ||
		XMVectorGetY(vPos) < -1.0f || XMVectorGetY(vPos) > 1.0f)
	{
		return _float2(-1.0f, -1.0f); // 화면 밖의 위치를 반환
	}

	_uint				iNumView = { 1 };
	D3D11_VIEWPORT		ViewportDesc{};
	m_pContext->RSGetViewports(&iNumView, &ViewportDesc);

	_float				fViewWidth = ViewportDesc.Width;
	_float				fViewHeight = ViewportDesc.Height;

	// x : -1 -> 0, 1->Width   
	// y : 1 -> 0, -1 -> height
	_float2 vScreenPos = _float2((XMVectorGetX(vPos) + 1.0f) * (fViewWidth * 0.5f),  // X 좌표 변환
		(1.0f - XMVectorGetY(vPos)) * (fViewHeight * 0.5f)); // Y 좌표 변환

	return vScreenPos;
}

_float2 CGameObject::Calculate_ScreenPosition_ClampSide(_float vOffsetY)
{
	// 카메라의 뷰 행렬과 투영 행렬을 가져옴
	_matrix		ViewMatrix = m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW);
	_matrix		ProjMatrix = m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ);

	// 뷰 행렬과 투영행렬을 곱해여 투영 스페이스로 변환함 (-1 ~ 1)사이의 값 좌상단이 (-1,1)
	_vector		vPos = Get_Position();
	vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + vOffsetY);
	
	_vector vViewPos = XMVector3TransformCoord(vPos, ViewMatrix);	//뷰 스페이스
	vPos = XMVector3TransformCoord(vViewPos, ProjMatrix);				// 투영 스페이스

	_float3 vProjPos;
	XMStoreFloat3(&vProjPos, vPos);

	// 뒤에 있을 때
	if (XMVectorGetZ(vViewPos) < 0.0f)
	{
		if (XMVectorGetX(vViewPos) <= 0.0f)
			vProjPos.x = -0.93f;
		else
			vProjPos.x = 0.93f;

		if (XMVectorGetY(vViewPos) <= 0.0f)
			vProjPos.y = -0.9f;
		else
			vProjPos.y = 0.93f;
	}
	// 정면에 보일 때
	else
	{
		if (vProjPos.x <= -0.93f)
			vProjPos.x = -0.93f;
		else if (vProjPos.x >= 0.93f)
			vProjPos.x = 0.93f;

		if (vProjPos.y <= -0.93f)
			vProjPos.y = -0.93f;
		else if (vProjPos.y >= 0.93f)
			vProjPos.y = 0.93f;

	}

	_uint				iNumView = { 1 };
	D3D11_VIEWPORT		ViewportDesc{};
	m_pContext->RSGetViewports(&iNumView, &ViewportDesc);

	_float				fViewWidth = ViewportDesc.Width;
	_float				fViewHeight = ViewportDesc.Height;

	// x : -1 -> 0, 1->Width   
	// y : 1 -> 0, -1 -> height
	_float2 vScreenPos = _float2((vProjPos.x + 1.0f) * (fViewWidth * 0.5f),  // X 좌표 변환
		(1.0f - vProjPos.y) * (fViewHeight * 0.5f)); // Y 좌표 변환

	return vScreenPos;
}


//if (XMVectorGetY(vViewPos) <= 0.0f)
//	vProjPos.y = -0.95f;  
//else
//	vProjPos.y = 0.95f;  

HRESULT CGameObject::Add_Component(_uint iLevelIndex, const _wstring & strPrototypeTag, const _wstring & strComponentTag, CComponent** ppOut, void * pArg)
{
	if (nullptr != Find_Component(strComponentTag))
		return E_FAIL;

	CComponent*		pComponent = m_pGameInstance->Clone_Component(iLevelIndex, strPrototypeTag, pArg);
	if (nullptr == pComponent)
		return E_FAIL;

	m_Components.emplace(strComponentTag, pComponent);

	*ppOut = pComponent;

	Safe_AddRef(pComponent);
	
	return S_OK;
}

void CGameObject::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);

	for (auto& Pair : m_Components)
		Safe_Release(Pair.second);

	m_Components.clear();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
