#pragma once

#include "Client_Defines.h"
#include "Skill.h"

#include "Effect_TextureScale.h"

BEGIN(Engine)
class CShader;
class CModel;
class CRigidBody;
class CCollider;
END

BEGIN(Client)

class CSkill_Roar : public CSkill
{

protected:
	CSkill_Roar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkill_Roar(const CSkill_Roar& Prototype);
	virtual ~CSkill_Roar() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void OnCollisionEnter(CGameObject* pOther) override;
	virtual void OnCollisionStay(CGameObject* pOther)  override;
	virtual void OnCollisionExit(CGameObject* pOther)  override;

public:
	void		Appear();
	void		Disappear();

private:
	vector<CEffect_TextureScale*> m_EffectTextures;

	_uint					m_iAppearIndex = { 0 };

	_float4					m_vEffectColor = {};

	_bool					m_isDisappear = { false };

private:
	HRESULT Ready_Components();

public:
	static CSkill_Roar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END