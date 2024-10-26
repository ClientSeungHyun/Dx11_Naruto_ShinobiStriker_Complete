#pragma once

#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Engine)
class CShader;
class CModel;
class CFsm;
END

BEGIN(Client)

class CWhiteZetsu final : public CMonster
{
public:
	enum WHITEZETSUSTATE
	{
		CHASE = MONSTER_END,
		FALL,
		GETUP,
		BACKDASH,

		GROGGY,
		HIT1,
		HIT2,
		HIT_FLYAWAY,
		HIT_FLYGROUND,
		HIT_DOWN,

		ATTACK1,
		ATTACK_FAR,

		STATE_END
	};

private:
	CWhiteZetsu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWhiteZetsu(const CWhiteZetsu& Prototype);
	virtual ~CWhiteZetsu() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void OnCollisionEnter(CGameObject* pOther) override;
	virtual void OnCollisionStay(CGameObject* pOther)  override;
	virtual void OnCollisionExit(CGameObject* pOther)  override;

public:
	virtual void		Damaged_To_Player(_uint iState, _float fDamage = 10.f, _bool isStrongAttack = false) override;
	virtual void		Change_State_GeneralHit() override;
	virtual void		Change_State_FlyAway() override;
	virtual void		Change_State_FlyGround() override;
	virtual void		Change_State_Groggy() override;
	virtual void		Change_State_HitDown() override;

private:
	HRESULT Ready_Components(void* pArg);
	HRESULT Ready_State();

public:
	static CWhiteZetsu* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END