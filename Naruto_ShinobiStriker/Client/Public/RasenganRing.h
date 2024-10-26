#pragma once
#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CRasenganRing : public CEffect
{

private:
	CRasenganRing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRasenganRing(const CRasenganRing& Prototype);
	virtual ~CRasenganRing() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void		Appear(CGameObject* pOwner, _float fInitScale = 1.f, _float fTargetScale = 2.f, _float fScaleSpeed = 1.f, _float fLifeTime = 1.f, _float4 vColor = { 0.05f, 0.75f, 0.9f, 1.f });
	void		DisAppear();

private:
	CGameObject*	m_pOwner = { nullptr };

	_float			m_fCurrentTime = { 0.f };
	_float			m_fLifeTime = { 1.f };
	_float			m_fCurrentScale = { 0.01f };
	_float			m_fTargetScale = { 1.f };
	_float			m_fScaledSpeed = { 4.f };
	_float			m_fTexCoordX = 0.f;

private:
	virtual HRESULT Ready_Components();

public:
	static CRasenganRing* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();
};

END