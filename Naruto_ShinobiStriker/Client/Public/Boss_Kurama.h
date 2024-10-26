#pragma once

#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Engine)
class CShader;
class CModel;
class CFsm;
END

BEGIN(Client)

class CBoss_Kurama final : public CMonster
{
public:
	enum TENTAILSTATE
	{
		ROAR = MONSTER_END,
		HIT,
		CHANCE,
		JUMP,
		LEAVE,
		HEAL,
		FALL,
		LAND,

		ATTACK1,
		ATTACK2,
		JUMPCRUSH,
		BITE,

		BLAST,
		AERIAL_BLAST,
		THREE_BLAST,
		SPECIALATTACK,

		STATE_END
	};

private:
	CBoss_Kurama(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoss_Kurama(const CBoss_Kurama& Prototype);
	virtual ~CBoss_Kurama() = default;

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
	virtual void	Damaged_To_Player(_uint iState, _float fDamage = 10.f, _bool isStrongAttack = false) override;

	void			Start_Roar();
	void			End_Roar();

	void			Shoot_FoxSphere(_int iBoneIndex = -1);
	void			Shoot_FoxBlast(_fvector vTargetPos);

	void			Create_Rasengan();
	void			Shoot_Rasengan();

	void			Appear_TargetUI();

private:
	_bool							m_isChance = { false };

	class CSkill_Roar*				m_pSkill_Roar = { nullptr };
	class CSkill_KuramaRasengan*	m_pSkill_KuramaRasengan = { nullptr };

	class CUI_BossHP* m_pUI_HP = { nullptr };
	class CUI_BossTarget* m_pUI_Icon = { nullptr };

private:
	HRESULT Ready_Components(void* pArg);
	HRESULT Ready_State();
	HRESULT Ready_Skill();
	HRESULT Ready_UI();

public:
	static CBoss_Kurama* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END