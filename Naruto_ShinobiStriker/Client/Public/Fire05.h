#pragma once
#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CFire05 : public CEffect
{
public:
	enum FIRE_TYPE { AMATERAS, FIREBALL, FIRE_END };

private:
	CFire05(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFire05(const CFire05& Prototype);
	virtual ~CFire05() = default;

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
	void		Appear(_fvector vPos, _float fScale, FIRE_TYPE eType = AMATERAS);
	void		DisAppear();

private:
	class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	class CTexture* m_pTexture_Ring = { nullptr };	// 마스크용
	class CTexture* m_pTexture_Sphere = { nullptr }; // 마스크용

	CGameObject* m_pOwner = { nullptr };

	_uint		m_iTexCoordCol = { 0 };
	_uint		m_iTexCoordRow = { 0 };

	_uint		m_iNumCol = { 0 };
	_uint		m_iNumRow = { 0 };

	_uint		m_iFrameIndex = { 0 };

	_float		m_fCurrentTime = {};
	_float		m_fFrame = {};
	_float		m_fCurrentScale = {};
	_float		m_fTargetScale = {};

	_bool		m_isDisappear = { false };

	FIRE_TYPE	m_eType = { FIRE_END };

private:
	virtual HRESULT Ready_Components();

public:
	static CFire05* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();
};

END