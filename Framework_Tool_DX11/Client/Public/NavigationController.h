#pragma once

#include "Client_Defines.h"
#include "Base.h"

#include "GameInstance.h"

#include "Cell.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)

class CNavigationController : public CBase
{
	DECLARE_SINGLETON(CNavigationController)

private:
	CNavigationController();
	virtual ~CNavigationController() = default;

public:
	_uint	Get_CellSize() { return (_uint)m_Cells.size(); }

	_uint	Get_SelectCellIndex() { return m_iSelectCellIndex; }
	void	Set_SelectCell(_uint iIndex) { m_iSelectCellIndex = iIndex; }


public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	HRESULT Render();

public:
	void Add_Point(_fvector vPos);
	HRESULT Add_Cell(_float3* vPoints);

	void Sort_Cell(_float3* vPoints);

	CCell* Get_SelectCell();
	CCell* Get_Cell(_uint iIndex);

	void Erase_Back();
	void Erase_Select();
	void Erase_All();

	void Change_SelectCell(_float3* vPoint);

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };

	class CShader* m_pShader = { nullptr };

	vector<class CCell*>	m_Cells;
	vector<_float3>			m_vClickPoints;

	_uint					m_iSelectCellIndex = {};

	_float3					m_vClickPos = {};
	_float					m_fClosetDistance = {};

private:
	_float3 Find_ClosetPoint(_fvector vPos);

public:
	virtual void Free() override;
};

END

