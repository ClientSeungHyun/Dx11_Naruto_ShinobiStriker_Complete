#pragma once
#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CSmokeRing : public CEffect
{
private:
	CSmokeRing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSmokeRing(const CSmokeRing& Prototype);
	virtual ~CSmokeRing() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void		Appear(_fvector vPos, _float fScaleSpeed = 1.f, _float fInitScale = 3.f, _float fTargetScale = 5.f);
	void		DisAppear();

	_float		m_fCurrentTime = {};
	_float		m_fScaleSpeed = {};
	_float		m_fDissloveAmount = {};
	_float		m_fCurrentScale = {};
	_float		m_fTargetScale = {};

private:
	virtual HRESULT Ready_Components();

public:
	static CSmokeRing* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();
};

END