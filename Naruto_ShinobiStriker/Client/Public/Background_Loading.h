#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CBackGround_Loading final : public CUIObject
{
private:
	CBackGround_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBackGround_Loading(const CBackGround_Loading& Prototype);
	virtual ~CBackGround_Loading() = default;

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

	_uint		m_iTextureNum = {};

private:
	HRESULT Ready_Components();

public:
	static CBackGround_Loading* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END