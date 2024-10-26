#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class CBone final : public CBase
{
private:
	CBone();
	virtual ~CBone() = default;

public:
	const _char*		Get_Name() const { return m_szName; }

	_matrix				Get_CombinedTransformationMatrix() const { return XMLoadFloat4x4(&m_CombinedTransformationMatrix); }
	void				Set_CombinedTransformationMatrix(_matrix CombinedMatrix) { XMStoreFloat4x4(&m_CombinedTransformationMatrix, CombinedMatrix); }

	const _float4x4*	Get_CombinedTransformationMatrix_Ptr() const { return &m_CombinedTransformationMatrix; }

	_matrix				Get_TransformationMatrix() { return XMLoadFloat4x4(&m_TransformationMatrix); }
	void				Set_TransformationMatrix(_fmatrix TransformationMatrix) { XMStoreFloat4x4(&m_TransformationMatrix, TransformationMatrix); }

public:
	virtual HRESULT Initialize(HANDLE* pFile, _int iParentBoneIndex);
	void Update_CombinedTransformationMatrix(const vector<CBone*>& Bones, _fmatrix PreTransformMatrix);

	void Setting_ParentBoneName(class CModel* pModel);
	void Research_ParentBoneIndex(class CModel* pModel);

private:
	/* ���� �̸� */
	_char		m_szName[MAX_PATH] = {};
	_char		m_szParentName[MAX_PATH] = {};

	/* �θ� �������� ǥ���� �� ������ �������. */
	_float4x4	m_TransformationMatrix = {};

	/* �� ���� �������� ���� ��� ( �� ���� ��� * �θ��� ����������� ) */
	/* m_TransformationMatrix * Parent's m_CombinedTransformationMatrix */
	_float4x4	m_CombinedTransformationMatrix = {};

	_int		m_iParentBoneIndex = { -1 };

public:
	static CBone* Create(HANDLE* pFile, _int iParentBoneIndex);
	CBone* Clone();
	virtual void Free() override;
};

END