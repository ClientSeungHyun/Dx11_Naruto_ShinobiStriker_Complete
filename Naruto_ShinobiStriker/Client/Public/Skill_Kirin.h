#pragma once

#include "Client_Defines.h"
#include "Skill.h"

#include "Kirin.h"
#include "Chidori_Center.h"

BEGIN(Engine)
class CShader;
class CModel;
class CRigidBody;
class CCollider;
END

BEGIN(Client)

class CSkill_Kirin : public CSkill
{

protected:
	CSkill_Kirin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkill_Kirin(const CSkill_Kirin& Prototype);
	virtual ~CSkill_Kirin() = default;

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
	void		Appear(_fvector vKirinPos);
	void		DisAppear();

	void		Explosion();

private:
	_float						m_fExplosionTime = { 2.f };
	_float						m_fEffectTime = { 0.f };

	_bool						m_isExplosion = { false };

	CKirin*						m_pKirin = { nullptr };
	CChidori_Center*			m_pChidoriCenter = { nullptr };

private:
	HRESULT Ready_Components();

public:
	static CSkill_Kirin* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END