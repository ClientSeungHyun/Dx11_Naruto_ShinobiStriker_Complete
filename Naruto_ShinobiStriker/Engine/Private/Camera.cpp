#include "..\Public\Camera.h"
#include "GameInstance.h"

CCamera::CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CCamera::CCamera(const CCamera& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera::Initialize(void* pArg)
{
	CAMERA_DESC* pDesc = static_cast<CAMERA_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	// float3로 불러온다면 이런식으로 w를 1로 채워서 넘겨주는 것도 가능
	//XMVectorSetW(XMLoadFloat3(&pDesc->vEye), 1);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pDesc->vEye));
	m_pTransformCom->LookAt(XMLoadFloat4(&pDesc->vAt));

	m_fFovy = pDesc->fFovy;
	m_fAspect = pDesc->fAspect;
	m_fNear = pDesc->fNear;
	m_fFar = pDesc->fFar;
	m_fInitFovy = m_fFovy;

	return S_OK;
}

void CCamera::Priority_Update(_float fTimeDelta)
{
	m_pGameInstance->Set_Transform(CPipeLine::D3DTS_VIEW, m_pTransformCom->Get_WorldMatrix_Inverse());
	m_pGameInstance->Set_Transform(CPipeLine::D3DTS_PROJ, XMMatrixPerspectiveFovLH(m_fFovy, m_fAspect, m_fNear, m_fFar));

	m_pGameInstance->Set_StaticTransform(CPipeLine::D3DTS_VIEW, m_pTransformCom->Get_WorldMatrix_Inverse());
	m_pGameInstance->Set_StaticTransform(CPipeLine::D3DTS_PROJ, XMMatrixPerspectiveFovLH(m_fInitFovy, m_fAspect, m_fNear, m_fFar));
}

void CCamera::Update(_float fTimeDelta)
{

}

void CCamera::Late_Update(_float fTimeDelta)
{
}

HRESULT CCamera::Render()
{
	return S_OK;
}

void CCamera::Start_Shake(_float fPower, _float fDuration)
{
	m_fShakePower = fPower;
	m_fShakeDuration = fDuration;
	m_fShakeTime = 0.f;
	m_vOriginLook = _float3(0.f, 0.f, 0.f);

	m_isShake = true;
}

void CCamera::ZoomIn(_float fFovyOffset, _float fDuration)
{
	if (m_isZoomIn)
		return;

	m_fTargetFovy = m_fInitFovy + fFovyOffset;
	m_fZoomDuration = fDuration;
	m_fZoomTime = 0.f;

	m_isZoomIn = true;
	m_isZoomOut = false;
}

void CCamera::ZoomOut(_float fDuration)
{
	m_fZoomInFovy = m_fFovy;
	m_fTargetFovy = m_fInitFovy;
	m_fZoomDuration = fDuration;
	m_fZoomTime = 0.f;

	m_isZoomIn = false;
	m_isZoomOut = true;
}

void CCamera::Free()
{
	__super::Free();
}
