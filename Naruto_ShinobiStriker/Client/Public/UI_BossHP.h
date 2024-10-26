#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CUI_BossHP final : public CUIObject
{
public:
	enum ICON_TYPE { ICON_SUSANOO, ICON_KURAMA, ICON_END };

public:
	typedef struct : public CUIObject::UI_DESC
	{
		_float* pBossHP;
		class CMonster* pMonster;
	}HPUI_DESC;

private:
	CUI_BossHP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_BossHP(const CUI_BossHP& Prototype);
	virtual ~CUI_BossHP() = default;

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
	CTexture* m_pTexture_Icon = { nullptr };	//說除儀

private:
	class CMonster* m_pMonster = { nullptr };

	_float*			m_pBossHP = { nullptr };
	_float			m_fMaxHP = {};
	_float			m_fDamagedTime = { 0.f };

	_bool			m_isDamaged = { false };

	_float2			m_TextCoord = { 1.f,1.f };

	ICON_TYPE		m_eIconType = { ICON_END };

private:
	HRESULT Ready_Components();

public:
	static CUI_BossHP* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END