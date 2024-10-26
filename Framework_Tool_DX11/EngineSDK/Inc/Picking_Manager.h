#pragma once
#include "Base.h"
#include "GameObject.h"

BEGIN(Engine)

class CPicking_Manager final : public CBase
{
public:
	CPicking_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPicking_Manager() = default;

public:
	class CGameObject*	Get_PickedObject() { return m_pPickObject; }
	void				Set_PickedObject(class CGameObject* pObject) { 
		if (nullptr != m_pPickObject)
		{
			m_pPickObject->Set_IsPicked(false);
		}

		m_pPickObject = pObject;

		pObject->Set_IsPicked(true);
	}

	void Set_IsInUI(_bool isIn) { m_isInUl = isIn; }

	_float3 Get_PickingPos() { return m_vPickingPos; }

	_vector Get_RayPos_World() { return XMLoadFloat3(&m_vRayPos); }
	_vector Get_RayDir_World() { return XMLoadFloat3(&m_vRayDir); }

	_vector Get_RayPos_Local() { return XMLoadFloat3(&m_vRayPos_InLocalSpace); }
	_vector Get_RayDir_Local() { return XMLoadFloat3(&m_vRayDir_InLocalSpace); }

public:
	HRESULT Initialize(HWND hWnd, _uint iWinSizeX, _uint iWinSizeY);
	void Update();
	void Transform_ToLocalSpace(const _matrix WorldMatrixInverse);
	_bool isPicked_InWorldSpace(_float3& vPointA, _float3& vPointB, _float3& vPointC, _float3* pOut);

	_bool isPicked_InLocalSpace(const _fvector& vPointA, const _fvector& vPointB, const _fvector& vPointC, _float3* pOut);
	_bool isPicked_InLocalSpace(const _fvector& vPointA, const _fvector& vPointB, const _fvector& vPointC, const _gvector& vRayPos, _float3* pOut);

public:
	void	Add_PickingList(class CGameObject* pObj);
	_bool	Is_Picking();

private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };
	class CGameInstance*		m_pGameInstance = { nullptr };

	HWND						m_hWnd = {};
	_uint						m_iWinSizeX = {};
	_uint						m_iWinSizeY = {};

	_bool						m_isInUl = { false };

private:
	_float3						m_vRayPos = {};
	_float3						m_vRayDir = {};

private:
	_float3						m_vRayPos_InLocalSpace = {};
	_float3						m_vRayDir_InLocalSpace = {};

private:
	list<class CGameObject*>	m_PickingObjects;
	class CGameObject*			m_pPickObject = { nullptr };
	_float3						m_vPickingPos = {};

public:
	static CPicking_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd, _uint iWinSizeX, _uint iWinSizeY);
	virtual void Free() override;
};

END