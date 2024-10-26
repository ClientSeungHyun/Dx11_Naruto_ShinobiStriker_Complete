#pragma once
#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CRasenShuriken_Blade : public CEffect
{

private:
	CRasenShuriken_Blade(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRasenShuriken_Blade(const CRasenShuriken_Blade& Prototype);
	virtual ~CRasenShuriken_Blade() = default;

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

	_float			m_fCurrentScale = 0.1f;
	_float			m_fTargetScale = 3.5f;

	_float			m_fRadian = 0.f;
	_float			m_fRotationsSpeed = 15.f;
	_float			m_fScaledSpeed = 1.f;

private:
	virtual HRESULT Ready_Components();

public:
	static CRasenShuriken_Blade* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();
};

END