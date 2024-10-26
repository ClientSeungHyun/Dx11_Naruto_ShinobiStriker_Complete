#pragma once
#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CCollider;
END

BEGIN(Client)

class CEffect_RasenkoStar : public CEffect
{
private:
	CEffect_RasenkoStar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_RasenkoStar(const CEffect_RasenkoStar& Prototype);
	virtual ~CEffect_RasenkoStar() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void		Appear(CGameObject* pOwner);
	void		DisAppear();

private:
	CGameObject*	m_pOwner = { nullptr };

	_float			m_fCurrentTime = { 0.f };
	_float			m_fTexCoordY = {};

private:
	virtual HRESULT Ready_Components();

public:
	static CEffect_RasenkoStar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();
};

END