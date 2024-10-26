#include "stdafx.h"

#include "PlayerCamera.h"
#include "GameInstance.h"

#include "Player.h"

CPlayerCamera::CPlayerCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera{ pDevice, pContext }
{
}

CPlayerCamera::CPlayerCamera(const CCamera& Prototype)
	:CCamera{ Prototype }
{
}

HRESULT CPlayerCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayerCamera::Initialize(void* pArg)
{
	CAMERA_PLAYER_DESC* pDesc = static_cast<CAMERA_PLAYER_DESC*>(pArg);
	m_pPlayer = pDesc->pPlayer;
	m_fSpeed = pDesc->fSpeed;

	m_vOffset = _float3(0.0f, 1.0f, 0.0f);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	_vector vInitPos = m_pPlayer->Get_Transform()->Get_State(CTransform::STATE::STATE_POSITION) + XMVectorSet(0.0f, 5.0f, -5.0f, 0.f);
	m_pTransformCom->Set_State(CTransform::STATE::STATE_POSITION, vInitPos);

	_vector vOffset = m_pTransformCom->Get_State(CTransform::STATE_POSITION) - m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	vOffset = XMVector3Normalize(vOffset);

	_vector vRadian = XMVector3Dot(vOffset, XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	XMStoreFloat(&m_fRadian, vRadian);
	m_fRadian = acosf(m_fRadian);

	return S_OK;
}

void CPlayerCamera::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

	if (m_isControl)
	{
		m_pGameInstance->Set_Transform(CPipeLine::D3DTS_PROJ, XMMatrixPerspectiveFovLH(m_fSkillFovy, m_fAspect, m_fNear, m_fFar));
	}
}

void CPlayerCamera::Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	if (m_isShake)
	{
		m_fShakeTime += fTimeDelta;
		if (m_fShakeTime >= m_fShakeDuration)
		{
			m_fShakeTime = 0.f;
			m_isShake = false;
		}

		// 쉐이크 계산 (랜덤 값 생성)
		float xOffset = (rand() % 100 / 100.0f) * m_fShakePower - (m_fShakePower / 2);
		float yOffset = (rand() % 100 / 100.0f) * m_fShakePower - (m_fShakePower / 2);
		_vector shakeOffset = XMVectorSet(xOffset, yOffset, 0.f, 0.f);

		// 카메라 위치에 쉐이크 오프셋 추가
		_vector currentPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		currentPos += shakeOffset;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, currentPos);
	}
	
	if (m_isZoomIn)
	{
		m_fZoomTime += fTimeDelta;
		m_fZoomTime /= m_fZoomDuration;
		if (m_fZoomTime >= 1.f)
		{
			m_fFovy = m_fTargetFovy;
			m_fZoomTime = 1.f;
		}
		else
			m_fFovy = m_pGameInstance->Lerp(m_fInitFovy, m_fTargetFovy, m_fZoomTime);
	}
	else if (m_isZoomOut)
	{
		m_fZoomTime += fTimeDelta;
		m_fZoomTime /= m_fZoomDuration;
		if (m_fZoomTime >= 1.f)
		{
			m_fFovy = m_fTargetFovy;
			m_fZoomTime = 1.f;
		}
		else
		{
			m_fFovy = m_pGameInstance->Lerp(m_fZoomInFovy, m_fTargetFovy, m_fZoomTime);
		}
	}

	if (m_isControl)
	{
		Control_Camera(fTimeDelta);
	}
	else
	{
		Control_Player(fTimeDelta);
	}
}

void CPlayerCamera::Late_Update(_float fTimeDelta)
{
	//m_pGameInstance->Set_Listener(Get_Position());
}

HRESULT CPlayerCamera::Render()
{
	return S_OK;
}

void CPlayerCamera::Control_Player(_float fTimeDelta)
{	// 회전용 플레이어 포지션
	_vector		vPlayerPosition = m_pPlayer->Get_Position() + XMLoadFloat3(&m_vOffset);

	_matrix		WorldMatrix = m_pTransformCom->Get_WorldMatrix();

	/* 플레이어 우치로 이동한 카메라의 월드 행렬 */
	_matrix RotationMatrix = m_pTransformCom->Get_WorldMatrix();
	RotationMatrix = XMMatrixTranslationFromVector(vPlayerPosition);

	// 플레이어 위치로 변환한 행렬을 로컬로 내림
	_matrix		LocalMatrix = WorldMatrix * XMMatrixInverse(nullptr, RotationMatrix);

#pragma region 카메라 회전

	POINT		ptMouse = m_pGameInstance->Get_MosePos();

	_long	MouseMove = { 0 };

	// 좌 우
	if (MouseMove = ptMouse.x - m_ptOldMousePos.x)
	{
		if (MouseMove < -5)
			m_fCurrentRotationX -= 0.1f * fTimeDelta;
		else if (MouseMove > 5)
			m_fCurrentRotationX += 0.1f * fTimeDelta;

		if (fabs(m_fCurrentRotationX) > 0.03f)
			m_fCurrentRotationX = (m_fCurrentRotationX / fabs(m_fCurrentRotationX)) * 0.05f;
	}
	else if (MouseMove == 0)
	{
		m_fCurrentRotationX -= m_fCurrentRotationX * 0.3f;
		if (fabs(m_fCurrentRotationX) < 0.0005f)
			m_fCurrentRotationX = 0.0f;
	}

	// 상 하
	if (MouseMove = ptMouse.y - m_ptOldMousePos.y)
	{
		if (MouseMove < -5)
			m_fCurrentRotationY -= 0.1f * fTimeDelta;
		else if (MouseMove > 5)
			m_fCurrentRotationY += 0.1f * fTimeDelta;

		if (fabs(m_fCurrentRotationY) > 0.03f)
			m_fCurrentRotationY = (m_fCurrentRotationY / fabs(m_fCurrentRotationY)) * 0.05f;
	}
	else if (MouseMove == 0)
	{
		m_fCurrentRotationY -= m_fCurrentRotationY * 0.3f;
		if (fabs(m_fCurrentRotationY) < 0.0005f)
			m_fCurrentRotationY = 0.0f;
	}

	// 회전이 있으면
	if (m_fCurrentRotationX)
	{
		LocalMatrix = LocalMatrix * XMMatrixRotationQuaternion(XMQuaternionRotationAxis(XMVectorSet(0.0f, 1.0f, 0.f, 0.f), m_fCurrentRotationX));
	}

	if (m_fCurrentRotationY)
	{
		LocalMatrix = LocalMatrix * XMMatrixRotationQuaternion(XMQuaternionRotationAxis(LocalMatrix.r[0], m_fCurrentRotationY));
	}

#pragma endregion

	// 회전 상태를 저장해둔 월드 행렬임
	WorldMatrix = LocalMatrix * RotationMatrix;

	_vector vOffset = WorldMatrix.r[3] - vPlayerPosition;
	vOffset = XMVector3Normalize(vOffset);

	_vector		vPlayerUp = XMVector3Normalize(WorldMatrix.r[1]);
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	//XMVector3Normalize(m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_UP));

// 현재 카메라 각도
	if (m_fCurrentRotationY)
		m_fRadian = acosf(XMVectorGetX(XMVector3Dot(vUp, vOffset)));

	_float fAngle = XMConvertToDegrees(m_fRadian);

	if (fAngle < 30.0f)
	{
		m_fRadian = XMConvertToRadians(30.0f);
		m_fCurrentRotationY = 0.0f;
	}
	if (fAngle > 97.f)
	{
		m_fRadian = XMConvertToRadians(97.f);
		m_fCurrentRotationY = 0.0f;
	}

#pragma region 플레이어 따라가기


	_vector vRight = WorldMatrix.r[3] - vPlayerPosition;			// 직선 벡터
	vRight = XMVector3Normalize(XMVector3Cross(vRight, vUp));		// 외적으로 right 벡터 구하기

	// 카메라의 목표 위치를 설정함
	_vector vTargetPosition = XMVector3Rotate(vUp, XMQuaternionRotationAxis(-vRight, m_fRadian));
	vTargetPosition = vPlayerPosition + vTargetPosition * 5.f;

	_vector vTargetLook = -vOffset;
	vTargetLook = XMVector3Normalize(vTargetLook);

	_vector vCameraPosition = WorldMatrix.r[3];
	_vector vCameraLook = XMVector3Normalize(WorldMatrix.r[2]);

	_float fLerpSpeed = 8.f * fTimeDelta;
	if (m_pPlayer->Get_Fsm()->Get_CurrentState() == CPlayer::DASH)
		fLerpSpeed = 1.f;

	_vector vFinalPosition = XMVectorLerp(vCameraPosition, vTargetPosition, fLerpSpeed);
	_vector vFinalLook = XMVectorLerp(vCameraLook, vTargetLook, fLerpSpeed);
	vFinalLook = XMVector3Normalize(vFinalLook);

	XMVECTOR vFinalRight = XMVector3Cross(vUp, vFinalLook);
	vFinalRight = XMVector3Normalize(vFinalRight);

	XMVECTOR vFinalUp = XMVector3Cross(vFinalLook, vFinalRight);
	vFinalUp = XMVector3Normalize(vFinalUp);

	WorldMatrix.r[0] = vFinalRight;
	WorldMatrix.r[1] = vFinalUp;
	WorldMatrix.r[2] = vFinalLook;
	WorldMatrix.r[3] = vFinalPosition;
#pragma endregion

	m_pTransformCom->Set_WorldMatrix(WorldMatrix);

	if (m_pPlayer->Get_IsWall() == false)
	{
		_vector vCurrentPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_float vPlayerPosY = XMVectorGetY(vPlayerPosition);

		if (XMVectorGetY(vCurrentPos) < vPlayerPosY - 0.8f)
		{
			vCurrentPos = XMVectorSetY(vCurrentPos, vPlayerPosY - 0.8f);
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCurrentPos);
		}
	}

	m_pTransformCom->Set_WorldMatrix(WorldMatrix);

	m_ptOldMousePos = ptMouse;
}

void CPlayerCamera::Control_Camera(_float fTimeDelta)
{
	_vector vStartPos = XMLoadFloat3(&m_vSkillCameraStartPos);
	_vector vEndPos = XMLoadFloat3(&m_vSkillCameraEndPos);
	_vector vNewPos = vStartPos;

	if (m_isLerp)
	{
		m_fSkillLerpTime += fTimeDelta;

		_float fRatio = m_fSkillLerpTime / m_fSkillLerpDuration;

		vNewPos = XMVectorLerp(vStartPos, vEndPos, fRatio);
		if (fRatio >= 1.f)
		{
			m_fSkillLerpTime = m_fSkillLerpDuration;
			vNewPos = vEndPos;
			m_isLerpEnd = true;
		}
	}

	if (m_isKeepAt)
	{
		m_pTransformCom->LookAt(XMLoadFloat3(&m_vSkillCameraAt));
	}
	else
	{
		_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();

		_vector vAt = WorldMatrix.r[3] + WorldMatrix.r[2] * 2.f;

		m_pTransformCom->LookAt(vAt);
	}
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vNewPos);
}

void CPlayerCamera::Setting_CameraControl(_float3 vStartPos, _float3 vEndPos, _float3 vCameraAt, _float fFovy, _bool isKeepAt)
{
	m_vSkillCameraStartPos = vStartPos;
	m_vSkillCameraEndPos = vEndPos;
	m_vSkillCameraAt = vCameraAt;
	m_isKeepAt = isKeepAt;

	if (fFovy == 0.f)
		m_fSkillFovy = m_fFovy;
	else
		m_fSkillFovy = fFovy;

	m_isLerp = false;
}

void CPlayerCamera::Setting_CameraControl(_fvector vStartPos, _fvector vEndPos, _fvector vCameraAt, _float fFovy, _bool isKeepAt)
{
	XMStoreFloat3(&m_vSkillCameraStartPos, vStartPos);
	XMStoreFloat3(&m_vSkillCameraEndPos, vEndPos);
	XMStoreFloat3(&m_vSkillCameraAt, vCameraAt);
	m_isKeepAt = isKeepAt;

	if (fFovy == 0.f)
		m_fSkillFovy = m_fFovy;
	else
		m_fSkillFovy = m_fFovy + fFovy;

	m_isLerp = false;
}

void CPlayerCamera::Start_CameraControl()
{
	m_pTransformCom->LookAt(XMLoadFloat3(&m_vSkillCameraAt));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_vSkillCameraStartPos));
	m_isControl = true;
}

void CPlayerCamera::Start_CameraLerp(_float fLerpDuration)
{
	m_fSkillLerpTime = 0.f;
	m_fSkillLerpDuration = fLerpDuration;
	m_isLerp = true;
	m_isLerpEnd = false;
}

void CPlayerCamera::End_CameraControl()
{
	m_isControl = false;
	m_isLerp = false;
	m_isLerpEnd = false;
}

HRESULT CPlayerCamera::Ready_Components()
{
	return S_OK;
}

void CPlayerCamera::Rotation_CustomLevel(_float fTimeDelta)
{
}

void CPlayerCamera::CameraShaking(_matrix WorldMatrix)
{
}

CPlayerCamera* CPlayerCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayerCamera* pInstance = new CPlayerCamera(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CCamera_Player"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CPlayerCamera::Clone(void* pArg)
{
	CPlayerCamera* pInstance = new CPlayerCamera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CCamera_Player"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerCamera::Free()
{
	__super::Free();
}
