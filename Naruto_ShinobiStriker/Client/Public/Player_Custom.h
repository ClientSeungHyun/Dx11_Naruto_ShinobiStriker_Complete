#pragma once

#include "Client_Defines.h"
#include "Player.h"

#include "Shadow_Custom.h"

#include "Skill_RasenShuriken.h"

BEGIN(Engine)
class CShader;
class CModel;
class CFsm;
class CNavigation;
END

BEGIN(Client)

class CPlayer_Custom final : public CPlayer
{
public:
	enum SHADOW { SHA_LEFT, SHA_RIGHT, SHA_END };
	enum PARTID { PART_BODYONCE, PART_BODYUPPER, PART_BODYLOWER, PART_FACE, PART_HEAD, PART_ACCESSORIE, PART_END };
	enum CUSTOMSTATE
	{
		// 사스케 스킬
		CHIDORI_CHARGE = PLAYERSTATE_END, CHIDORI_RUN, CHIDORI_ATTACK,

		AERIAL_CHIDORI_CHARGE, AERIAL_CHIDORI_RUN, AERIAL_CHIDORI_ATTACK,

		FIREBALL, AERIAL_FIREBALL,

		KIRIN,

		// 나루토 스킬
		RASENGUN_CHARGE, RASENGUN_RUN, RASENGUN_ATTACK,

		AERIAL_RASENGUN_CHARGE, AERIAL_RASENGUN_RUN, AERIAL_RASENGUN_ATTACK,

		RASENSHURIKEN, AERIAL_RASENSHURIKEN,

		MASSIVERASENGAN,

		// 사륜안 스킬
		AMATERAS, TSUKUYOMI, SUSANOO,

		// 미나토 스킬
		TYPEZERO, RAIJIN, RASENKO,

		STATE_END
	};
public:
	typedef struct {
		_uint iUpperID;
		_uint iLowerID;
		_uint iFaceID;
		_uint iHeadID;
		_uint iAcceID;
		_uint iOnceID;
		_uint iEyeID;
		_bool isOnceClothes;
	}CUSTOM_INFO;

	typedef struct : public PLAYER_DESC{
		CUSTOM_INFO tCustomInfo = {};
	}CUSTOM_DESC;

private:
	CPlayer_Custom(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer_Custom(const CPlayer_Custom& Prototype);
	virtual ~CPlayer_Custom() = default;

public:
	CUSTOM_INFO Get_CustomInfo() { return m_tCustomInfo; }

public:
	virtual HRESULT Initialize_Prototype() override;
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
	virtual void	Change_IsRender(_bool isRender) override;
	virtual void On_MotionTrail() override;
	virtual void Off_Motiontrail() override;

	void	Create_Shadow(CShadow_Custom::SHADOW_DESC tDesc);

	void Change_Face(_uint iID);
	void Change_Upper(_uint iID);
	void Change_Lower(_uint iID);
	void Change_Once(_uint iID);
	void Change_Head(_uint iID);
	void Change_Eye(_uint iID);

	_float Get_SkillCollTime1();
	_float Get_SkillCollTime2();
	_float Get_SkillCollTimeSP();

	void Clear_SkillCollTime1();
	void Clear_SkillCollTime2();
	void Clear_SkillCollTimeSP();

	void	Create_Chidori();
	void	Run_Chidori();
	void	Explosion_Chidori();

	void	Create_FireBall();

	void	Create_Kirin(_fvector vKirinPos);

	void	Create_Amateras();
	void	Create_Tsukuyomi();

	void	Create_Rasengan();
	void	Run_Rasengan();
	void	Explosion_Rasengan();
	void	Disappear_Rasengan();

	void	Create_RasenShuriken();
	void	Shoot_RasenShuriken();

	void	Create_MassiveRasengan();
	void	Shoot_MassiveRasengan();

	void	Create_EffectTypeZero(_uint iEffectNum, _float4x4 vWorldMatrix);

	void	Create_RasnkoStar();
	void	Disappear_RasnkoStar();

	class CSkill_Susanoo* Get_Susanoo() { return m_pSkill_Susanoo; }

private:
	class CNavigation*	m_pNavigationCom = { nullptr };

	CUSTOM_INFO			m_tCustomInfo = {};

	POINT				m_ptOldMousePos = {};

private:
	class CSkill_Chidori*		m_pSkill_Chidori = { nullptr };
	class CSkill_FireBall*		m_pSkill_FireBall = { nullptr };
	class CSkill_Kirin*			m_pSkill_Kirin = { nullptr };

	class CSkill_Rasengan*		m_pSkill_Rasengan = { nullptr };
	class CSkill_RasenShuriken* m_pSkill_RasenShuriken = { nullptr };
	class CSkill_MassiveRasengan* m_pSkill_MassiveRasengan = { nullptr };

	class CSkill_Amateras*		m_pSkill_Amateras = { nullptr };
	class CSkill_Tsukuyomi*		m_pSkill_Tsukuyomi = { nullptr };
	class CSkill_Susanoo*		m_pSkill_Susanoo = { nullptr };

	class CEffect_TypeZero* m_pEffectTypeZero[10] = { nullptr };
	class CEffect_RasenkoStar* m_pEffectRasenkoStar = { nullptr };

	class CShadow_Custom*		m_pShadow[SHA_END] = { nullptr, nullptr };

private:
	void Parts_Priority_Update(_float fTimeDelta);
	void Parts_Update(_float fTimeDelta);
	void Parts_Late_Update(_float fTimeDelta);

	HRESULT Ready_Components();
	HRESULT Ready_State();
	HRESULT Ready_PartObjects();
	HRESULT Ready_Skill();
	HRESULT Ready_Shadow();

	HRESULT Save_CustomInfo();
	HRESULT Load_CustomInfo();

public:
	static CPlayer_Custom* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END