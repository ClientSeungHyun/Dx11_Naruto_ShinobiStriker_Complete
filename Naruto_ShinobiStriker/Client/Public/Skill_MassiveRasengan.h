#pragma once

#include "Client_Defines.h"
#include "Skill.h"

#include "MassiveRasengan_Circle.h"
#include "MassiveRasengan_Sphere.h"

BEGIN(Engine)
class CShader;
class CModel;
class CRigidBody;
class CCollider;
END

BEGIN(Client)

class CSkill_MassiveRasengan : public CSkill
{

protected:
	CSkill_MassiveRasengan(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkill_MassiveRasengan(const CSkill_MassiveRasengan& Prototype);
	virtual ~CSkill_MassiveRasengan() = default;

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

	void		Shoot();
	void		Explosion();

protected:
	class CPlayer* m_pPlayer = { nullptr };

	_float						m_fFlyTime = { 3.f };
	_float						m_fExplosionTime = { 2.5f };
	_float						m_fRingTime = {};

	_float3						m_vMoveDir = {};

	_bool						m_isShoot = { false };
	_bool						m_isExplosion = { false };

	CMassiveRasengan_Circle*	m_pMassiveRasengan_Circle = { nullptr };
	CMassiveRasengan_Sphere*	m_pMassiveRasengan_Spher = { nullptr };

protected:
	HRESULT Ready_Components();

public:
	static CSkill_MassiveRasengan* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END