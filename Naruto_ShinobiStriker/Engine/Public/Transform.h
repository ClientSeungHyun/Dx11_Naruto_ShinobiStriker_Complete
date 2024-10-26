#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	typedef struct
	{
		_float		fSpeedPerSec{}, fRotationPerSec{};
	}TRANSFORM_DESC;

	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };

private:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CTransform() = default;

public:
	_vector				Get_State(STATE eState) { return XMLoadFloat4x4(&m_WorldMatrix).r[eState]; }
	void				Set_State(STATE eState, _fvector vState) { XMStoreFloat3((_float3*)&m_WorldMatrix.m[eState][0], vState); }

	_matrix				Get_WorldMatrix() const { return XMLoadFloat4x4(&m_WorldMatrix); }	//연산용
	void				Set_WorldMatrix(const _matrix WorldMatrix) { XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix); }
	const _float4x4*	Get_WorldMatrix_Ptr() { return &m_WorldMatrix; }	//저장용

	_matrix				Get_WorldMatrix_Inverse() const { return XMMatrixInverse(nullptr, Get_WorldMatrix()); }


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg = nullptr);

public:
	_float3		Get_Scaled() const;
	void		Set_Scaled(_float fX, _float fY, _float fZ);

	void		Move_Dir(_float fTimeDelta,_float3 vDir, _float fSpeed = 0.f);

	void		Go_Straight(_float fTimeDelta, _float fSpeed = 0.f, class CNavigation* pNavigation = nullptr);
	void		Go_Backward(_float fTimeDelta, _float fSpeed = 0.f);
	void		Go_Right(_float fTimeDelta, _float fSpeed = 0.f);
	void		Go_Left(_float fTimeDelta, _float fSpeed = 0.f);

	void		Turn(const _vector& vAxis, _float fTimeDelta, _float fRotationPerSec = 0.f);
	void		Turn(_bool isRotationX, _bool isRotationY, _bool isRotationZ, _float fTimeDelta, _float fRotationPerSec = 0.f);
	void		Turn_Lerp(_fvector vLook, _float fTurnSpeed, _float fTimeDelta);
	void		Rotation(const _vector& vAxis, _float fRadian);
	void		Rotation(_float fX, _float fY, _float fZ);

	void		LookAt(_fvector vAt);
	void		LookAt_Forward(_fvector vAt);	// y축은 고려하지 않은 lookat
	void		BillBoard();
	void		BillBoardXZ();

	void		Set_NewUp(_vector vNewUp);
	void		Set_NewLook(_vector vNewLook);

public:
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName);

private:
	_float			m_fSpeedPerSec{};
	_float			m_fRotationPerSec{};

	_float4x4		m_WorldMatrix{};

public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual CComponent* Clone(void* pArg) { return nullptr; }
	virtual void Free() override;
};

END