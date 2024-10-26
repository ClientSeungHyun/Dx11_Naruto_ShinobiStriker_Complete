#pragma once

#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Engine)
class CShader;
class CModel;
class CTexture;
class CCollider;
END

BEGIN(Client)

class CSkill_Susanoo final : public CGameObject
{
public:
	enum SKILLSUSANOOSTATE { SKILL_START, SKIL_LOOP, SKILL_END, SKILL_DISAPPEAR };
private:
	CSkill_Susanoo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkill_Susanoo(const CSkill_Susanoo& Prototype);
	virtual ~CSkill_Susanoo() = default;

public:
	void Set_Owner(CPlayer* pPlayer) { m_pOwner = pPlayer; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void OnCollisionEnter(CGameObject* pOther) override;
	virtual void OnCollisionStay(CGameObject* pOther)  override;
	virtual void OnCollisionExit(CGameObject* pOther)  override;

public:
	void Appear();
	void Change_Animation(SKILLSUSANOOSTATE eState);

private:
	class CModel*			m_pModelCom = { nullptr };
	class CShader*			m_pShaderCom = { nullptr };
	class CCollider*		m_pColliderCom = { nullptr };
	class CTexture*			m_pDissloveTexture = { nullptr };
	class CPlayer*			m_pOwner = { nullptr };

	_uint					m_iAnimationIndex[SKILL_DISAPPEAR] = {};

	_float					m_fDissloveRatio = {};
	_float					m_fEffectTime = { 0.f };

	_float4x4				m_ChidoriWorldMatrix = {};

	class CChidoriTrail*	m_pChidoriTrail[3] = { nullptr };

	SKILLSUSANOOSTATE		m_eState = { SKILL_DISAPPEAR };

private:
	HRESULT Ready_Components(void* pArg);

public:
	static CSkill_Susanoo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END