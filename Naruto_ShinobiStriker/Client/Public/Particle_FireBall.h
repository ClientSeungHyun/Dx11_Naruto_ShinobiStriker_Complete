#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Point_Instance;
END

BEGIN(Client)

class CParticle_FireBall final : public CGameObject
{
	typedef struct {
		_float4 vPos;
		_float4 vVelocity;
		_float4 vScale;
		_float2 vLifeTime;
	}HITEFFECT_DATA;
private:
	CParticle_FireBall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticle_FireBall(const CParticle_FireBall& Prototype);
	virtual ~CParticle_FireBall() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Appear(_fvector vStartPos);

public:
	class CShader* m_pShaderCom = { nullptr };
	class CTexture* m_pTextureCom = { nullptr };
	class CVIBuffer_Point_Instance* m_pVIBufferCom = { nullptr };

	_float	m_fCurrentTime = {};

	_float3 m_vVelocity = {};
	_float4 m_vColor = {};

private:
	HRESULT Ready_Components();

public:
	static CParticle_FireBall* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END