#pragma once

#include "Client_Defines.h"
#include "Skill.h"

#include "RasenShuriken_Sphere.h"
#include "RasenShuriken_Blade.h"
#include "RasenShuriken_BladeFly.h"
#include "RasenShuriken_BladeLine.h"
#include "RasenShuriken_Circle.h"
#include "ExplosionSphere.h"

BEGIN(Engine)
class CShader;
class CModel;
class CRigidBody;
class CCollider;
END

BEGIN(Client)

class CSkill_RasenShuriken : public CSkill
{

protected:
	CSkill_RasenShuriken(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkill_RasenShuriken(const CSkill_RasenShuriken& Prototype);
	virtual ~CSkill_RasenShuriken() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg = nullptr ) override;
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

	void		Shoot();
	void		Explosion();

protected:
	CPlayer* m_pPlayer = { nullptr };

	_float						m_fFlyTime = { 1.f };
	_float						m_fExplosionTime = { 3.5f };
	_float						m_fExplosionRingTime = { 0.f };

	_float						m_fSacle = { 0.1f };
	_float						m_fExplosionTargetScale = { 9.f };

	_float3						m_vMoveDir = {};

	_bool						m_isShoot = { false };
	_bool						m_isExplosion = { false };

	CRasenShuriken_Sphere*		m_pRasenShuriken_Sphere = { nullptr };
	CRasenShuriken_Blade*		m_pRasenShuriken_Blade = { nullptr };
	CRasenShuriken_BladeFly*	m_pRasenShuriken_BladeFly = { nullptr };
	CRasenShuriken_BladeLine*	m_pRasenShuriken_BladeLine = { nullptr };
	CRasenShuriken_Circle*		m_pRasenShuriken_Circle = { nullptr };
	CExplosionSphere*			m_pExplosionSphere = { nullptr };

protected:
	HRESULT Ready_Components();

public:
	static CSkill_RasenShuriken* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END