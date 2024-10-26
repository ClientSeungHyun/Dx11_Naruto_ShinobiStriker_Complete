#pragma once

#include "GameObject.h"

/* ���� ������ ���� ��, ��������� �����, �����ν��Ͻ� �ȿ� �ִ� ���������ο� ������ �ش�.  */
/* ������� ������ ���� �����͵��� ������ �մ�. ����� == cTransform */

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

	// ī�޶� ����ŷ
	_float				m_fShakeTime = {};
	_float				m_fShakeDuration = {};
	_float				m_fShakePower = {};
	_bool				m_isShake = { false };
	_float3				m_vOriginLook = {};

	// �� �� �ƿ�
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