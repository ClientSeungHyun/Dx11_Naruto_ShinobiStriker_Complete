#pragma once
#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CRasengan_CreateSmoke : public CEffect
{

private:
	CRasengan_CreateSmoke(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRasengan_CreateSmoke(const CRasengan_CreateSmoke& Prototype);
	virtual ~CRasengan_CreateSmoke() = default;

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
	CGameObject*	m_pOwner = { nullptr };

	_bool			m_isEnd = false;

	_float			m_fTexCoord = 3.0f;

	_float			m_fAliveTime = 5.0f;
	_float			m_fCurrentTime = 0.0f;

private:
	virtual HRESULT Ready_Components();

public:
	static CRasengan_CreateSmoke* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();
};

END