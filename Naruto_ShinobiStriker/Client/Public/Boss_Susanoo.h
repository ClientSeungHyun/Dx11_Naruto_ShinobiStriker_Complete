#pragma once

#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Engine)
class CShader;
class CModel;
class CFsm;
class CTexture;
END

BEGIN(Client)

class CBoss_Susanoo final : public CMonster
{
public:
	enum SUSANOOSTATE
	{
		ROAR = MONSTER_END, 
		HIT,
		CHANCE,

		DOUBLESLASH,
		GROUNDPUNCH,
		RISING,
		GROUNDSLASH,

		STATE_END
	};

private:
	CBoss_Susanoo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoss_Susanoo(const CBoss_Susanoo& Prototype);
	virtual ~CBoss_Susanoo() = default;

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
	void Contorl_RootBone();

	HRESULT Summon_Blade();

public:
	void Appear_GroundSlash();
	void Appear_BladeSlsh();
	void Appear_GroundBreak(_float3 vSize = _float3(1.f, 1.f, 1.f));

	void Appear_TargetUI();

private:
	class CTexture*				m_pDissloveTexture = { nullptr };

	_bool						m_isChance = { false };

	_uint						m_iRootBoneIndeox = { false };

	_float						m_fDissloveRatio = { 1.f };

	_float3						m_vPrevRootBonePos = {};

	class CSusanooBlade*		m_pBlade = { nullptr };

	class CSkill_GroundSlash*	m_pSkill_GroundSlash = { nullptr };

	class CUI_BossHP*			m_pUI_HP = { nullptr };
	class CUI_BossTarget*		m_pUI_Icon = { nullptr };

private:
	HRESULT Ready_Components(void* pArg);
	HRESULT Ready_State();
	HRESULT Ready_Skill();
	HRESULT Ready_UI();

public:
	static CBoss_Susanoo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END