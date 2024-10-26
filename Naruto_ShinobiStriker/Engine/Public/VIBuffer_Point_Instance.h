#pragma once

#include "VIBuffer_Instancing.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Point_Instance final : public CVIBuffer_Instancing
{
private:
	CVIBuffer_Point_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Point_Instance(const CVIBuffer_Point_Instance& Prototype);
	virtual ~CVIBuffer_Point_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype(const CVIBuffer_Instancing::INSTANCE_PARTICLE_DESC& Desc) override;
	virtual HRESULT Initialize(void* pArg) override;

	HRESULT ReSetting_Instance(const INSTANCE_PARTICLE_DESC& Desc, void* pInstanceVertices);

public:
	virtual void Spread(_float fTimeDelta) override;
	virtual void Drop(_float fTimeDelta) override;
	void Up(_float fTimeDelta);
	void Explosion(_float fTimeDelta, _fvector vVelocity);

public:
	void Reset();

public:
	static CVIBuffer_Point_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const CVIBuffer_Instancing::INSTANCE_PARTICLE_DESC& Desc);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};


END