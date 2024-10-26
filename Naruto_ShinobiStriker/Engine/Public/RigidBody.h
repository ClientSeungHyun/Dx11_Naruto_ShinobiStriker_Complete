#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CRigidBody final : public CComponent
{
public:
    enum FORCEMODE { FORCE, ACCELERATION, IMPLUSE, VELOCITYCHANGE, MODE_END };

private:
    CRigidBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CRigidBody(const CRigidBody& Prototype);
    virtual ~CRigidBody() = default;

public:
    class CGameObject*  GetOwner() { return m_pOwner; }
    void                SetOwner(class CGameObject* pOwner) { m_pOwner = pOwner; }

    _float              Get_Mass() { return m_fMass; }
    void                Set_Mass(_float fMass) { m_fMass = fMass; }

    _float3             Get_Force() { return m_vForce; }

    _float3             Get_Velocity() { return m_vVelocity; }
    void                Set_Velocity(_float3 vVelocity) { m_vVelocity = vVelocity; }

    void                AddVelocity(_vector vAddV) { XMStoreFloat3(&m_vVelocity, XMLoadFloat3(&m_vVelocity) + vAddV); }

    _bool               Get_IsGravity() { return m_isGravity; }
    void                Set_IsGravity(bool isGravity) {
        m_isGravity = isGravity;
        if (!m_isGravity && m_vVelocity.y < 0)
            m_vVelocity.y = 0.f;
    }

    _bool               Get_IsGround() { return m_isGround; }
    void                Set_IsGround(_bool isGround) { m_isGround = isGround; }

    _float              Get_GravityScale() { return m_fGravityScale; }
    void                Set_GravityScale(_float fGravityScale) { m_fGravityScale = fGravityScale; }

    _bool               Get_IsFriction() { return m_isFriction; }
    void                Set_IsFriction(_bool isFriction) { m_isFriction = isFriction; }

    void                Set_VelocityLimit(_float3 vMaxVelocity) { m_vMaxVelocity = vMaxVelocity; }

    void                Set_Friction(_float3 vFriction) { m_vFriction = vFriction; }

    void                Set_GravityLimit(_float fGravityLimit) { m_fGravityLimit = fGravityLimit; }

    void                Set_GravityDir(_vector vDir);

    void                Clear_Force() { m_vForce = _float3(0.f, 0.f, 0.f); }
    void                Clear_Accel() { m_vAccel = _float3(0.f, 0.f, 0.f); }
    void                Clear_Velocity() { m_vVelocity = _float3(0.f, 0.f, 0.f); }
    void                Clear_All() { Clear_Force(); Clear_Accel(); Clear_Velocity(); }

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg) override;
    void Update(_float fTimeDelta);

public:
    void Add_Force(FORCEMODE eForceMode, _fvector vForce);   // ���� �ִ� �Լ�
    void Add_Force_Direction(_fvector vDirection, _fvector vMagnitude, FORCEMODE eForceMode); // Ư�� �������� ���� �ִ� �Լ�



private:
    class CGameObject*      m_pOwner = { nullptr };

    _float                  m_fMass = { 1.f };             // ����
    _float                  m_fGravityScale = { 2.f };     // �߷� ũ��
    _float                  m_fGravityLimit = { 10.f };     // �߷� �ִ� ��

    _float3                 m_vFriction = { 1.f,1.f,1.f };         // ������
    _float3                 m_vForce = { 0.f,0.f,0.f };           // �������� ��
    _float3                 m_vAccel = { 0.f,0.f,0.f };           // ���ӵ�
    _float3                 m_vVelocity = { 0.f,0.f,0.f };        // �ӵ�
    _float3                 m_vMaxVelocity = { 20.f,15.f,20.f };     // �ְ� �ӵ�
    _float3                 m_vGraivtyDirection = { 0.f,1.f,0.f };

    _bool                   m_isActive = { true };  // Ȱ��ȭ ���־�?
    _bool                   m_isGravity = { true }; // �߷� �����Ұž�?
    _bool                   m_isGround = { false }; // ���� �پ��־�?
    _bool                   m_isFriction = { true };
    _bool                   m_isKinematic = { false }; // true�� ����ȿ���� �� �� �ž�

private:
    void Calculate_Tranform(_float fTimeDelta); // ���� ��ġ ���
    void Calculate_Gravity(_float fTimeDelta); // �߷� ���
    void Calculate_Friction(_float fTimeDelta); // ���� ���

public:
    static CRigidBody* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CComponent* Clone(void* pArg) override;
    virtual void Free() override;
};

END