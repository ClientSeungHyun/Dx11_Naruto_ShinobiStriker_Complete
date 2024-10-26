#pragma once
#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CCollider;
class CSound;
END

BEGIN(Client)

class CFoxExplosionSphere : public CEffect
{

private:
	CFoxExplosionSphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFoxExplosionSphere(const CFoxExplosionSphere& Prototype);
	virtual ~CFoxExplosionSphere() = default;

public:
	void		Set_Owner(CGameObject* pOwner) { m_pOwner = pOwner; }
	_float		Get_CurrentTime() { return m_fCurrentTime; }

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
	class CCollider* m_pColliderCom = { nullptr };
	class CSound* m_pSoundCom = { nullptr };

	CGameObject* m_pOwner = { nullptr };

	_float			m_fCurrentTime = { 0.f };
	_float			m_fCurrentScale = 0.1f;
	_float			m_fTargetScale = 3.5f;

	_float			m_fRotationsSpeed = 15.f;
	_float			m_fScaledSpeed = 1.f;

	_float			m_fAlphaRatio = {};
	_float			m_fTexCoordX = {};

private:
	virtual HRESULT Ready_Components();

public:
	static CFoxExplosionSphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();
};

END