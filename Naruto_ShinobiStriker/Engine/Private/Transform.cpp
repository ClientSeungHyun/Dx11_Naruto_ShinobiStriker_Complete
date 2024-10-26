#include "..\Public\Transform.h"
#include "Shader.h"
#include "Navigation.h"
#include "Camera.h"
#include "GameInstance.h"

CTransform::CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
	m_eComponentType = TRANSFORM;
}

HRESULT CTransform::Initialize_Prototype()
{
	m_eComponentType = TRANSFORM;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	return S_OK;
}

HRESULT CTransform::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		TRANSFORM_DESC* pDesc = static_cast<TRANSFORM_DESC*>(pArg);

		m_fSpeedPerSec = pDesc->fSpeedPerSec;
		m_fRotationPerSec = pDesc->fRotationPerSec;
	}
	else
	{
		m_fSpeedPerSec = 0.f;
		m_fRotationPerSec = 0.f;
	}

	return S_OK;
}

_float3 CTransform::Get_Scaled() const
{
	_matrix		WorldMatrix = Get_WorldMatrix();

	return _float3(XMVectorGetX(XMVector3Length(WorldMatrix.r[STATE_RIGHT])),
		XMVectorGetX(XMVector3Length(WorldMatrix.r[STATE_UP])),
		XMVectorGetX(XMVector3Length(WorldMatrix.r[STATE_LOOK])));
}

void CTransform::Set_Scaled(_float fX, _float fY, _float fZ)
{
	Set_State(STATE_RIGHT, XMVector3Normalize(Get_State(STATE_RIGHT)) * fX);
	Set_State(STATE_UP, XMVector3Normalize(Get_State(STATE_UP)) * fY);
	Set_State(STATE_LOOK, XMVector3Normalize(Get_State(STATE_LOOK)) * fZ);
}

void CTransform::Move_Dir(_float fTimeDelta, _float3 vDir, _float fSpeed)
{
	_vector		vPosition = Get_State(STATE_POSITION);

	if (fSpeed <= 0.f)
		fSpeed = m_fSpeedPerSec;

	vPosition += XMVector3Normalize(XMLoadFloat3(&vDir)) * fSpeed * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Straight(_float fTimeDelta, _float fSpeed, class CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vLook = Get_State(STATE_LOOK);

	if (fSpeed <= 0.f)
		fSpeed = m_fSpeedPerSec;

	vPosition += XMVector3Normalize(vLook) * fSpeed * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);

	//if (nullptr == pNavigation)
	//{
	//	Set_State(STATE_POSITION, vPosition);
	//}
	//else
	//{
	//	if (true == pNavigation->isMove(vPosition))
	//	{
	//		Set_State(STATE_POSITION, vPosition);
	//		pNavigation->SetUp_OnCell(this, 0.1f);
	//	}
	//	else
	//	{

	//	}
	//}
}

void CTransform::Go_Backward(_float fTimeDelta, _float fSpeed)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vLook = Get_State(STATE_LOOK);

	if (fSpeed <= 0.f)
		fSpeed = m_fSpeedPerSec;

	vPosition -= XMVector3Normalize(vLook) * fSpeed * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Right(_float fTimeDelta, _float fSpeed)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vRight = Get_State(STATE_RIGHT);

	if (fSpeed <= 0.f)
		fSpeed = m_fSpeedPerSec;

	vPosition += XMVector3Normalize(vRight) * fSpeed * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Left(_float fTimeDelta, _float fSpeed)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vRight = Get_State(STATE_RIGHT);

	if (fSpeed <= 0.f)
		fSpeed = m_fSpeedPerSec;

	vPosition -= XMVector3Normalize(vRight) * fSpeed * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

// 서서히 돌아감
void CTransform::Turn(const _vector& vAxis, _float fTimeDelta, _float fRotationPerSec)
{
	_vector		vRight = Get_State(STATE_RIGHT);
	_vector		vUp = Get_State(STATE_UP);
	_vector		vLook = Get_State(STATE_LOOK);

	_matrix		RotationMatrix{};

	if(fRotationPerSec == 0)
		RotationMatrix = XMMatrixRotationAxis(vAxis, m_fRotationPerSec * fTimeDelta);
	else
		RotationMatrix = XMMatrixRotationAxis(vAxis, fRotationPerSec * fTimeDelta);

	Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::Turn(_bool isRotationX, _bool isRotationY, _bool isRotationZ, _float fTimeDelta, _float fRotationPerSec)
{
	_float fRotation = fRotationPerSec;
	if (fRotation == 0.f)
		fRotation = m_fRotationPerSec;

	_vector		vRotation = XMQuaternionRotationRollPitchYaw(fRotation * fTimeDelta * isRotationX,
		fRotation * fTimeDelta * isRotationY,
		fRotation * fTimeDelta * isRotationZ);

	_vector		vRight = Get_State(STATE_RIGHT);
	_vector		vUp = Get_State(STATE_UP);
	_vector		vLook = Get_State(STATE_LOOK);

	_matrix		RotationMatrix = XMMatrixRotationQuaternion(vRotation);

	Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::Turn_Lerp(_fvector vLook, _float fTurnSpeed, _float fTimeDelta)
{
	if (XMVectorGetY(vLook) == -1.f)
		return;

	_vector vCurrentLook = Get_State(STATE::STATE_LOOK);
	vCurrentLook = XMVector3Normalize(vCurrentLook);

	if (XMVectorGetX(XMVector3Length(vCurrentLook - vLook)) < 0.1f)
		return;

	_vector vCurrentUp = Get_State(STATE::STATE_UP);
	vCurrentUp = XMVector3Normalize(vCurrentUp);

	_vector vTargetLook = vLook;
	vTargetLook = XMVector3Normalize(vTargetLook);

	// 새로운 right 벡터 구함
	_vector vTargetRight = XMVector3Cross(vCurrentUp, vTargetLook);

	// 새로운 right 벡터로 새로운 look 벡터를 구함
	vTargetLook = XMVector3Cross(vTargetRight, vCurrentUp);
	vTargetLook = XMVector3Normalize(vTargetLook);

	_vector vRadian = XMVector3Dot(vTargetLook, vCurrentLook);
	_float fRadian = {};

	XMStoreFloat(&fRadian, vRadian);
	fRadian = acosf(fRadian);;

	if (fRadian > 1.0f)
	{
		fRadian = 1.0f;
	}
	else if (fRadian < -1.0f)
	{
		fRadian = -1.0f;
	}

	if (fRadian <= fTurnSpeed * fTimeDelta)
	{
		vCurrentLook = vTargetLook;

		_vector vCurrentRight = XMVector3Cross(vCurrentUp, vCurrentLook);
		vCurrentLook = XMVector3Cross(vCurrentRight, vCurrentUp);
		vCurrentUp = XMVector3Cross(vCurrentLook, vCurrentRight);

		_float3 vScale = Get_Scaled();

		Set_State(STATE_RIGHT, XMVector3Normalize(vCurrentRight) * vScale.x);
		Set_State(STATE_UP, XMVector3Normalize(vCurrentUp) * vScale.y);
		Set_State(STATE_LOOK, XMVector3Normalize(vCurrentLook) * vScale.z);

		return;
	}

	_vector vCrossUp = XMVector3Cross(vCurrentLook, vTargetLook);

	_float fDot;
	XMStoreFloat(&fDot, XMVector3Dot(vCurrentUp, vCrossUp));

	if (fDot < 0)
		fTurnSpeed *= -1;

	// 쿼터니언 생성
	XMVECTOR vRotQuaternion = XMQuaternionRotationAxis(vCurrentUp, fTurnSpeed * fTimeDelta);

	// 회전 행렬 생성
	XMMATRIX RotationMatrix = XMMatrixRotationQuaternion(vRotQuaternion);

	XMMATRIX WorldMatirx = XMLoadFloat4x4(&m_WorldMatrix) * RotationMatrix;
	_float4x4 World4x4;
	XMStoreFloat4x4(&World4x4, WorldMatirx);

	// 상태 업데이트
	Set_State(STATE_RIGHT, XMLoadFloat4((_float4*)&World4x4.m[0]));
	Set_State(STATE_UP, XMLoadFloat4((_float4*)&World4x4.m[1]));
	Set_State(STATE_LOOK, XMLoadFloat4((_float4*)&World4x4.m[2]));
}

// 원하는 회전으로 세팅
void CTransform::Rotation(const _vector& vAxis, _float fRadian)
{
	_float3		vScaled = Get_Scaled();

	_vector		vRight = Get_State(STATE_RIGHT);
	_vector		vUp = Get_State(STATE_UP);
	_vector		vLook = Get_State(STATE_LOOK);

	_matrix		RotationMatrix{};

	RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);

	Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::Rotation(_float fX, _float fY, _float fZ)
{
	_vector		vRotation = XMQuaternionRotationRollPitchYaw(fX, fY, fZ);

	_float3		vScaled = Get_Scaled();

	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScaled.x;
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScaled.y;
	_vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScaled.z;

	_matrix		RotationMatrix = XMMatrixRotationQuaternion(vRotation);

	Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::LookAt(_fvector vAt)
{
	_float3		vScale = Get_Scaled();

	_vector		vPosition = Get_State(STATE_POSITION);

	_vector		vLook = vAt - vPosition;

	// Up 벡터와 Look 벡터를 외적하여 Right 벡터를 구함
	_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);

	// Look 벡터와 Right 벡터를 외적하여 진짜 Up 벡터를 구함
	_vector		vUp = XMVector3Cross(vLook, vRight);

	// 스케일을 맞춰줌
	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScale.x);
	Set_State(STATE_UP, XMVector3Normalize(vUp) * vScale.y);
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScale.z);
}

void CTransform::LookAt_Forward(_fvector vAt)
{
	_float3		vScale = Get_Scaled();

	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	_vector		vLook = vAt - vPosition;

	// Up 벡터와 Look 벡터를 외적하여 Right 벡터를 구함
	_vector		vRight = XMVector3Cross(vUp, vLook);

	vLook = XMVector3Cross(vRight, vUp);

	// 스케일을 맞춰줌
	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScale.x);
	Set_State(STATE_UP, XMVector3Normalize(vUp) * vScale.y);
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScale.z);
}

void CTransform::BillBoard()
{
	_float3		vScale = Get_Scaled();

	_vector		vPosition = Get_State(STATE_POSITION);

	CGameObject* pCamera = m_pGameInstance->Find_Camera(m_pGameInstance->Get_CurLevelIndex());
	_vector		vLook = vPosition - pCamera->Get_Position();

	// Up 벡터와 Look 벡터를 외적하여 Right 벡터를 구함
	_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);

	// Look 벡터와 Right 벡터를 외적하여 진짜 Up 벡터를 구함
	_vector		vUp = XMVector3Cross(vLook, vRight);

	// 스케일을 맞춰줌
	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScale.x);
	Set_State(STATE_UP, XMVector3Normalize(vUp) * vScale.y);
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScale.z);
}

void CTransform::BillBoardXZ()
{
	_float3		vScale = Get_Scaled();

	_vector		vPosition = XMVectorSetY(Get_State(STATE_POSITION), 0.f);

	CGameObject* pCamera = m_pGameInstance->Find_Camera(m_pGameInstance->Get_CurLevelIndex());
	_vector		vLook = vPosition - XMVectorSetY(pCamera->Get_Position(),0.f);

	// Up 벡터와 Look 벡터를 외적하여 Right 벡터를 구함
	_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);

	// Look 벡터와 Right 벡터를 외적하여 진짜 Up 벡터를 구함
	_vector		vUp = XMVector3Cross(vLook, vRight);

	// 스케일을 맞춰줌
	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScale.x);
	Set_State(STATE_UP, XMVector3Normalize(vUp) * vScale.y);
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScale.z);
}

void CTransform::Set_NewUp(_vector vNewUp)
{
	// Up 벡터를 기반으로 월드 행렬 재 설정
	_vector vLook = Get_State(CTransform::STATE_LOOK);
	vLook = XMVector3Normalize(vLook);

	_vector vRight = XMVector3Cross(vNewUp, vLook);

	vLook = XMVector3Cross(vRight, vNewUp);

	_float3 vScale = Get_Scaled();

	Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(vRight) * vScale.x);
	Set_State(CTransform::STATE_UP, XMVector3Normalize(vNewUp) * vScale.y);
	Set_State(CTransform::STATE_LOOK, XMVector3Normalize(vLook) * vScale.z);
}

void CTransform::Set_NewLook(_vector vNewLook)
{
	vNewLook = XMVector3Normalize(vNewLook);

	_vector vUp = Get_State(CTransform::STATE_UP);
	vUp = XMVector3Normalize(vUp);

	_vector vRight = XMVector3Cross(vUp, vNewLook);

	vUp = XMVector3Cross(vNewLook, vRight);

	_float3 vScale = Get_Scaled();

	Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(vRight) * vScale.x);
	Set_State(CTransform::STATE_UP, XMVector3Normalize(vUp) * vScale.y);
	Set_State(CTransform::STATE_LOOK, XMVector3Normalize(vNewLook) * vScale.z);
}

HRESULT CTransform::Bind_ShaderResource(CShader* pShader, const _char* pConstantName)
{
	return pShader->Bind_Matrix(pConstantName, &m_WorldMatrix);

}

CTransform* CTransform::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CTransform* pInstance = new CTransform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CTransform"));
		Safe_Release(pInstance);
	}


	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CTransform"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTransform::Free()
{
	__super::Free();
}
