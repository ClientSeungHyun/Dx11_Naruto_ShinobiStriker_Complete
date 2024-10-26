#include "..\Public\VIBuffer_Trail_Instance.h"

#include "GameInstance.h"

CVIBuffer_Trail_Instance::CVIBuffer_Trail_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer_Instancing{ pDevice, pContext }
{
}

CVIBuffer_Trail_Instance::CVIBuffer_Trail_Instance(const CVIBuffer_Trail_Instance& Prototype)
	: CVIBuffer_Instancing{ Prototype }
{
}

HRESULT CVIBuffer_Trail_Instance::Initialize_Prototype(const INSTACNE_TRAIL_DESC& Desc)
{
	m_iNumVertexBuffers = 2;
	m_iNumIndices = 1;
	m_iIndexStride = 2;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

	m_iNumInstance = Desc.iNumInstance;
	m_iInstanceStride = sizeof(VTXTRAILINSTANCE);
	m_iIndexCountPerInstance = 1;

#pragma region INDEX_BUFFER
	/* �ε��������� ������ ä���ְ� */
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* �������۷� �����Ѵ�. */
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iIndexStride;

	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pIndices;

	/* �ε������۸� �����Ѵ�. */
	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

#pragma region INSTANCE_BUFFER

	ZeroMemory(&m_InstanceBufferDesc, sizeof m_InstanceBufferDesc);
	m_InstanceBufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstance;
	m_InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC; /* �������۷� �����Ѵ�. */
	m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_InstanceBufferDesc.MiscFlags = 0;
	m_InstanceBufferDesc.StructureByteStride = m_iInstanceStride;

	m_pInstanceVertices = new VTXPOINTINSTANCE[m_iNumInstance];
	ZeroMemory(m_pInstanceVertices, sizeof(VTXPOINTINSTANCE) * m_iNumInstance);

	VTXPOINTINSTANCE* pInstanceVertices = static_cast<VTXPOINTINSTANCE*>(m_pInstanceVertices);

	// �����ŭ resize�ϰ� ���� ��ķ� �ʱ�ȭ �صα�
	vector<_float4x4> TrailMatrices;
	_float4x4 InitMatirx;
	XMStoreFloat4x4(&InitMatirx, XMMatrixIdentity());

	TrailMatrices.resize(m_iNumInstance, InitMatirx);

	// ��� _float4x4�� ���� ũ�⸦ ������ ������ vector�� �ʱ�ȭ �����Ұ���
	ZeroMemory(&m_InstanceInitialData, sizeof m_InstanceInitialData);
	m_InstanceInitialData.pSysMem = TrailMatrices.data();

#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Trail_Instance::Initialize(void* pArg)
{
#pragma region INSTANCE_BUFFER

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Trail_Instance::Bind_Buffers()
{
	ID3D11Buffer* pVertexBuffers[] = {
		m_pVB,
		m_pVBInstance,
	};

	_uint		iVertexStrides[] = {
		m_iVertexStride,
		m_iInstanceStride,
	};

	_uint		iOffsets[] = {
		0,
		0,
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffsets);
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_eTopology);

	return S_OK;
}

HRESULT CVIBuffer_Trail_Instance::Render()
{
	m_pContext->DrawIndexedInstanced(m_iIndexCountPerInstance, (_uint)m_TrailPoints.size(), 0, 0, 0);

	return S_OK;
}

void CVIBuffer_Trail_Instance::Add_TrailPoint(_fmatrix WorldMatirx, _uint iNumTrail)
{
	if (iNumTrail <= 3)
		iNumTrail = m_iNumInstance;

	_vector vCurrentPos(WorldMatirx.r[3]);

	TRAILPOINT tNewTrail = {};

	_matrix NewMatrix = XMMatrixIdentity();
	//_float4x4 NewMatrix4x4 = {};

	// ó���̶�� ��� �࿡ ��ġ�� �־� � �������� ���� �������� ������
	if (m_TrailPoints.empty())
	{
		for (_uint i = 0; i < 4; ++i)
		{
			NewMatrix.r[i] = vCurrentPos;
		}
	}
	else
	{
		TRAILPOINT& tPrevTrail = m_TrailPoints.back();
		_matrix vPrevPointMatrix = XMLoadFloat4x4(&tPrevTrail.PointMatrix);

		_vector vCameraPos = m_pGameInstance->Get_CamPosition_Vector();
		_vector vPrevTrailDir = XMLoadFloat3(&tPrevTrail.vTrailPos) - vCurrentPos;

		_vector vLook = vCameraPos - vCurrentPos;
		_vector vRight = XMVector3Normalize(XMVector3Cross(vPrevTrailDir, vLook));

		_vector vCurrRightPos = vCurrentPos + vRight * m_fTrailSize;
		_vector vCurrLeftPos = vCurrentPos - vRight * m_fTrailSize;

		_vector vPrevRightPos = vPrevPointMatrix.r[0];
		_vector vPrevLeftPos = vPrevPointMatrix.r[1];

		// �� �� �� ���� �׸� Ʈ������ ��ġ�� ��������
		NewMatrix.r[0] = vCurrLeftPos;	//�»�
		NewMatrix.r[1] = vCurrRightPos;	//���
		NewMatrix.r[2] = vPrevLeftPos;	//����
		NewMatrix.r[3] = vPrevRightPos;	//����
	}

	XMStoreFloat3(&tNewTrail.vTrailPos, vCurrentPos);
	XMStoreFloat4x4(&tNewTrail.PointMatrix, NewMatrix);
	m_TrailPoints.emplace_back(tNewTrail);

	// ���� ����Ʈ�� �� Ŀ���� �� ũ�� ���� �� ���� �������� (�տ��� �����Ȱ�)
	while (m_TrailPoints.size() >= iNumTrail)
	{
		m_TrailPoints.pop_front();
	}
}

void CVIBuffer_Trail_Instance::Add_TrailPoint(_fvector vPos, _uint iNumTrail)
{
	if (iNumTrail <= 3)
		iNumTrail = m_iNumInstance;

	_vector vCurrentPos = vPos;

	TRAILPOINT tNewTrail = {};

	_matrix NewMatrix = XMMatrixIdentity();
	//_float4x4 NewMatrix4x4 = {};

	// ó���̶�� ��� �࿡ ��ġ�� �־� � �������� ���� �������� ������
	if (m_TrailPoints.empty())
	{
		for (_uint i = 0; i < 4; ++i)
		{
			NewMatrix.r[i] = vCurrentPos;
		}
	}
	else
	{
		TRAILPOINT& tPrevTrail = m_TrailPoints.back();
		_matrix vPrevPointMatrix = XMLoadFloat4x4(&tPrevTrail.PointMatrix);

		_vector vCameraPos = m_pGameInstance->Get_CamPosition_Vector();
		_vector vPrevTrailDir = XMLoadFloat3(&tPrevTrail.vTrailPos) - vCurrentPos;

		_vector vLook = vCameraPos - vCurrentPos;
		_vector vRight = XMVector3Normalize(XMVector3Cross(vPrevTrailDir, vLook));

		_vector vCurrRightPos = vCurrentPos + vRight * m_fTrailSize;
		_vector vCurrLeftPos = vCurrentPos - vRight * m_fTrailSize;

		_vector vPrevRightPos = vPrevPointMatrix.r[0];
		_vector vPrevLeftPos = vPrevPointMatrix.r[1];

		// �� �� �� ���� �׸� Ʈ������ ��ġ�� ��������
		NewMatrix.r[0] = vCurrLeftPos;	//�»�
		NewMatrix.r[1] = vCurrRightPos;	//���
		NewMatrix.r[2] = vPrevLeftPos;	//����
		NewMatrix.r[3] = vPrevRightPos;	//����
	}

	XMStoreFloat3(&tNewTrail.vTrailPos, vCurrentPos);
	XMStoreFloat4x4(&tNewTrail.PointMatrix, NewMatrix);
	m_TrailPoints.emplace_back(tNewTrail);

	// ���� ����Ʈ�� �� Ŀ���� �� ũ�� ���� �� ���� �������� (�տ��� �����Ȱ�)
	while (m_TrailPoints.size() >= iNumTrail)
	{
		m_TrailPoints.pop_front();
	}
}

void CVIBuffer_Trail_Instance::Add_TrailPoint_ForWire(_fmatrix WorldMatrix, _fvector vHandPos, _float fSize)
{

	TRAILPOINT tNewTrail = {};
	_matrix NewMatrix = XMMatrixIdentity();

	m_TrailPoints.clear();

	_vector vCameraPos = m_pGameInstance->Get_CamPosition_Vector();

	_vector vLook = WorldMatrix.r[2]; // up���ͷ� ���
	_vector vPos = WorldMatrix.r[3];

	_vector vDir = XMVector3Normalize(vCameraPos - vPos);	// look���ͷ� ���

	_vector vRight = XMVector3Normalize(XMVector3Cross(vLook, vDir));

	vRight *= fSize;

	_vector vCurrRightPos = vPos + vRight; // ��
	_vector vCurrLeftPos = vPos - vRight; // ��
	_vector vPrevRightPos = vHandPos + vRight; // ��
	_vector vPrevLeftPos = vHandPos - vRight; // ��

	NewMatrix.r[0] = vCurrRightPos;
	NewMatrix.r[1] = vCurrLeftPos;
	NewMatrix.r[2] = vPrevRightPos;
	NewMatrix.r[3] = vPrevLeftPos;

	XMStoreFloat3(&tNewTrail.vTrailPos, vPos);
	XMStoreFloat4x4(&tNewTrail.PointMatrix, NewMatrix);

	m_TrailPoints.emplace_back(tNewTrail);

}

void CVIBuffer_Trail_Instance::Update_Trail()
{
	if (m_TrailPoints.size() < 5)
		return;

	vector<_float4x4> WorldMatrix;
	WorldMatrix.reserve(m_TrailPoints.size());

	for (auto& TrailPoint : m_TrailPoints)
	{
		WorldMatrix.push_back(TrailPoint.PointMatrix);
	}

	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	memcpy(SubResource.pData, WorldMatrix.data(), sizeof(_float4x4) * WorldMatrix.size());

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Trail_Instance::Update_Trail_ForWire()
{
	vector<_float4x4> WorldMatrix;
	WorldMatrix.reserve(1);

	for (auto& TrailPoint : m_TrailPoints)
	{
		WorldMatrix.push_back(TrailPoint.PointMatrix);
		break;
	}

	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	memcpy(SubResource.pData, WorldMatrix.data(), sizeof(_float4x4));

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Trail_Instance::Clear_Trail()
{
	m_TrailPoints.clear();

	vector<_float4x4> WorldMatrix;
	WorldMatrix.reserve(1);

	for (auto& TrailPoint : m_TrailPoints)
	{
		WorldMatrix.push_back(TrailPoint.PointMatrix);
		break;
	}

	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	memcpy(SubResource.pData, WorldMatrix.data(), sizeof(_float4x4));

	m_pContext->Unmap(m_pVBInstance, 0);
}

CVIBuffer_Trail_Instance* CVIBuffer_Trail_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const INSTACNE_TRAIL_DESC& Desc)
{
	CVIBuffer_Trail_Instance* pInstance = new CVIBuffer_Trail_Instance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(Desc)))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_Trail_Instance"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CComponent* CVIBuffer_Trail_Instance::Clone(void* pArg)
{
	CVIBuffer_Trail_Instance* pInstance = new CVIBuffer_Trail_Instance(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CVIBuffer_Trail_Instance"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Trail_Instance::Free()
{
	__super::Free();

}
