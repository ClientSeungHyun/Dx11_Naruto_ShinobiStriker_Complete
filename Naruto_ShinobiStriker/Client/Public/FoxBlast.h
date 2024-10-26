#pragma once
#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CCollider;
END

BEGIN(Client)

class CFoxBlast : public CEffect
{

private:
	CFoxBlast(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFoxBlast(const CFoxBlast& Prototype);
	virtual ~CFoxBlast() = default;

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
	virtual void OnCollisionEnter(CGameObject* pOther) override;
	virtual void OnCollisionStay(CGameObject* pOther)  override;
	virtual void OnCollisionExit(CGameObject* pOther)  override;

public:
	void		Appear(_fvector vInitPos, _fvector vTargetPos);
	void		DisAppear();

private:
	class CCollider* m_pColliderCom = { nullptr };
	class CTexture* m_pBlastTexture = { nullptr };

	CGameObject* m_pOwner = { nullptr };

	_float			m_fCurrentTime = { 0.f };
	_float			m_fRotationsSpeed = 15.f;
	_float			m_fAlphaRatio = {};
	_float			m_fTexCoordY = {};
	_float			m_fDamageTime = { 0.f };

	_float3			m_vInitPos = {};
	_float3			m_vInitScale = {};
	_float3			m_vCurrentScale = {};
	_float3			m_vTargetScale = {};


private:
	virtual HRESULT Ready_Components();

public:
	static CFoxBlast* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();
};

END