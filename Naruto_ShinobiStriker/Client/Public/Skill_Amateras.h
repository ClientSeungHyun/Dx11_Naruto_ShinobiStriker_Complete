#pragma once

#include "Client_Defines.h"
#include "Skill.h"

BEGIN(Engine)
class CShader;
class CModel;
class CRigidBody;
class CCollider;
END

BEGIN(Client)

class CSkill_Amateras : public CSkill
{

protected:
	CSkill_Amateras(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkill_Amateras(const CSkill_Amateras& Prototype);
	virtual ~CSkill_Amateras() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void		Appear(class CGameObject* pTargetMonster = nullptr, _float fScale = 3.f);
	void		DisAppear();

protected:
	class CGameObject*				m_pTargetMonster = { nullptr };
	_uint						m_iNumEffect = { 70 };

	_float						m_fScale = {};
	_float						m_fExplosionTime = { 2.f };
	_float						m_fFireTime = { 0.f };
	_float						m_fAmaterasTime = { 0.f };
	_float						m_fDamagedTime = { 0.f };

	_bool						m_isFirst = { false };
	_bool						m_isBoss = { false };

private:
	HRESULT Ready_Components();

	void Create_AmaterasEffect(_uint iNum);
	void Create_Fire05(_uint iNum = 1);

public:
	static CSkill_Amateras* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END