#pragma once
#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CExplosionCircle : public CEffect
{

private:
	CExplosionCircle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CExplosionCircle(const CExplosionCircle& Prototype);
	virtual ~CExplosionCircle() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void		Appear(_float3 vPos, _float4 vColor, _float fInitScale, _float fTargetScale, _float fScaleSpeed, _float fLifeTime);
	void		DisAppear();

private:
	class CVIBuffer_Rect*	m_pVIBufferCom = { nullptr };

	_float		m_fCurrentScale = {};
	_float		m_fTargetScale = {};
	_float		m_fScaleSpeed = {};
	_float		m_fLifeTime = {};
	_float		m_fCurrentTime = {};

private:
	virtual HRESULT Ready_Components();

public:
	static CExplosionCircle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();
};

END