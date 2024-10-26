#pragma once

#include "Client_Defines.h"
#include "Skill.h"

#include "Rasengan_Center.h"
#include "Rasengan_CreateSmoke.h"
#include "Rasengan_RunSmoke.h"

BEGIN(Engine)
class CShader;
class CModel;
class CRigidBody;
class CCollider;
END

BEGIN(Client)

class CSkill_Rasengan : public CSkill
{

protected:
	CSkill_Rasengan(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkill_Rasengan(const CSkill_Rasengan& Prototype);
	virtual ~CSkill_Rasengan() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void OnCollisionEnter(CGameObject* pOther) override;
	virtual void OnCollisionStay(CGameObject* pOther) override;
	virtual void OnCollisionExit(CGameObject* pOther) override;

public:
	void		Appear();
	void		DisAppear();

	void		Run();
	void		Explosion();

protected:
	CPlayer* m_pPlayer = { nullptr };

	_float						m_fFlyTime = { 3.f };
	_float						m_fExplosionTime = { 2.5f };
	_float						m_fParticleTime = { 0.f };
	_float						m_fRingTime = { 0.f };

	_float3						m_vMoveDir = {};

	_bool						m_isRun = { false };
	_bool						m_isExplosion = { false };
	_bool						m_isLack = { false };

	CRasengan_Center*			m_pRasengan_Center = { nullptr };
	CRasengan_CreateSmoke*		m_pRasengan_CreateSmoke = { nullptr };
	CRasengan_RunSmoke*			m_pRasengan_RunSmoke = { nullptr };

protected:
	HRESULT Ready_Components();

public:
	static CSkill_Rasengan* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END