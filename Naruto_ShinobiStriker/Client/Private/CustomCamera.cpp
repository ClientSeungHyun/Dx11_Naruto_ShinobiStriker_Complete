#include "stdafx.h"

#include "GameInstance.h"
#include "CustomCamera.h"

CCustomCamera::CCustomCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera{ pDevice, pContext }
{
}

CCustomCamera::CCustomCamera(const CCamera& Prototype)
	:CCamera{ Prototype }
{
}

HRESULT CCustomCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCustomCamera::Initialize(void* pArg)
{
	CAMERA_CUSTOM_DESC* Desc = static_cast<CAMERA_CUSTOM_DESC*>(pArg);

	m_fSensor = Desc->fSensor;

	if (FAILED(__super::Initialize(Desc)))
		return E_FAIL;

	m_fInitFovy = Desc->fFovy;
	XMStoreFloat3(&m_vInitPos, Get_Position());

	return S_OK;
}

void CCustomCamera::Priority_Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	POINT		ptMouse = m_pGameInstance->Get_MosePos();

	_long	MouseMove = {};


	if (KEY_HOLD(KEY::LSHIFT))
	{
		if (KEY_HOLD(KEY::W))
			m_pTransformCom->Go_Straight(fTimeDelta);
		if (KEY_HOLD(KEY::S))
			m_pTransformCom->Go_Backward(fTimeDelta);
		if (KEY_HOLD(KEY::A))
			m_pTransformCom->Go_Left(fTimeDelta);
		if (KEY_HOLD(KEY::D))
			m_pTransformCom->Go_Right(fTimeDelta);


		if (MouseMove = ptMouse.x - m_ptOldMousePos.x)
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * MouseMove * 0.1f);
		}

		if (MouseMove = ptMouse.y - m_ptOldMousePos.y)
		{
			m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), fTimeDelta * MouseMove * 0.1f);
		}
	}

	if (KEY_TAP(KEY::UP))
		Start_ZoomIn(XMLoadFloat3(&m_vInitPos), 0.2f, 1.f);
	if (KEY_TAP(KEY::DOWN))
		End_ZoomIn(1.f);

	if (m_isZoomIn)
	{
		ZoomIn(fTimeDelta);
	}
	else if (m_isZoomOut)
	{
		ZoomOut(fTimeDelta);
	}
		

	__super::Priority_Update(fTimeDelta);

	m_ptOldMousePos = ptMouse;
}

void CCustomCamera::Update(_float fTimeDelta)
{
}

void CCustomCamera::Late_Update(_float fTimeDelta)
{
}

HRESULT CCustomCamera::Render()
{
	return S_OK;
}

void CCustomCamera::ZoomIn(_float fTimeDelta)
{
	m_fZoomTime += fTimeDelta;

	_float fProgress = min(m_fZoomTime / m_fZoomDuration, 1.0f);
	m_fFovy = m_fInitFovy - (m_fZoom * fProgress);

	_vector vPos = Get_Position();
	_vector vNewPos = XMVectorLerp(vPos, XMLoadFloat3(&m_vTargetPos), m_fZoomTime / m_fZoomDuration);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vNewPos);

	if (m_fZoomTime >= m_fZoomDuration)
	{
		m_fZoomTime = m_fZoomDuration;
		m_fFovy = m_fInitFovy - m_fZoom;
		m_isZoomIn = false;
		m_isStateZoomIn = true;
	}
}

void CCustomCamera::ZoomOut(_float fTimeDelta)
{
	m_fZoomTime += fTimeDelta;

	_float fProgress = min(m_fZoomTime / m_fZoomDuration, 1.0f);
	m_fFovy = m_fInitFovy - (m_fZoom * (1.0f - fProgress));

	_vector vPos = Get_Position();
	_vector vNewPos = XMVectorLerp(vPos, XMLoadFloat3(&m_vInitPos), m_fZoomTime / m_fZoomDuration);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vNewPos);

	if (m_fZoomTime >= m_fZoomDuration)
	{
		m_fZoomTime = m_fZoomDuration;
		m_fFovy = m_fInitFovy;
		m_isZoomOut = false;
		m_isStateZoomIn = false;
	}
}

void CCustomCamera::Start_ZoomIn(_fvector vZoomPos, _float fZoom, _float fDuration)
{
	if (m_isZoomIn || m_isStateZoomIn)
		return;

	XMStoreFloat3(&m_vTargetPos, vZoomPos);
	m_fZoom = fZoom;
	m_fZoomTime = 0.f;
	m_fZoomDuration = fDuration;

	m_isZoomIn = true;
}

void CCustomCamera::End_ZoomIn(_float fDuration)
{
	if (m_isZoomOut || !m_isStateZoomIn)
		return;

	m_fZoomTime = 0.f;
	m_fZoomDuration = fDuration;

	m_isZoomOut = true;
}

HRESULT CCustomCamera::Ready_Components()
{
	return S_OK;
}

CCustomCamera* CCustomCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCustomCamera* pInstance = new CCustomCamera(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CCamera_Player"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CCustomCamera::Clone(void* pArg)
{
	CCustomCamera* pInstance = new CCustomCamera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CCamera_Player"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCustomCamera::Free()
{
	__super::Free();
}
