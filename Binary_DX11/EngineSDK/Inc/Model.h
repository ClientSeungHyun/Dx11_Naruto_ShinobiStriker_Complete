#pragma once

#include "Component.h"
#include "Bone.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };

private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& Prototype);
	virtual ~CModel() = default;

public:
	vector<class CMesh*>& Get_Meshes() { return m_Meshes; }

	_uint Get_NumMeshes() const { return (_uint)m_Meshes.size(); }

	TYPE Get_ModelType() const { return m_eType; }

	_uint Get_BoneIndex(const _char* pBoneName) const;
	_matrix Get_BoneCombindTransformationMatrix(_uint iBoneIndex) const { return m_Bones[iBoneIndex]->Get_CombinedTransformationMatrix(); }

public:
	virtual HRESULT Initialize_Prototype(TYPE eType, const _char* pModelFilePath, const _char* pMaterialPath, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render(_uint iMeshIndex);

public:
	HRESULT Bind_Material(class CShader* pShader, _char* pConstantName, aiTextureType eMAterialType, _uint iMeshIndex);

private:
	/* ���� �־��� ��ο� �ش��ϴ� ������ ������ �о aiScene��ü�� �������ش�. */
	Assimp::Importer				m_Importer;

	/* ���Ϸκ��� �о�� ��� ������ �����ϰ� �ִ´�. */
	const aiScene*					m_pAIScene = { nullptr };
	TYPE							m_eType = { TYPE_END };

private: /* �޽��� ������ �����Ѵ�. */
	_uint							m_iNumMeshes = { 0 };
	vector<class CMesh*>			m_Meshes;
	_float4x4						m_PreTransformMatrix = {};

private:
	_uint							m_iNumMaterials = { 0 };
	vector<MESH_MATERIAL>			m_Materials;

private:
	vector<class CBone*>			m_Bones;

private:
	_bool							m_isLoop = { false };
	_uint							m_iCurrentAnimIndex = { 0 };
	_uint							m_iNumAnimations = { 0 };
	vector<class CAnimation*>		m_Animations;

public:
	HRESULT	Ready_Meshes(HANDLE* pFile);
	HRESULT Ready_Materials(HANDLE* pFile, const _char* pModelFilePath);
	HRESULT Ready_Bones(HANDLE* pFile, const aiNode* pAIBone, _int iParentBoneIndex);
	HRESULT Ready_Animations(HANDLE* pFile);

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const _char* pModelFilePath, const _char* pMaterailPath, _fmatrix PreTransformMatrix = XMMatrixIdentity());
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END