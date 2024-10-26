#pragma once
#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Trail_Instance;
END

BEGIN(Client)

class CChidoriTrail : public CEffect
{
private:
	CChidoriTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CChidoriTrail(const CChidoriTrail& Prototype);
	virtual ~CChidoriTrail() = default;

public:
	void Set_TrailPos(_fvector vPos) { XMStoreFloat3(&m_vTrailPos, vPos); }
	void Set_TrailSize(_float fSize) { m_fTrailSize = fSize; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void		Appear(_float4 vColor = _float4(0.07f, 0.36f, 0.91f, 0.8f), _uint iNumTrail = 0);
	void		DisAppear();
	void		Clear();

private:
	class CVIBuffer_Trail_Instance* m_pVIBufferCom = { nullptr };

	_uint						m_iNumTrail = {};
	_bool						m_isDisappear = { false };

	_float3						m_vTrailPos = {};
	_float						m_fTrailSize = {};


private:
	virtual HRESULT Ready_Components();

public:
	static CChidoriTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();
};

END