#include "..\Public\Light_Manager.h"
#include "Light.h"

CLight_Manager::CLight_Manager()
{
}

const LIGHT_DESC* CLight_Manager::Get_LightDesc(_uint iIndex) const
{
	auto	iter = m_Lights.begin();

	for (size_t i = 0; i < iIndex; i++)
		++iter;

	return (*iter)->Get_LightDesc();
}

HRESULT CLight_Manager::Initialize()
{

	return S_OK;
}

HRESULT CLight_Manager::Add_Light(const LIGHT_DESC& LightDesc)
{
	CLight* pLight = CLight::Create(LightDesc);
	if (nullptr == pLight)
		return E_FAIL;

	m_Lights.push_back(pLight);

	return S_OK;
}

HRESULT CLight_Manager::Render(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	// 빛에 관한 정보를 렌더 타겟에 그리기위함

	// 원래 빛의 수 마다 렌더 타겟을 만드는 것이 맞음
	// 하지만 그러면 빛의 수 만큼 렌더 타겟을 만들어야 해서 비용이 커짐
	// 따라서 우리는 빛을 한 렌더타겟에 블렌드 해서 그릴 것이기 때문

	for (auto& pLight : m_Lights)
		pLight->Render(pShader, pVIBuffer);

	//m_Lights.clear();

	return S_OK;
}

void CLight_Manager::Clear()
{
	for (auto& pLight : m_Lights)
		Safe_Release(pLight);

	m_Lights.clear();
}

CLight_Manager* CLight_Manager::Create()
{
	CLight_Manager* pInstance = new CLight_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Cloned : CLight_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLight_Manager::Free()
{
	__super::Free();

	for (auto& pLight : m_Lights)
		Safe_Release(pLight);

	m_Lights.clear();
}