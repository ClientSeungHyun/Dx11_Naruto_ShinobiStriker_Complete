#pragma once
#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CEffect_TextureScale : public CEffect
{
public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC{
		_wstring strTextureTag;
		_float4	vColor;
		_float fDuration;
	}TEXTURE_SCALE_DESC;

private:
	CEffect_TextureScale(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_TextureScale(const CEffect_TextureScale& Prototype);
	virtual ~CEffect_TextureScale() = default;

public:
	void		Set_Owner(CGameObject* pOwner) { m_pOwner = pOwner; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void		Appear(_float fInitSize, _float fAlpha, _bool isRandomRotation);
	void		DisAppear();

private:
	class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	CGameObject* m_pOwner = { nullptr };

	_float			m_fCurrentTime = { 0.f };
	_float			m_fScaleDuration = { 0.f };

	_float			m_fSize = {};

private:
	virtual HRESULT Ready_Components(_wstring strTextureTag);

public:
	static CEffect_TextureScale* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();
};

END