#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
class CSound;
END

BEGIN(Client)

class CCustomPannel final : public CUIObject
{
public:
	enum CUSTOMPANNES_TYPE { PANNEL_MAIN, PANNEL_HEAD, PANNEL_EYE, PANNEL_FACE, PANNEL_ONCE, PANNEL_UPPER, PANNEL_LOWER, PANNEL_END };

public:
	typedef struct : public CUIObject::UI_DESC
	{
		_uint				iNumButton;
		CUSTOMPANNES_TYPE	eType;
	}PANNEL_DESC;

private:
	CCustomPannel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCustomPannel(const CCustomPannel& Prototype);
	virtual ~CCustomPannel() = default;

public:
	_uint Get_SelectIndex() { return m_iSelectIndex; }
	CUSTOMPANNES_TYPE Get_PannelType() { return m_ePannelType; }

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

	void Clear_Select();

private:
	class CShader* m_pShaderCom = { nullptr };
	class CTexture* m_pTextureCom = { nullptr };
	class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	class CSound* m_pSoundCom = { nullptr };

private:
	_int					m_iSelectIndex = { 0 };
	_int					m_iChildIndex = { 0 };

	_uint					m_iNumButton = {};

	_float					m_fCurrentTime = { 0.f };

	_bool					m_isFirst = { true };
	_bool					m_isSelectConfirmButton = { false };

	CUSTOMPANNES_TYPE		m_ePannelType = { PANNEL_END };

	vector<class CCustom_Button*> m_Buttons;

private:
	void Control_UI();

	HRESULT Ready_Components();
	HRESULT Ready_Child();

public:
	static CCustomPannel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END