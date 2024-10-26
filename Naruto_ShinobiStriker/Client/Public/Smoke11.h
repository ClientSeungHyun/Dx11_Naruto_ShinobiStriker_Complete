#pragma once
#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CSmoke11 : public CEffect
{
public:
	enum SMOKETYPE { SMOKE_DUST, SMOKE_BLACK, SMOKE_BROWN, SMOKE_WHITE, SMOKE_END };

private:
	CSmoke11(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSmoke11(const CSmoke11& Prototype);
	virtual ~CSmoke11() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void		Appear(_float3 vCenterPos, _float3  vRange = _float3(1.f, 1.f, 1.f), _float fInitScale = 0.5f, SMOKETYPE eType = SMOKE_DUST);
	void		DisAppear();

private:
	class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	_uint		m_iTexCoordCol = { 0 };
	_uint		m_iTexCoordRow = { 0 };

	_uint		m_iNumCol = { 0 };
	_uint		m_iNumRow = { 0 };

	_uint		m_iFrameIndex = { 0 };

	_float		m_fCurrentTime = {};
	_float		m_fFrame = {};
	_float		m_fCurrentScale = {};
	_float		m_fTargetScale = {};
	_float		m_fScaledSpeed = {};

	_float3		m_vMoveDir = {};
	_float4		m_vInitColor = {};

	SMOKETYPE	m_eType = {};
private:
	virtual HRESULT Ready_Components();

public:
	static CSmoke11* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();
};

END