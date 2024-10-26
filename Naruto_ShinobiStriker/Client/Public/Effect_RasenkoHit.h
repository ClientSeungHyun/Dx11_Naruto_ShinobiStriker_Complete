#pragma once
#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CEffect_RasenkoHit : public CEffect
{
private:
	CEffect_RasenkoHit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_RasenkoHit(const CEffect_RasenkoHit& Prototype);
	virtual ~CEffect_RasenkoHit() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void		Appear(_fvector vPos);
	void		DisAppear();

private:
	class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	CGameObject* m_pOwner = { nullptr };
	_uint		m_iFrameIndex = { 0 };

	_float		m_fCurrentTime = {};

	_float3		m_vInitScaleX = { 10.f, 1.f, 1.f };
	_float3		m_vTargetScaleX = { 1.f, 1.f,1.f };
	_float3		m_vInitScaleY = { 1.f, 1.f, 1.f };
	_float3		m_vTargetScaleY = { 1.f, 10.f,1.f };
	_float3		m_vCurrentScale = { 1.f,1.f,1.f };

	_bool		m_isScaleX = { false };
	_bool		m_isScaleY = { false };

private:
	virtual HRESULT Ready_Components();

public:
	static CEffect_RasenkoHit* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();
};

END