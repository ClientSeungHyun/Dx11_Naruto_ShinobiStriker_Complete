#pragma once

#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Engine)
class CShader;
class CModel;
class CFsm;
END

BEGIN(Client)

class CTenTail final : public CMonster
{
public:
	enum TENTAILSTATE
	{
		CHASE = MONSTER_END,
		HIT,

		ATTACK1,
		ATTACK_RUSH,

		STATE_END
	};

private:
	CTenTail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTenTail(const CTenTail& Prototype);
	virtual ~CTenTail() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	class CCollider* m_pColliderCom = { nullptr };

private:
	HRESULT Ready_Components(void* pArg);
	HRESULT Ready_State();

public:
	static CTenTail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END