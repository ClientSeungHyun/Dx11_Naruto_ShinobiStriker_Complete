#pragma once
#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CEffect_GroundBreak : public CEffect
{
private:
	CEffect_GroundBreak(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_GroundBreak(const CEffect_GroundBreak& Prototype);
	virtual ~CEffect_GroundBreak() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void		Appear(_fvector vPos, _float3 vSize = _float3(1.f,1.f,1.f), _float4 vColor = _float4(0.43f, 0.37f, 0.32f, 1.f));
	void		DisAppear();

	_float		m_fCurrentTime = {};

private:
	virtual HRESULT Ready_Components();

public:
	static CEffect_GroundBreak* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();
};

END