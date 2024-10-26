#include "..\Public\Mesh.h"
#include "GameInstance.h"
#include "Bone.h"

CMesh::CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer{ pDevice, pContext }
{
}

CMesh::CMesh(const CMesh& Prototype)
	: CVIBuffer{ Prototype }
{
}

HRESULT CMesh::Initialize_Prototype(HANDLE* pFile, const CModel* pModel, _fmatrix PreTransformMatrix)
{
	_ulong dwByte = 0;

	ReadFile (*pFile, &m_szName, MAX_PATH, &dwByte, nullptr);

	ReadFile(*pFile, &m_iMaterialIndex, sizeof(_uint), &dwByte, nullptr);
	m_iNumVertexBuffers = 1;
	ReadFile(*pFile, &m_iNumVertices, sizeof(_uint), &dwByte, nullptr);
	ReadFile(*pFile, &m_iNumIndices, sizeof(_uint), &dwByte, nullptr);
	ReadFile(*pFile, &m_iNumFaces, sizeof(_uint), &dwByte, nullptr);
	m_iIndexStride = 4;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER

	HRESULT hr = pModel->Get_ModelType() == CModel::TYPE_NONANIM ? Ready_VertexBuffer_NonAnim(pFile, PreTransformMatrix) : Ready_VertexBuffer_Anim(pFile, pModel);
	if (FAILED(hr))
		return E_FAIL;

#pragma endregion

#pragma region INDEX_BUFFER
	/* �ε��������� ������ ä���ְ� */
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* �������۷� �����Ѵ�. */
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iIndexStride;

	m_pIndices = new _uint[m_iNumIndices];
	ZeroMemory(m_pIndices, sizeof(_uint) * m_iNumIndices);

	_uint		iNumIndices = { 0 };

	for (size_t i = 0; i < m_iNumFaces; i++)
	{
		ReadFile(*pFile, &m_pIndices[iNumIndices++], sizeof(_uint), &dwByte, nullptr);
		ReadFile(*pFile, &m_pIndices[iNumIndices++], sizeof(_uint), &dwByte, nullptr);
		ReadFile(*pFile, &m_pIndices[iNumIndices++], sizeof(_uint), &dwByte, nullptr);
	}

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = m_pIndices;

	/* �ε������۸� �����Ѵ�. */
	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	//Safe_Delete_Array(m_pIndices);

#pragma endregion

	return S_OK;
}

HRESULT CMesh::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CMesh::Bind_BoneMatrices(CModel* pModel, CShader* pShader, const _char* pConstantName, _float4x4* BoneMatrix)
{
	ZeroMemory(m_BoneMatrices, sizeof(_float4x4) * g_iMaxMeshBones);

	for (size_t i = 0; i < m_iNumBones; i++)
	{
		if (-1 == m_BoneIndices[i] || pModel->Get_Bones().size() <= m_BoneIndices[i])
			continue;
		XMStoreFloat4x4(&m_BoneMatrices[i], XMLoadFloat4x4(&m_OffsetMatrices[i]) * pModel->Get_BoneCombindTransformationMatrix(m_BoneIndices[i]));
	}

	if (nullptr != BoneMatrix)
	{
		memcpy(BoneMatrix, m_BoneMatrices, sizeof(_float4x4) * m_iNumBones);
	}

	return pShader->Bind_Matrices(pConstantName, m_BoneMatrices, m_iNumBones);
}

void CMesh::ReSetting_BoneIndex(CModel *pCurrentModel, CModel* pChangeModel)
{
	m_BoneIndices.clear();

	for (auto& BoneName : m_BoneNaems)
	{
		_int iIndex = pChangeModel->Get_BoneIndex(BoneName.c_str());

		if (iIndex == -1)
		{
			CBone* pNewBone = pCurrentModel->Find_Bone(BoneName);
			pChangeModel->Add_Bone(pNewBone);

			iIndex = pChangeModel->Get_BoneIndex(BoneName.c_str());
		}

		m_BoneIndices.emplace_back(iIndex);
	}
}

HRESULT CMesh::Ready_VertexBuffer_NonAnim(HANDLE* pFile, _fmatrix PreTransformMatrix)
{
	_ulong dwByte = 0;
	m_iVertexStride = sizeof(VTXMESH);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* �������۷� �����Ѵ�. */
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	/* �������ۿ� ä���� ������ ��������ؼ� �ӽ������� ������ �Ҵ��Ѵ�. */
	m_pVertices = new VTXMESH[m_iNumVertices];
	ZeroMemory(m_pVertices, sizeof(VTXMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		ReadFile(*pFile, &m_pVertices[i], sizeof(VTXMESH), &dwByte, nullptr);

		XMStoreFloat3(&m_pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&m_pVertices[i].vPosition), PreTransformMatrix)); 
		//XMStoreFloat3(&m_pVertices[i].vNormal, XMVector3TransformCoord(XMLoadFloat3(&m_pVertices[i].vNormal), PreTransformMatrix));
		//XMStoreFloat3(&m_pVertices[i].vTangent, XMVector3TransformCoord(XMLoadFloat3(&m_pVertices[i].vTangent), PreTransformMatrix));

		//m_ConvexHullPoints.push_back(m_pVertices[i].vPosition);
	}

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = m_pVertices;

	/* �������۸� �����Ѵ�. */
	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	//Select_InitPoint();

	return S_OK;
}

HRESULT CMesh::Ready_VertexBuffer_Anim(HANDLE* pFile, const CModel* pModel)
{
	_ulong dwByte = 0;
	m_iVertexStride = sizeof(VTXANIMMESH);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* �������۷� �����Ѵ�. */
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	/* �������ۿ� ä���� ������ ��������ؼ� �ӽ������� ������ �Ҵ��Ѵ�. */
	m_pAnimVertices = new VTXANIMMESH[m_iNumVertices];
	ZeroMemory(m_pAnimVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		ReadFile(*pFile, &m_pAnimVertices[i], sizeof(VTXANIMMESH), &dwByte, nullptr);
	}

	ReadFile(*pFile, &m_iNumBones, sizeof(_uint), &dwByte, nullptr);

	m_OffsetMatrices.resize(m_iNumBones);

	for (size_t i = 0; i < m_iNumBones; i++)
	{
		ReadFile(*pFile, &m_OffsetMatrices[i], sizeof(_float4x4), &dwByte, nullptr);

		_uint iNumWeights = { 0 };
		ReadFile(*pFile, &iNumWeights, sizeof(_uint), &dwByte, nullptr);

		/* �� ���� ��� �ִ� ��ü �����߿� �� �޽ð� ����ϴ� �����̸��� ���� ���� �ε����� �����صд�. */
		_char szBoneName[MAX_PATH];
		ReadFile(*pFile, &szBoneName, MAX_PATH, &dwByte, nullptr);

		string strBoneName = szBoneName;
		m_BoneNaems.emplace_back(strBoneName);
		m_BoneIndices.emplace_back(pModel->Get_BoneIndex(szBoneName));

		/* �� ���� �(mNumWeights) �������� ������ �ִ°�? */
		for (size_t j = 0; j < iNumWeights; j++)
		{
			_uint iWeightIndex = { 0 };
			ReadFile(*pFile, &iWeightIndex, sizeof(_uint), &dwByte, nullptr);

			ReadFile(*pFile, &m_pAnimVertices[iWeightIndex].vBlendIndices, sizeof(XMUINT4), &dwByte, nullptr);
			ReadFile(*pFile, &m_pAnimVertices[iWeightIndex].vBlendWeights, sizeof(XMFLOAT4), &dwByte, nullptr);

		}
	}

	if (0 == m_iNumBones)
	{
		m_iNumBones = 1;

		// ���� ���� ��Ȳ������ �޽��� �̸��� ���� ���� �̸��� ã�� ������ 1�� �÷� �־���
		// Weight�� 0�̾ fWeightW�� ����ξ��⿡ ��� �������� �������
		// OffsetMatrix�� ������ -> �׵����� �����ع�����
		m_BoneIndices.emplace_back(pModel->Get_BoneIndex(m_szName));

		_float4x4	OffsetMatrix;
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());
		m_OffsetMatrices.emplace_back(OffsetMatrix);
	}

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = m_pAnimVertices;

	/* �������۸� �����Ѵ�. */
	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	return S_OK;
}

void CMesh::Select_InitPoint()
{
	// 1. ���� ���� �� P0�� ã���ϴ�.
	m_vInitPoints[0] = *std::min_element(m_ConvexHullPoints.begin(), m_ConvexHullPoints.end(), [](const _float3 a, const _float3 b) {
		if (a.z != b.z) return a.z < b.z;
		if (a.y != b.y) return a.y < b.y;
		return a.x < b.x;
		});

	XMVECTOR vPoint0 = XMLoadFloat3(&m_vInitPoints[0]);

	// 2. P0���� ���� �� �� P1�� ã���ϴ�.
	m_vInitPoints[1] = *std::max_element(m_ConvexHullPoints.begin(), m_ConvexHullPoints.end(), [&](const _float3 p1, const _float3 p2) {
		XMVECTOR vPoint1 = XMLoadFloat3(&p1);
		XMVECTOR vPoint2 = XMLoadFloat3(&p2);

		return XMVectorGetX(XMVector3Length(vPoint0 - vPoint1)) < XMVectorGetX(XMVector3Length(vPoint0 - vPoint2));
		});

	XMVECTOR vPoint1 = XMLoadFloat3(&m_vInitPoints[1]);

	// 3. P0-P1 �������� ���� �� �� P2�� ã���ϴ�.
	XMVECTOR lineDir = XMVectorSubtract(vPoint1, vPoint0);
	m_vInitPoints[2] = *std::max_element(m_ConvexHullPoints.begin(), m_ConvexHullPoints.end(), [&](const _float3& p1, const _float3& p2) {
		XMVECTOR vPoint1 = XMLoadFloat3(&p1);
		XMVECTOR vPoint2 = XMLoadFloat3(&p2);

		// �� ���� P0 ���� ���͸� ����մϴ�
		XMVECTOR v1 = XMVectorSubtract(vPoint1, vPoint0);
		XMVECTOR v2 = XMVectorSubtract(vPoint2, vPoint0);

		// ������ ����ϰ� ���̸� ���մϴ�
		XMVECTOR crossProd1 = XMVector3Cross(lineDir, v1);
		XMVECTOR crossProd2 = XMVector3Cross(lineDir, v2);

		_float length1 = XMVectorGetX(XMVector3Length(crossProd1));
		_float length2 = XMVectorGetX(XMVector3Length(crossProd2));

		return length1 < length2;
		});

	XMVECTOR vPoint2 = XMLoadFloat3(&m_vInitPoints[2]);

	// 4. P0-P1-P2 ��鿡�� ���� �� �� P3�� ã���ϴ�.
	XMVECTOR normal = XMVector3Cross(XMVectorSubtract(vPoint2, vPoint0), XMVectorSubtract(vPoint1, vPoint0));
	m_vInitPoints[3] = *std::max_element(m_ConvexHullPoints.begin(), m_ConvexHullPoints.end(), [&](const _float3 p1, const _float3 p2) {
		XMVECTOR vPoint1 = XMLoadFloat3(&p1);
		XMVECTOR vPoint2 = XMLoadFloat3(&p2);
		return XMVectorGetX(XMVector3Dot(XMVectorSubtract(vPoint1, vPoint0), normal)) < XMVectorGetX(XMVector3Dot(XMVectorSubtract(vPoint2, vPoint0), normal));
		});
}

CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HANDLE* pFile, const CModel* pModel, _fmatrix PreTransformMatrix)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFile, pModel, PreTransformMatrix)))
	{
		MSG_BOX(TEXT("Failed to Created : CMesh"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CComponent* CMesh::Clone(void* pArg)
{
	CMesh* pInstance = new CMesh(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMesh"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMesh::Free()
{
	__super::Free();
	
	Safe_Delete_Array(m_pVertices);
	Safe_Delete_Array(m_pAnimVertices);
	Safe_Delete_Array(m_pIndices);

}
