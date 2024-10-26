#pragma once
#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CCollider;
END

BEGIN(Client)

class CEffect_TypeZero : public CEffect
{
public:
	enum TYPEZERO { T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T_END };

public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		TYPEZERO eType;
	}TYPEZERO_DESC;

private:
	CEffect_TypeZero(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_TypeZero(const CEffect_TypeZero& Prototype);
	virtual ~CEffect_TypeZero() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void		Appear(_fmatrix PlayerWorldMatrix);
	void		DisAppear();

	_float			m_fCurrentTime = { 0.f };
	_float			m_fTexCoordX = {};

	TYPEZERO		m_eType;

private:
	virtual HRESULT Ready_Components();

public:
	static CEffect_TypeZero* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();
};

END