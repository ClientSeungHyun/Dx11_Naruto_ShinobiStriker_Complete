#pragma once
#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CCollider;
END

BEGIN(Client)

class CBladeSlash : public CEffect
{

private:
	CBladeSlash(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBladeSlash(const CBladeSlash& Prototype);
	virtual ~CBladeSlash() = default;

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

	_float		m_fCurrentTime = {};
	_float3		m_vMoeDir = {};

private:
	virtual HRESULT Ready_Components();

public:
	static CBladeSlash* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();
};

END