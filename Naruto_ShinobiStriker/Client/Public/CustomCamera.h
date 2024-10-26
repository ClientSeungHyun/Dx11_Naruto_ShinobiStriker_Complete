#pragma once
#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

class CCustomCamera :
	public CCamera
{
public:
	typedef struct : public CCamera::CAMERA_DESC {
		_float fSensor;
	}CAMERA_CUSTOM_DESC;

private:
	CCustomCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCustomCamera(const CCamera& Prototype);
	virtual ~CCustomCamera() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void ZoomIn(_float fTimeDelta);
	void ZoomOut(_float fTimeDelta);

	void Start_ZoomIn(_fvector vZoomPos, _float fZoom = 0.f, _float fDuration = 1.f);
	void End_ZoomIn(_float fDuration = 1.f);

private:
	HRESULT					Ready_Components();

private:
	_float					m_fSensor = {};
	_float					m_fInitFovy = {};
	_float3					m_vInitPos = {};
	POINT					m_ptOldMousePos = {};

private:
	_float					m_fZoomTime = { 0.f };
	_float					m_fZoomDuration = { 1.f };
	_float					m_fZoom = { 0.f };
	_float3					m_vZoomPos = {};

	_bool					m_isZoomIn = { false };
	_bool					m_isZoomOut = { false };

	_bool					m_isStateZoomIn = { false };

private:
	_float Lerp(_float a, _float b, _float t) {
		return a + t * (b - a);
	}

public:
	static CCustomCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END