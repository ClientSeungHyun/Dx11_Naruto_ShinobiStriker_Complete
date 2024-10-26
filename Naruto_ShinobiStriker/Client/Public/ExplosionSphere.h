#pragma once
#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CExplosionSphere : public CEffect
{

private:
	CExplosionSphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CExplosionSphere(const CExplosionSphere& Prototype);
	virtual ~CExplosionSphere() = default;

public:
	void		Set_Owner(CGameObject* pOwner) { m_pOwner = pOwner; }
	_float		Get_CurrentTime() { return m_fCurrentTime; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void		Appear();
	void		DisAppear();

	CGameObject* m_pOwner = { nullptr };

	_float			m_fCurrentTime = { 0.f };
	_float			m_fCurrentScale = 0.1f;
	_float			m_fTargetScale = 3.5f;

	_float			m_fRotationsSpeed = 15.f;
	_float			m_fScaledSpeed = 1.f;

	_float			m_fAlphaRatio = {};
	_float			m_fTexCoordY = {};

private:
	virtual HRESULT Ready_Components();

public:
	static CExplosionSphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();
};

END