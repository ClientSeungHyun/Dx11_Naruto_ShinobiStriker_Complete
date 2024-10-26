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
	_char*			Get_Name() { return m_szName; }

	_uint			Get_MaterialIndex() { return m_iMaterialIndex; }
	VTXMESH*		Get_Vertices() { return m_pVertices; }
	VTXANIMMESH*	Get_AnimVertices() { return m_pAnimVertices; }
	_uint*			Get_Indices() { return m_pIndices; }
	_uint			Get_NumBone() { return m_iNumBones; }

public:
	virtual HRESULT Initialize_Prototype(HANDLE* pFile, const CModel* pModel, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Bind_BoneMatrices(CModel* pModel, class CShader* pShader, const _char* pConstantName, _float4x4* BoneMatrix);

public:
	void  ReSetting_BoneIndex(CModel* pCurrentModel, CModel* pChangeModel);

private:
	_char				m_szName[MAX_PATH] = "";
	_uint				m_iMaterialIndex = { 0 };
	_uint				m_iNumBones = { 0 };

private:
	_uint				m_iNumFaces = {};
	VTXMESH*			m_pVertices = { nullptr };
	VTXANIMMESH*		m_pAnimVertices = { nullptr };
	_uint*				m_pIndices = { nullptr };

	vector<_float3>		m_ConvexHullPoints;
	_float3				m_vInitPoints[4];

private:
	/* uint : �𵨿� �ε��س��� ��ü �� ���� �ε����� �̾߱��Ѵ�. */
	/* ���� ����� ���� �ҷ����� ��� ���� �ൿ�� ��ġ�ع��� */
	vector<_uint>		m_BoneIndices;
	vector<string>		m_BoneNaems;

	_float4x4			m_BoneMatrices[g_iMaxMeshBones] = {};
	vector<_float4x4>	m_OffsetMatrices;

private:
	HRESULT	Ready_VertexBuffer_NonAnim(HANDLE* pFile, _fmatrix PreTransformMatrix);
	HRESULT	Ready_VertexBuffer_Anim(HANDLE* pFile, const CModel* pModel);

	void	Select_InitPoint();

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HANDLE* pFile, const CModel* pModel, _fmatrix PreTransformMatrix);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END