#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CUI_PlayerSP final : public CUIObject
{
public:
	typedef struct : public CUIObject::UI_DESC
	{
		class CPlayer* pPlayer;
	}SPUI_DESC;

private:
	CUI_PlayerSP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_PlayerSP(const CUI_PlayerSP& Prototype);
	virtual ~CUI_PlayerSP() = default;

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
	class CTexture* m_pTextureCom = { nullptr };
	class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	CTexture* m_pTexture_SkillSP = { nullptr };

	HRESULT Ready_Components();

public:
	static CUI_PlayerSP* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END