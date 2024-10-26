#include "..\Public\Model.h"
#include "Mesh.h"
#include "Animation.h"
#include "Texture.h"
#include "Shader.h"


CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
	m_eComponentType = MODEL;
}

CModel::CModel(const CModel& Prototype)
	: CComponent{ Prototype }
	, m_iNumMeshes{ Prototype.m_iNumMeshes }
	, m_Meshes{ Prototype.m_Meshes }
	, m_iNumMaterials{ Prototype.m_iNumMaterials }
	, m_Materials{ Prototype.m_Materials }
	, m_Bones{ Prototype.m_Bones }
	, m_PreTransformMatrix{ Prototype.m_PreTransformMatrix }
	, m_iCurrentAnimIndex{ Prototype.m_iCurrentAnimIndex }
	, m_iNumAnimations{ Prototype.m_iNumAnimations }
	, m_Animations{ Prototype.m_Animations }
{
	m_eComponentType = MODEL;

	for (auto& pAnimation : m_Animations)
		Safe_AddRef(pAnimation);

	for (auto& pBone : m_Bones)
		Safe_AddRef(pBone);

	for (auto& Material : m_Materials)
	{
		for (auto& pTexture : Material.pMaterialTextures)
			Safe_AddRef(pTexture);
	}

	for (auto& pMesh : m_Meshes)
		Safe_AddRef(pMesh);

	m_eComponentType = MODEL;
}

_uint CModel::Get_BoneIndex(const _char* pBoneName) const
{
	_uint iBoneIndex = { 0 };
	auto iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)-> _bool
		{
			if (0 == strcmp(pBoneName, pBone->Get_Name()))
				return true;
			++iBoneIndex;
			return false;
		});

	if (iter == m_Bones.end())
		MSG_BOX(TEXT("���µ�?"));

	return iBoneIndex;
}

HRESULT CModel::Initialize_Prototype(TYPE eType, const _char* pModelFilePath, const _char* pMaterialPath, _fmatrix PreTransformMatrix)
{
	_uint		iFlag = { 0 };

	/* ���� : ��� �޽ð� �� ������ �������� �׎´ϴ�. */
	/* ���� : ���� �����ϴ� ��� �޽õ��� ���� ������ ����(�޽ø� ��ġ�ϱ����� ������ ��ġ�� ���缭)��� �̸� ��ȯ���ش�.*/

	//iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;
	iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

	/*aiProcess_GlobalScale*/

	if (eType == TYPE_NONANIM)
		iFlag |= aiProcess_PreTransformVertices;

	m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
	if (nullptr == m_pAIScene)
		return E_FAIL;

#pragma region ��� �����ϱ�
	// ���, �̸�, Ȯ���� ���ϱ�
	char szName[MAX_PATH] = "";
	_splitpath_s(pModelFilePath, nullptr, 0, nullptr, 0, szName, MAX_PATH, nullptr, 0);

	string strFolderName;

	std::filesystem::path path(pModelFilePath);
	if (path.has_parent_path()) {
		strFolderName = path.parent_path().filename().string();
	}

	char szCreateFolderPath[MAX_PATH];
	if (eType == TYPE_NONANIM)
		strcpy_s(szCreateFolderPath, "../Bin/ModelData/NonAnim/");
	else
		strcpy_s(szCreateFolderPath, "../Bin/ModelData/Anim/");

	string strCreatePath = szCreateFolderPath + strFolderName + "/";

	strcpy_s(szCreateFolderPath, strCreatePath.c_str());

	// ���� �����ϱ�
	fs::path filePath(szCreateFolderPath);
	fs::create_directory(filePath);

	char szExt[MAX_PATH] = ".dat";
	strcat_s(szCreateFolderPath, szName);
	strcat_s(szCreateFolderPath, szExt);

	_tchar szFinalPath[MAX_PATH] = TEXT("");
	MultiByteToWideChar(CP_ACP, 0, szCreateFolderPath, (_uint)strlen(szCreateFolderPath), szFinalPath, MAX_PATH);

#pragma endregion

	HANDLE hFile = CreateFile(szFinalPath, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
	{
		MSG_BOX(TEXT("Failed to Open Models data"));
		return E_FAIL;
	}

	// �ʱ� �������� ���¸� �츮�� ���ϴ� transfrom���� �ٲٱ� ���� ���
	XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);
	m_eType = eType;

	if (eType == TYPE_ANIM)
	{
		if (FAILED(Ready_Bones(&hFile, m_pAIScene->mRootNode, -1)))
			return E_FAIL;
	}

	if (FAILED(Ready_Meshes(&hFile)))
		return E_FAIL;

	if (FAILED(Ready_Materials(&hFile, pMaterialPath)))
		return E_FAIL;

	if (eType == TYPE_ANIM)
	{
		if (FAILED(Ready_Animations(&hFile)))
			return E_FAIL;
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{

	return S_OK;
}

HRESULT CModel::Render(_uint iMeshIndex)
{
	// Ŭ���̾�Ʈ���� ��ȸ�ϱ� ����

	m_Meshes[iMeshIndex]->Bind_Buffers();
	m_Meshes[iMeshIndex]->Render();

	return S_OK;
}

HRESULT CModel::Bind_Material(CShader* pShader, _char* pConstantName, aiTextureType eMAterialType, _uint iMeshIndex)
{
	_uint iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();

	m_Materials[iMaterialIndex].pMaterialTextures[eMAterialType]->Bind_ShadeResource(pShader, pConstantName, 0);

	return E_NOTIMPL;
}

HRESULT CModel::Ready_Meshes(HANDLE* pFile)
{
	_ulong dwByte = 0;

	m_iNumMeshes = m_pAIScene->mNumMeshes;
	WriteFile(*pFile, &m_iNumMeshes, sizeof(_uint), &dwByte, nullptr);

	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, pFile, this, m_pAIScene->mMeshes[i], XMLoadFloat4x4(&m_PreTransformMatrix));
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.emplace_back(pMesh);
	}

	return S_OK;
}

HRESULT CModel::Ready_Materials(HANDLE* pFile, const _char* pModelFilePath)
{
	_ulong dwByte = 0;

	// ���͸����� ���� �����صα�
	m_iNumMaterials = m_pAIScene->mNumMaterials;
	WriteFile(*pFile, &m_iNumMaterials, sizeof(_uint), &dwByte, nullptr);

	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
		MESH_MATERIAL		MeshMaterial{};

		aiMaterial* pAIMaterial = m_pAIScene->mMaterials[i];

		// �ؽ��Ĵ� �ִ� 18������ ���� ����
		for (size_t j = 1; j < AI_TEXTURE_TYPE_MAX; j++)
		{
			// pAIMaterial->GetTextureCount(j);
			aiString			strTexturePath;

			_char				szDrive[MAX_PATH] = "";
			_char				szDirectory[MAX_PATH] = "";
			_char				szFileName[MAX_PATH] = "";
			_char				szExt[MAX_PATH] = "";

			_char				szTexturePath[MAX_PATH] = "";

			// ���� �ؽ������� ���������� ���ϱ� 0�� None�̹Ƿ� 1���� ����
			// 1 = diffuse, 2 = specular ���
			// �ش� �ؽ��İ� �������� �� ���� �츮�� 0�� ������
			// �ؽ��Ŀ� �ش��ϴ� ��� ��������
			HRESULT hr = pAIMaterial->GetTexture(aiTextureType(j), 0, &strTexturePath);
			_bool isHaveTexture = hr == S_OK ? true : false;
			WriteFile(*pFile, &isHaveTexture, sizeof(_bool), &dwByte, nullptr);
			if (!isHaveTexture)
				continue;

			// ���͸��� ���ǵ� ��θ� �ſ��� �� ����
			// ����̺�� ���丮 ��� ��������
			_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);

			// ���� �̸� ��������
			_splitpath_s(strTexturePath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

			// ���ļ� ���� ��� �����
			strcpy_s(szTexturePath, szDrive);
			strcat_s(szTexturePath, szDirectory);
			strcat_s(szTexturePath, szFileName);
			strcat_s(szTexturePath, szExt);

			WriteFile(*pFile, szTexturePath, MAX_PATH, &dwByte, nullptr);
		}
	}

	return S_OK;
}

HRESULT CModel::Ready_Bones(HANDLE* pFile, const aiNode* pAIBone, _int iParentBoneIndex)
{
	_ulong dwByte = 0;

	CBone* pBone = CBone::Create(pFile, pAIBone, iParentBoneIndex);
	if (nullptr == pBone)
		return E_FAIL;

	m_Bones.emplace_back(pBone);

	_uint iNumChildren = pAIBone->mNumChildren;
	WriteFile(*pFile, &iNumChildren, sizeof(_uint), &dwByte, nullptr);

	_int		iParentIndex = (_int)m_Bones.size() - 1;

	for (size_t i = 0; i < iNumChildren; ++i)
	{
		Ready_Bones(pFile, pAIBone->mChildren[i], iParentBoneIndex);
	}
	
	return S_OK;
}

HRESULT CModel::Ready_Animations(HANDLE* pFile)
{
	_ulong dwByte = 0;

	m_iNumAnimations = m_pAIScene->mNumAnimations;
	WriteFile(*pFile, &m_iNumAnimations, sizeof(_uint), &dwByte, nullptr);

	for (size_t i = 0; i < m_iNumAnimations; i++)
	{
		CAnimation* pAnimation = CAnimation::Create(pFile, m_pAIScene->mAnimations[i]);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.emplace_back(pAnimation);
	}
	return S_OK;
}

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const _char* pModelFilePath, const _char* pMaterailPath, _fmatrix PreTransformMatrix)
{
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, pModelFilePath, pMaterailPath, PreTransformMatrix)))
	{
		MSG_BOX(TEXT("Failed to Created : CModel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CComponent* CModel::Clone(void* pArg)
{
	CModel* pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CModel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CModel::Free()
{
	__super::Free();

	for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);
	m_Animations.clear();

	for (auto& pBone : m_Bones)
		Safe_Release(pBone);
	m_Bones.clear();

	for (auto& Material : m_Materials)
	{
		for (auto& pTexture : Material.pMaterialTextures)
			Safe_Release(pTexture);
	}
	m_Materials.clear();

	for (auto& pMesh : m_Meshes)
	{
		Safe_Release(pMesh);
	}
	m_Meshes.clear();
}
