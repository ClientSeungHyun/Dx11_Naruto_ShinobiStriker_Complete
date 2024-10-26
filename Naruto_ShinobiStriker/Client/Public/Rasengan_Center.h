#pragma once
#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CRasengan_Center : public CEffect
{

private:
	CRasengan_Center(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRasengan_Center(const CRasengan_Center& Prototype);
	virtual ~CRasengan_Center() = default;

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
	void		Appear();
	void		DisAppear();

private:
	class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	class CTexture* m_pTexture_Ring = { nullptr };	// 마스크용
	class CTexture* m_pTexture_Sphere = { nullptr }; // 마스크용

	CGameObject*	m_pOwner = { nullptr };

	_float			m_fTexCoord = 3.0f;
	_float			m_fCurrentScale = 0.01f;
	_float			m_fTargetScale = 0.4f;
	_float			m_fScaledSpeed = 1.f;
	_float			m_fRadian = 0.f;

	_bool			m_isRing = { false };

private:
	virtual HRESULT Ready_Components();

public:
	static CRasengan_Center* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();
};

END