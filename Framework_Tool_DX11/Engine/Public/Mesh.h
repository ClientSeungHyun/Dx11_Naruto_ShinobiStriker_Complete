#pragma once

#include "VIBuffer.h"
#include "Model.h"

/* �� = �޽� + �޽� + �޽� .. */
/* �޽÷� �����ϴ� ���� : ������ ��ü�� �����ϰ� ������ֱ� ���ؼ�.*/
/* �޽� = �������� + �ε��� */

BEGIN(Engine)

class CMesh final : public CVIBuffer
{
private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& Prototype);
	virtual ~CMesh() = default;

public:
	_char* Get_Name() { return m_szName; }

	_uint			Get_MaterialIndex() { return m_iMaterialIndex; }
	VTXMESH* Get_Vertices() { return m_pVertices; }
	VTXANIMMESH* Get_AnimVertices() { return m_pAnimVertices; }
	_uint* Get_Indices() { return m_pIndices; }

public:
	virtual HRESULT Initialize_Prototype(HANDLE* pFile, const CModel* pModel, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Bind_BoneMatrices(const CModel* pModel, class CShader* pShader, const _char* pConstantName);

public:
	virtual _bool	isPicking(const _fmatrix& WorldMatrix, _float3* pOut) override;
	_bool			isPicking_Surface(const _fmatrix& WorldMatrix, _fvector fRayPos, _float3* pOut);

private:
	VTXMESH*			m_pVertices = { nullptr };
	VTXANIMMESH*		m_pAnimVertices = { nullptr };
	_uint*				m_pIndices = { nullptr };

private:
	_uint				m_iNumFaces = {};
	_char				m_szName[MAX_PATH] = "";
	_uint				m_iMaterialIndex = { 0 };
	_uint				m_iNumBones = { 0 };

	/* uint : �𵨿� �ε��س��� ��ü �� ���� �ε����� �̾߱��Ѵ�. */
	/* ���� ����� ���� �ҷ����� ��� ���� �ൿ�� ��ġ�ع��� */
	vector<_int>		m_BoneIndices;

	_float4x4			m_BoneMatrices[g_iMaxMeshBones] = {};
	vector<_float4x4>	m_OffsetMatrices;


private:
	HRESULT	Ready_VertexBuffer_NonAnim(HANDLE* pFile, _fmatrix PreTransformMatrix);
	HRESULT	Ready_VertexBuffer_Anim(HANDLE* pFile, const CModel* pModel);

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HANDLE* pFile, const CModel* pModel, _fmatrix PreTransformMatrix);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END