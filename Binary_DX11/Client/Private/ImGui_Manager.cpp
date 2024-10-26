#include "stdafx.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include "ImGui_Manager.h"
#include "Layer.h"
#include "TerrainController.h"
#include "ModelController.h"

#include "Terrain.h"
#include "NonAnimModel.h"

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

	m_pModelController = CModelController::Get_Instance();
	if (nullptr == m_pTerrainController)
		return E_FAIL;

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



	if (m_isPickingModel)
	{
		for (auto& LayerTag : m_LayerTags)
		{
			_tchar* szLayerTag = StringToTCHAR(LayerTag);
			CLayer* pNonAnimLayer = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, szLayerTag);

			if (nullptr != pNonAnimLayer)
			{
				for (auto& NonAnimModel : pNonAnimLayer->Get_ObjectList())
				{
					m_pGameInstance->Add_ModelPickingList(NonAnimModel);
				}
			}

			Safe_Delete(szLayerTag);
		}
	}

	if (ImGui::Button("Save_Models"))
	{
		Save_Models();
	}
	ImGui::SameLine();
	if (ImGui::Button("Save_Terrain"))
	{
	}

	if (ImGui::Button("Load_Models"))
	{
		Load_Models();
	}
	ImGui::SameLine();
	if (ImGui::Button("Load_Terrain"))
	{
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
			}

			ImGui::EndTabItem();
#pragma endregion

			ImGui::EndTabBar();
		}
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
			_char szList[MAX_PATH];
			WideCharToMultiByte(CP_ACP, 0, iter, MAX_PATH, szList, MAX_PATH, nullptr, nullptr);

			if (ImGui::Selectable(szList, iSelectModel == i))
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
				WideCharToMultiByte(CP_ACP, 0, CModelController::Get_Instance()->Get_PrtotypeTagIndex(iSelectModel), MAX_PATH, Desc.szModelTag, MAX_PATH, nullptr, nullptr);
				Desc.vPosition = _float3(m_vModelPos.x, m_vModelPos.y, m_vModelPos.z);
				Desc.vScale = _float3(fScale[0], fScale[1], fScale[2]);

				m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, Desc.szLayerTag, Desc.szPrototypeTag, &Desc);
			}
		}
		ImGui::EndTabBar();
	}

	ImGui::EndChild();
	ImGui::EndGroup();


}

void CImGui_Manager::Fix_Models()
{
	static int iSelectModel = 0;

	m_pSelectedObject = m_pGameInstance->Get_ModelPickedObject();

	CNonAnimModel* pNonAnimModel = dynamic_cast<CNonAnimModel*>(m_pSelectedObject);

	Safe_AddRef(m_pSelectedObject);
	Safe_AddRef(pNonAnimModel);

	ImGui::BeginGroup();
	ImGui::BeginChild("Model Options", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
	char szLayertag[MAX_PATH] = "";

	if (ImGui::BeginTabBar("Tabs", ImGuiTabBarFlags_None))
	{
		if (ImGui::BeginTabItem("Setting"))
		{
			// LayerTag 세팅하기
			ImGui::BulletText("Select LayerTag");

			if (nullptr != m_pSelectedObject)
			{
				_char* szLayer = pNonAnimModel->Get_NonAniModelDesc().szLayerTag;
				auto iter = find(m_LayerTags.begin(), m_LayerTags.end(), szLayer);
				if (iter != m_LayerTags.end())
				{
					m_iLayerTagIndex = iter - m_LayerTags.begin();
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


			if (nullptr != m_pSelectedObject)
			{
				m_pSelectedObject->Get_Transform()->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&vPosition));

				for (_uint i = 0; i < 3; ++i)
				{
					if (*((_float*)(&vScale) + i) <= 0)
					{
						*((_float*)(&vScale) + i) = 1.f;
					}
				}
				m_pSelectedObject->Get_Transform()->Set_Scaled(vScale.x, vScale.y, vScale.z);
			}
		}
		ImGui::EndTabBar();
	}

	ImGui::EndChild();
	ImGui::EndGroup();

	Safe_Release(pNonAnimModel);
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

void CImGui_Manager::ImGui_Spacing(_uint iSpaceCount)
{
	for (_uint i = 0; i < iSpaceCount; ++i)
	{
		ImGui::Spacing();
	}
}

void CImGui_Manager::Save_Models()
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

		CNonAnimModel::NONMODEL_DESC  NonAnimModelDesc;
		_int iNum = 0;

		_tchar* szLayerTag = StringToTCHAR(m_LayerTags[0]);
		list<CGameObject*>& ObjectList = m_pGameInstance->Get_ObjectList(LEVEL_GAMEPLAY, szLayerTag);

		iNum = (_int)ObjectList.size();

		hFile = CreateFile(OFN.lpstrFile, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);;
		if (0 == hFile)
		{
			CloseHandle(hFile);
			return;
		}


		/* 첫줄은 object 리스트의 size 받아서 갯수만큼 for문 돌리게 하려고 저장해놓음*/
		WriteFile(hFile, &(iNum), sizeof(_int), &dwByte, nullptr);

		for (auto& iter : ObjectList)
		{
			if (iter == nullptr)
				continue;

			CNonAnimModel* pNonAnimModel = dynamic_cast<CNonAnimModel*>(iter);
			NonAnimModelDesc = pNonAnimModel->Get_NonAniModelDesc();
			WriteFile(hFile, &NonAnimModelDesc, sizeof(CNonAnimModel::NONMODEL_DESC), &dwByte, nullptr);
		}

		Safe_Delete(szLayerTag);
		CloseHandle(hFile);
	}
}

void CImGui_Manager::Load_Models()
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

		_tchar* szLayerTag = StringToTCHAR(m_LayerTags[0]);
		list<CGameObject*>& ObjectList = m_pGameInstance->Get_ObjectList(LEVEL_GAMEPLAY, szLayerTag);


		hFile = CreateFile(OFN.lpstrFile, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (0 == hFile)
		{
			CloseHandle(hFile);
			return;
		}

		ReadFile(hFile, &(iNum), sizeof(_int), &dwByte, nullptr);

		for (size_t i = 0; i< iNum; ++i)
		{
			ReadFile(hFile, &NonAnimModelDesc, sizeof(CNonAnimModel::NONMODEL_DESC), &dwByte, nullptr);

			m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, NonAnimModelDesc.szLayerTag, NonAnimModelDesc.szPrototypeTag, &NonAnimModelDesc);
		}

		Safe_Delete(szLayerTag);
		CloseHandle(hFile);
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