#pragma once

#include "Client_Defines.h"
#include "Skill.h"

#include "Wood.h"
#include "MassiveRasengan_Sphere.h"

BEGIN(Engine)
class CShader;
class CModel;
class CRigidBody;
class CCollider;
END

BEGIN(Client)

class CSkill_ChangeWood : public CSkill
{

protected:
	CSkill_ChangeWood(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkill_ChangeWood(const CSkill_ChangeWood& Prototype);
	virtual ~CSkill_ChangeWood() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void		Appear();

private:
	CWood*						m_pWood = { nullptr };

private:
	HRESULT Ready_Components();

public:
	static CSkill_ChangeWood* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END