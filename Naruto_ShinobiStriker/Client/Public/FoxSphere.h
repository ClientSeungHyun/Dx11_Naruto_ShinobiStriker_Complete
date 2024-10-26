#pragma once
#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CCollider;
class CVIBuffer_Rect;
class CSound;
END

BEGIN(Client)

class CFoxSphere : public CEffect
{

private:
	CFoxSphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFoxSphere(const CFoxSphere& Prototype);
	virtual ~CFoxSphere() = default;

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
	class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	class CCollider* m_pColliderCom = { nullptr };
	class CTexture* m_pSmallTexture = { nullptr };
	class CSound* m_pSoundCom = { nullptr };

	_float3		m_vMoeDir = {};

	_float		m_fCurrentTime = {};
	_float		m_fInitScale = {};
	_float		m_fCurrentScale = {};
	_float		m_fReSizeTime = {};

	_bool		m_isExplosion = { false };

	class CFoxExplosionSphere* m_pFoxExplosionSphere = { nullptr };

private:
	virtual HRESULT Ready_Components();

public:
	static CFoxSphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();
};

END