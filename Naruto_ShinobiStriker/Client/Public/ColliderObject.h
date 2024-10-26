#pragma once
#include "Client_Defines.h"

#include "Monster.h"
#include "Collider.h"

#include "Bounding_OBB.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)

class CColliderObject abstract:
    public CGameObject
{
public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		class CGameObject* pOwner;
		CBounding_OBB::BOUNDING_OBB_DESC Desc_OBB;
	}OBB_COLLIDEROBJ_DESC;

protected:
	CColliderObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CColliderObject() = default;

public:
	class CCollider*	Get_ColliderCom() { return m_pColliderCom; }

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void OnCollisionEnter(CGameObject* pOther) override;
	virtual void OnCollisionStay(CGameObject* pOther) override;
	virtual void OnCollisionExit(CGameObject* pOther) override;

public:
	void Scaled(_float fX, _float fY, _float fZ);
	void Offset(_float fX, _float fY, _float fZ);

protected:
	class CCollider*		m_pColliderCom = { nullptr };
	CGameObject*		m_pOwner = { nullptr };

	_float3				m_vScale = {};
	_float3				m_vOffset = {};

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END