#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Point_Instance;
END

BEGIN(Client)

class CParticle_Misuok final : public CGameObject
{
	typedef struct {
		_float4 vPos;
		_float4 vVelocity;
		_float4 vScale;
		_float2 vLifeTime;
	}HITEFFECT_DATA;
private:
	CParticle_Misuok(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticle_Misuok(const CParticle_Misuok& Prototype);
	virtual ~CParticle_Misuok() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Appear(_fvector vStartPos, _fvector vColor, _uint iNumParticle, _float fPower = 1.f);

public:
	class CShader* m_pShaderCom = { nullptr };
	class CTexture* m_pTextureCom = { nullptr };
	class CVIBuffer_Point_Instance* m_pVIBufferCom = { nullptr };

	_uint m_iNumParticle = {};

	_float m_fCurrentTime = { 0.f };

	_float4 m_vColor = {};

	vector<HITEFFECT_DATA> m_InstanceData;

private:
	void Calculate_Gravity(_float fTimeDelta);
	HRESULT Ready_Components();

public:
	static CParticle_Misuok* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END