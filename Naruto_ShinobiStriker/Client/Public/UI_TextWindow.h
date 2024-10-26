#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CUI_TextWindow final : public CUIObject
{
public:
	enum TEXTWINDOW_TYPE { LEFT, CENTER, TYPE_END };

private:
	CUI_TextWindow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_TextWindow(const CUI_TextWindow& Prototype);
	virtual ~CUI_TextWindow() = default;

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

	void Change_Text(_wstring strText);
	void ReAppear(_float fTimeDelta);

	void Change_WindowType(TEXTWINDOW_TYPE eType);

private:
	class CShader* m_pShaderCom = { nullptr };
	class CTexture* m_pTextureCom = { nullptr };
	class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	CTexture* m_pTexture_Icon = { nullptr };

private:
	_uint	m_iIconNum = { 0 };

	_float  m_fInitX = { 230.f };
	_float	m_fTargetX = { 260.f };
	_float	m_fChnageTime = {0.f};

	_bool m_isChaninge = { false };

	_wstring m_strText = {};

	TEXTWINDOW_TYPE		m_eType = { TYPE_END };

private:
	HRESULT Ready_Components();

public:
	static CUI_TextWindow* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END