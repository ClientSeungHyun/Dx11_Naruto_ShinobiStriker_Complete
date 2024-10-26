#include "..\Public\Mesh.h"
#include "GameInstance.h"

CMesh::CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer{ pDevice, pContext }
{
}

CMesh::CMesh(const CMesh& Prototype)
	: CVIBuffer{ Prototype }
{
}

HRESULT CMesh::Initialize_Prototype(HANDLE* pFile, const CModel* pModel, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix)
{
	_ulong dwByte = 0;
	strcpy_s(m_szName, pAIMesh->mName.data);
	WriteFile(*pFile, &m_szName, MAX_PATH, &dwByte, nullptr);

	m_iMaterialIndex = pAIMesh->mMaterialIndex;
	WriteFile(*pFile, &m_iMaterialIndex, sizeof(_uint), &dwByte, nullptr);

	m_iNumVertices = pAIMesh->mNumVertices;
	WriteFile(*pFile, &m_iNumVertices, sizeof(_uint), &dwByte, nullptr);

	m_iNumIndices = pAIMesh->mNumFaces * 3;
	WriteFile(*pFile, &m_iNumIndices, sizeof(_uint), &dwByte, nullptr);

	m_iNumFaces = pAIMesh->mNumFaces;
	WriteFile(*pFile, &m_iNumFaces, sizeof(_uint), &dwByte, nullptr);

#pragma region VERTEX

	HRESULT hr = pModel->Get_ModelType() == CModel::TYPE_NONANIM ? Ready_VertexBuffer_NonAnim(pFile, pAIMesh, PreTransformMatrix) : Ready_VertexBuffer_Anim(pFile, pModel, pAIMesh);
	if (FAILED(hr))
		return E_FAIL;

#pragma endregion

#pragma region INDEX
	m_pIndices = new _uint[m_iNumIndices];

	_uint		iNumIndices = { 0 };
	for (size_t i = 0; i < m_iNumFaces; i++)
	{
		aiFace		AIFace = pAIMesh->mFaces[i];

		_uint a = AIFace.mIndices[0];
		m_pIndices[iNumIndices] = AIFace.mIndices[0];
		WriteFile(*pFile, &m_pIndices[iNumIndices++], sizeof(_uint), &dwByte, nullptr);

		a = AIFace.mIndices[1];
		m_pIndices[iNumIndices] = AIFace.mIndices[1];
		WriteFile(*pFile, &m_pIndices[iNumIndices++], sizeof(_uint), &dwByte, nullptr);

		a = AIFace.mIndices[2];
		m_pIndices[iNumIndices] = AIFace.mIndices[2];
		WriteFile(*pFile, &m_pIndices[iNumIndices++], sizeof(_uint), &dwByte, nullptr);
	}
#pragma endregion

	return S_OK;
}

HRESULT CMesh::Initialize(void* pArg)
{
	return S_OK;
}

_bool CMesh::isPicking(const _fmatrix& WorldMatrix, _float3* pOut)
{
	m_pGameInstance->Transform_MouseRay_ToLocalSpace(WorldMatrix);

	_uint		iNumIndices = { 0 };

	for (size_t i = 0; i < (m_iNumIndices / 3); i++)
	{

		if (true == m_pGameInstance->isPicked_InLocalSpace(XMLoadFloat3(&m_pVertices[m_pIndices[iNumIndices++]].vPosition),
			XMLoadFloat3(&m_pVertices[m_pIndices[iNumIndices++]].vPosition),
			XMLoadFloat3(&m_pVertices[m_pIndices[iNumIndices++]].vPosition),
			pOut))
			goto Compute_WorldPos;
	}
	return false;

Compute_WorldPos:
	_vector vOut = XMLoadFloat3(pOut);
	vOut = XMVector3TransformCoord(vOut, WorldMatrix);
	XMStoreFloat3(pOut, vOut);

	return true;
}

HRESULT CMesh::Ready_VertexBuffer_NonAnim(HANDLE* pFile, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix)
{
	_ulong dwByte = 0;

	m_pVertices = new VTXMESH[m_iNumVertices];
	ZeroMemory(m_pVertices, sizeof(VTXMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		// �������� �ʱ� ���¸� �츮�� ���ϴ� ��ġ, ȸ��, �����Ϸ� �ٲ��ֱ� ���� �����鿡�ٰ� �޾ƿ� ����� ������
		memcpy(&m_pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		memcpy(&m_pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&m_pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&m_pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));

		WriteFile(*pFile, &m_pVertices[i], sizeof(VTXMESH), &dwByte, nullptr);
	}

	return S_OK;
}

HRESULT CMesh::Ready_VertexBuffer_Anim(HANDLE* pFile, const CModel* pModel, const aiMesh* pAIMesh)
{
	_ulong dwByte = 0;

	m_pAnimVertices = new VTXANIMMESH[m_iNumVertices];
	ZeroMemory(m_pAnimVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&m_pAnimVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		memcpy(&m_pAnimVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&m_pAnimVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&m_pAnimVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));

		WriteFile(*pFile, &m_pAnimVertices[i], sizeof(VTXANIMMESH), &dwByte, nullptr);
	}

	/* �� ��ü�� ���� �ƴ� �� �޽��ϳ��� ������ �ִ� ���� ����. */
	m_iNumBones = pAIMesh->mNumBones;
	WriteFile(*pFile, &m_iNumBones, sizeof(_uint), &dwByte, nullptr);

	m_OffsetMatrices.resize(m_iNumBones);

	for (size_t i = 0; i < m_iNumBones; i++)
	{
		aiBone* pAIBone = pAIMesh->mBones[i];

		/* pAIBone->mOffsetMatrix : �������� ���� �ϳ��� ���븦 �����ϴ� ���, ���� ���¿� ���� ��Ű��(������ ��ġ * �����)�۾� ��, �߰����� ������ �ʿ��� �� �ִ�.  */
		memcpy(&m_OffsetMatrices[i], &pAIBone->mOffsetMatrix, sizeof(_float4x4));
		XMStoreFloat4x4(&m_OffsetMatrices[i], XMMatrixTranspose(XMLoadFloat4x4(&m_OffsetMatrices[i])));
		WriteFile(*pFile, &m_OffsetMatrices[i], sizeof(_float4x4), &dwByte, nullptr);

		_uint iNumWeights = pAIBone->mNumWeights;
		WriteFile(*pFile, &iNumWeights, sizeof(_uint), &dwByte, nullptr);

		/* �� ���� ��� �ִ� ��ü �����߿� �� �޽ð� ����ϴ� �����̸��� ���� ���� �ε����� �����صд�. */
		_char szBoneName[MAX_PATH] = "";
		strcpy_s(szBoneName, pAIBone->mName.data);
		WriteFile(*pFile, &szBoneName, MAX_PATH, &dwByte, nullptr);

		m_BoneIndices.emplace_back(pModel->Get_BoneIndex(pAIBone->mName.data));

		/* �� ���� �(mNumWeights) �������� ������ �ִ°�? */
		for (size_t j = 0; j < iNumWeights; j++)
		{
			WriteFile(*pFile, &pAIBone->mWeights[j].mVertexId, sizeof(_uint), &dwByte, nullptr);

			/* �� ���� ������ �ִ� j��° ������ �ε���(pAIBone->mWeights[j].mVertexId)*/
			if (0.f == m_pAnimVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.x)
			{
				m_pAnimVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.x = (_uint)i;
				m_pAnimVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.x = pAIBone->mWeights[j].mWeight;
			}

			else if (0.f == m_pAnimVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.y)
			{
				m_pAnimVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.y = (_uint)i;
				m_pAnimVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.y = pAIBone->mWeights[j].mWeight;
			}

			else if (0.f == m_pAnimVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.z)
			{
				m_pAnimVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.z = (_uint)i;
				m_pAnimVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.z = pAIBone->mWeights[j].mWeight;
			}

			else
			{
				m_pAnimVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.w = (_uint)i;
				m_pAnimVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.w = pAIBone->mWeights[j].mWeight;
			}

			WriteFile(*pFile, &m_pAnimVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices, sizeof(XMUINT4), &dwByte, nullptr);
			WriteFile(*pFile, &m_pAnimVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights, sizeof(XMFLOAT4), &dwByte, nullptr);

		}
	}

	// �ʿ� ���� �ڵ� �󿡼� ó�� ����
	//if (0 == m_iNumBones)
	//{
	//	m_iNumBones = 1;

	//	// ���� ���� ��Ȳ������ �޽��� �̸��� ���� ���� �̸��� ã�� ������ 1�� �÷� �־���
	//	// Weight�� 0�̾ fWeightW�� ����ξ��⿡ ��� �������� �������
	//	// OffsetMatrix�� ������ -> �׵����� �����ع�����
	//	m_BoneIndices.emplace_back(pModel->Get_BoneIndex(m_szName));

	//	_float4x4	OffsetMatrix;
	//	XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());
	//	m_OffsetMatrices.emplace_back(OffsetMatrix);
	//}

	return S_OK;
}

CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HANDLE* pFile, const CModel* pModel, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFile, pModel, pAIMesh, PreTransformMatrix)))
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
