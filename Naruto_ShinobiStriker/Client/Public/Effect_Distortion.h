#pragma once
#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CEffect_Distortion : public CEffect
{
public:
	enum DISTORTION_TYPE { DIST_RING, DIST_CIRCLE, DIST_RADIATION, DIST_RASEN, DIST_END };
private:
	CEffect_Distortion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Distortion(const CEffect_Distortion& Prototype);
	virtual ~CEffect_Distortion() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void		Appear(_fvector vPos, _float fInitScale = 1.f, _float fTargetScale = 2.f, _float fScaleSpeed = 2.f, _float fDuration = 0.2f, DISTORTION_TYPE eType = DIST_RING);
	void		DisAppear();

private:
	class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	_float		m_fCurrentTime = {};
	_float		m_fInitScale = {};
	_float		m_fCurrentScale = {};
	_float		m_fTargetScale = {};
	_float		m_fScaleSpeed = {};
	_float		m_fDuration = {};

	DISTORTION_TYPE m_eType = { DIST_END };

private:
	virtual HRESULT Ready_Components();

public:
	static CEffect_Distortion* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();
};

END