#pragma once
#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CMassiveRasengan_Sphere : public CEffect
{

private:
	CMassiveRasengan_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMassiveRasengan_Sphere(const CMassiveRasengan_Sphere& Prototype);
	virtual ~CMassiveRasengan_Sphere() = default;

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
	void		Appear(_float fScaleSpeed = 8.f, _float fTargetScale = 5.7f);
	void		DisAppear();

private:
	CGameObject* m_pOwner = { nullptr };

	_float			m_fTexCoord = 3.0f;
	_float			m_fCurrentScale = 0.1f;
	_float			m_fTargetScale = 3.5f;
	_float			m_fScaledSpeed = 1.f;

private:
	virtual HRESULT Ready_Components();

public:
	static CMassiveRasengan_Sphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();
};

END