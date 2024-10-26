#pragma once

#include "Bounding.h"

BEGIN(Engine)

class CBounding_Shpere final : public CBounding
{
public:
	typedef struct : public CBounding::BOUNDING_DESC
	{
		_float	fRadius;
	}BOUNDING_SPHERE_DESC;
private:
	CBounding_Shpere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBounding_Shpere() = default;

public:
	virtual HRESULT Initialize(CBounding::BOUNDING_DESC* pBoundingDesc) override;

private:
	BoundingSphere* m_pBoundingDesc = { nullptr };

public:
	static CBounding_Shpere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CBounding::BOUNDING_DESC* pBoundingDesc);
	virtual void Free() override;
};

END