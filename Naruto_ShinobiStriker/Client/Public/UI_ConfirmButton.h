#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CUI_ConfirmButton final : public CUIObject
{
public:
	typedef struct : public CUIObject::UI_DESC
	{
		_uint iButtonIndex;
		_bool isSelect = { false };
	}BUTTON_DESC;

private:
	CUI_ConfirmButton(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_ConfirmButton(const CUI_ConfirmButton& Prototype);
	virtual ~CUI_ConfirmButton() = default;

public:
	_bool Get_IsSelect() { return m_isSelect; }
	void Set_IsSelect(_bool isSelect) { m_isSelect = isSelect; }

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

public:
	class CShader* m_pShaderCom = { nullptr };
	class CTexture* m_pTextureCom = { nullptr };
	class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	_bool		m_isSelect = { false };
	_bool		m_isChangeLevel = { false };

	_uint		m_iButtonIndex = {};

	_float		m_fInitX = {};

	_wstring	m_strPartName;

private:
	HRESULT Ready_Components();

public:
	static CUI_ConfirmButton* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END