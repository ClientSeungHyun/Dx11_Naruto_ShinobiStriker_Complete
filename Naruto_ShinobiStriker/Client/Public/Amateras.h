#pragma once
#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CAmateras : public CEffect
{
public:
	enum AMATERAS_TYPE { AMATERAS0, AMATERAS1, AMATERAS2, AMATERAS3, AMATERAS4, AMATERAS5, AMATERAS_END };

public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		AMATERAS_TYPE eType;
	}AMATERAS_DESC;

private:
	CAmateras(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAmateras(const CAmateras& Prototype);
	virtual ~CAmateras() = default;

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
	CGameObject* m_pOwner = { nullptr };

	_float			m_fTexCoord = { -1.f };

	AMATERAS_TYPE	m_eType = { AMATERAS_END };

private:
	virtual HRESULT Ready_Components();

public:
	static CAmateras* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();
};

END