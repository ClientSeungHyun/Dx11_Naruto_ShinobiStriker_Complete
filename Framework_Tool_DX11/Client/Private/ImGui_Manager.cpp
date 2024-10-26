#include "stdafx.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include "ImGui_Manager.h"
#include "Layer.h"
#include "TerrainController.h"
#include "ModelController.h"
#include "NavigationController.h"

#include "Terrain.h"
#include "NonAnimModel.h"

#include "Cell.h"

IMPLEMENT_SINGLETON(CImGui_Manager)

string ToString(wstring value)
{
	string temp;
	temp.assign(value.begin(), value.end());
	return temp;
}

_tchar* StringToTCHAR(string& str)
{
	size_t cn;
	const _char* szChar = str.c_str();
	_uint iStrLength = 1 + (_uint)strlen(szChar);
	_tchar* sztChar = new _tchar[iStrLength];
	if (NULL == sztChar) throw std::bad_alloc();
	mbstowcs_s(&cn, sztChar, iStrLength, szChar, iStrLength); //sztChar, szChar, iStrLength);
	return (_tchar*)sztChar;
}

string TCHARToString(const _tchar* pTChar)
{
	size_t cn;
	_uint iStrLength = (_uint)wcslen((_tchar*)pTChar);
	_char* szChar = new _char[2 * iStrLength + 1];
	wcstombs_s(&cn, szChar, 2 * iStrLength + 1, (_tchar*)pTChar, 2 * iStrLength + 1);
	string strResult = szChar;
	//Safe_Delete_Array(szChar);

	return strResult;
}

CImGui_Manager::CImGui_Manager()
	: m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CImGui_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(m_pDevice, m_pContext);

	string strLayerTag = "Layer_Monster";
	m_LayerTags.push_back(strLayerTag);

	strLayerTag = "Layer_Building";
	m_LayerTags.push_back(strLayerTag);

	strLayerTag = "Layer_Terrain";
	m_LayerTags.push_back(strLayerTag);

	strLayerTag = "Layer_BackGround";
	m_LayerTags.push_back(strLayerTag);


	string strProtoTypeTag = "Prototype_GameObject_Monster";
	m_ProtoTypeTags.push_back(strProtoTypeTag);

	m_pTerrainController = CTerrainController::Get_Instance();
	if (nullptr == m_pTerrainController)
		return E_FAIL;
	else
		m_pTerrainController->Initialize();

	m_pModelController = CModelController::Get_Instance();
	if (nullptr == m_pModelController)
		return E_FAIL;
	else
		m_pModelController->Initialize();

	m_pNavigationController = CNavigationController::Get_Instance();
	if (nullptr == m_pNavigationController)
		return E_FAIL;
	else
		m_pNavigationController->Initialize(m_pDevice, m_pContext);

	return S_OK;
}

void CImGui_Manager::Render_ImGui()
{
	// 다른 함수에서 분리돼서 호출하면 에러가 남
	// 그래서 구조가 좀 다름

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// 업데이트 내에서 수치 조정할 것들을 처리해 줄 것임
	Update_ImGui();

	ImGui::Render();

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void CImGui_Manager::Update_ImGui()
{
	ImGui::Begin("TOOL PROJECT", NULL, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_HorizontalScrollbar
		| ImGuiWindowFlags_AlwaysVerticalScrollbar);

	// 위젯 안에 있는지 판단함
	_bool isInUI = false;

	ImVec2 window_pos = ImGui::GetWindowPos();
	ImVec2 window_size = ImGui::GetWindowSize();

	ImVec2 mouse_pos = ImGui::GetMousePos();
	if (mouse_pos.x >= window_pos.x && mouse_pos.x <= (window_pos.x + window_size.x) &&
		mouse_pos.y >= window_pos.y && mouse_pos.y <= (window_pos.y + window_size.y)) 
	{
		isInUI = true;
	}

	m_pGameInstance->Set_IsInUI(isInUI);

	if (m_isPickingModel || m_isPickingNavagition)
	{
		CLayer* pNonAnimLayer = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_GameObject_NonAnimModel"));

		if (nullptr != pNonAnimLayer)
		{
			for (auto& NonAnimModel : pNonAnimLayer->Get_ObjectList())
			{
				m_pGameInstance->Add_PickingList(NonAnimModel);
			}
		}
	}

	if (m_isPickingTerrain || m_isPickingNavagition)
	{
		CLayer* pNonAnimLayer = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"));

		if (nullptr != pNonAnimLayer)
		{
			for (auto& NonAnimModel : pNonAnimLayer->Get_ObjectList())
			{
				m_pGameInstance->Add_PickingList(NonAnimModel);
			}
		}
	}

	if (ImGui::Button("Save_Models"))
	{
		if (FAILED(Save_Models()) )
		{
			MSG_BOX(TEXT("모델 저장 실패"));
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Save_Terrain"))
	{
	}
	ImGui::SameLine();
	if (ImGui::Button("Save_Navigation"))
	{
		if (FAILED(Save_Navigation()))
		{
			MSG_BOX(TEXT("네비게이션 저장 실패"));
		}
	}

	if (ImGui::Button("Load_Models"))
	{
		if (FAILED(Load_Models()))
		{
			MSG_BOX(TEXT("모델 불러오기 실패"));
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Load_Terrain"))
	{
	}
	ImGui::SameLine();
	if (ImGui::Button("Load_Navigation"))
	{
		if (FAILED(Load_Navigation()))
		{
			MSG_BOX(TEXT("네비게이션 불러오기 실패"));
		}
	}

	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
	{
		if (ImGui::BeginTabItem("Terrain & Model Tool"))
		{

#pragma region Terrain
			ImGui::CollapsingHeader("Terrain");

			Setting_Terrain();

			ImGui_Spacing(7);
#pragma endregion

#pragma region Model
			ImGui::CollapsingHeader("Model");

			// 이 체크박스 누르고 터레인 클릭하면 그 위치로 포지션 바꾸기
			ImGui::Text("Picking_TerrainPos");
			ImGui::SameLine();
			ImGui::Checkbox("##Picking_TerrainPos", &m_isPickingTerrain);

			ImGui::Text("Picking_Model");
			ImGui::SameLine();
			ImGui::Checkbox("##Picking_Model", &m_isPickingModel);

			if (ImGui::BeginTabBar("Models", ImGuiTabBarFlags_None))
			{
				if (ImGui::BeginTabItem("Model List"))
				{
					Show_AllModel();
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Fix Models"))
				{
					Fix_Models();
					ImGui::EndTabItem();
				}

				ImGui::EndTabBar();

				if (m_pGameInstance->Get_KeyState(KEY::LBUTTON) == KEY_STATE::AWAY)
				{
					m_pGameInstance->Is_Picking();
				}
			}

			ImGui::EndTabItem();
#pragma endregion

		}

		if (ImGui::BeginTabItem("Navigation Tool"))
		{
			ImGui::CollapsingHeader("Navigation");

			Making_Navigation();

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
}

void CImGui_Manager::Setting_Terrain()
{

	CTerrainController::TERRAIN_DESC TerrainDesc = m_pTerrainController->Get_TerrainDesc();
	
	static _int iNumVertice[2] = { _int(TerrainDesc.VIBuffer_Terrain_Desc.iNumVerticesX), _int(TerrainDesc.VIBuffer_Terrain_Desc.iNumVerticesZ) };
	ImGui::Text("NumX / NumZ"); ImGui::SameLine();
	ImGui::DragInt2("##1", iNumVertice, 1);
	TerrainDesc.VIBuffer_Terrain_Desc.iNumVerticesX = iNumVertice[0];
	TerrainDesc.VIBuffer_Terrain_Desc.iNumVerticesZ = iNumVertice[1];

	ImGui::Text("Position X / Y / Z"); 
	ImGui::SameLine();
	ImGui::DragFloat3("##Position", (_float*)&TerrainDesc.VIBuffer_Terrain_Desc);
	
	if (ImGui::Button("Update"))
	{
		if (nullptr != m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_BackGround")))
		{
			m_pGameInstance->Event_DestoryObject(m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), 0));
			m_pTerrainController->Set_Terrain(nullptr);
		}

		CTerrainController::TERRAIN_DESC TerrainDesc = m_pTerrainController->Get_TerrainDesc();

		m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_Terrain"), &TerrainDesc);

		CTerrain* pTerrain = dynamic_cast<CTerrain*>(m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), 0));
		m_pTerrainController->Set_Terrain(pTerrain);
	}
	
	ImGui::SameLine();

	if (ImGui::Button("Delete"))
	{
		if (nullptr != m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_BackGround")))
		{
			m_pGameInstance->Event_DestoryObject(m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), 0));
			m_pTerrainController->Set_Terrain(nullptr);
		}
	}

	m_pTerrainController->Set_TerrainDesc(&TerrainDesc);
}

void CImGui_Manager::Show_AllModel()
{
	static int iSelectModel = 0;

	ImGui::BeginChild("All List", ImVec2(200, 0), true);

	int i = 0;
	if (CModelController::Get_Instance()->Get_PrototypeTags().size() != 0)
	{
		for (auto& iter : CModelController::Get_Instance()->Get_PrototypeTags())
		{
			_tchar szList[MAX_PATH];
			wcsncpy_s(szList, iter.c_str(), MAX_PATH - 1);

			_char pList[MAX_PATH];
			WideCharToMultiByte(CP_ACP, 0, szList, MAX_PATH, pList, MAX_PATH, nullptr, nullptr);

			if (ImGui::Selectable(pList, iSelectModel == i))
				iSelectModel = i;
			i++;
		}
	}

	ImGui::EndChild();
	ImGui::SameLine();


	ImGui::BeginGroup();
	ImGui::BeginChild("Model Options", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); 
	char szLayertag[MAX_PATH] = "";

	if (ImGui::BeginTabBar("Tabs", ImGuiTabBarFlags_None))
	{
		if (ImGui::BeginTabItem("Setting"))
		{
			// LayerTag 세팅하기
			ImGui::BulletText("Select LayerTag");

			if (ImGui::BeginCombo("LayerTag List", m_LayerTags[m_iLayerTagIndex].c_str()))
			{
				for (int i = 0; i < m_LayerTags.size(); i++)
				{
					if (ImGui::Selectable(m_LayerTags[i].c_str(), m_iLayerTagIndex == i))
						m_iLayerTagIndex = i;
				}
				ImGui::EndCombo();
			}

			if (ImGui::Button("Add LayerTag"))
			{
				ImGui::OpenPopup("Add LayerTag");
			}

			// PrototypeTag 세팅하기
			ImGui::BulletText("Select PrototypeTag");

			if (ImGui::BeginCombo("PrototypeTag List", m_ProtoTypeTags[m_iPrototypeTagIndex].c_str()))
			{
				for (int i = 0; i < m_ProtoTypeTags.size(); i++)
				{
					if (ImGui::Selectable(m_ProtoTypeTags[i].c_str(), m_iPrototypeTagIndex == i))
						m_iPrototypeTagIndex = i;
				}
				ImGui::EndCombo();
			}

			if (ImGui::Button("Add PrototypeTag"))
			{
				ImGui::OpenPopup("Add PrototypeTag");
			}
			Show_PopUp();

			ImGui::EndTabItem();

			// 모델 위치, 오프셋, 크기 등 설정하기
			ImGui::NewLine();
			ImGui::BulletText("Setting Transform");

			ImGui::Text("Position"); ImGui::SameLine();
			ImGui::DragFloat3("##Position", (_float*)&m_vModelPos);

			static _float fScale[3] = { 1.f , 1.f, 1.f };
			ImGui::Text("Scale"); ImGui::SameLine();
			ImGui::DragFloat3("##Scale", fScale);

			static _float fRotation[3] = { 0.f , 0.f, 0.f };
			ImGui::Text("Rotation"); ImGui::SameLine();
			ImGui::DragFloat3("##Rotation", fRotation);

			if (ImGui::Button("Add Model"))
			{
				CNonAnimModel::NONMODEL_DESC Desc = {};
				ZeroMemory(&Desc, sizeof(Desc));

				// 저장용으로 char로 저장한 구조체
				strncpy_s(Desc.szLayerTag, m_LayerTags[m_iLayerTagIndex].c_str(), sizeof(Desc.szLayerTag) - 1);
				strncpy_s(Desc.szPrototypeTag, m_ProtoTypeTags[m_iPrototypeTagIndex].c_str(), sizeof(Desc.szPrototypeTag) - 1);
				WideCharToMultiByte(CP_ACP, 0, CModelController::Get_Instance()->Get_PrtotypeTagIndex(iSelectModel).c_str(), MAX_PATH, Desc.szModelTag, MAX_PATH, nullptr, nullptr);
				Desc.vPosition = _float3(m_vModelPos.x, m_vModelPos.y, m_vModelPos.z);
				Desc.vScale = _float3(fScale[0], fScale[1], fScale[2]);

				Desc.isCollision = true;
				m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, "Layer_GameObject_NonAnimModel", Desc.szPrototypeTag, &Desc);
			}
		}
		ImGui::EndTabBar();
	}

	ImGui::EndChild();
	ImGui::EndGroup();


}

void CImGui_Manager::Fix_Models()
{
	static int iSelectCurretnModel = 0;

	ImGui::BeginChild("Current List", ImVec2(200, 0), true);

	int i = 0;
	if (m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_GameObject_NonAnimModel")) != nullptr)
	{
		for (auto& iter : m_pGameInstance->Get_ObjectList(LEVEL_GAMEPLAY, TEXT("Layer_GameObject_NonAnimModel")))
		{
			CNonAnimModel* pNonAnimModel = dynamic_cast<CNonAnimModel*>(iter);

			if (ImGui::Selectable(pNonAnimModel->Get_NonAniModelDesc().szModelTag, iSelectCurretnModel == i))
			{
				iSelectCurretnModel = i;
			}
			i++;
		}
	}

	ImGui::EndChild();
	ImGui::SameLine();

	CGameObject* pSelectObject = m_pGameInstance->Get_PickedObject();
	if (m_pSelectedObject != pSelectObject)
	{

	}

	m_pSelectedObject = m_pGameInstance->Get_PickedObject();
	

	if (m_pSelectedObject == nullptr)
		return;

	ImGui::BeginGroup();
	ImGui::BeginChild("Model Options", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); 
	char szLayertag[MAX_PATH] = "";

	if (ImGui::BeginTabBar("Tabs", ImGuiTabBarFlags_None))
	{
		if (ImGui::BeginTabItem("Setting"))
		{
			CNonAnimModel* pNonAnimModel = dynamic_cast<CNonAnimModel*>(m_pSelectedObject);

			Safe_AddRef(m_pSelectedObject);
			Safe_AddRef(pNonAnimModel);

			_bool isCollision = pNonAnimModel->Get_NonAniModelDesc().isCollision;
			ImGui::Text("CollisionObject");
			ImGui::SameLine();
			ImGui::Checkbox("##Collision_Object", &isCollision);

			CNonAnimModel::NONMODEL_DESC pDesc;
			memcpy(&pDesc, &pNonAnimModel->Get_NonAniModelDesc(), sizeof(pDesc));

			pDesc.isCollision = isCollision;
			pNonAnimModel->Set_NonAniModelDesc(&pDesc);



			// LayerTag 세팅하기
			ImGui::BulletText("Select LayerTag");

			if (nullptr != m_pSelectedObject)
			{
				_char* szLayer = pNonAnimModel->Get_NonAniModelDesc().szLayerTag;
				auto iter = find(m_LayerTags.begin(), m_LayerTags.end(), szLayer);
				if (iter != m_LayerTags.end())
				{
					m_iLayerTagIndex = _uint(iter - m_LayerTags.begin());
					ImGui::Text("Current LayerTag : "); ImGui::SameLine();
					ImGui::Text(m_LayerTags[m_iLayerTagIndex].c_str());
				}
			}

			if (ImGui::BeginCombo("LayerTag List", m_LayerTags[m_iPrototypeTagIndex].c_str()))
			{
				for (int i = 0; i < m_LayerTags.size(); i++)
				{
					if (ImGui::Selectable(m_LayerTags[i].c_str(), m_iLayerTagIndex == i))
					{
						m_iLayerTagIndex = i;

						CNonAnimModel::NONMODEL_DESC pDesc;
						memcpy(&pDesc, &pNonAnimModel->Get_NonAniModelDesc(), sizeof(pDesc));
						strcpy_s(pDesc.szLayerTag, MAX_PATH, m_LayerTags[i].c_str());

						pNonAnimModel->Set_NonAniModelDesc(&pDesc);
					}

				}
				ImGui::EndCombo();
			}

			if (ImGui::Button("Add LayerTag"))
			{
				ImGui::OpenPopup("Add LayerTag");
			}

			ImGui_Spacing(5);

			// PrototypeTag 세팅하기
			ImGui::BulletText("Select PrototypeTag");

			if (nullptr != m_pSelectedObject)
			{
				_char* szPrototype = pNonAnimModel->Get_NonAniModelDesc().szPrototypeTag;
				auto iter = find(m_ProtoTypeTags.begin(), m_ProtoTypeTags.end(), szPrototype);
				if (iter != m_ProtoTypeTags.end())
				{
					m_iPrototypeTagIndex = _uint(iter - m_ProtoTypeTags.begin());
					ImGui::Text("Current PrototypeTag : "); ImGui::SameLine();
					ImGui::Text(m_ProtoTypeTags[m_iPrototypeTagIndex].c_str());
				}
			}

			if (ImGui::BeginCombo("PrototypeTag List", m_ProtoTypeTags[m_iPrototypeTagIndex].c_str()))
			{
				for (int i = 0; i < m_ProtoTypeTags.size(); i++)
				{
					if (ImGui::Selectable(m_ProtoTypeTags[i].c_str(), m_iPrototypeTagIndex == i))
					{
						m_iPrototypeTagIndex = i;

						CNonAnimModel::NONMODEL_DESC pDesc;
						memcpy(&pDesc, &pNonAnimModel->Get_NonAniModelDesc(), sizeof(pDesc));
						strcpy_s(pDesc.szPrototypeTag, MAX_PATH, m_ProtoTypeTags[i].c_str());
						
						pNonAnimModel->Set_NonAniModelDesc(&pDesc);
					}
				}
				ImGui::EndCombo();
			}

			if (ImGui::Button("Add PrototypeTag"))
			{
				ImGui::OpenPopup("Add PrototypeTag");
			}
			Show_PopUp();

			ImGui::EndTabItem();

			// 모델 위치, 오프셋, 크기 등 설정하기
			ImGui::NewLine();
			ImGui::BulletText("Setting Transform");

			static _float3 vPosition = _float3(0.f, 0.f, 0.f);
			if (nullptr != m_pSelectedObject)
				XMStoreFloat3(&vPosition,  m_pSelectedObject->Get_Position());

			ImGui::Text("Position"); ImGui::SameLine();
			ImGui::DragFloat3("##Position", (_float*)&vPosition);

			static _float3 vScale = _float3(1.f, 1.f, 1.f);
			if (nullptr != m_pSelectedObject)
				vScale = m_pSelectedObject->Get_Transform()->Get_Scaled();

			ImGui::Text("Scale"); ImGui::SameLine();
			ImGui::DragFloat3("##Scale", (_float*)&vScale);

			static _float3 vRotation = _float3(0.f, 0.f, 0.f);
			ImGui::Text("Rotation"); ImGui::SameLine();
			ImGui::DragFloat3("##Rotation", (_float*)&vRotation);

			if (ImGui::Button("Delete"))
			{
				m_pGameInstance->Event_DestoryObject(m_pSelectedObject);
				m_pSelectedObject = nullptr;
			}


			if (nullptr != pNonAnimModel)
			{
				CNonAnimModel::NONMODEL_DESC pDesc;
				memcpy(&pDesc, &pNonAnimModel->Get_NonAniModelDesc(), sizeof(pDesc));

				pDesc.vPosition = vPosition;
				pNonAnimModel->Get_Transform()->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&vPosition));

				for (_uint i = 0; i < 3; ++i)
				{
					if (*((_float*)(&vScale) + i) <= 0)
					{
						*((_float*)(&vScale) + i) = 1.f;
					}
				}
				pDesc.vScale = vScale;

				pDesc.vRotation = vRotation;
				pNonAnimModel->Get_Transform()->Set_Scaled(vScale.x, vScale.y, vScale.z);
				pNonAnimModel->Get_Transform()->Rotation(XMConvertToRadians(vRotation.x), XMConvertToRadians(vRotation.y), XMConvertToRadians(vRotation.z));
				pNonAnimModel->Set_NonAniModelDesc(&pDesc);
			}


			Safe_Release(pNonAnimModel);
		}

		ImGui::EndTabBar();
	}

	ImGui::EndChild();
	ImGui::EndGroup();

	Safe_Release(m_pSelectedObject);
}

void CImGui_Manager::Show_PopUp()
{
	if (ImGui::BeginPopupModal("Add LayerTag", NULL, ImGuiWindowFlags_MenuBar))
	{
		static char LayerBuffer[MAX_PATH] = "";

		ImGui::Text("Enter Layer Tag");
		ImGui::InputText("Layer Tag", LayerBuffer, MAX_PATH);

		if (ImGui::Button("Add Layer Tag"))
		{
			bool bFailed = false;

			if (m_LayerTags.size() != 0)
			{
				for (auto& iter : m_LayerTags)
				{
					if (!strcmp("", LayerBuffer) || !strcmp(iter.c_str(), LayerBuffer))
					{
						bFailed = true;
						ImGui::OpenPopup("Failed");
					}

				}
			}
			if (!bFailed)
			{
				string NewLayer = LayerBuffer;
				m_LayerTags.push_back(NewLayer);
				ImGui::OpenPopup("Succese");
			}

		}

		ImGui::SameLine();
		if (ImGui::Button("Close"))  ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}

	if (ImGui::BeginPopupModal("Add PrototypeTag", NULL, ImGuiWindowFlags_MenuBar))
	{
		static char PrototypeBuffer[MAX_PATH] = "";

		ImGui::Text("Enter Prototype Tag");
		ImGui::InputText("Prototype Tag", PrototypeBuffer, MAX_PATH);

		if (ImGui::Button("Add Prototype Tag"))
		{
			bool bFailed = false;

			if (m_ProtoTypeTags.size() != 0)
			{
				for (auto& iter : m_ProtoTypeTags)
				{
					if (!strcmp("", PrototypeBuffer) || !strcmp(iter.c_str(), PrototypeBuffer))
					{
						bFailed = true;
						ImGui::OpenPopup("Failed");
					}

				}
			}
			if (!bFailed)
			{
				string NewLayer = PrototypeBuffer;
				m_ProtoTypeTags.push_back(NewLayer);
				ImGui::OpenPopup("Succese");
			}

		}

		ImGui::SameLine();
		if (ImGui::Button("Close"))  ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}
}

HRESULT CImGui_Manager::Save_Models()
{
	OPENFILENAME OFN;
	_tchar szLpstrFile[256] = TEXT("파일이름");
	_tchar szLpstrInitialDir[256] = TEXT("./");
	_tchar filter[] = L"데이터\0*.dat\0텍스트\0*.txt";

	memset(&OFN, 0, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner = g_hWnd;
	OFN.lpstrInitialDir = szLpstrInitialDir;
	OFN.lpstrFilter = filter;
	OFN.lpstrFile = szLpstrFile;
	OFN.nMaxFile = 256;
	OFN.Flags = OFN_SHOWHELP | OFN_OVERWRITEPROMPT;

	if (GetSaveFileName(&OFN) != 0)
	{
		HANDLE hFile = 0;
		_ulong dwByte = 0;


		hFile = CreateFile(OFN.lpstrFile, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);;
		if (0 == hFile)
		{
			CloseHandle(hFile);
			return E_FAIL;
		}

		CNonAnimModel::NONMODEL_DESC  NonAnimModelDesc;

		_tchar* szLayerTag = TEXT("Layer_GameObject_NonAnimModel");
		if (nullptr != m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, szLayerTag))
		{
			list<CGameObject*>& ObjectList = m_pGameInstance->Get_ObjectList(LEVEL_GAMEPLAY, szLayerTag);

			_int iObjectListSize = (_int)ObjectList.size();

			WriteFile(hFile, &(iObjectListSize), sizeof(_int), &dwByte, nullptr);

			for (auto& iter : ObjectList)
			{
				if (iter == nullptr)
					continue;

				CNonAnimModel* pNonAnimModel = dynamic_cast<CNonAnimModel*>(iter);
				if (nullptr != pNonAnimModel)
				{
					NonAnimModelDesc = pNonAnimModel->Get_NonAniModelDesc();
					WriteFile(hFile, &NonAnimModelDesc, sizeof(CNonAnimModel::NONMODEL_DESC), &dwByte, nullptr);
				}
			}

		}
		CloseHandle(hFile);
	}

	return S_OK;
}

HRESULT CImGui_Manager::Load_Models()
{
	OPENFILENAME OFN;
	_tchar szLpstrFile[256] = TEXT("파일이름");
	_tchar szLpstrInitialDir[256] = TEXT("./");
	_tchar filter[] = L"모든 파일\0*.*\0텍스트 파일\0*.txt\0fbx 파일\0*.fbx";

	memset(&OFN, 0, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner = g_hWnd;
	OFN.lpstrInitialDir = szLpstrInitialDir;
	OFN.lpstrFilter = filter;
	OFN.lpstrFile = szLpstrFile;
	OFN.nMaxFile = 256;
	OFN.Flags = OFN_SHOWHELP | OFN_OVERWRITEPROMPT;

	if (GetOpenFileName(&OFN) != 0)
	{

		HANDLE hFile = 0;
		_ulong dwByte = 0;

		CNonAnimModel::NONMODEL_DESC  NonAnimModelDesc;
		_int iNum;

		hFile = CreateFile(OFN.lpstrFile, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (0 == hFile)
		{
			CloseHandle(hFile);
			return E_FAIL;
		}

		ReadFile(hFile, &(iNum), sizeof(_int), &dwByte, nullptr);

		for (size_t i = 0; i < iNum; ++i)
		{
			ReadFile(hFile, &NonAnimModelDesc, sizeof(CNonAnimModel::NONMODEL_DESC), &dwByte, nullptr);

			m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, "Layer_GameObject_NonAnimModel", NonAnimModelDesc.szPrototypeTag, &NonAnimModelDesc);
		}

		CloseHandle(hFile);
	}

	return S_OK;
}

void CImGui_Manager::Making_Navigation()
{
	
	ImGui::Text("Making Navigation");
	ImGui::SameLine();
	ImGui::Checkbox("##Making Navigation", &m_isPickingNavagition);

	static int selected = 0;
	{
		ImGui::BeginChild("Cell List", ImVec2(200, 0), true);

		if (m_pNavigationController->Get_CellSize() != 0)
		{
			for (_uint i = 0; i < m_pNavigationController->Get_CellSize();)
			{
				_char szCell[MAX_PATH] = "Cell ";
				_char szNum[MAX_PATH];
				sprintf_s(szNum, "%d", i);
				strcat_s(szCell, szNum);

				if (ImGui::Selectable(szCell, m_iSelectCellIndex == i))
				{
					m_iSelectCellIndex = i;
				}
				i++;
			}
		}
		ImGui::EndChild();
	}
	ImGui::SameLine();

	ImGui::BeginGroup();
	ImGui::BeginChild("Cell view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us

	ImGui::CollapsingHeader("Show Current Cell");
	ImGui::Text("Cell Index : "); ImGui::SameLine();  ImGui::Text("%d", m_iSelectCellIndex);
	m_pNavigationController->Set_SelectCell(m_iSelectCellIndex);

	CCell* pCurrentCell = m_pNavigationController->Get_SelectCell();

	static _float3 vPoints[3] = { _float3(0.f,0.f,0.f),_float3(0.f,0.f,0.f),_float3(0.f,0.f,0.f) };

	if (pCurrentCell != nullptr)
	{
		vPoints[0].x = XMVectorGetX(pCurrentCell->Get_Point(CCell::POINT_A));
		vPoints[0].y = XMVectorGetY(pCurrentCell->Get_Point(CCell::POINT_A));
		vPoints[0].z = XMVectorGetZ(pCurrentCell->Get_Point(CCell::POINT_A));

		vPoints[1].x = XMVectorGetX(pCurrentCell->Get_Point(CCell::POINT_B));
		vPoints[1].y = XMVectorGetY(pCurrentCell->Get_Point(CCell::POINT_B));
		vPoints[1].z = XMVectorGetZ(pCurrentCell->Get_Point(CCell::POINT_B));

		vPoints[2].x = XMVectorGetX(pCurrentCell->Get_Point(CCell::POINT_C));
		vPoints[2].y = XMVectorGetY(pCurrentCell->Get_Point(CCell::POINT_C));
		vPoints[2].z = XMVectorGetZ(pCurrentCell->Get_Point(CCell::POINT_C));
	}

	ImGui::Text("PointA :"); ImGui::SameLine(); ImGui::DragFloat3("##PointA",(_float*)&vPoints[0]);
	ImGui::Text("PointB :"); ImGui::SameLine(); ImGui::DragFloat3("##PointB", (_float*)&vPoints[1]);
	ImGui::Text("PointC :"); ImGui::SameLine(); ImGui::DragFloat3("##PointC", (_float*)&vPoints[2]);

	if (pCurrentCell != nullptr)
	{
		if (!XMVector3Equal(pCurrentCell->Get_Point(CCell::POINT_A), XMLoadFloat3(&vPoints[0]))
			|| !XMVector3Equal(pCurrentCell->Get_Point(CCell::POINT_B), XMLoadFloat3(&vPoints[1]))
			|| !XMVector3Equal(pCurrentCell->Get_Point(CCell::POINT_C), XMLoadFloat3(&vPoints[2])))
			m_pNavigationController->Change_SelectCell(vPoints);
	}


	if (ImGui::Button("Erase Recent Cell"))
	{
		m_pNavigationController->Erase_Back();
		m_iSelectCellIndex = 0;
	}

	if (ImGui::Button("Erase Select Cell"))
		m_pNavigationController->Erase_Select();

	if (ImGui::Button("Erase All Cell"))
		m_pNavigationController->Erase_All();

	ImGui::EndChild();
	ImGui::EndGroup();

	if (m_pGameInstance->Get_KeyState(KEY::Z) == KEY_STATE::AWAY)
	{
		if (true == m_pGameInstance->Is_Picking())
		{
			_float3 fPosition = m_pGameInstance->Get_PickingPos();
			_vector vPosition = XMLoadFloat3(&fPosition);
			vPosition = XMVectorSetW(vPosition, 1.f);
			m_pNavigationController->Add_Point(vPosition);
			//m_fClickPoint = m_pNavigationController->Get_ClickedPos();
		}

	}

	m_pNavigationController->Render();
}

HRESULT CImGui_Manager::Save_Navigation()
{
	OPENFILENAME OFN;
	_tchar szLpstrFile[256] = TEXT("파일이름");
	_tchar szLpstrInitialDir[256] = TEXT("./");
	_tchar filter[] = L"데이터\0*.dat\0텍스트\0*.txt";

	memset(&OFN, 0, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner = g_hWnd;
	OFN.lpstrInitialDir = szLpstrInitialDir;
	OFN.lpstrFilter = filter;
	OFN.lpstrFile = szLpstrFile;
	OFN.nMaxFile = 256;
	OFN.Flags = OFN_SHOWHELP | OFN_OVERWRITEPROMPT;

	if (GetSaveFileName(&OFN) != 0)
	{
		HANDLE hFile = 0;
		_ulong dwByte = 0;


		hFile = CreateFile(OFN.lpstrFile, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);;
		if (0 == hFile)
		{
			CloseHandle(hFile);
			return E_FAIL;
		}

		_uint iCellSize = m_pNavigationController->Get_CellSize();
		WriteFile(hFile, &(iCellSize), sizeof(_uint), &dwByte, nullptr);

		_float3	 vPoints[3];
		for (_uint i = 0; i < iCellSize; ++i)
		{
			CCell* pCell = m_pNavigationController->Get_Cell(i);

			XMStoreFloat3(&vPoints[0], pCell->Get_Point(CCell::POINT_A));
			XMStoreFloat3(&vPoints[1], pCell->Get_Point(CCell::POINT_B));
			XMStoreFloat3(&vPoints[2], pCell->Get_Point(CCell::POINT_C));

			WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
		}

		CloseHandle(hFile);
	}
}

HRESULT CImGui_Manager::Load_Navigation()
{
	OPENFILENAME OFN;
	_tchar szLpstrFile[256] = TEXT("파일이름");
	_tchar szLpstrInitialDir[256] = TEXT("./");
	_tchar filter[] = L"모든 파일\0*.*\0텍스트 파일\0*.txt\0fbx 파일\0*.fbx";

	memset(&OFN, 0, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner = g_hWnd;
	OFN.lpstrInitialDir = szLpstrInitialDir;
	OFN.lpstrFilter = filter;
	OFN.lpstrFile = szLpstrFile;
	OFN.nMaxFile = 256;
	OFN.Flags = OFN_SHOWHELP | OFN_OVERWRITEPROMPT;

	if (GetOpenFileName(&OFN) != 0)
	{

		HANDLE hFile = 0;
		_ulong dwByte = 0;

		hFile = CreateFile(OFN.lpstrFile, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (0 == hFile)
		{
			CloseHandle(hFile);
			return E_FAIL;
		}

		_uint iCellSize = {};
		ReadFile(hFile, &(iCellSize), sizeof(_uint), &dwByte, nullptr);

		_float3	 vPoints[3];
		for (_uint i = 0; i < iCellSize; ++i)
		{
			ReadFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

			m_pNavigationController->Add_Cell(vPoints);
		}

		CloseHandle(hFile);
	}

	return S_OK;
}

void CImGui_Manager::ImGui_Spacing(_uint iSpaceCount)
{
	for (_uint i = 0; i < iSpaceCount; ++i)
	{
		ImGui::Spacing();
	}
}


void CImGui_Manager::Free()
{
	__super::Free();

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pTerrainController);
	Safe_Release(m_pModelController);

	Safe_Release(m_pGameInstance);
}