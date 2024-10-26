#pragma once
#include "Base.h"

BEGIN(Engine)

class CPicking_Manager final : public CBase
{
public:
	CPicking_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPicking_Manager() = default;

public:
	_vector				Get_RayPos() { return XMLoadFloat3(&m_vRayPos); }
	void				Set_RayPos(_vector vRayPos) { XMStoreFloat3(&m_vRayPos, vRayPos); }

	_vector				Get_RayDir() { return XMLoadFloat3(&m_vRayDir); }
	void				Set_RayDir(_vector vRayDir) { XMStoreFloat3(&m_vRayDir, XMVector3Normalize(vRayDir)); }

	class CGameObject*	Get_Player() { return m_pPlayer; }
	void				Set_Player(class CGameObject* pPlayer) { m_pPlayer = pPlayer; }

public:
	HRESULT Initialize(HWND hWnd, _uint iWinSizeX, _uint iWinSizeY);
	void Update();
	void Transform_ToLocalSpace(const _matrix WorldMatrixInverse);
	_bool isPicked_InWorldSpace(_float3& vPointA, _float3& vPointB, _float3& vPointC, _float3* pOut);

	_bool isPicked_InLocalSpace(const _fvector& vPointA, const _fvector& vPointB, const _fvector& vPointC, _float3* pOut);

public:
	void	Add_ModelPickingList(class CGameObject* pObj);

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };

	HWND						m_hWnd = {};
	_uint						m_iWinSizeX = {};
	_uint						m_iWinSizeY = {};

private:
	_float3						m_vRayPos = {};
	_float3						m_vRayDir = {};

private:
	_float3						m_vRayPos_InLocalSpace = {};
	_float3						m_vRayDir_InLocalSpace = {};

private:
	class CGameObject*			m_pPlayer = { nullptr };

	list<class CGameObject*>	m_ModelPickingObjects;
	_float3						m_vModelPickingPos = {};

private:
	void IsPicking_Mesh(class CGameObject* pObj);

public:
	static CPicking_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd, _uint iWinSizeX, _uint iWinSizeY);
	virtual void Free() override;
};

END