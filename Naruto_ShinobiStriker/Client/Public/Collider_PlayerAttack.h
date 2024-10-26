#pragma once
#include "Client_Defines.h"

#include "ColliderObject.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)

class CCollider_PlayerAttack final :
	public CColliderObject
{
private:
	CCollider_PlayerAttack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCollider_PlayerAttack() = default;

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
	class CSound*		m_pSoundCom = { nullptr };	// 목소리용
	class CPlayer*		m_pPlayer = { nullptr };

	_bool				m_isPlaySound = { false };

private:
	void Enter_BaseAttack(CMonster* pOther);
	void Collision_Boss(CMonster* pBoss);
	void Collision_Monster(CMonster* pMonster);

	void Appear_Distortion(CMonster* pMonster);

public:
	static CCollider_PlayerAttack* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual CGameObject* Clone(void* pArg) { return nullptr; }
	virtual void Free() override;
};

END