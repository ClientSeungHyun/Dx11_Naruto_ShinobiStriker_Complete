#pragma once

#include "Base.h"

BEGIN(Engine)

class CLight_Manager final : public CBase
{
private:
	CLight_Manager();
	virtual ~CLight_Manager() = default;

public:
	const LIGHT_DESC* Get_LightDesc(_uint iIndex) const;

	_float4x4 Get_StaticLightViewMatrix()const { return m_StaticLightViewMatrix; }
	void Set_StaticLightViewMatrix(_matrix vStaticViewMatirx) { XMStoreFloat4x4(&m_StaticLightViewMatrix, vStaticViewMatirx); }


public:
	HRESULT Initialize();
	HRESULT Add_Light(const LIGHT_DESC& LightDesc);
	HRESULT Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);

	void Clear();

private:
	list<class CLight*>				m_Lights;

	_float4x4						m_StaticLightViewMatrix = {};

public:
	static CLight_Manager* Create();
	virtual void Free() override;
};

END