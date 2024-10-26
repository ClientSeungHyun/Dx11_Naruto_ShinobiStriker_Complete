#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CUI_TargetCursor final : public CUIObject
{
public:
	typedef struct : public CUIObject::UI_DESC
	{
		class CPlayer* pPlayer;
	}CURSOR_DESC;

private:
	CUI_TargetCursor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_TargetCursor(const CUI_TargetCursor& Prototype);
	virtual ~CUI_TargetCursor() = default;

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

private:
	class CShader* m_pShaderCom = { nullptr };
	class CTexture* m_pTextureCom = { nullptr };	// »¡°£»ö
	class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	class CPlayer* m_pPlayer = { nullptr };

	_float3			m_vInitSize = {};
	_float3			m_vAppearSize = {};
	_float3			m_vDisappearSize = {};

private:
	HRESULT Ready_Components();

public:
	static CUI_TargetCursor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END