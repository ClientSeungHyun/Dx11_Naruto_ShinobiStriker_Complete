#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class CBone final : public CBase
{
private:
	CBone();
	virtual ~CBone() = default;

public:
	const _char* Get_Name() const { return m_szName; }

	_matrix			Get_CombinedTransformationMatrix() const { return XMLoadFloat4x4(&m_CombinedTransformationMatrix); }

public:
	virtual HRESULT Initialize(HANDLE* pFile, const aiNode* pAIBone, _int iParentBoneIndex);

private:
	/* ���� �̸� */
	_char		m_szName[MAX_PATH] = {};

	/* �θ� �������� ǥ���� �� ������ �������. */
	_float4x4	m_TransformationMatrix = {};

	/* �� ���� �������� ���� ��� ( �� ���� ��� * �θ��� ����������� ) */
	/* m_TransformationMatrix * Parent's m_CombinedTransformationMatrix */
	_float4x4	m_CombinedTransformationMatrix = {};

	_int				m_iParentBoneIndex = { -1 };

public:
	static CBone* Create(HANDLE* pFile, const aiNode* pAIBone, _int iParentBoneIndex);
	virtual void Free() override;
};

END