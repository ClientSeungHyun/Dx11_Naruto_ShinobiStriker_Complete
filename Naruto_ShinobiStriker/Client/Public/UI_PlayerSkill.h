#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CUI_PlayerSkill final : public CUIObject
{
public:
	enum SKILL_ICON {
		SKILL_WOOD, SKILL_RASENRAN, SKILL_RASENSHURIKEN, SKILL_CHIDORI, SKILL_FIREBALL,
		SKILL_AMATERAS, SKILL_TSUKUYOMI, SKILL_TYPEZERO, SKILL_RAJIN2, SKILL_END
	};

	enum SP_ICON {
		SP_MASSIVE, SP_KIRIN, SP_SUSANOO, SP_RASENKO, SP_END };

public:
	typedef struct : public CUIObject::UI_DESC
	{
		_bool isBaseSkill;
	}SKILL_DESC;

private:
	CUI_PlayerSkill(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_PlayerSkill(const CUI_PlayerSkill& Prototype);
	virtual ~CUI_PlayerSkill() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void Appear(_float fTimeDelta) override;
	virtual void Disappear(_float fTimeDelta) override;

	virtual void Start_Appear() override;
	virtual void Start_Disappear() override;

	HRESULT Change_Skill(_uint iIconNum, _float* pSkillCollTime);

private:
	class CShader* m_pShaderCom = { nullptr };
	class CTexture* m_pTextureCom = { nullptr };
	class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	class CPlayer* m_pPlayer = { nullptr };

	CTexture* m_pTexture_SkillIcon = { nullptr };
	CTexture* m_pTexture_Shadow = { nullptr };

private:
	_uint			m_iSkillIcon = { SKILL_END };

	_float				m_fTime = { 0.f };
	_float*				m_pCoolTime = { nullptr };
	_float2				m_TextCoord = { 1.f,1.f };

	_bool				m_isBaseSkill = { false };

private:
	HRESULT Ready_Components();

public:
	static CUI_PlayerSkill* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END