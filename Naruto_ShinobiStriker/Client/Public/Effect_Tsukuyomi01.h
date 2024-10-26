#pragma once
#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CEffect_Tsukuyomi01 : public CEffect
{
public: 
	enum TSUKUYOMI { BASE, ACTION, TS_END };

private:
	CEffect_Tsukuyomi01(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Tsukuyomi01(const CEffect_Tsukuyomi01& Prototype);
	virtual ~CEffect_Tsukuyomi01() = default;

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
	void		Appear(_fvector vPos, _float fScale = 0.5f, _float fTargetScale = 1.f, TSUKUYOMI eType = BASE);
	void		DisAppear();

	void		Start_Reverse();

private:
	class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	CGameObject*	m_pOwner = { nullptr };

	_float			m_fAngle = {};
	_float			m_fCurrentScale = {};
	_float			m_fTargetScale = {};

	_bool			m_isReverse = { false };

	TSUKUYOMI		m_eTsukuyomiType = TS_END;

private:
	virtual HRESULT Ready_Components();

public:
	static CEffect_Tsukuyomi01* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();
};

END