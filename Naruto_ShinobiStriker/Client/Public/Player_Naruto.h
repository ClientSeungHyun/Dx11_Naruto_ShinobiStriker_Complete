#pragma once

#include "Client_Defines.h"
#include "Player.h"

#include "Shadow_Naruto.h"

BEGIN(Engine)
class CShader;
class CModel;
class CFsm;
END

BEGIN(Client)

class CPlayer_Naruto final : public CPlayer
{
	
public:
	enum SHADOW { SHA_LEFT, SHA_RIGHT, SHA_END };

	enum NARUTOSTATE
	{
		RASENGUN_CHARGE = PLAYERSTATE_END,
		RASENGUN_RUN,
		RASENGUN_ATTACK,

		AERIAL_RASENGUN_CHARGE,
		AERIAL_RASENGUN_RUN,
		AERIAL_RASENGUN_ATTACK,

		RASENSHURIKEN,
		AERIAL_RASENSHURIKEN,

		STATE_END
	};
private:
	CPlayer_Naruto(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer_Naruto(const CPlayer_Naruto& Prototype);
	virtual ~CPlayer_Naruto() = default;

public:
	_bool			Get_IsSenninEye() { return m_isSenninEye; }
	void			Set_IsSenninEye(_bool isSennin) { m_isSenninEye = isSennin; }

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
	void	Create_ShadowNaruto(CShadow_Naruto::SHADOW_NARUTO_DESC tDesc);

	void	Create_RasenShuriken();
	void	Shoot_RasenShuriken();
	_float	Get_RasenShurikenCoolTime();

	void	Create_Rasengan();
	void	Run_Rasengan();
	void	Explosion_Rasengan();
	_float	Get_RasenganCoolTime();

private:
	class CShadow_Naruto* m_pShadow[SHA_END] = { nullptr, nullptr };

	_uint					m_iSenninIndex = { 0 };
	_bool					m_isSenninEye = { false };


	class CSkill_Rasengan*		m_pSkill_Rasengan = { nullptr };
	_float						m_fCoolTime_Rasengan = 0.f;

	class CSkill_RasenShuriken*	m_pSkill_RasenShuriken = { nullptr };
	_float						m_fCoolTime_RasenShuriken = 0.f;


private:
	HRESULT Ready_Components();
	HRESULT Ready_State();
	HRESULT Ready_Shadow();
	HRESULT Ready_Skill();

public:
	static CPlayer_Naruto* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END