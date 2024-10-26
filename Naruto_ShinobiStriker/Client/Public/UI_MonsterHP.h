#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CUI_MonsterHP final : public CUIObject
{
public:
	typedef struct : public CUIObject::UI_DESC
	{
		_float* pMonsterHP;
		class CMonster* pMonster;
	}HPUI_DESC;

private:
	CUI_MonsterHP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_MonsterHP(const CUI_MonsterHP& Prototype);
	virtual ~CUI_MonsterHP() = default;

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

	void Damaged();

private:
	class CShader* m_pShaderCom = { nullptr };
	class CTexture* m_pTextureCom = { nullptr };	// 說除儀
	class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	CTexture* m_pTexture_Guage = { nullptr };		//蟾煙儀
	CTexture* m_pTexture_DamageEffect = { nullptr };	//說除儀

private:
	class CMonster* m_pMonster = { nullptr };

	_float*			m_pMonsterHP = { nullptr };
	_float			m_fMaxHP = {};
	_float			m_fDamagedTime = { 0.f };

	_bool			m_isDamaged = { false };

	_float2			m_TextCoord = { 1.f,1.f };

private:
	HRESULT Ready_Components();

public:
	static CUI_MonsterHP* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END