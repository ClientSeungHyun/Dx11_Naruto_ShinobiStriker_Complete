#pragma once

#include "Client_Defines.h"
#include "Base.h"


#include "GameInstance.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)
class CImGui_Manager : public CBase
{
	DECLARE_SINGLETON(CImGui_Manager)

private:
	CImGui_Manager();
	virtual ~CImGui_Manager() = default;

public:
	_bool				Get_IsPickingTerrain() { return m_isPickingTerrain; }
	void				Set_IsPickingTerrain(_bool isPicking) { m_isPickingTerrain = isPicking; }

	_bool				Get_IsPickingModel() { return m_isPickingModel; }
	void				Set_IsPickingModel(_bool isPicking) { m_isPickingModel = isPicking; }

	_float3				Get_ModelPos() const { return m_vModelPos; }
	void				Set_ModelPos(_float3 vPos) { m_vModelPos = vPos; }

	class CGameObject*	Get_SelectedObject() { return m_pSelectedObject; }
	void				Set_SelectedObject(class CGameObject* pObject) { m_pSelectedObject = pObject; }

public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Render_ImGui();

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

private:
	void Update_ImGui();

private:
#pragma region Terrain

	void Setting_Terrain();

#pragma endregion

#pragma region Model

	void Show_AllModel();
	void Fix_Models();
	void Show_PopUp();

	HRESULT Save_Models();
	HRESULT Load_Models();

#pragma endregion

#pragma region Navigation
	void Making_Navigation();

	HRESULT Save_Navigation();
	HRESULT Load_Navigation();

#pragma endregion


	void ImGui_Spacing(_uint iSpaceCount = 1);


private:
	class CGameInstance*			m_pGameInstance = { nullptr };

	class CTerrainController*		m_pTerrainController = { nullptr };
	class CModelController*			m_pModelController = { nullptr };
	class CNavigationController*	m_pNavigationController = { nullptr };

	// Model
	class CGameObject*		m_pSelectedObject = { nullptr };

	_bool					m_isPickingTerrain = { false };
	_bool					m_isPickingModel = { false };

	_uint					m_iProtytypeIndex_NonAnimModel = { 0 };

	_uint					m_iLayerTagIndex = {};
	vector<string>			m_LayerTags;

	_uint					m_iPrototypeTagIndex = {};
	vector<string>			m_ProtoTypeTags;

	_float3					m_vModelPos = {};


	// Navigation
	_bool					m_isPickingNavagition = { false };
	_int					m_iSelectCellIndex = 0;

public:
	virtual void Free() override;
};

END