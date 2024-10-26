#pragma once

#include "Client_Defines.h"
#include "Skill.h"

#include "FireBall.h"
#include "FireBall_Effect.h"
#include "FireBall_FlySmoke.h"

BEGIN(Engine)
class CShader;
class CModel;
class CRigidBody;
class CCollider;
END

BEGIN(Client)

class CSkill_FireBall : public CSkill
{

protected:
	CSkill_FireBall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkill_FireBall(const CSkill_FireBall& Prototype);
	virtual ~CSkill_FireBall() = default;

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

	void		Explosion();

private:
	_float						m_fExplosionTime = { 2.f };
	_float						m_fTrailFireTime = { 0.f };

	_float3						m_vLook = {};

	_bool						m_isExplosion = { false };

	CFireBall*					m_pFireBall = { nullptr };
	CFireBall_Effect*			m_pFireBallEffect = { nullptr };
	CFireBall_FlySmoke*			m_pFireBallFlySmoke = { nullptr };
	class CParticle_FireBall*	m_pParticle_FireBAll = { nullptr };
private:
	HRESULT Ready_Components();

public:
	static CSkill_FireBall* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END