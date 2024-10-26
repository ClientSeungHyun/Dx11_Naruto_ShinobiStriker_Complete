#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CCustom_Button final : public CUIObject
{
public:
	typedef struct : public CUIObject::UI_DESC
	{
		_uint iButtonIndex;
		_bool isSelect = { false };
	}BUTTON_DESC;

private:
	CCustom_Button(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCustom_Button(const CCustom_Button& Prototype);
	virtual ~CCustom_Button() = default;

public:
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

	_uint		m_iButtonIndex = {};

	_float		m_fInitX = {};

	_wstring	m_strPartName;

private:
	HRESULT Ready_Components();

	void Choice_PartName();

	void Choice_MainName();
	void Choice_HeadName();
	void Choice_EyeName();
	void Choice_FaceName();
	void Choice_OnceName();
	void Choice_UpperName();
	void Choice_LowerName();

public:
	static CCustom_Button* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END