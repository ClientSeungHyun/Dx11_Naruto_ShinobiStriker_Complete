#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CUI_Target final : public CUIObject
{
private:
	CUI_Target(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Target(const CUI_Target& Prototype);
	virtual ~CUI_Target() = default;

public:
	void Set_Target(class CMonster* pMonster) { m_pMonster = pMonster; }

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
	class CTexture* m_pTextureCom = { nullptr };	// »¡°£»ö
	class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	class CMonster* m_pMonster = { nullptr };

private:
	HRESULT Ready_Components();

public:
	static CUI_Target* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END