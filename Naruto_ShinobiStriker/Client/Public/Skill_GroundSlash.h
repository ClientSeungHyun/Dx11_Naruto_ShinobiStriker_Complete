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

class CSkill_GroundSlash : public CSkill
{

protected:
	CSkill_GroundSlash(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkill_GroundSlash(const CSkill_GroundSlash& Prototype);
	virtual ~CSkill_GroundSlash() = default;

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
	class CModel* m_pModelCom = { nullptr };
	class CShader* m_pShaderCom = { nullptr };
	class CTexture* m_pTextureCom = { nullptr };
	class CTexture* m_pMaskTextureCom = { nullptr };

	_uint					m_iAppearIndex = { 0 };

	_float4					m_vEffectColor = {};
	_float2					m_vTexCoord = {};

	_float					m_fCurrentScale = {};

	_bool					m_isDisappear = { false };

private:
	HRESULT Ready_Components();

public:
	static CSkill_GroundSlash* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END