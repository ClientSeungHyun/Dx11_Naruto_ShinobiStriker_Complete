#pragma once

#include "Client_Defines.h"
#include "Skill.h"

#include "Chidori_Center.h"
#include "ChidoriTrail.h"

BEGIN(Engine)
class CShader;
class CModel;
class CRigidBody;
class CCollider;
END

BEGIN(Client)

class CSkill_Chidori : public CSkill
{

protected:
	CSkill_Chidori(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkill_Chidori(const CSkill_Chidori& Prototype);
	virtual ~CSkill_Chidori() = default;

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

private:
	CPlayer*					m_pPlayer = { nullptr };
	_uint						m_iNumEffect = { 70 };

	_float						m_fExplosionTime = { 2.f };
	_float						m_fEffectTime = { 0.f };
	_float						m_fParticleTime = { 0.f };

	_bool						m_isLack = { false };
	_bool						m_isRun = { false };
	_bool						m_isExplosion = { false };
	_bool						m_isPlayLoopSound = { false };

	CChidori_Center*			m_pChidoriCenter = { nullptr };
	CChidoriTrail*				m_pChidoriTrail[3] = { nullptr };


private:
	HRESULT Ready_Components();

public:
	static CSkill_Chidori* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END