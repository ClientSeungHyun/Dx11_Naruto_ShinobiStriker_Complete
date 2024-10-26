#pragma once
#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CFireBall_Effect : public CEffect
{

private:
	CFireBall_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFireBall_Effect(const CFireBall_Effect& Prototype);
	virtual ~CFireBall_Effect() = default;

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
	void		Appear();
	void		DisAppear();

private:
	CGameObject* m_pOwner = { nullptr };

	_float			m_fTexCoordX = { 0.f };
	_float			m_fCurrentScale = { 0.01f };
	_float			m_fTargetScale = { 1.f };
	_float			m_fScaledSpeed = { 4.f };

private:
	virtual HRESULT Ready_Components();

public:
	static CFireBall_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();
};

END