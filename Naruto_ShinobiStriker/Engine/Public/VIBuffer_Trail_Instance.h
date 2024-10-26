#pragma once

#include "VIBuffer_Instancing.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Trail_Instance final : public CVIBuffer_Instancing
{
public:
	typedef struct {
		_uint		iNumInstance;
	}INSTACNE_TRAIL_DESC;

	typedef struct {
		_float3 vTrailPos;
		_float4x4 PointMatrix;
	}TRAILPOINT;

private:
	CVIBuffer_Trail_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Trail_Instance(const CVIBuffer_Trail_Instance& Prototype);
	virtual ~CVIBuffer_Trail_Instance() = default;

public:
	void Set_TrailSize(_float fSize) { m_fTrailSize = fSize; }

public:
	virtual HRESULT Initialize_Prototype(const INSTACNE_TRAIL_DESC& Desc);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Bind_Buffers() override;
	virtual HRESULT Render() override;

	void	Add_TrailPoint(_fmatrix WorldMatirx, _uint iNumTrail = 0);
	void	Add_TrailPoint(_fvector vPos, _uint iNumTrail = 0);
	void    Add_TrailPoint_ForWire(_fmatrix WorldMatrix, _fvector vHandPos, _float fSize);
	void	Update_Trail();
	void	Update_Trail_ForWire();
	void	Clear_Trail();
private:
	list<TRAILPOINT>	m_TrailPoints;

	_float				m_fTrailSize = { 0.01f };

public:
	static CVIBuffer_Trail_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const INSTACNE_TRAIL_DESC& Desc);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};


END