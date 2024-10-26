#include "stdafx.h"
#include "Player_MotionTrail.h"
#include "Player_Custom.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"

CPlayer_MotionTrail::CPlayer_MotionTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CPlayer_MotionTrail::CPlayer_MotionTrail(const CPlayer_MotionTrail& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CPlayer_MotionTrail::Initialize(_uint iNumMesh)
{
	m_iNumMesh = iNumMesh;
	m_iMaxMotionTrailCount = 20;

	return S_OK;
}

void CPlayer_MotionTrail::Priority_Update(_float fTimeDelta)
{


}

void CPlayer_MotionTrail::Update(_float fTimeDelta)
{

	if (m_AlphaTimes.size() > 0 && m_AlphaTimes[0] <= 0.05f)
	{
		if (m_MotionTrailsInfos.size() > 0)
		{
			for (_uint i = 0; i < m_iNumMesh; ++i)
			{
				Safe_Delete_Array(m_MotionTrailsInfos[0][i]);
			}
			Safe_Delete_Array(m_MotionTrailsInfos[0]);

			m_AlphaTimes.erase(m_AlphaTimes.begin());
			m_MotionTrailsInfos.erase(m_MotionTrailsInfos.begin());
			m_MotionWorldMatrixs.erase(m_MotionWorldMatrixs.begin());
		}

	}

	for (_uint i = 0; i < m_AlphaTimes.size(); ++i)
	{
		m_AlphaTimes[i] -= 1.5f * fTimeDelta;
		if (m_AlphaTimes[i] <= 0.f)
			m_AlphaTimes[i] = 0.f;
	}
}

void CPlayer_MotionTrail::Late_Update(_float fTimeDelta)
{
	m_fDeleteTime += fTimeDelta;

	/*if (m_fDeleteTime >= 0.2f)
	{
		if (m_MotionTrailsInfos.size() > 0)
		{
			for (_uint i = 0; i < m_iNumMesh; ++i)
			{
				Safe_Delete_Array(m_MotionTrailsInfos[0][i]);
			}
			Safe_Delete_Array(m_MotionTrailsInfos[0]);
			m_AlphaTimes.erase(m_AlphaTimes.begin());
			m_MotionTrailsInfos.erase(m_MotionTrailsInfos.begin());
			m_MotionWorldMatrixs.erase(m_MotionWorldMatrixs.begin());
		}

		m_fDeleteTime = 0.f;
	}*/
}

HRESULT CPlayer_MotionTrail::Render()
{
	return S_OK;
}

void CPlayer_MotionTrail::Add_Info(_float4x4** BoneMatrix, _float4x4 WorldMatrix)
{
	if (m_MotionTrailsInfos.size() >= m_iMaxMotionTrailCount)
	{
		for (_uint i = 0; i < m_iNumMesh; ++i)
		{
			Safe_Delete_Array(m_MotionTrailsInfos[0][i]);
		}
		Safe_Delete_Array(m_MotionTrailsInfos[0]);
		m_MotionTrailsInfos.erase(m_MotionTrailsInfos.begin());
		m_MotionWorldMatrixs.erase(m_MotionWorldMatrixs.begin());
		m_AlphaTimes.erase(m_AlphaTimes.begin());

		m_fDeleteTime = 0.f;
	}

	_float4x4** BoneMatrices = new _float4x4 * [m_iNumMesh];

	for (_uint i = 0; i < m_iNumMesh; ++i)
	{
		BoneMatrices[i] = new _float4x4[g_iMaxMeshBones];

		memcpy(BoneMatrices[i], BoneMatrix[i], sizeof(_float4x4) * g_iMaxMeshBones);
	}

	m_AlphaTimes.push_back(1.f);
	m_MotionTrailsInfos.push_back(BoneMatrices);
	m_MotionWorldMatrixs.push_back(WorldMatrix);
}

CPlayer_MotionTrail* CPlayer_MotionTrail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNumMesh)
{
	CPlayer_MotionTrail* pInstance = new CPlayer_MotionTrail(pDevice, pContext);

	if (FAILED(pInstance->Initialize(iNumMesh)))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer_MotionTrail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_MotionTrail::Free()
{

	__super::Free();

	for (auto& Matix : m_MotionTrailsInfos)
	{
		for (_uint i = 0; i < m_iNumMesh; ++i)
		{
			Safe_Delete_Array(Matix[i]);
		}
		Safe_Delete_Array(Matix);
	}
}

void CPlayer_MotionTrail::Clear()
{
	for (auto& Matix : m_MotionTrailsInfos)
	{
		for (_uint i = 0; i < m_iNumMesh; ++i)
		{
			Safe_Delete_Array(Matix[i]);
		}
		Safe_Delete_Array(Matix);
	}
	m_MotionTrailsInfos.clear();
	m_MotionWorldMatrixs.clear();
}
