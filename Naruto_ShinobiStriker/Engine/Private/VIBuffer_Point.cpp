#include "VIBuffer_Point.h"

CVIBuffer_Point::CVIBuffer_Point(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer{ pDevice, pContext }
{
	m_eComponentType = VIBUFFER_RECT;
}

CVIBuffer_Point::CVIBuffer_Point(const CVIBuffer_Point& Prototype)
	: CVIBuffer{ Prototype }
{
	m_eComponentType = VIBUFFER_RECT;
}

HRESULT CVIBuffer_Point::Initialize_Prototype()
{
	m_iNumVertexBuffers = 1;				// ���ؽ� ���۰� �� ����?
	m_iNumVertices = 1;						// ������ �� �� ���ž�?
	m_iVertexStride = sizeof(VTXPOINT);	// ���� �ϳ��� ũ�Ⱑ ���̾�?
	m_iNumIndices = 1;						// �ε����� �� ����?
	m_iIndexStride = 2;						// �ε��� �ϳ��� ũ���? - 2 or 4
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;	// �ε��� ������? - ������ 2�ϱ� 16��Ʈ ���ٴ°��� 16��Ʈ == 2����Ʈ
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;	// ������ ��� �����Ұ���

	// �������� ��ġ�� �������ֱ� ����
	m_pVerticesPos = new _float3[m_iNumVertices];

#pragma region VERTEX_BUFFER

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;	// ũ�� ���� ���� ũ�� * ���� ����
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;	
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;			// ������ ���ؽ� ���� ������
	m_BufferDesc.CPUAccessFlags = 0;							// CPU ���� �㰡 �� �Ұ���
	m_BufferDesc.MiscFlags = 0;									// ���� ������� ���� 0
	m_BufferDesc.StructureByteStride = m_iVertexStride;			// ���� ũ��

	/* �������ۿ� ä���� ������ ��������ؼ� �ӽ������� ������ �Ҵ��Ѵ�. */
	VTXPOINT*		pVertices = new VTXPOINT;
	ZeroMemory(pVertices, sizeof(VTXPOINT));

	pVertices[0].vPosition = m_pVerticesPos[0] = _float3(0.f, 0.f, 0.f);
	pVertices->vPSize = _float2(1.f, 1.f);

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pVertices;

	/* �������۸� �����Ѵ�. */
	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INDEX_BUFFER

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* �������۷� �����Ѵ�. */
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iIndexStride;

	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices); // �̹� 0�� ������

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pIndices;

	/* �ε������۸� �����Ѵ�. */
	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion


	return S_OK;
}


HRESULT CVIBuffer_Point::Initialize(void* pArg)
{
	return S_OK;
}

CVIBuffer_Point* CVIBuffer_Point::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_Point* pInstance = new CVIBuffer_Point(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_Point"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Point::Clone(void* pArg)
{
	CVIBuffer_Point* pInstance = new CVIBuffer_Point(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CVIBuffer_Point"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Point::Free()
{
	__super::Free();

}
