#pragma once
#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CLightning : public CEffect
{
public:
	enum LIGHTNING_TYPE {
		LIGHTNING_A1, LIGHTNING_A2, LIGHTNING_B1, LIGHTNING_B2, LIGHTNING_B3, LIGHTNING_B4, LIGHTNING_C,
		LIGHTNING_D, LIGHTNING_E, LIGHTNING_F, LIGHTNING_CHIDORI, LIGHTNING_END	};

public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		LIGHTNING_TYPE eType;
	}LIGHTNING_DESC;

private:
	CLightning(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLightning(const CLightning& Prototype);
	virtual ~CLightning() = default;

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
	void		Appear(_fvector vPos, _float fScale = 1.f);
	void		DisAppear();

private:
	CGameObject*	m_pOwner = { nullptr };

	_bool			m_isEnd = false;

	_float			m_fTexCoord = 0.f;

	_float			m_fAliveTime = 5.0f;
	_float			m_fCurrentTime = 0.0f;

	LIGHTNING_TYPE	m_eType = { LIGHTNING_END };

private:
	virtual HRESULT Ready_Components();

public:
	static CLightning* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();
};

END