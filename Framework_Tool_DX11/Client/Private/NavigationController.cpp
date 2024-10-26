#include "stdafx.h"
#include "NavigationController.h"

#include "GameInstance.h"

IMPLEMENT_SINGLETON(CNavigationController)

CNavigationController::CNavigationController()
	:m_pGameInstance{ CGameInstance::Get_Instance() }
{
}

HRESULT CNavigationController::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;

	Safe_AddRef(pDevice);
	Safe_AddRef(pContext);

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Cell.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	return S_OK;
}

HRESULT CNavigationController::Render()
{
	if (m_Cells.size() == 0)
		return S_OK;

	_float4x4			WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
	WorldMatrix._42 += 0.1f;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;



#ifdef _DEBUG

	for (auto& pCell : m_Cells)
	{
		if (nullptr != pCell)
		{
			if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
				return E_FAIL;

			_float4		vColor = _float4(0.f, 1.f, 0.f, 1.f);

			if(m_Cells[m_iSelectCellIndex] == pCell)
				vColor = _float4(1.f, 0.f, 0.f, 1.f);

			m_pShader->Bind_RawValue("g_vColor", &vColor, sizeof(_float4));

			m_pShader->Begin(0);
			pCell->Render();
		}

	}

#endif // _DEBUG
	return S_OK;
}

void CNavigationController::Add_Point(_fvector vPos)
{
	_float3 vClickPos;

	// 일정 이하 거리에 점이 있으며 그 점 위치로 바꿈
	vClickPos = Find_ClosetPoint(vPos);

	// 거리가 멀면 그냥 그대로 들어가게
	if (m_fClosetDistance > 0.8f)
		XMStoreFloat3(&vClickPos, vPos);

	if (2 < m_vClickPoints.size())
		return;

	//if (0 == m_vClickedPoints.size())
	//	Add_ClickedSymbol(vClickPosition, SYMBOL1);
	//else if (1 == m_vClickedPoints.size())
	//	Add_ClickedSymbol(vClickPosition, SYMBOL2);

	m_vClickPoints.push_back(vClickPos);
	m_vClickPos = vClickPos;

	if (3 == m_vClickPoints.size())
	{
		_float3 vPoints[3];

		for (_uint i = 0; i < 3; ++i)
			vPoints[i] = m_vClickPoints[i];

		Add_Cell(vPoints);

		m_vClickPoints.clear();
	}
}

HRESULT CNavigationController::Add_Cell(_float3* vPoints)
{
	Sort_Cell(vPoints);

	for (_uint i = 0; i < 3; ++i)
	{
		for (_uint j = 0; j < 3; ++j)
		{
			if (fabs(vPoints[i].x - vPoints[j].x) < 0.1)
			{
				if (vPoints[i].x < vPoints[j].x)
					vPoints[j].x = vPoints[i].x;
				else
					vPoints[i].x = vPoints[j].x;
			}

			if (fabs(vPoints[i].y - vPoints[j].y) < 0.1)
			{
				if (vPoints[i].y < vPoints[j].y)
					vPoints[j].y = vPoints[i].y;
				else
					vPoints[i].y = vPoints[j].y;
			}

			if (fabs(vPoints[i].z - vPoints[j].z) < 0.1)
			{
				if (vPoints[i].z < vPoints[j].z)
					vPoints[j].z = vPoints[i].z;
				else
					vPoints[i].z = vPoints[j].z;
			}
		}
	}

	CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, (_int)m_Cells.size());
	if (nullptr == pCell)
		return E_FAIL;

	m_Cells.push_back(pCell);
}

void CNavigationController::Sort_Cell(_float3* vPoints)
{
	_float3 vCurrenPoints[3];
	memcpy(vCurrenPoints, vPoints, sizeof(_float3) * 3);
	
	// 가장 작은 x 찾기
	// 가장 작은 x가 0 중간이 1 큰 값이 2로 정렬됨
	for (_uint i = 0; i < 2; ++i)
	{
		_uint iIndex_MinX = i;
		_bool isSwap = false;
		for (_uint j = i + 1; j < 3; ++j)
		{
			if (vCurrenPoints[iIndex_MinX].x > vCurrenPoints[j].x)
			{
				iIndex_MinX = j;
				isSwap = true;
			}
		}

		if (isSwap)
		{
			_float3 vTemp;
			vTemp = vCurrenPoints[i];
			vCurrenPoints[i] = vCurrenPoints[iIndex_MinX];
			vCurrenPoints[iIndex_MinX] = vTemp;
		}
	}

	// 만약 작은 x의 z값이 중간 x의 z값보다 작으면 반대로 넣어주기
	// 중간 값은 항상 0번째에 너어주면 됨
	vPoints[0] = vCurrenPoints[1];
	if (vCurrenPoints[0].z < vCurrenPoints[1].z)
	{
		vPoints[1] = vCurrenPoints[2];
		vPoints[2] = vCurrenPoints[0];
	}
	else
	{
		vPoints[1] = vCurrenPoints[0];
		vPoints[2] = vCurrenPoints[2];
	}
}

CCell* CNavigationController::Get_SelectCell()
{
	if (m_Cells.size() == 0 || m_iSelectCellIndex >= m_Cells.size())
		return nullptr;

	return m_Cells[m_iSelectCellIndex];
}

CCell* CNavigationController::Get_Cell(_uint iIndex)
{
	if (m_Cells.size() == 0 || iIndex >= m_Cells.size())
		return nullptr;

	return m_Cells[iIndex];
}

void CNavigationController::Erase_Back()
{
	if (m_Cells.size() == 0)
		return;

	Safe_Release(m_Cells.back());
	m_Cells.pop_back();

	m_iSelectCellIndex = 0;
}

void CNavigationController::Erase_Select()
{
	for (auto& iter = m_Cells.begin(); iter != m_Cells.end(); )
	{
		if (*iter == m_Cells[m_iSelectCellIndex])
		{
			Safe_Release(*iter);
			iter = m_Cells.erase(iter);
			return;
		}
		else
			++iter;
	}
}

void CNavigationController::Erase_All()
{
	for (auto& pCell : m_Cells)
		Safe_Release(pCell);
	m_Cells.clear();
}

void CNavigationController::Change_SelectCell(_float3* vPoint)
{
	if (m_Cells.size() == 0 || m_Cells.size() <= m_iSelectCellIndex)
		return;

	CCell* pCell = m_Cells[m_iSelectCellIndex];

	//_vector vChangePoint[CCell::POINT_END];
	//_float3 vChangePoint_Store[CCell::POINT_END];

	//vChangePoint[CCell::POINT_A] = pCell->Get_Point(CCell::POINT_A);
	//vChangePoint[CCell::POINT_B] = pCell->Get_Point(CCell::POINT_B);
	//vChangePoint[CCell::POINT_C] = pCell->Get_Point(CCell::POINT_C);

	//vChangePoint[ePoint] = XMLoadFloat3(vPoint[ePoint]);

	//XMStoreFloat3(&vChangePoint_Store[0], vChangePoint[0]);
	//XMStoreFloat3(&vChangePoint_Store[1], vChangePoint[1]);
	//XMStoreFloat3(&vChangePoint_Store[2], vChangePoint[2]);

	Sort_Cell(vPoint);

	m_Cells[m_iSelectCellIndex]->Set_Point(vPoint);
}

_float3 CNavigationController::Find_ClosetPoint(_fvector vPos)
{
	_float3 vClilckPos;
	XMStoreFloat3(&vClilckPos, vPos);

	m_fClosetDistance = 100000.f;

	if (m_Cells.size() == 0)
		return vClilckPos;

	XMVECTOR vRayPos_Local = m_pGameInstance->Get_RayPos_Local();
	XMVECTOR vRayDir_Local = XMVector3Normalize(m_pGameInstance->Get_RayDir_Local());

	for (_uint i = 0; i < m_Cells.size(); ++i)
	{
		CCell* pCell = m_Cells[i];
		if (nullptr == pCell)
			continue;

		_vector vPoint[CCell::POINT_END];

		vPoint[CCell::POINT_A] = pCell->Get_Point(CCell::POINT_A);
		vPoint[CCell::POINT_B] = pCell->Get_Point(CCell::POINT_B);
		vPoint[CCell::POINT_C] = pCell->Get_Point(CCell::POINT_C);

		_float fDistance_A = XMVectorGetX(XMVector3Length(vPoint[CCell::POINT_A] - vPos));
		_float fDistance_B = XMVectorGetX(XMVector3Length(vPoint[CCell::POINT_B] - vPos));
		_float fDistance_C = XMVectorGetX(XMVector3Length(vPoint[CCell::POINT_C] - vPos));
		_float fClosetDistance = min(fDistance_A, fDistance_B);
		fClosetDistance = min(fClosetDistance, fDistance_C);

		//만약 이전에 순회한 셀보다 거리가 크다면 continue,
		//더 작다면 해당 점을 click 한 점으로 Store해준다.
		if (m_fClosetDistance < fClosetDistance)
			continue;

		m_fClosetDistance = fClosetDistance;
		if (fDistance_A == m_fClosetDistance)
			XMStoreFloat3(&vClilckPos, vPoint[CCell::POINT_A]);
		else if (fDistance_B == m_fClosetDistance)
			XMStoreFloat3(&vClilckPos, vPoint[CCell::POINT_B]);
		else if (fDistance_C == m_fClosetDistance)
			XMStoreFloat3(&vClilckPos, vPoint[CCell::POINT_C]);
	}

	return vClilckPos;
}

void CNavigationController::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pDevice);
	Safe_Release(m_pGameInstance);
}
