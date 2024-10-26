#pragma once
#include "Client_Defines.h"

#include "ColliderObject.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)

class CCollider_MonsterAttack final :
	public CColliderObject
{
private:
	CCollider_MonsterAttack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCollider_MonsterAttack() = default;

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

private:
	MONSTER_ID		m_eMonsterID = { MON_END };

public:
	static CCollider_MonsterAttack* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual CGameObject* Clone(void* pArg) { return nullptr; }
	virtual void Free() override;
};

END