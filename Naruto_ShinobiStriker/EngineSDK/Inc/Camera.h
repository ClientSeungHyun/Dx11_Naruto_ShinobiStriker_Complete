#pragma once

#include "GameObject.h"

/* 원근 투영을 위한 뷰, 투영행렬을 만들고, 게임인스턴스 안엥 있는 파이프라인에 저장해 준다.  */
/* 투영행렬 생성을 위한 데이터들을 가지고 잇는. 뷰행렬 == cTransform */

BEGIN(Engine)

class ENGINE_DLL CCamera abstract : public CGameObject
{
public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		_float4		vEye;
		_float4		vAt;

		_float		fFovy = {};
		_float		fAspect = {};
		_float		fNear = {};
		_float		fFar = {};
	}CAMERA_DESC;

protected:
	CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera(const CCamera& Prototype);
	virtual ~CCamera() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

public:
	virtual void Start_Shake(_float fPower, _float fDuration);

	void ZoomIn(_float fFovy, _float fDuration = 1.f);
	void ZoomOut(_float fDuration = 1.f);

protected:
	_float				m_fFovy = {};
	_float				m_fAspect = {};
	_float				m_fNear = {};
	_float				m_fFar = {};

	// 카메라 쉐이킹
	_float				m_fShakeTime = {};
	_float				m_fShakeDuration = {};
	_float				m_fShakePower = {};
	_bool				m_isShake = { false };
	_float3				m_vOriginLook = {};

	// 줌 인 아웃
	_float				m_fInitFovy = {};
	_float				m_fZoomInFovy = {};
	_float				m_fTargetFovy = {};
	_float				m_fZoomTime = {};
	_float				m_fZoomDuration = {};

	_bool				m_isZoomIn = { false };
	_bool				m_isZoomOut = { false };

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END