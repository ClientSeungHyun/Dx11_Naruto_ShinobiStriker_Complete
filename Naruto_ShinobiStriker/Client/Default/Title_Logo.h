#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CTitle_Logo final : public CUIObject
{
private:
	CTitle_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTitle_Logo(const CTitle_Logo& Prototype);
	virtual ~CTitle_Logo() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	class CShader* m_pShaderCom = { nullptr };
	class CTexture* m_pTextureCom = { nullptr };
	class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	_float		m_fTime = { 0.f };
	_bool		m_isBlink = { false };

private:
	HRESULT Ready_Components();

public:
	static CTitle_Logo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END