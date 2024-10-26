#include "RigidBody.h"

#include "GameObject.h"
#include "Transform.h"

CRigidBody::CRigidBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice,  pContext }
{
	m_eComponentType = RIGIDBODY;
}

CRigidBody::CRigidBody(const CRigidBody& Prototype)
	: CComponent{ Prototype }
{
	m_eComponentType = RIGIDBODY;
}

void CRigidBody::Set_GravityDir(_vector vDir)
{
	XMStoreFloat3(&m_vGraivtyDirection, vDir);
}

HRESULT CRigidBody::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CRigidBody::Initialize(void* pArg)
{
	m_isActive =  true;
	m_isGravity =  true;
	m_isGround =  false;
	m_isFriction =  true;
	m_isKinematic = false;
	m_fGravityLimit = 5.f;

	return S_OK;
}

void CRigidBody::Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	_vector vForce = XMLoadFloat3(&m_vForce);
	_vector vVelocity = XMLoadFloat3(&m_vVelocity);
	_vector vAccel = XMLoadFloat3(&m_vAccel);

	//�������� ���� ���� ���� ����
	//���� ���� �ӵ��� ����
	if (XMVector3Length(vForce).m128_f32[0] != 0.f)
	{
		//���ӵ��� �̿��ؼ� �ӵ��� ������Ų��.
		vVelocity += vAccel;
		XMStoreFloat3(&m_vVelocity, vVelocity);
	}

	Calculate_Tranform(fTimeDelta);

	//�� �ʱ�ȭ
	m_vForce = { 0.f, 0.f, 0.f };
}

void CRigidBody::Calculate_Tranform(_float fTimeDelta)
{
	Calculate_Friction(fTimeDelta);

	//�߷� ���� && !m_isGround
	if (m_isGravity)
	{
		Calculate_Gravity(fTimeDelta);
	}

	_vector vCurrentPosition = m_pOwner->Get_Transform()->Get_State(CTransform::STATE_POSITION);

	//�ӵ� ����
	if (fabsf(m_vMaxVelocity.x) < fabsf(m_vVelocity.x))
	{
		m_vVelocity.x = (m_vVelocity.x / fabsf(m_vVelocity.x)) * fabsf(m_vMaxVelocity.x);
	}

	// �������� y�� ����
	if (m_vVelocity.y <= -m_vMaxVelocity.y)
	{
		m_vVelocity.y = (m_vVelocity.y / fabsf(m_vVelocity.y)) * fabsf(m_vMaxVelocity.y);
	}

	_vector vNewPosition = vCurrentPosition + (XMLoadFloat3(&m_vVelocity) * fTimeDelta);

	m_pOwner->Get_Transform()->Set_State(CTransform::STATE_POSITION, vNewPosition);
}

void CRigidBody::Calculate_Gravity(_float fTimeDelta)
{
	_vector vGravity = XMLoadFloat3(&m_vGraivtyDirection) * m_fGravityScale * fTimeDelta;

	if (fabsf(XMVectorGetY(vGravity)) > m_fGravityLimit)
	{
		vGravity = XMVectorSetY(vGravity, m_fGravityLimit);
	}

	XMStoreFloat3(&m_vVelocity, XMLoadFloat3(&m_vVelocity) - vGravity);
}

void CRigidBody::Calculate_Friction(_float fTimeDelta)
{
	if (!m_isFriction)
		return;

	XMVECTOR vVelocity = XMLoadFloat3(&m_vVelocity);

	if (XMVector3Equal(vVelocity, XMVectorZero()))
	{
		return; // �ӵ��� 0�̸� ������ ������ �ʿ䰡 ����
	}

	// ������ ������ ����
	_vector vFricitionDir = XMVector3Normalize(vVelocity) * -1.f;
	_vector vFriction = vFricitionDir * XMLoadFloat3(&m_vFriction) * fTimeDelta;

	_float fVelocityLength = XMVectorGetX(XMVector3Length(vVelocity));
	_float fFrictionLength = XMVectorGetX(XMVector3Length(vFriction));

	//������ �� ū ���
	if (fFrictionLength >= fVelocityLength)
	{
		m_vVelocity = { 0.f, 0.f, 0.f };
	}
	else
	{
		XMStoreFloat3(&m_vVelocity, XMLoadFloat3(&m_vVelocity) + vFriction);
	}
}

void CRigidBody::Add_Force(FORCEMODE eForceMode, _vector vForce)
{
	XMStoreFloat3(&m_vForce, vForce);

	_vector		vAccel = XMLoadFloat3(&m_vAccel);
	_vector		vVelocity = XMLoadFloat3(&m_vVelocity);

	switch (eForceMode)
	{
		//������ �߰����� ����(���Ը� ����)
	case Engine::CRigidBody::FORCE:
		//���ӵ��� = �� / ����
		vAccel += vForce / m_fMass;
		XMStoreFloat3(&m_vAccel, vAccel);
		break;

		//������ �߰����� ����(���Ը� ���� �� ��)
	case Engine::CRigidBody::ACCELERATION:
		vAccel += vForce;
		XMStoreFloat3(&m_vAccel, vAccel);
		break;

		//���������� �о��� ����(���Ը� ����)
	case Engine::CRigidBody::IMPLUSE:
		vVelocity += vForce / m_fMass;
		XMStoreFloat3(&m_vVelocity, vVelocity);
		break;

		//���������� �о��� ����(���Ը� ���� �� ��)
	case Engine::CRigidBody::VELOCITYCHANGE:
		vVelocity += vForce;
		XMStoreFloat3(&m_vVelocity, vVelocity);
		break;

	case Engine::CRigidBody::MODE_END:
		break;
	default:
		break;
	}
}

void CRigidBody::Add_Force_Direction(_fvector vDirection, _fvector vMagnitude, FORCEMODE eForceMode)
{
	_vector vDir = XMVector3Normalize(vDirection);

	_float vForceX = XMVectorGetX(vDir) * XMVectorGetX(vMagnitude);    // ����� ũ�⸦ ���Ͽ� �� ���� ����
	_float vForceY = XMVectorGetY(vDir) * XMVectorGetY(vMagnitude);
	_float vForceZ = XMVectorGetZ(vDir) * XMVectorGetZ(vMagnitude);

	_vector vForce = XMVectorSet(vForceX, vForceY, vForceZ, 0.f);

	Add_Force(eForceMode, vForce); // ���� Add_Force �Լ� �̿�
}

CRigidBody* CRigidBody::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRigidBody* pInstance = new CRigidBody(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CRigidBody"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* ������ü�� ã�Ƽ� ������ü�� Clone�Լ��� ȣ���Ѵ�.  */
CComponent* CRigidBody::Clone(void* pArg)
{
	CRigidBody* pInstance = new CRigidBody(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CRigidBody"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRigidBody::Free()
{
	__super::Free();
}
