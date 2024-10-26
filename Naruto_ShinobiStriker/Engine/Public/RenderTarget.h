#pragma once

#include "Base.h"

BEGIN(Engine)

class CRenderTarget final : public CBase
{
private:
	CRenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderTarget() = default;

public:
	ID3D11RenderTargetView* Get_RTV() const {
		return m_pRTV;
	}

public:
	HRESULT Initialize(_uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName);
	void Clear();

	HRESULT Create_Texture(const _wstring& strFilePath);
	HRESULT Load_Texture(const _wstring& strTextureFilePath);

#ifdef _DEBUG
public:
	HRESULT Initialize_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);

#endif


private:
	_uint			m_iWidht = {};
	_uint			m_iHeight = {};
	DXGI_FORMAT		m_ePixelFormat = {};

	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	ID3D11Texture2D* m_pTexture2D = { nullptr };
	ID3D11RenderTargetView* m_pRTV = { nullptr };
	ID3D11ShaderResourceView* m_pSRV = { nullptr };
	_float4						m_vClearColor = {};

#ifdef _DEBUG

private:
	_float4x4					m_WorldMatrix = {};

#endif

public:
	static CRenderTarget* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	virtual void Free() override;
};

END